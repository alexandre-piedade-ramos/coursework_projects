#! /bin/bash
rsqlc='psql --username=alexandre --dbname=projeto  '
declare -a tables=("clinica" "enfermeiro" "medico" "trabalha" "paciente" "consulta" "observacao"  "receita" )

$rsqlc --file ./data/schema.sql

unzip -o -q ./data/table_values.zip -d ./data/

for i in "${tables[@]}"
do
	$rsqlc -c "COPY $i FROM STDIN WITH CSV HEADER;" < ./data/$i.csv ; #> /dev/null
	rm ./data/$i.csv
done


