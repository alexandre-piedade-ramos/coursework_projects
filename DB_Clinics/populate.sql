BEGIN;
-- using \copy instead of COPY because it reads the file with client side permissions, hopefully avoiding permission an/or path issues.
\copy clinica from './data/clinica.csv' WITH CSV HEADER;
\copy enfermeiro FROM './data/enfermeiro.csv' WITH CSV HEADER;
\copy medico FROM './data/medico.csv' WITH CSV HEADER;
\copy trabalha FROM './data/trabalha.csv' WITH CSV HEADER;
\copy paciente FROM './data/paciente.csv' WITH CSV HEADER;
\copy consulta FROM './data/consulta.csv' WITH CSV HEADER;
\copy observacao FROM './data/observacao.csv' WITH CSV HEADER;
\copy receita FROM './data/receita.csv' WITH CSV HEADER;

SELECT setval('consulta_id_seq', max(id)) FROM consulta;

insert into paciente values
('35847584735', '193584735', 'Elvis Presley', '913584747', 'R da Boavista LOTE 2º DTO 7940-134 - Cuba', '1935-01-08');

insert into consulta values
( nextval('consulta_id_seq'),'35847584735', '583145960', 'Ultramedical', '2023-05-01', '12:30:00', '321464711559' ),
( nextval('consulta_id_seq'),'35847584735', '583145960', 'Ultramedical', '2023-06-05', '12:00:00', '321802557017' ),
( nextval('consulta_id_seq'),'35847584735', '583145960', 'Ultramedical', '2023-07-03', '12:30:00', '321890030382' ),
( nextval('consulta_id_seq'),'35847584735', '583145960', 'Ultramedical', '2023-08-07', '12:30:00', '321994466317' ),
( nextval('consulta_id_seq'),'35847584735', '583145960', 'Ultramedical', '2023-09-04', '12:30:00', '321594979855' ),
( nextval('consulta_id_seq'),'35847584735', '583145960', 'Ultramedical', '2023-10-02', '12:30:00', '321613559676' ),
( nextval('consulta_id_seq'),'35847584735', '583145960', 'Ultramedical', '2023-11-06', '12:30:00', '321656756933' ),
( nextval('consulta_id_seq'),'35847584735', '583145960', 'Ultramedical', '2023-12-04', '12:30:00', '321918270631' ),
( nextval('consulta_id_seq'),'35847584735', '583145960', 'Ultramedical', '2024-01-01', '12:30:00', '321349322899' ),
( nextval('consulta_id_seq'),'35847584735', '583145960', 'Ultramedical', '2024-02-05', '12:00:00', '321134781395' ),
( nextval('consulta_id_seq'),'35847584735', '583145960', 'Ultramedical', '2024-03-04', '12:00:00', '321319912808' ),
( nextval('consulta_id_seq'),'35847584735', '583145960', 'Ultramedical', '2024-04-01', '12:30:00', '321173660534' ),
( nextval('consulta_id_seq'),'35847584735', '583145960', 'Ultramedical', '2024-05-06', '12:30:00', '321910384517' );


insert into observacao values
('93985' ,'Heartbreak', null),
('93986' ,'Heartbreak', null),
('93987' ,'Heartbreak', null),
('93988' ,'Heartbreak', null),
('93989' ,'Heartbreak', null),
('93990' ,'Heartbreak', null),
('93991' ,'Heartbreak', null),
('93992' ,'Heartbreak', null),
('93993' ,'Heartbreak', null),
('93994' ,'Heartbreak', null),
('93995' ,'Heartbreak', null),
('93996' ,'Heartbreak', null),
('93997' ,'Heartbreak', null);

insert into receita values
('321464711559' , 'Edoxaban', 2 ),
('321802557017' , 'Edoxaban', 2 ),
('321890030382' , 'Edoxaban', 2 ),
('321994466317' , 'Edoxaban', 2 ),
('321594979855' , 'Edoxaban', 2 ),
('321613559676' , 'Edoxaban', 2 ),
('321656756933' , 'Edoxaban', 2 ),
('321918270631' , 'Edoxaban', 2 ),
('321349322899' , 'Edoxaban', 2 ),
('321134781395' , 'Edoxaban', 2 ),
('321319912808' , 'Edoxaban', 2 ),
('321173660534' , 'Edoxaban', 2 ),
('321910384517' , 'Edoxaban', 2 );

COMMIT;
ANALYZE; -- recommended after large updates to the database by the docs

