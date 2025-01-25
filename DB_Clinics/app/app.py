from os import environ
from flask import Flask, jsonify, request, url_for
from psycopg.rows import namedtuple_row
from psycopg_pool import ConnectionPool
from psycopg import errors
import logging
# Use the DATABASE_URL environment variable if it exists, otherwise use the default.
# Use the format postgres://username:password@hostname/database_name to connect to the database.

#if postgres running through docker but not flask
DATABASE_URL = "postgres://postgres:postgres@localhost:5432/postgres"

#if both postgres and flask are running through docker 
#DATABASE_URL = "postgres://postgres:postgres@postgres/postgres"

app = Flask(__name__)

logging.basicConfig(level=logging.DEBUG,
                    format='[%(asctime)s] %(levelname)s in %(module)s: %(message)s',
                    datefmt='%Y-%m-%d %H:%M:%S')

app.logger.info(DATABASE_URL)
pool = ConnectionPool(
    conninfo=DATABASE_URL,
    kwargs={
        "autocommit": True, 
        "row_factory": namedtuple_row,
    },
    min_size=1,
    max_size=5,
    open=True,
    #check=ConnectionPool.check_connection,
    name="postgres_pool",
    timeout=5,
)

@app.route("/", methods=("GET",))
def list_clinics():
    """Lista todas as clínicas (nome e morada)."""
    try:
        with pool.connection() as conn:
            with conn.cursor() as cur:
                clinics = cur.execute(
                    """
                    SELECT nome, morada
                    FROM clinica;
                    """,
                    {},
                ).fetchall()
    except Exception as e:
       app.logger.debug("%s : %s", type(e), e)
       return jsonify({'Erro':' Alguma coisa correu mal do nosso lado'}), 500 

    return jsonify(clinics), 200

@app.route("/c/<clinica>/", methods=("GET",))
def list_especialidades(clinica):
    """Lista todas as especialidades oferecidas na <clinica>."""
    clinica = clinica
    try:
        with pool.connection() as conn:
            with conn.cursor() as cur:
                especialidades = cur.execute(
                    """
                    SELECT DISTINCT especialidade
                    FROM trabalha
                    JOIN medico using (nif)
                    WHERE trabalha.nome=%s; 
                    """,
                    (clinica,),
                ).fetchall()
                app.logger.debug(cur.rownumber)
                if cur.rownumber==0:
                    return jsonify({'Erro':' Nome de clinica não encontrado'}), 400 
    except Exception as e:
       app.logger.debug("%s : %s", type(e), e)
       return jsonify({'Erro':' Alguma coisa correu mal do nosso lado'}), 500
    
    return jsonify(especialidades), 200 

@app.route("/c/<clinica>/<especialidade>/", methods=("GET",))
def available_time_slots(clinica, especialidade):
    """"
    Lista todos os médicos (nome) da <especialidade> que
    trabalham na <clínica> e os primeiros três horários
    disponíveis para consulta de cada um deles (data e hora) até ao final do ano.
    """

    clinica = clinica
    especialidade = especialidade
    try:
        with pool.connection() as conn:
            with conn.cursor() as cur:
                ret = cur.execute(
                    """
                    with all_time_slots as(
                    --all timeslots until the end of year
                        with times as (
                            select '08:00'::time + c * interval '30 minutes' as hora
                            from generate_series(0, 21) as c
                            where  c!=10 and c!=11 --lunchtime
                        ),
                        dates as (
                            select d as data
                            from generate_series(
                                    current_date,
                                    (date_trunc('year', CURRENT_DATE) + interval '1 year' - interval '1 day'),
                                    '1 day') as d
                        )
                        select d.data, t.hora 
                        from times t cross join dates d
                        where (d.data > current_date or t.hora > current_time) 
                    ),
                    free_time_slots as(
                        --every free time slot for every worday of doctors of the specialty in the clinic, for the rest of the year
                        select med.nif, med.nome, ts.data, ts.hora
                        from trabalha t 
                        join medico med using (nif)
                        join all_time_slots ts on extract(isodow from ts.data)=(t.dia_da_semana)
                        left join consulta c using (nif, data, hora)
                        where t.nome=%(clinica)s
                        and med.especialidade=%(especialidade)s
                        and c.id is null --não há consulta marcada
                    )
                    --select the earliest n 
                    select nome, to_char(data+hora, 'YYYY-MM-DD HH24:MI:SS') as timestamp
                    from free_time_slots fs1
                    where %(n)s >
                    (
                        select count(*)
                        from free_time_slots fs2
                        where fs1.nif = fs2.nif 
                        and ((fs1.data + fs1.hora) > (fs2.data + fs2.hora))   
                    );
                    """,
                    {'clinica': clinica, 'especialidade':especialidade, 'n':3}
                    #to_char conversion is required, else the data type returned is datetime which is not json serializable (not sure what that means, but thats the error)                    
                ).fetchall()

                if cur.rownumber ==0:
                    return jsonify({'Erro':
                                    'Não encontramos horarios disponiveis para a clinica/especialidade fornecidas.'
                                    }), 400
    except Exception as e:
        app.logger.debug("%s : %s", type(e), e)
        return jsonify({'Erro':' Alguma coisa correu mal do nosso lado'}), 500
    
    res={}
    for key, value in jsonify(ret).get_json():
        if key not in res:
            res[key] = []
        res[key]+=[value,]

    return jsonify(res), 200
        
@app.route("/a/<clinica>/registar/", methods=("POST",))
def marcar_consulta(clinica):
    """
    Registra uma marcação de consulta na <clinica> na base
    de dados (populando a respectiva tabela). Recebe como
    argumentos um paciente, um médico, e uma data e hora
    (posteriores ao momento de agendamento).
    """
    paciente = request.args.get("paciente") # ssn
    medico = request.args.get("medico") # nif
    data = request.args.get("data") # YYYY-MM-DD
    hora = request.args.get("hora") # HH24:mm

    if(paciente is None or medico is None or data is None or hora is None ):
        return jsonify({"error": "Missing parameters",
                        "parameters": {
                        "paciente": "<11 digitos do ssn de um paciente registado>",
                        "medico": "<9 digitos do nif de um medico>",
                        "data": "<YYYY-MM-DD>",
                        "hora": "<HH24:mm:ss>"
                        }
        }), 400
    
    if(not paciente.isdigit() or len(paciente)!=11):
        return jsonify({'Erro':'SSN de paciente é invalido'}), 400
    if(not medico.isdigit() or len(medico)!=9):
        return jsonify({'Erro':'NIF de medico é invalido'}), 400
    
    try:
        with pool.connection() as conn: 
            with conn.cursor() as cur:
                cur.execute(
                    """
                    insert into consulta (ssn, nif, nome, data, hora, codigo_sns)
                    select %s, %s, %s, %s, %s, null
                    where ( select %s::date + %s::time >= current_timestamp)
                    returning id;
                    """,
                    (paciente, medico, clinica, data, hora, data, hora)
                )
                res = cur.fetchone()
    except errors.RaiseException as e:
        app.logger.debug("%s : %s", type(e), e)
        return jsonify({'Erro':' %s'% (e.diag.message_primary)}), 400
    except errors.UniqueViolation as e:
        app.logger.debug("%s : %s", type(e), e)
        if(e.diag.constraint_name == 'consulta_ssn_data_hora_key'):
            return jsonify({'Erro':' Doente já tem marcação à mesma hora'}), 400
        return jsonify({'Erro':' Medico já tem marcação à mesma hora'}), 400
    except errors.CheckViolation as e:
        app.logger.debug("%s : %s", type(e), e)
        return jsonify({'Erro':' Marcações nos intervalos de 30 minutos no horário 8-13h e 14-19h'}), 400
    except errors.ForeignKeyViolation as e:
        app.logger.debug("%s : %s", type(e), e)
        vfkc=e.diag.message_primary.split('\"')[3]
        if(vfkc=='consulta_ssn_fkey'):
            return jsonify({'Erro':' SSN de paciente não encontrado'}), 400
        elif(vfkc=='consulta_nome_fkey'):
            return jsonify({'Erro':' Nome de clinica não encontrado'}), 400
        elif(vfkc=='consulta_nif_fkey'):
            return jsonify({'Erro':' NIF de medico não encontrado'}), 400
    except (errors.InvalidDatetimeFormat, errors.DatetimeFieldOverflow) as e:
        app.logger.debug("%s : %s", type(e), e)
        return jsonify({'Erro':' Data e/ou hora são invalidas'}), 400
    except Exception as e:
       app.logger.debug("%s : %s", type(e), e)
       return jsonify({'Erro':' Alguma coisa correu mal do nosso lado'}), 500
    
    if(res is None):
        return jsonify({'Erro':' Marcação tem de ser  no futuro'}) , 400
    
    return  jsonify({'Sucesso':'Marcação realizada'}) , 200


@app.route("/a/<clinica>/cancelar/", methods=("DELETE",))
def cancelar_consulta(clinica):
    """
    Cancela uma marcação de consulta que ainda não se realizou
    na <clinica> (o seu horário é posterior ao momento do
    cancelamento), removendo a entrada da respectiva tabela na
    base de dados. Recebe como argumentos um paciente, um
    médico, e uma data e hora.
    """
    paciente = request.args.get("paciente") # ssn
    medico = request.args.get("medico") # nif
    data = request.args.get("data") # YYYY-MM-DD
    hora = request.args.get("hora") # HH24:mm

    if(paciente is None or medico is None or data is None or hora is None ):
        return jsonify({"error": "Missing parameters",
                        "parameters": {
                        "paciente": "<11 digitos do ssn de um paciente registado>",
                        "medico": "<9 digitos do nif de um medico>",
                        "data": "<YYYY-MM-DD>",
                        "hora": "<HH24:mm:ss>"
                        }
        }), 400

    if(not paciente.isdigit() or len(paciente)!=11):
        return jsonify({'Erro':'SSN de paciente é invalido'}), 400
    if(not medico.isdigit() or len(medico)!=9):
        return jsonify({'Erro':'NIF de medico é invalido'}), 400
    
    try:
        with pool.connection() as conn: 
            with conn.cursor() as cur:
                cur.execute(
                    """
                    delete from consulta
                        where ssn = %s
                        and nif = %s
                        and nome = %s
                        and data = %s
                        and hora = %s
                        and (select (%s::date + %s::time) > current_timestamp)
                    returning id; 
                    """,
                    (paciente, medico, clinica, data, hora, data, hora)
                )
                res = cur.fetchone()
    except (errors.InvalidDatetimeFormat, errors.DatetimeFieldOverflow) as e:
        app.logger.debug("%s : %s", type(e), e)
        return jsonify({'Erro':' Data e/ou hora são invalidas'}), 400
    except Exception as e:
       app.logger.debug("%s : %s", type(e), e)
       return jsonify({'Erro':' Alguma coisa correu mal do nosso lado'}), 500
    
    if(res is None):
        return jsonify({'Erro':' Marcação não existe ou já passou'}), 400
    
    return  jsonify({'Sucesso':'Marcação cancelada'}) , 200