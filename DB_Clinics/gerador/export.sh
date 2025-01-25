#! /bin/bash
rsqlc='psql --username=alexandre --dbname=projeto -q '
declare -a tables=("clinica" "enfermeiro" "medico" "trabalha" "paciente" "consulta" "observacao"  "receita" )

$rsqlc --file ./data/schema.sql

for i in "${tables[@]}"
do
	$rsqlc -c "COPY $i TO STDOUT WITH CSV HEADER" > ./data/$i #> /dev/null
done

for i in "${tables[@]}"
do
	zip -q ./data/table_values ./data/$i
    rm ./data/$i
done

