import random
import datetime
import csv

# Constantes que controlam o número de entidades criadas no gerador
NUM_CLINICAS = 5
NUM_ENFERMEIROS_POR_CLINICA = random.randint(5,6)
NUM_ENFERMEIROS = NUM_ENFERMEIROS_POR_CLINICA * NUM_CLINICAS
NUM_MEDICOS = 60
NUM_PACIENTES = 5000
NUM_PESSOAS = NUM_ENFERMEIROS + NUM_MEDICOS + NUM_PACIENTES
NUM_PESSOAS_E_CLINICAS = NUM_CLINICAS + NUM_ENFERMEIROS + NUM_MEDICOS + NUM_PACIENTES

# Abrir os ficheiros .txt e guardar os conteudos numa lista
with open('primeiro_nome.txt') as primeiro_nome_file:
    primeiro_nome_lista = primeiro_nome_file.read().splitlines()

with open('ultimo_nome.txt') as ultimo_nome_file:
    ultimo_nome_lista = ultimo_nome_file.read().splitlines()

with open('moradas.txt') as moradas_file:
    apartamentos = ["RC", "1º DTO", "1º ESQ", "2º DTO", "2º ESQ", "3º DTO", "3º ESQ"]
    
    moradas_lista = []
    for morada in random.sample(list(csv.reader(moradas_file)), NUM_PESSOAS_E_CLINICAS):
        moradas_lista.append(f"{morada[0]} LOTE {random.choice(apartamentos)} {morada[1]} - {morada[2]}")

with open('medicamentos.txt') as medicamentos_file:
    medicamentos_lista = medicamentos_file.read().splitlines()

with open('especialidades.txt') as especialidades_file:
    especialidades_lista = especialidades_file.read().splitlines()

with open('nomes_clinicas.txt') as nomes_clinicas_file:
    nomes_clinicas_total = nomes_clinicas_file.read().splitlines()
    nomes_clinicas = random.sample(nomes_clinicas_total, NUM_CLINICAS)

with open('sintomas_qualitativos.txt') as sintomas_qualitativos_file:
    sintomas_qualitativos_lista = sintomas_qualitativos_file.read().splitlines()

with open('sintomas_quantitativos.txt') as sintomas_quantitativos_file:
    sintomas_quantitativos_lista = []
    for line in sintomas_quantitativos_file:
        sintomas_quantitativos_lista.append(line.replace("\n", "").strip().split(", "))

# Criar lista com valores aleátorios (todos os valores são diferentes uns dos outros)
# A quantidade de valores é baseado na quantidade de entidades
telefones_lista = random.sample(range(910000000, 990000000), NUM_PESSOAS_E_CLINICAS)
nif_lista = random.sample(range(100000000, 999999999), NUM_PESSOAS)
ssn_lista = random.sample(range(10000000000, 99999999999), NUM_PESSOAS)
codigo_sns_lista = random.sample(range(100000000000, 999999999999), (365*2*4*NUM_MEDICOS))


# Variáveis para controlar a criação de entidades
telefone_index_counter = 0
moradas_index_counter = 0
nif_index_counter = 0
ssn_index_counter = 0
codigo_sns_index_counter = 0


def main():
    clinica_gerador()
    enfermeiro_gerador()
    medico_gerador()
    trabalha_gerador()
    paciente_gerador()
    
    # Gerar consultas e guardar o número das receitas e o número de consultas em variáveis
    num_receitas, num_consultas = consulta_gerador()
    
    # Gerar receitas tendo em conta o número de receitas criadas em consulta_gerador()
    receita_gerador(num_receitas)

    # Gerar observações tendo em conta o número de consultas criadas em consulta_gerador()
    observacao_gerador(num_consultas)


def clinica_gerador():
    with open('clinica.csv', 'w', newline='') as file:
        writer = csv.writer(file)   

        # Escrever header
        writer.writerow(['nome','telefone', 'morada'])

        global telefone_index_counter
        global moradas_index_counter
        global nif_index_counter

        # Lista que vai guardar as moradas usadas para as clinicas
        clinicas_moradas = []

        # Pegar na primeira morada de moradas_lista, dar append a essa morada na lista clinicas_moradas
        # Remover essa morada de moradas_lista porque as outras entidades não podem ter moradas repetidas
        clinicas_moradas.append(moradas_lista[0])
        moradas_lista.pop(0)
        _, primeira_localidade_clinica = clinicas_moradas[0].split(" - ")

        # Escolher uma morada com localidade diferente da primeira
        # Remover a segunda morada escolhida da moradas_lista
        for morada in moradas_lista:
            __, localidade = morada.split(" - ")
            if localidade != primeira_localidade_clinica:
                clinicas_moradas.append(morada)
                moradas_lista.remove(morada)
                segunda_localidade_clinica = localidade
                break
        
        # Escolher uma morada com localidade diferente da primeira e da segunda 
        # Remover a terceira morada escolhida da moradas_lista
        for morada in moradas_lista:
            __, localidade = morada.split(" - ")
            if localidade != primeira_localidade_clinica and localidade != segunda_localidade_clinica:
                clinicas_moradas.append(morada)
                moradas_lista.remove(morada)
                break

        # Preencher a lista com moradas com um número fixo de clinicas
        for morada in moradas_lista:
            if len(clinicas_moradas) != NUM_CLINICAS:
                clinicas_moradas.append(morada)
                moradas_lista.remove(morada)
            else:
                break

        # Gerar csv com clinicas            
        for i in range(NUM_CLINICAS):
            writer.writerow([f'{nomes_clinicas[i]}',
            f'{telefones_lista[telefone_index_counter]}',
            f'{clinicas_moradas[i]}'])
            telefone_index_counter += 1


def enfermeiro_gerador():
    with open('enfermeiro.csv', 'w', newline='') as file:
        writer = csv.writer(file)
    
        # Escrever header
        writer.writerow(['nif', 'nome','telefone', 'morada', 'nome_clinica'])

        global telefone_index_counter
        global moradas_index_counter
        global nif_index_counter

        # Gerar 5-6 enfermeiros para cada clinica
        for c in range(NUM_CLINICAS):
            for e in range(NUM_ENFERMEIROS_POR_CLINICA):
                    writer.writerow([f'{nif_lista[nif_index_counter]}',
                    f'{random.choice(primeiro_nome_lista)} {random.choice(ultimo_nome_lista)}',
                    f'{telefones_lista[telefone_index_counter]}',
                    f'{moradas_lista[moradas_index_counter]}',
                    f'{nomes_clinicas[c]}'])
                    nif_index_counter += 1
                    telefone_index_counter += 1
                    moradas_index_counter += 1



def medico_gerador():
    with open('medico.csv', 'w', newline='') as file:
        writer = csv.writer(file)
        
        # Escrever header 
        writer.writerow(['nif','nome', 'telefone', 'morada', 'especialidade'])

        # Lista com a especialidade e o número de médicos que estão associados com essa especialidade
        medico_especialidade_lista = [('Clínica geral', 20), ('Ortopedia', 1), ('Cardiologia', 1)]

        global telefone_index_counter
        global moradas_index_counter
        global nif_index_counter

        # Criar 20 médicos de clínica geral, 1 de ortopedia, 1 de cardiologia.
        for especialidade in medico_especialidade_lista:
            for num_medicos in range (especialidade[1]):
                writer.writerow([
                    f'{nif_lista[nif_index_counter]}',
                    f'{random.choice(primeiro_nome_lista)} {random.choice(ultimo_nome_lista)}',
                    f'{telefones_lista[telefone_index_counter]}',
                    f'{moradas_lista[moradas_index_counter]}',
                    f'{especialidade[0]}'
                    ])
                nif_index_counter += 1
                telefone_index_counter += 1
                moradas_index_counter += 1

        # Criar o resto dos médicos com uma especialidade aleatória
        for num in range (NUM_MEDICOS - 22):
            writer.writerow([
                f'{nif_lista[nif_index_counter]}',
                f'{random.choice(primeiro_nome_lista)} {random.choice(ultimo_nome_lista)}',
                f'{telefones_lista[telefone_index_counter]}',
                f'{moradas_lista[moradas_index_counter]}',
                f'{random.choice(especialidades_lista)}'
                ])
            nif_index_counter += 1
            telefone_index_counter += 1
            moradas_index_counter += 1



def paciente_gerador():
    with open('paciente.csv', 'w', newline='') as file:
        writer = csv.writer(file)
        
        # Escrever header
        writer.writerow(['ssn', 'nif','nome', 'telefone', 'morada', 'data_nasc'])

        global telefone_index_counter
        global moradas_index_counter
        global nif_index_counter
        
        # Gerar pacientes
        for num in range (NUM_PACIENTES):
            writer.writerow([
                f'{ssn_lista[num]}', 
                f'{nif_lista[nif_index_counter]}',
                f'{random.choice(primeiro_nome_lista)} {random.choice(ultimo_nome_lista)}',
                f'{telefones_lista[telefone_index_counter]}',
                f'{moradas_lista[moradas_index_counter]}',
                f'{random.randint(1920,2022)}-{random.randint(1,12)}-{random.randint(1,28)}'
                ])
            nif_index_counter += 1
            telefone_index_counter += 1
            moradas_index_counter += 1

def trabalha_gerador():
    with open('trabalha.csv', 'w', newline='') as file:
        writer = csv.writer(file)

        # Escrever header
        writer.writerow(['nif', 'nome', 'dia_da_semana'])

        with open('medico.csv', 'r', newline='') as file:
            nif_medicos_lista = []
            for line in csv.reader(file):
                nif_medicos_lista.append(line[0])
            nif_medicos_lista.pop(0)
        

        # Todos os médicos vão trabalhar nas 5 clinicas durante a semana, 1 dia da semana para cada clinica
        # No loop: Escolhe-se o 1º médico dos 60, escolhe-se 1 dia da semana para cada clinica, escolhe-se
        #          outro médico com o nif a seguir para fazer o mesmo. Os dias da semana escolhidos para
        #          o médico seguinte vão ser diferentes do médico anterior.
        
        # O "dia_da_semana" está a ir de 1 a 7, quando chega a 7 volta ao 1, o ciclo continua.
        # dia_da_semana = 1 representa Segunda-feira, 7 representa Domingo.
        # Isto serve para os médicos não estarem em duas clínicas no mesmo dia da semana e para "espalhar" os médicos igualmente pelos dias da semana.    

        dia_da_semana = 1
        
        for nif_medico in nif_medicos_lista:
            for clinica in nomes_clinicas:
                if dia_da_semana > 7:
                    dia_da_semana = 1                
                writer.writerow([f'{nif_medico}', f'{clinica}', f'{dia_da_semana}'])
                
                dia_da_semana +=  1


def consulta_gerador():

    # Tirar a escala diretamente dos médicos de trabalha.csv
    with open('trabalha.csv', 'r', newline='') as file:
        escala = []
        for line in csv.reader(file):
            escala.append(line)
        escala.pop(0)

    # Popular todos os dias de 2023 e 2024
    starting_date = datetime.date(2023, 1, 1)
    end_date = datetime.date(2025, 1, 1)
    
    # Id da primeira consulta
    id = 1

    global codigo_sns_index_counter
    global ssn_index_counter

    with open('consulta.csv', 'w', newline='') as file:
        writer = csv.writer(file)

        # Escrever header
        writer.writerow(['id','ssn', 'nif', 'nome', 'data', 'hora', 'codigo_sns'])
        
        # Explicação da contraption:
        # Pegar na data, encontrar o dia da semana que corresponde a essa data, encontrar na escala os médicos que trabalham nesse dia da semana.
        # Criar consulta nessa data com esses médicos, na clinica correspondente, em 2 horas diferentes, com uma chance de 80% de haver uma receita médica em cada consulta.
        # Cada médico vai ter 3 consultas por dia e trabalhar 5 vezes por semana.
        # Incrementar a data e continuar o loop até preenchermos 2023 e 2024.
        # Parar loop quando todos os dias de 2023 e 2024 estiveram preenchidos
        
        while starting_date != end_date:
            if starting_date == datetime.date(2024, 6, 1):
                num_observacoes = id - 1
            for horas in criar_horas_randoms():        
                # Cada elemento da escala tem o médico, clinica e dia da semana.
                for elemento_escala in escala:       
                    # Se o dia da semana da data escolhida for igual ao dia da semana da escala -> continuar
                    if int(elemento_escala[2]) == int(starting_date.weekday() + 1):            
                        # Escolher 2 horas diferentes da lista de horas                       
                        # Se chegarmos ao ultimo paciente voltamos ao primeiro paciente
                        if ssn_index_counter == NUM_PACIENTES:
                            ssn_index_counter = 0
                        
                        # Há uma chance de 80% de ser criado um código SNS
                        codigo_sns_chance = random.choices((['', f'{codigo_sns_lista[codigo_sns_index_counter]}']), weights=(20,80))
                        
                        if starting_date >= datetime.date(2024, 6, 1):
                            codigo_sns_chance = [""]

                        writer.writerow([f'{id}',
                                        f'{ssn_lista[ssn_index_counter]}', 
                                        f'{elemento_escala[0]}', 
                                        f'{elemento_escala[1]}', 
                                        f'{starting_date}', 
                                        f'{horas}', 
                                        f'{codigo_sns_chance[0]}'])
                        
                        # Só incrementar o código SNS se for criado na consulta
                        if codigo_sns_chance[0] != "":
                            codigo_sns_index_counter += 1                   

                        ssn_index_counter += 1
                        id += 1
        
            # Adicionar 1 dia à data
            starting_date += datetime.timedelta(days=1)
    
    
    num_receitas = codigo_sns_index_counter
    return num_receitas, num_observacoes


def receita_gerador(num_receitas):
    with open('receita.csv', 'w', newline='') as file:
        writer = csv.writer(file)

        # Criar header
        writer.writerow(['codigo_sns', 'medicamento', 'quantidade'])

        for num_receita in range(num_receitas):            
            # Escolher 1-6 medicamentos diferentes da lista de medicamentos
            medicamentos_escolhidos = random.sample(medicamentos_lista, random.randint(1,6))
            
            for medicamento in medicamentos_escolhidos:
                writer.writerow([f'{codigo_sns_lista[num_receita]}', 
                f'{medicamento}', 
                f'{random.randint(1,3)}'])


def observacao_gerador(num_consultas):
    with open('observacao.csv', 'w', newline='') as file:
        writer = csv.writer(file)
        
        # Criar header
        writer.writerow(['id', 'parametro', 'valor'])
        
        for i in range(num_consultas):
            # Gerar 1 a 5 sintomas sem valor por consulta
            sintomas_qualitativos_escolhidos = random.sample(sintomas_qualitativos_lista, random.randint(1,5))
            for sintoma_qualitativo in sintomas_qualitativos_escolhidos:
                writer.writerow([f'{1 + i}', f'{sintoma_qualitativo}', ''])

            # Gerar 1 a 3 sintomas com valor por consulta
            sintomas_quantitativos_escolhidos = random.sample(sintomas_quantitativos_lista, random.randint(0,3))
            if sintomas_quantitativos_escolhidos:
                for sintoma_quantitativo in sintomas_quantitativos_escolhidos:
                    writer.writerow([f'{i + 1}', 
                                    f'{sintoma_quantitativo[0]}', 
                                    f'{max(round(random.gauss(0.5*(float(sintoma_quantitativo[1])+float(sintoma_quantitativo[2])),1.25) ,2),float(sintoma_quantitativo[1]))}'])
        

def criar_horas_randoms():
    # Horários possiveis para as consultas
    horas_possiveis = ['08:00','08:30','09:00','09:30','10:00','10:30','11:00','11:30','12:00','12:30','14:00','14:30','15:00','15:30','16:00','16:30','17:00','17:30','18:00','18:30']
    return sorted(random.sample(horas_possiveis, 3))


main()
