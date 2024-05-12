
; *********************************************************************************
; * IST-UL
; * 2022/23
; * Modulo: 	grupo37.asm
; * Grupo 37: 
;   - André Venceslau  | 96159
;   - Alexandre Ramos  | 102598
;   - Guilherme Cirilo | 106245

; * Descrição: versão final do projeto
; *
; *********************************************************************************
;
;TECLAS:
;   D: desce os asteroide
;   5: move o missil
;   B: decrementa a energia
;   C: incrementa a energia
;
; *********************************************************************************
; * Constantes
; *********************************************************************************

; *********************************************************************************
; * Color Pallete
; *********************************************************************************
MUD_GREEN       EQU     30B0H
;16 named html colors
AQUA    EQU 	0F0FFH
BLACK   EQU      0000H
BLUE    EQU     0F00FH
FUCHSIA EQU     0FF0FH
GREY    EQU     0F888H
GREEN   EQU     0F080H
LIME    EQU     0F0F0H
MAROON  EQU     0F800H
NAVY    EQU     0F008H
OLIVE   EQU     0F880H
PURPLE  EQU     0F808H
RED     EQU     0FF00H
SILVER  EQU     0FCCCH
TEAL    EQU     0F080H
WHITE   EQU     0FFFFH
YELLOW  EQU     0FFF0H

; *********************************************************************************
; * Constantes Teclado
; *********************************************************************************

TEC_IN              EQU 0C000H  ;
TEC_OUT             EQU 0E000H  ;
LAST_KB_LINE        EQU 0008H   ;
FIRST_KB_LINE       EQU 0001H   ;
KEY_SHOOT_0		    EQU	0000H   ; dispara sonda
KEY_SHOOT_1         EQU 0001H
KEY_SHOOT_2         EQU 0002H
GAME_PAUSE_KEY      EQU 000FH
; *********************************************************************************
; * Constantes Energy Display
; *********************************************************************************
DISPLAY         EQU 0A000H  ; endereço dos displays de 7 segmentos (periferico POUT-1)
INIT_ENRG       EQU 100d     ;
ENERG_VAR       EQU 3d   ;
MINE_ASTRO_ENERG_GAIN   EQU 25d
ASTEROID_ENERGY EQU 25
MAX_VAL_3_DIG   EQU 999     ;

; *********************************************************************************
; * Constantes SONDA
; *********************************************************************************
LARGURA_SONDA 		EQU  0001H  ; largura da sonda
ALTURA_SONDA	 	EQU  0001H  ; altura da sonda
LINHA_SONDA        	EQU  26     ; linha da sonda
COLUNA_SONDA		EQU  32     ; coluna da sonda
SONDA_REACH			EQU  000CH	; Alcance da sonda
N_SONDAS            EQU     3d  ; numero de sondas
BYTES_DEF_SONDA_POS EQU    10d  ; numero de bytes que definem uma sonda

POS_INICIAL_SONDAS_Y EQU     26d
POS_INICIAL_SONDA_0_X EQU     26d          
POS_INICIAL_SONDA_1_X EQU     32d
POS_INICIAL_SONDA_2_X EQU     38d      
; *********************************************************************************
; * Constantes SONDA BAKGROUND
; *********************************************************************************
MIN_LINE			EQU  0000H      ; primeira linha do display
MAX_LINE            EQU 31          ; última linha do display
; *********************************************************************************
; * Constantes de comandos do MediaCenter
; *********************************************************************************

COMANDOS		EQU	6000H		                    ; endereço de base dos comandos do MediaCenter

DEFINE_LINHA    EQU COMANDOS + 0AH		            ; endereço do comando para definir a linha
DEFINE_COLUNA   EQU COMANDOS + 0CH		            ; endereço do comando para definir a coluna
DEFINE_PIXEL    EQU COMANDOS + 12H		            ; endereço do comando para escrever um pixel
APAGA_AVISO     EQU COMANDOS + 40H		            ; endereço do comando para apagar o aviso de nenhum cenário selecionado
APAGA_ECRÃ	    EQU COMANDOS + 02H		            ; endereço do comando para apagar todos os pixels já desenhados
SELECIONA_CENARIO_FUNDO     EQU COMANDOS + 42H		; endereço do comando para selecionar uma imagem de fundo
APAGA_CENARIO_FRONTAL     EQU COMANDOS + 44H		; endereço do comando para apagar uma imagem de frente
SELECIONA_CENARIO_FRONTAL     EQU COMANDOS + 46H		; endereço do comando para selecionar uma imagem de frente
TOCA_SOM         EQU COMANDOS + 5AH
CHANGE_LAYER     EQU COMANDOS + 4H

LAYER_ASTRO_1    EQU 0
LAYER_ASTRO_2    EQU 1
LAYER_ASTRO_3    EQU 2
LAYER_ASTRO_4    EQU 3
LAYER_SHIP      EQU 4
; *********************************************************************************
LARGURA_SHIP EQU 15
ALTURA_SHIP EQU 5

TRUE    EQU 1
FALSE   EQU 0

LINHA        	EQU  0          ; linha do boneco (a meio do ecrã))
COLUNA			EQU  0          ; coluna do boneco (a meio do ecrã)

MIN_COLUNA		EQU  0		    ; número da coluna mais à esquerda que o objeto pode ocupar
MAX_COLUNA		EQU  63         ; número da coluna mais à direita que o objeto pode ocupar
LARGURA_ASTRO			EQU	5		    ; largura do boneco
COR_PIXEL		        EQU	0FF00H	    ; cor do pixel: vermelho em ARGB (opaco e vermelho no máximo, verde e azul a 0)
ALTURA_ASTRO            EQU 5           ; altura do boneco

TAMANHO_PILHA   EQU 100H                ; tamanho da pilha


GAME_STATE_PLAYING  EQU 1               ; estado do jogo (ativo)
GAME_STATE_PAUSED   EQU 2               ; estado do jogo (pausa)

; *********************************************************************************
; * Dados 
; *********************************************************************************
	PLACE       1000H

	STACK TAMANHO_PILHA			; espaço reservado para a pilha (200H bytes, pois são 100H words)
SP_inicial:				; este é o endereço (1200H) com que o SP deve ser 
						; inicializado. O 1.º end. de retorno será 
						; armazenado em 11FEH (1200H-2)

    STACK TAMANHO_PILHA
SP_teclado:

    STACK TAMANHO_PILHA
SP_blink_panel:

    STACK TAMANHO_PILHA*N_SONDAS
SP_sondas:

evento_sondas:
    LOCK 0                  ; Lock das sondas
evento_blink_panel:
    LOCK 0                  ; Lock do painel

evento_tecla_premida:
    LOCK 0FFFFH             ; Lock tecleda para as teclas

evento_shoot_sonda:
    LOCK 0                  ; Lock da sonda disparada

interrupts_table:
    WORD    rot_int_0			        ; rotina de atendimento da interrupção 0
	WORD    rot_int_1_sonda			    ; rotina de atendimento da interrupção 1
	WORD    rot_int_2_dec_energy		; rotina de atendimento da interrupção 2
    WORD    rot_int_3_panel             ; rotina de atendimento da interrupção 3

; **********************************************************************


DEF_ASTRO:					                            ; tabela que define o asteroide (cor, largura, pixels)
	WORD    LARGURA_ASTRO
    WORD    ALTURA_ASTRO 
	WORD	0, 0, COR_PIXEL, 0, 0		            ; # # #   as cores podem ser diferentes de pixel para pixel
    WORD    0, COR_PIXEL, 0, COR_PIXEL, 0
    WORD	COR_PIXEL, 0, COR_PIXEL, 0, COR_PIXEL
    WORD    0, COR_PIXEL, 0, COR_PIXEL, 0
    WORD	0, 0, COR_PIXEL, 0, 0

DEF_ASTRO_MINERABLE:                                 ; tabela que define o asteroide mineravel (cor, largura, pixels)
	WORD	LARGURA_ASTRO
    WORD    ALTURA_ASTRO 
    WORD	BLUE, 0, BLUE, 0, BLUE
    WORD    0, BLUE, 0, BLUE, 0
    WORD	BLUE, 0, BLUE, 0, BLUE
    WORD    0, BLUE, 0, BLUE, 0
    WORD	BLUE, 0, BLUE, 0, BLUE

DEF_ASTRO_1_IMAGE:
    WORD    0
DEF_ASTRO_2_IMAGE:
    WORD    0
DEF_ASTRO_3_IMAGE:
    WORD    0
DEF_ASTRO_4_IMAGE:
    WORD    0

DEF_ASTRO_POS_1:
    WORD    0 ; linha 
    WORD    0 ; coluna
DEF_ASTRO_POS_2:
    WORD    0 ; linha 
    WORD    0 ; coluna
DEF_ASTRO_POS_3:
    WORD    0 ; linha 
    WORD    0 ; coluna
DEF_ASTRO_POS_4:
    WORD    0 ; linha 
    WORD    0 ; coluna

DEF_MOVE_ASTRO:     ; variável que indica se é para avançar o asteróide
    WORD    0
DEF_ASTRO_DIR_1:
    WORD    0
DEF_ASTRO_DIR_2:
    WORD    0
DEF_ASTRO_DIR_3:
    WORD    0
DEF_ASTRO_DIR_4:
    WORD    0
DEF_ASTRO_DELETE_1:
    WORD    0

DEF_ASTRO_DELETE_2:
    WORD    0
DEF_ASTRO_DELETE_3:
    WORD    0
DEF_ASTRO_DELETE_4:
    WORD    0
; **********************************************************************
 
DEF_SONDA:						; tabela que define o boneco (largura, altura, cor)
	WORD    LARGURA_SONDA	; largura da sonda
	WORD 	ALTURA_SONDA	; altura da sonda
	WORD	COR_PIXEL		; # # #   as cores podem ser diferentes de pixel para pixel

; ( linha, coluna, direção, flag, posição inicial )
DEF_SONDA_POS:				                        ; tabela  que define as posições da sonda
    WORD    0, 0, -1, 0, POS_INICIAL_SONDA_0_X      ; left sonda
    WORD    0, 0, 0, 0, POS_INICIAL_SONDA_1_X       ; middle sonda
    WORD    0, 0, 1, 0, POS_INICIAL_SONDA_2_X       ; right sonda

; **********************************************************************

ENERGY: 
    WORD    INIT_ENRG
; **********************************************************************

DEF_BLINK_COLORS:       ; 16 colors from an old html standard
    WORD    AQUA,BLACK,BLUE,FUCHSIA,GREY,GREEN,LIME,MAROON,NAVY,OLIVE,PURPLE,RED,SILVER,TEAL,WHITE,YELLOW
; **********************************************************************

DEF_SHIP:
    WORD    LARGURA_SHIP      ; LARGURA
    WORD    ALTURA_SHIP       ; ALTURA
    WORD    0,0,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,0,0
    WORD    0,RED,MUD_GREEN,MUD_GREEN,MUD_GREEN,MUD_GREEN,MUD_GREEN,MUD_GREEN,MUD_GREEN,MUD_GREEN,MUD_GREEN,MUD_GREEN,MUD_GREEN,RED,0
    WORD    RED,MUD_GREEN,MUD_GREEN,MUD_GREEN,0,0,0,0,0,0,0,MUD_GREEN,MUD_GREEN,MUD_GREEN,RED
    WORD    RED,MUD_GREEN,MUD_GREEN,MUD_GREEN,0,0,0,0,0,0,0,MUD_GREEN,MUD_GREEN,MUD_GREEN,RED
    WORD    RED,MUD_GREEN,MUD_GREEN,MUD_GREEN,MUD_GREEN,MUD_GREEN,MUD_GREEN,MUD_GREEN,MUD_GREEN,MUD_GREEN,MUD_GREEN,MUD_GREEN,MUD_GREEN,MUD_GREEN,RED

DEF_SHIP_LINE:
    WORD    27
DEF_SHIP_COLUMN:
    WORD    25
; **********************************************************************

DEF_PANEL:
    WORD    7
    WORD    2
    WORD    0,0,0,0,0,0,0
    WORD    0,0,0,0,0,0,0

DEF_PANEL_LINE:
    WORD    29
DEF_PANEL_COLUMN:
    WORD    29
; **********************************************************************
DEF_GAME_STATE:
    WORD    GAME_STATE_PLAYING

; *********************************************************************************
; * Código
; *********************************************************************************
PLACE   0                                   ; o código tem de começar em 0000H
setup:
	MOV     SP, SP_inicial		            ; inicializa SP para a palavra a seguir
						                    ; à última da pilha
    MOV     BTE, interrupts_table


    MOV     [APAGA_AVISO], R1	            ; apaga o aviso de nenhum cenário selecionado (o valor de R1 não é relevante)
    MOV     [APAGA_ECRÃ], R1	            ; apaga todos os pixels já desenhados (o valor de R1 não é relevante)
    MOV	    R1, 3			                ; cenário de inicio de jogo
    MOV     [SELECIONA_CENARIO_FRONTAL], R1	; seleciona o cenário de fundo
    MOV     R1, INIT_ENRG
    MOV     [ENERGY], R1
    CALL    display_decimal_energy

    CALL    wait_for_first_keypress         ; until the first key is pressed he game wont start

    MOV     [APAGA_CENARIO_FRONTAL], R1
    MOV	    R1, 0			                ; cenário de fundo número 0
    MOV     [SELECIONA_CENARIO_FUNDO], R1	; seleciona o cenário de fundo

    EI0                                     ; permite interrupções 0
    EI1                                     ; permite interrupções 1
    EI2                                     ; permite interrupções 2
    EI3                                     ; permite interrupções 3
    EI                                      ; permite interrupções geral

    CALL    create_asteroides
    CALL    draw_ship
    CALL    draw_blink_panel
	CALL	keyboard			            ; leitura às teclas

    MOV	    R11, N_SONDAS
loop_init_sonda_processes:
    SUB	    R11, 1
	CALL	move_sonda			
    CMP     R11, 0
    JNZ	    loop_init_sonda_processes

    
end_start:
    YIELD
    CALL move_asteroides
    JMP end_start

; *********************************************************************************
; * Astro
; *********************************************************************************

; **********************************************************************
; GENERATE_RAND - gera valor aleatório entre 0 e 2
;   Retorna: 
;           R10 - valores 0, 1, ou 2
; **********************************************************************
generate_rand:
    PUSH    R9

    MOV     R9, TEC_OUT
    MOVB    R10, [R9]
    MOV     R9, 00FFH
    AND     R10,R9
    MOV     R9, 3
    MOD     R10, R9

    POP     R9
    RET

; **********************************************************************
; CREATE_START_POS_DIR_PAIR - cria posição inicial e direção 
;                                   para os asteroides
;   Retorna: 
;           R2 - posição do asteroide
;           R10 - direção do asteroide
; **********************************************************************
create_start_pos_dir_pair:
    PUSH    R9

    CALL    generate_rand
left:
    CMP     R10, 0
    JNZ     right
    MOV     R2, 0
    MOV     R10, 1
    JMP     end_create_pair

right:
    CMP     R10, 1
    JNZ     middle
    MOV     R2, MAX_COLUNA
    SUB     R2, 2
    MOV     R10, -1
    JMP     end_create_pair

middle:
    ; select dir 
    MOV     R2, COLUNA_SONDA
    CALL    generate_rand
    ADD     R10, -1
end_create_pair:    
    POP     R9
    RET

; **********************************************************************
; CREATE_ASTEROIDES - Cria os asteroides
; **********************************************************************
create_asteroides:
    PUSH    R1
    PUSH    R3
    PUSH    R5

    MOV     R1, LAYER_ASTRO_1           ; layer do asteroide
    MOV     [CHANGE_LAYER], R1          ; muda o layer do asteroide

    MOV     R3, DEF_ASTRO_POS_1         ; obtém tabela de posições
    MOV     R5, DEF_ASTRO_DIR_1         ; obtém tabela de direção
    MOV     R4, DEF_ASTRO_1_IMAGE       ; obtém tabela do imagem
    CALL    create_asteroide


    MOV     R1, LAYER_ASTRO_2           ; layer do asteroide
    MOV     [CHANGE_LAYER], R1          ; muda o layer do asteroide

    MOV     R3, DEF_ASTRO_POS_2         ; obtém tabela de posições
    MOV     R5, DEF_ASTRO_DIR_2         ; obtém tabela de direção
    MOV     R4, DEF_ASTRO_2_IMAGE       ; obtém tabela do imagem
    CALL    create_asteroide

    MOV     R1, LAYER_ASTRO_3           ; layer do asteroide
    MOV     [CHANGE_LAYER], R1          ; muda o layer do asteroide

    MOV     R3, DEF_ASTRO_POS_3         ; obtém tabela de posições
    MOV     R5, DEF_ASTRO_DIR_3         ; obtém tabela de direção
    MOV     R4, DEF_ASTRO_3_IMAGE       ; obtém tabela do imagem
    CALL    create_asteroide

    MOV     R1, LAYER_ASTRO_4           ; layer do asteroide
    MOV     [CHANGE_LAYER], R1          ; muda o layer do asteroide

    MOV     R3, DEF_ASTRO_POS_4         ; obtém tabela de posições
    MOV     R5, DEF_ASTRO_DIR_4         ; obtém tabela de direção
    MOV     R4, DEF_ASTRO_4_IMAGE       ; obtém tabela do imagem
    CALL    create_asteroide

    ; mudar layer de escrita para o layer inicial
    MOV     R1, LAYER_SHIP              ; layer da nave
    MOV     [CHANGE_LAYER], R1          ; muda para o layer da nave

    POP     R5
    POP     R3
    POP     R1
    RET
; **********************************************************************
; CREATE_ASTEROIDE - Create o asteroide
; Argumentos:
;           R3 - tabela de posições 
;           R4 - tabela do asteroide
;           R5 - tabela da sua direção
; **********************************************************************
create_asteroide:
    PUSH    R1
    PUSH    R2
    PUSH    R3          ; astro position
    PUSH    R4          ; endereço de memória do boneco do astro em questão
    PUSH    R5          ; astro dir
    PUSH    R9
    PUSH    R10

                            ;set das posições do asteroid para o canto inferior esquerdo
    MOV     R1, 0 
    MOV     [R3], R1        ; set x = 0
    ADD     R3, 2           ; avancar para o endereço em memória do y
    
    CALL    create_start_pos_dir_pair
    MOV     [R3], R2                        ; set y = 0

    MOV     [R5], R10                       ; dir
    
    ; choose astro version
    MOV     R9, TEC_OUT
    MOVB    R10, [R9]
    MOV     R9, 00F0H
    AND     R10, R9
    SHR     R10, 4
    MOV     R9, 4
    MOD     R10, R9
    JZ      minerable

    MOV     R9, DEF_ASTRO
    MOV     [R4], R9
    JMP     draw

minerable:
    MOV     R9, DEF_ASTRO_MINERABLE
    MOV	    [R4], R9 ; load asteroid image

draw:
    MOV     R4, [R4]
    CALL    desenha_boneco

    POP     R10
    POP     R9
    POP     R5
    POP     R4
    pop     R3
    pop     R2
    pop     R1
    RET

; **********************************************************************
; MOVE_ASTEROIDES - Move os asteroides
; **********************************************************************
move_asteroides:
    PUSH    R1
    PUSH    R3
    PUSH    R4
    PUSH    R5
    PUSH    R8
    PUSH    R11

    MOV     R1, [DEF_MOVE_ASTRO]        ; obtém variável
    CMP     R1, 1                       ; compara se pode avançar os  o asteróides
    JNZ     end_move_astros             ; 
move_1:
    MOV     R1, LAYER_ASTRO_1           ; obtém layer do asteróide
    MOV     [CHANGE_LAYER], R1          ; altera layer para o do asteróide

    MOV     R3, DEF_ASTRO_POS_1         ; obtém tabela de posições
    MOV     R5, DEF_ASTRO_DIR_1         ; obtém tabela de direção
    MOV     R4, DEF_ASTRO_1_IMAGE       ; obtém tabela do imagem
    CALL    move_asteroide

    MOV     R1, [DEF_ASTRO_DELETE_1]    ; obtém variável
    CMP     R1, 1                       ; compara se pode apagar o asteróide
    JNZ     move_2                      ; se não for zero não apaga
    
    MOV     R7, DEF_ASTRO_POS_1         ; obtém tabela de posições
    MOV     R5, DEF_ASTRO_DIR_1         ; obtém tabela de direção
    MOV     R4, DEF_ASTRO_1_IMAGE       ; obtém tabela do imagem
    MOV     R11, DEF_ASTRO              ; obtém tabela do boneco do asteróide
    MOV     R8, [R4]
    CMP     R8, R11
    JZ      sound_1
    CALL    increase_energy
    JMP     delete_1
sound_1:
    MOV     R9, 1                       ; som com número 1
    MOV     [TOCA_SOM], R9              ; comando para tocar o som
delete_1: 
    CALL    delete_astro
    MOV     R1, 0
    MOV     [DEF_ASTRO_DELETE_1], R1    ; repõe a variável a 0 

move_2:
    MOV     R1, LAYER_ASTRO_2           ; obtém layer do asteróide
    MOV     [CHANGE_LAYER], R1          ; altera layer para o do asteróide

    MOV     R3, DEF_ASTRO_POS_2         ; obtém tabela de posições
    MOV     R5, DEF_ASTRO_DIR_2         ; obtém tabela de direção
    MOV     R4, DEF_ASTRO_2_IMAGE       ; obtém tabela do imagem
    CALL    move_asteroide

    MOV     R1, [DEF_ASTRO_DELETE_2]    ; obtém variável
    CMP     R1, 1                       ; compara se pode apagar o asteróide
    JNZ     move_3                      ; se não for zero não apaga
    
    MOV     R7, DEF_ASTRO_POS_2         ; obtém tabela de posições
    MOV     R5, DEF_ASTRO_DIR_2         ; obtém tabela de direção
    MOV     R4, DEF_ASTRO_2_IMAGE       ; obtém tabela do imagem
    MOV     R11, DEF_ASTRO              ; obtém tabela do boneco do asteróide
    MOV     R8, [R4]
    CMP     R8, R11
    JZ      sound_2
    CALL    increase_energy
    JMP     delete_2
sound_2:
    MOV     R9, 1                       ; som com número 1
    MOV     [TOCA_SOM], R9              ; comando para tocar o som
delete_2: 
    CALL    delete_astro
    MOV     R1, 0
    MOV     [DEF_ASTRO_DELETE_2], R1    ; repõe a variável a 0 

move_3:
    MOV     R1, LAYER_ASTRO_3           ; obtém layer do asteróide
    MOV     [CHANGE_LAYER], R1          ; altera layer para o do asteróide

    MOV     R3, DEF_ASTRO_POS_3         ; obtém tabela de posições
    MOV     R5, DEF_ASTRO_DIR_3         ; obtém tabela de direção
    MOV     R4, DEF_ASTRO_3_IMAGE       ; obtém tabela do imagem
    CALL    move_asteroide

    MOV     R1, [DEF_ASTRO_DELETE_3]    ; obtém variável
    CMP     R1, 1                       ; compara se pode apagar o asteróide
    JNZ     move_4                      ; se não for zero não apaga
    
    MOV     R7, DEF_ASTRO_POS_3         ; obtém tabela de posições
    MOV     R5, DEF_ASTRO_DIR_3         ; obtém tabela de direção
    MOV     R4, DEF_ASTRO_3_IMAGE       ; obtém tabela do imagem
    MOV     R11, DEF_ASTRO              ; obtém tabela do boneco do asteróide
    MOV     R8, [R4]                    ; obtém variável
    CMP     R8, R11
    JZ      sound_3                    ; 
    CALL    increase_energy
    JMP     delete_3
end_move_astros:
    POP     R11
    POP     R8
    POP     R5
    POP     R4
    POP     R3
    POP     R1
    RET
sound_3:
    MOV     R9, 1                       ; som com número 1
    MOV     [TOCA_SOM], R9              ; comando para tocar o som
delete_3: 
    CALL    delete_astro                
    MOV     R1, 0
    MOV     [DEF_ASTRO_DELETE_3], R1    ; repõe a variável a 0 
move_4:
    MOV     R1, LAYER_ASTRO_4           ; obtém layer do asteróide
    MOV     [CHANGE_LAYER], R1          ; altera layer para o do asteróide

    MOV     R3, DEF_ASTRO_POS_4         ; obtém tabela de posições
    MOV     R5, DEF_ASTRO_DIR_4         ; obtém tabela de direção
    MOV     R4, DEF_ASTRO_4_IMAGE       ; obtém tabela do imagem
    CALL    move_asteroide

    MOV     R1, 0
    MOV     R1, [DEF_ASTRO_DELETE_4]    ; obtém variável
    CMP     R1, 1                       ; compara se pode apagar o asteróide
    JNZ     not_delete_4
    
    MOV     R7, DEF_ASTRO_POS_4         ; obtém tabela de posições
    MOV     R5, DEF_ASTRO_DIR_4         ; obtém tabela de direção
    MOV     R4, DEF_ASTRO_4_IMAGE       ; obtém tabela do imagem
    MOV     R11, DEF_ASTRO              ; obtém tabela do boneco do asteróide
    MOV     R8, [R4]
    CMP     R8, R11
    JZ      sound_4
    CALL    increase_energy
    JMP     delete_4
sound_4:
    MOV     R9, 1                       ; som com número 1
    MOV     [TOCA_SOM], R9              ; comando para tocar o som
delete_4:
    CALL    delete_astro
    MOV     R1, 0
    MOV     [DEF_ASTRO_DELETE_4], R1    ; repõe a variável a 0 

not_delete_4:
    ;mudar layer de escrita para o layer inicial
    MOV     R1, LAYER_SHIP              ; obtém tabela layer_ship
    MOV     [CHANGE_LAYER], R1          ; repõe o layer original
    
    MOV     R1, 0
    MOV     [DEF_MOVE_ASTRO], R1        ; repõe a 0
    JMP     end_move_astros

; **********************************************************************
; MOVE_ASTEROIDE - Move o asteroide
; Argumentos:
;           R3 - tabela de posições 
;           R4 - tabela do asteroide
;           R5 - tabela da sua direção
; **********************************************************************
move_asteroide:

    PUSH    R1 
    PUSH    R2 
    PUSH    R3
    PUSH    R4 
    PUSH    R5
    PUSH    R6
    PUSH    R7
    PUSH    R8
    PUSH    R9

    MOV     R8, R4

    MOV     R7, R3
    CALL    load_astro_position

    MOV     R4, [R8]
    CALL	apaga_boneco
    
    ; advance asteroid diagonally 
    MOV     R3, R7
    MOV     R1, [R3]	        ; obtém linha	
    ADD	    R1, 1		        
    MOV     [R3], R1            ; guarda linha em memória

    ADD     R3, 2               ; read next word
    MOV     R2, [R3]            ; obtém coluna
    MOV     R6,  [R5]
    ADD 	R2, R6              
    MOV     [R3], R2            ; guarda coluna em memória
    CALL    check_astro_bounds  ; 
    CMP     R9, 0               ; check da flag
    JNZ     destroy_astro

    MOV     R4, [R8]
	CALL    desenha_boneco

    CALL    check_collision_ship

    JMP     end_move_astro

destroy_astro:
    MOV     R4, R8
    CALL    delete_astro

end_move_astro:
    POP     R9
    POP     R8
    POP     R7
    POP     R6
    POP     R5
    POP     R4
    POP     R3
    POP     R2
    POP     R1
    RET

; **********************************************************************
; LOAD_ASTRO_POSITION - Carrega a posição do asteroide (linha e coluna)
; Argumentos:
;           R7 - tabela de posições do asteroide
; Retorno: 
;           R1 - linha do asteroide
;           R2 - coluna do asteroide
; **********************************************************************
load_astro_position:
    MOV     R3, R7
    MOV     R1, [R3]            ; linha do asteroide
    ADD     R3, 2
    MOV     R2, [R3]            ; coluna do asteroide
    RET

check_astro_bounds:
    ; verifica se o asteroide saiu do mapa
    ; Returns on R9
    PUSH    R1
    PUSH    R2
    PUSH    R3
    PUSH    R7

    ; r2 = y
    CALL    load_astro_position
    MOV     R9, MAX_LINE
    CMP     R1, R9 ; verifica se y > do que o número de linhas
    JNZ     not_out
    MOV     R9, 1
    JMP     end_check_bounds
not_out:
    MOV     R9, 0
end_check_bounds:
    POP     R7
    POP     R3
    POP     R2
    POP     R1  
    RET

; **********************************************************************
; DELETE_ASTRO - Apaga o asteroide 
;			    com a forma e cor definidas na tabela indicada.
; Argumentos:   
;               R4 - tabela que define o boneco
; **********************************************************************
delete_astro:
    PUSH    R1
    PUSH    R2
    PUSH    R3
    PUSH    R4
    PUSH    R8

    MOV     R8, R4
    
    CALL    load_astro_position
    MOV	    R4, [R8]
    CALL	apaga_boneco
    MOV     R3, R7
    MOV     R4, R8
    CALL    create_asteroide

    POP     R8
    POP     R4
    POP     R3
    POP     R2
    POP     R1
    RET


; **********************************************************************
; DESENHA_BONECO - Desenha um boneco na linha e coluna indicadas
;			    com a forma e cor definidas na tabela indicada.
; Argumentos:   R1 - linha
;               R2 - coluna
;               R4 - tabela que define o boneco
;
; **********************************************************************
desenha_boneco:
    PUSH    R1
	PUSH	R2
	PUSH	R3
	PUSH	R4
	PUSH	R5
    PUSH    R6
	PUSH    R7
    PUSH    R8

    MOV	    R5, [R4]		        ; obtém a largura do boneco
    MOV     R7, [R4]                ; guarda largura

    ADD     R4, 2                   ; endereço da cor do 1º pixel (2 porque a largura é uma word)
    
    MOV     R6, [R4]                ; obtém a altura
    MOV     R8, R2                  ; guarda a linha em R8

	ADD	    R4, 2			        ; endereço da cor do 1º pixel (2 porque a altura é uma word)

desenha_pixels:       		        ; desenha os pixels do boneco a partir da tabela
	MOV	    R3, [R4]			    ; obtém a cor do próximo pixel do boneco
	CALL	escreve_pixel		    ; escreve cada pixel do boneco
	ADD	    R4, 2			        ; endereço da cor do próximo pixel (2 porque cada cor de pixel é uma word)
    ADD     R2, 1                   ; próxima coluna
    SUB     R5, 1			        ; menos uma coluna para tratar
    JNZ     desenha_pixels          ; continua até percorrer toda a largura do objeto
  
    MOV     R5, R7                  ; põe altura em R5
    ADD     R1, 1                   ; incrementa a linha
    mov     R2, R8                  ; coluna em R2
    SUB     R6, 1                   ; decrementa a altura
    JNZ     desenha_pixels          ; se a altura for superior a 0 continua o loop
    

    POP     R8
    POP     R7
    POP     R6
    POP	    R5
	POP	    R4
	POP	    R3
	POP	    R2
    POP     R1
	RET

; **********************************************************************
; APAGA_BONECO - Apaga um boneco na linha e coluna indicadas
;			  com a forma definida na tabela indicada.
; Argumentos:   R1 - linha
;               R2 - coluna
;               R4 - tabela que define o boneco
;
; **********************************************************************
apaga_boneco:
    PUSH    R1
	PUSH	R2
	PUSH	R3
	PUSH	R4
	PUSH	R5
    PUSH    R6
	PUSH    R7
    PUSH    R8


    MOV	    R5, [R4]			; obtém a largura do boneco
    MOV     R7, [R4]            ; guarda largura

    ADD     R4, 2               ;  endereço da cor do 1º pixel (2 porque a largura é uma word)
    
    MOV     R6, [R4]            ; obtém a altura
    MOV     R8, R2

	ADD	    R4, 2			    ; endereço da cor do 1º pixel (2 porque a altura é uma word)

apaga_pixels:       		        ; desenha os pixels do boneco a partir da tabela
	MOV	    R3, 0			        ; obtém a cor do próximo pixel do boneco
	CALL	escreve_pixel		    ; escreve cada pixel do boneco
	ADD	    R4, 2			        ; endereço da cor do próximo pixel (2 porque cada cor de pixel é uma word)
    ADD     R2, 1                   ; próxima coluna
    SUB     R5, 1			        ; menos uma coluna para tratar
    JNZ     apaga_pixels            ; continua até percorrer toda a largura do objeto
  
    MOV     R5, R7                  ; largura em R5
    ADD     R1, 1                   ; incrementa a linha
    MOV     R2, R8                  ; coluna em R2
    SUB     R6, 1                   ; decrementa a altura 
    JNZ     apaga_pixels            ; se a altura não for 0 repete o ciclo
    

    POP     R8
    POP     R7
    POP     R6
    POP	    R5
	POP	    R4
	POP	    R3
	POP	    R2
    POP     R1
	RET
    ; **********************************************************************
; CHECK_COLLISION - Testa se a existe colisão entre asteroide e sonda
; Argumentos:	
;			R1 - linha da sonda
;           R2 - coluna da sonda
;           R5 - linha do asteroide
;           R6 - coluna do asteroide
;		
; Retorna: 	R9 - flag que indica a colisão
;            ( 0 se não houve e 1 se houve )
; **********************************************************************
check_collision:
    PUSH    R10
    MOV     R9, 0

check_upper_bound:
    CMP     R1, R5
    JLT     end_check_collision
check_lower_bound:
    ADD     R5, ALTURA_ASTRO
    CMP     R1, R5    
    JGT     end_check_collision
check_left_bound:
    CMP     R2, R6
    JLT     end_check_collision
check_right_bound:
    ADD     R6, LARGURA_ASTRO
    CMP     R2, R6    
    JGT     end_check_collision
    MOV     R9, 1
end_check_collision:
    POP     R10    
    RET
; **********************************************************************
; CHECK_COLLISIONS - Testa se a existe colisão nos asteroides
; Argumentos:	
;			R1 - linha da sonda
;           R2 - coluna da sonda
;		
; Retorna: 	R8 - flag que indica a colisão
;           ( 0 se não houve e 1 se houve )
; **********************************************************************
check_collisions:
    PUSH    R1
    PUSH    R2
    PUSH    R3
    PUSH    R5
    PUSH    R6
    PUSH    R7
    PUSH    R9

collision_1:
    MOV     R3, DEF_ASTRO_POS_1             ; obtém tabela de posições
    MOV     R5, [R3]                        ; obtém linha
    ADD     R3, 2               
    MOV     R6, [R3]                        ; obtém coluna
    CALL    check_collision
    CMP     R9, 1                           ; compara se houve colisão
    JNZ     collision_2

    MOV     R9, 1
    MOV     [DEF_ASTRO_DELETE_1], R9

    MOV     R6, R8 
    MOV     R8, 1
    JMP     collision_end
collision_2:
    MOV     R3, DEF_ASTRO_POS_2             ; obtém tabela de posições
    MOV     R5, [R3]                        ; obtém linha
    ADD     R3, 2
    MOV     R6, [R3]                        ; obtém coluna
    CALL    check_collision 
    CMP     R9, 1                           ; compara se houve colisão
    JNZ     collision_3             

    MOV     R9, 1
    MOV     [DEF_ASTRO_DELETE_2], R9

    MOV     R6, R8 
    MOV     R8, 1
    JMP     collision_end
collision_3:
    MOV     R3, DEF_ASTRO_POS_3             ; obtém tabela de posições
    MOV     R5, [R3]                        ; obtém linha
    ADD     R3, 2   
    MOV     R6, [R3]                        ; obtém coluna
    CALL    check_collision             
    CMP     R9, 1                           ; compara se houve colisão
    JNZ     collision_4
    ; call delete_astro
    MOV     R9, 1
    MOV     [DEF_ASTRO_DELETE_3], R9
    
    MOV     R6, R8 
    MOV     R8, 1
    JMP     collision_end
collision_4:
    MOV     R3, DEF_ASTRO_POS_4             ; obtém tabela de posições
    MOV     R5, [R3]                        ; obtém linha
    ADD     R3, 2
    MOV     R6, [R3]                        ; obtém coluna
    CALL    check_collision
    CMP     R9, 1
    JNZ     collision_end
    ; call delete_astro
    CMP     R9, 1                           ; compara se houve colisão
    MOV     [DEF_ASTRO_DELETE_4], R9

    MOV     R6, R8 
    MOV     R8, 1
collision_end:
    POP     R9
    POP     R7
    POP     R6
    POP     R5
    POP     R3
    POP     R2
    POP     R1
    RET
; **********************************************************************
; MOVE_SONDA - Desenha a sonda na linha e coluna indicadas
;                com a forma e cor definidas na tabela indicada.
; Argumentos:
;        R11- num da sonda [0;2]
; Assinatura dos registos:   
;               R1 - linha
;               R2 - coluna
;               R4 - tabela que define a sonda
;               R6 - tabela de posições da sonda
;               R9 - flag da sonda
; **********************************************************************
PROCESS SP_sondas
move_sonda:
    MOV	    R1, TAMANHO_PILHA
    MUL	    R1, R11
    SUB	    SP, R1
wait_for_next_shot:
    MOV     R8, [evento_shoot_sonda]
    CMP     R11,R8
    JNZ     wait_for_next_shot
    
    MOV     R9, 0                       ; som com número 0
    MOV     [TOCA_SOM], R9              ; comando para tocar o som

    MOV     R10, R11
    SHL     R10, 1

    MOV     R3, R11
    MOV     R6, DEF_SONDA_POS       ; endereço da tabela que define as posições das sondas
    MOV     R8 , BYTES_DEF_SONDA_POS
    MUL     R3, R8
    ADD     R6, R3 
    MOV     R5, SONDA_REACH         ; obtém a distância limite da sonda
    MOV     R4, DEF_SONDA           ; endereço da tabela que define o boneco

    MOV     R1, [ENERGY]
    SUB     R1, 5
    MOV     [ENERGY], R1
    CALL    display_decimal_energy 
check_empty:
    MOV     R1, [R6]
    MOV     R2, [R6+2]                ; obtém a coluna da sonda
    MOV     R3, [R6+6]                ;the 4th word tells wether or not the sonda is already active
    CMP     R3, 0
    JNZ     desenha_sonda
    CALL    init_sonda
    CALL    desenha_boneco
    
    JMP     check_limit
desenha_sonda:
    MOV     R9, R1
    MOV     R10, R2
    ADD     R1, 1               ; soma 1 para apagar o pixel abaixo
    MOV     R8, [R6+4];         ;the velocity vector in x is the 3rd word in sonda definition
    SUB     R2, R8
    CALL    apaga_boneco
    MOV     R1, R9              ; volta a por o valor correto para desenhar o pixel da sonda atual
    MOV     R2, R10
    CALL    desenha_boneco


    ; R6 linha R6+2 coluna
    MOV     R8, R6
    CALL    check_collisions
    CMP     R8, 1
    JNZ     desenha_sonda_collisions_checked
    JMP     delete

desenha_sonda_collisions_checked:
    MOV     R8, [evento_sondas]     ; LOCK
check_limit:
    CALL    testa_limites_sonda     ; testa se já chegou ao limite
    CMP     R7, 0                  
    JZ      delete                   ; apaga pixel se chegar ao final
mov_sonda:
    ADD     R1, R7                  ; soma a linha
    ADD     R2, R8                  ; soma a coluna
    MOV     [R6], R1                ; guarda em memória a linha atual
    ADD     R6, 2
    MOV     [R6], R2
    SUB     R6, 2
    JMP     desenha_sonda
delete:
    CALL    apaga_sonda
end_sonda:
    JMP     wait_for_next_shot

; **********************************************************************
; APAGA_SONDA - Apaga a sonda na linha e colunas indicadas
;                com a forma e cor definidas na tabela indicada.
; Argumentos:
;        R1 - linha
;        R2 - Coluna
;        R4 - tabela que define a sonda
;        R6 - tabela de posições da sonda
; **********************************************************************
apaga_sonda:
    PUSH    R1
    PUSH    R2
    PUSH    R4
    PUSH    R6

    CALL    apaga_boneco        ; apaga a sonda com a linha e coluna        
    MOV     R1,0                ; 
    MOV     [R6+6], R1          ; altera a flag da sonda

    POP     R6
    POP     R4
    POP     R2
    POP     R1
    RET
; **********************************************************************
; INIT_SONDA - inicializa a sonda
; Argumentos:
;           R6 - endereço das posições da sonda a inicializar
; Retorna:  R1 (linha) e R2 (coluna) com os valores da sonda
; **********************************************************************
init_sonda:
    PUSH    R6
    PUSH    R7
init_loop:
    MOV     R1, POS_INICIAL_SONDAS_Y
    MOV     R2, [R6+8]      ;fourth word of the sonda's properties is its starting column
    MOV     R7,1
    MOV     [R6+6], R7
    MOV     [R6+2],R1
    MOV     [R6],R2
init_sonda_end:
    POP     R7
    POP     R6
    RET
; **********************************************************************
; TESTA_LIMITES_SONDA - Testa se a sonda chegou aos limites do ecrã e nesse caso
;			   impede o movimento (força R7 a 0)
; Argumentos:	
;			R1 - linha em que o objeto está
;           R5 - limite da distância da sonda
;			R7 - sentido de movimento da sonda (valor a somar à linha
;				em cada movimento: +1 para a baixo, -1 para a cima)
;           R8 - sentido de movimento da sonda (valor a somar à coluna
;				em cada movimento: +1 para a direita, -1 para a esquerda)
;
; Retorna: 	R7 - 0 se já tiver chegado ao limite ou -1 para subir
;           R8 - 0, -1, +1 (middle, left and right sonda)
; **********************************************************************
testa_limites_sonda:
	PUSH 	R1
    PUSH    R2
    PUSH    R4

limite_init:
    MOV     R4, LINHA_SONDA
    SUB     R4, R1
	CMP 	R4, R5
	JZ		reset_sonda_limite
	MOV		R7, -1			            ; passa a deslocar-se para a cima
    MOV     R8, [R6+4];                 ; the velocity vector in x is the 3rd word in sonda definition
    JMP     end_teste_limite
reset_sonda_limite:
    MOV     R7, 0
end_teste_limite:
    POP     R4
    POP     R2
	POP 	R1
	RET
; **********************************************************************
; Desenha ship
;   Assinatura dos registos:
;       R1 Linha do pixel superior esquerdo da nave
;       R2 coluna do pixel superior esquerdo da nave
;       R4 Primeiro endereço da tabela de words que defina a nava
; **********************************************************************
draw_ship:
    PUSH    R1
    PUSH    R2
    PUSH    R4

    ;args for desenha_boneco
    MOV     R1, [DEF_SHIP_LINE]
    MOV     R2, [DEF_SHIP_COLUMN]
    MOV     R4, DEF_SHIP
    CALL    desenha_boneco

    POP     R4
    POP     R2
    POP     R1 
    RET

; **********************************************************************
; CHECK_COLLISION_SHIP - Testa se houve colisão com a nave
; Argumentos:	
;			R1 - linha da sonda
;           R2 - coluna da sonda
;		
; Retorna: 	R8 - flag que indica a colisão
;           ( 0 se não houve e 1 se houve )
; **********************************************************************
check_collision_ship:
    PUSH    R1
    PUSH    R2
    PUSH    R3
    PUSH    R5

    MOV     R5, LARGURA_SHIP
    ; se linha inferior astro superior a linha nave
    MOV     R3, [DEF_SHIP_LINE]
    ADD     R1, ALTURA_ASTRO
    CMP     R1, R3
    JLT     end_collision_ship

    ; se ship_left <= asteroide_left <= ship_right
    MOV     R3, [DEF_SHIP_COLUMN]
    CMP     R2, R3
    JLT     check_other_vertex
    ADD     R3, R5
    CMP     R2, R3
    JGT     check_other_vertex

    CALL    end_game_collision

check_other_vertex:
    ; se ship_left <= asteroide_left <= ship_right
    MOV     R3, [DEF_SHIP_COLUMN]
    ADD     R2, LARGURA_ASTRO
    CMP     R2, R3
    JLT     check_other_vertex
    ADD     R3, R5
    CMP     R2, R3
    JGT     check_other_vertex
end_collision_ship:
    POP     R5
    POP     R3
    POP     R2
    POP     R1
    RET

; **********************************************************************
; Desenha painel que pisca
;   Assinatura dos registos:
;       R1 Linha do pixel superior esquerdo da nave
;       R2 coluna do pixel superior esquerdo da nave
;       R4 Primeiro endereço da tabela de words que defina a nava
; **********************************************************************
PROCESS SP_blink_panel
draw_blink_panel:
    MOV     R4, DEF_PANEL            ; args for randomize_colors
draw_blink_panel_cycle:
    CALL    randomize_panel_colors
    
    MOV     R1, [DEF_PANEL_LINE]        ; tabela que define a linha do painel
    MOV     R2, [DEF_PANEL_COLUMN]      ; tabela que define a coluna do painel
    CALL    desenha_boneco
    MOV     R11, [evento_blink_panel]   ; LOCK
    JMP     draw_blink_panel_cycle
draw_blink_panel_cycle_end:

; **********************************************************************
; Seleciona cores pseudo-aleatoriamente da tabela DEF_BLINK_COLORS
; para piscar o painel da nave
;   Assinatura dos registos:
;       R4 Primeiro endereço da tabela de words que define o painel
;       R8 Para acesso ao pixel n da tabela de pixeis do painel
;       R9 Inicialmente para a altura do painel, depois para uma mascara
;       R10 Primeiro endereço da tabela de words DEF_BLINK_COLORS, de 16 cores
;       R11 Para receber valores pseudo aleatorios do teclado
; **********************************************************************
randomize_panel_colors:
    PUSH    R4
    PUSH    R5
    PUSH    R8
    PUSH    R9
    PUSH    R10
    PUSH    R11

    MOV     R8, [R4]
    MOV     R9, [R4+2]
    MUL     R8, R9
    SUB     R8, 1
    SHL     R8, 1                       ; R8 WILL BE USED TO SELECT THE DEF_PANEL_CELLS_COLOR WORDS FROM LAST TO FIRST
    ADD     R4, 4
    MOV     R9, 00F0H                   ; mask to select the 2nd nibble
    MOV     R10, DEF_BLINK_COLORS
    MOV     R5, TEC_OUT
randomize_panel_colors_loop:
    MOVB    R11, [R5]
    AND     R11, R9
    SHR     R11, 3                      ; SHR 4 TIMES TO SELECT RANGE [0,15], SHL 1 TIME TO MULTIPLY BY 2 TO SELECT WORDS IN RANGE [0,15] RATHER THAN BYTES   
    ADD     R11, R10
    MOV     R7, [R11]
    MOV     [R4+R8], R7
    SUB     R8, 2
    
    JNN     randomize_panel_colors_loop
    POP     R11
    POP     R10
    POP     R9
    POP     R8
    POP     R5
    POP     R4
    RET
; **********************************************************************
; DISPLAY_DECIMAL_ENERGY - mostra energia em decimal
; **********************************************************************
display_decimal_energy:
    PUSH    R0
    PUSH    R1
    PUSH    R2
    PUSH    R3
    PUSH    R4

    MOV     R0, DISPLAY         ; obtém variável DISPLAY
    MOV     R1, [ENERGY]        ; obtém variável da energia atual guardada em memória

    MOV     R2, 100d            ; energia inicial
    MOV     R3, R1              ; 

    DIV     R3, R2  
    MOV     R4, R3
    SHL     R4, 8

    MOV     R3, R1
    MOD     R3, R2
    MOV     R2, 10d
    DIV     R3, R2
    SHL     R3, 4
    OR      R4, R3

    MOV     R3, R1
    MOV     R2, 100d
    MOD     R3, R2
    MOV     R2, 10d
    MOD     R3, R2
    OR      R4, R3
  
    MOV     [R0], R4        ; põe a energia no display

    POP     R4
    POP     R3
    POP     R2
    POP     R1
    POP     R0
    RET

; **********************************************************************
; INCREASE_ENEGRY - Aumenta a energia
; **********************************************************************
increase_energy:
    PUSH    R0
    PUSH    R1
    PUSH    R2
    PUSH    R9

    MOV     R9, 2                   ; som com número 2
    MOV     [TOCA_SOM], R9          ; comando para tocar o som
    
    MOV     R0, DISPLAY
    MOV     R1, [ENERGY]
    MOV     R2,MINE_ASTRO_ENERG_GAIN
    ADD     R1, R2
    MOV     R2, MAX_VAL_3_DIG
    CMP     R2, R1
    JN      inc_enrg_display_end
    MOV     [ENERGY], R1
    MOV     [R0], R1
inc_enrg_display_end:
    CALL    display_decimal_energy
    POP     R9
    POP     R2
    POP     R1
    POP     R0
    RET

wait_for_first_keypress:
    PUSH    R1
    PUSH    R2
    PUSH    R3
    PUSH    R6
    PUSH    R7
    PUSH    R8

    MOV     R1, LAST_KB_LINE        ; linha atual do teclado
    MOV     R2, 0000H               ; output do teclado
    MOV     R3, 0000H               ; tecla premida [0,F]; 
    MOV     R6, TEC_IN              ; endereços do input/output do teclado
    MOV     R7, 0000H               ; contador
    MOV     R8, 000FH;              ; masks

wait_for_first_keypress_wait_keypress:
    MOV     R6, TEC_IN          ; 
    MOVB    [R6], R1            ;
    MOV     R6, TEC_OUT         ;
    MOVB    R2, [R6]            ;
    AND     R2, R8              ; dispose from trash bits sent by the kb
    JZ      wait_for_first_keypress_next_kb_line        ; AND will set Z to one if all bit in R2 after the operation are zero
    JMP     wait_for_first_keypress_END
wait_for_first_keypress_next_kb_line:
    CMP     R1, 0001H           ; check if the first line has been reached
    JNZ     wait_for_first_keypress_nl_kb_next          ;
wait_for_first_keypress_nl_kb_reset:
    MOV     R1, LAST_KB_LINE    ;
    JMP     wait_for_first_keypress_wait_keypress
wait_for_first_keypress_nl_kb_next:
    SHR     R1, 1               ;
    JMP     wait_for_first_keypress_wait_keypress

wait_for_first_keypress_END:
    POP     R8
    POP     R7
    POP     R6
    POP     R3
    POP     R2
    POP     R1
    RET
; **********************************************************************
; keyboard - Varre o keyboard para detetar input do utilizador, e chama
;            as rotinas adequadas. Depois espera pelo proximo input.
; Assinatura dos registos:   
;         R1, LAST_KB_LINE   - linha atual do teclado       
;         R2, 0000H          - output do teclado
;         R3, 0000H          - valor em hexa da tecla premida [0,F]
;         R4, 0000H          - para verificar se a tecla premida corresponde a uma tecla de comando
;         R5, 0000H          - guarda r3 para aguardar que a tecla deixe de ser premida
;         R6, TEC_IN         - endereços do input/output do teclado
;         R7, 0000H          - contador
;         R8, 000FH;         - mascara para selecionar o primeiro nibble
; **********************************************************************
PROCESS SP_teclado
keyboard:
    MOV     R1, LAST_KB_LINE        ; linha atual do teclado
    MOV     R2, 0000H               ; output do teclado
    MOV     R3, 0000H               ; tecla premida [0,F]; 
    MOV     R6, TEC_IN              ; endereços do input/output do teclado
    MOV     R7, 0000H               ; contador
    MOV     R8, 000FH;              ; masks

wait_keypress:
    WAIT
    MOV     R6, TEC_IN          ; 
    MOVB    [R6], R1            ;
    MOV     R6, TEC_OUT         ;
    MOVB    R2, [R6]            ;
    AND     R2, R8              ; dispose from trash bits sent by the kb
    JZ      next_kb_line        ; AND will set Z to one if all bit in R2 after the operation are zero
    MOV     R3,R1
    JMP     key_pressed

next_kb_line:
    CMP     R1, 0001H           ; check if the first line has been reached
    JNZ     nl_kb_next          ;
nl_kb_reset:
    MOV     R1, LAST_KB_LINE    ;
    JMP     wait_keypress
nl_kb_next:
    SHR     R1, 1               ;
    JMP     wait_keypress
    
key_pressed:            ; wait until the key is released to execute the keypress
    YIELD                   ;não pode ficar em espera pois há uma tecla premida
    MOVB    R5, [R6]
    MOV     R8, 000FH       
    AND     R5, R8          ; dispose from trash bits sent by the kb
    CMP     R5, 0           ;
    JNZ     key_pressed
count_kb_rows:
    CMP     R3, 0001H
    JZ      count_kb_rows_end
    ADD     R7, 1d
    SHR     R3, 1
    JMP     count_kb_rows   
count_kb_rows_end:

    MOV     R3, R7                  ;
    SHL     R3, 2                   ; each row has 4 keys; shl 2 same as MUL 4
    MOV     R7, 0000H               ;
count_kb_columns:
    CMP     R2, 0001H
    JZ      count_kb_columns_end
    ADD     R7, 1d
    SHR     R2, 1
    JMP     count_kb_columns;
count_kb_columns_end:
    ADD     R3, R7
    
check_if_playing:
    MOV     R1, [DEF_GAME_STATE]        ; obtém o estado do jogo
    CMP     R1, GAME_STATE_PLAYING      ; compara a ver se o jogo está em pausa
    JZ      pause_game
unpause_game:  
    MOV     R1, GAME_STATE_PLAYING      ; obtém variável de estado
    MOV     [DEF_GAME_STATE], R1        ; guarda em memória
    EI
    JMP     keypress_ran

pause_game:
    MOV     R4, GAME_PAUSE_KEY          ; obtém variável de estado
    CMP     R3, R4  
    JNZ     shoot_sonda_0               ; salta para a próxima sonda
    DI
    MOV     R1, GAME_STATE_PAUSED       ; obtém variável de estado
    MOV     [DEF_GAME_STATE], R1        ; guarda em memória
    CALL    wait_for_first_keypress

shoot_sonda_0:
    MOV     R4, KEY_SHOOT_0             ; obtém variável key_shoot_0
    CMP     R3, R4
    JNZ     shoot_sonda_1               ; salta para a próxima sonda
    MOV     R8, 0
    MOV     [evento_shoot_sonda], R8    ; unlock evento shoot_sonda
    JMP     keypress_ran

shoot_sonda_1:
    MOV     R4, KEY_SHOOT_1             ; obtém variável key_shoot_0
    CMP     R3, R4
    JNZ     shoot_sonda_2               ; salta para a próxima sonda
    MOV     R8, 1
    MOV     [evento_shoot_sonda], R8    ; unlock evento shoot_sonda
    JMP     keypress_ran

shoot_sonda_2:
    MOV     R4, KEY_SHOOT_2             ; obtém variável key_shoot_0
    CMP     R3, R4
    JNZ     keypress_ran
    MOV     R8, 2
    MOV     [evento_shoot_sonda], R8    ; unlock evento shoot_sonda
    JMP     keypress_ran

keypress_ran:
    MOV     R1, LAST_KB_LINE        ; linha atual do teclado
    MOV     R2, 0000H               ; output do teclado
    MOV     R3, 0000H               ; tecla premida [0,F]; 
    MOV     R6, TEC_IN              ; endereços do input/output do teclado
    MOV     R7, 0000H               ; contador
    MOV     R8, 000FH;              ; masks
    JMP     wait_keypress

; **********************************************************************
; END_GAME_COLLISION - Fim do jogo
; **********************************************************************
end_game_collision:
    DI
    MOV     R9, 3                           ; som com número 3
    MOV     [TOCA_SOM], R9                  ; comando para tocar o som
    MOV     [APAGA_ECRÃ], R1	            ; apaga todos os pixels já desenhados (o valor de R1 não é relevante)
	MOV	    R1, 1			                ; cenário de fundo número 2
    MOV     [SELECIONA_CENARIO_FRONTAL], R1	; seleciona o cenário de fundo
end_game_collision_loop:
    JMP     end_game_collision_loop

end_game_no_energy:
    DI
    MOV     [APAGA_ECRÃ], R1	            ; apaga todos os pixels já desenhados (o valor de R1 não é relevante)
	MOV	    R1, 2			                ; cenário de fundo número 2
    MOV     [SELECIONA_CENARIO_FRONTAL], R1	; seleciona o cenário de fundo
end_game_no_energy_loop:
    JMP     end_game_no_energy_loop
; **********************************************************************
; ESCREVE_PIXEL - Escreve um pixel na linha e coluna indicadas.
; Argumentos:   R1 - linha
;               R2 - coluna
;               R3 - cor do pixel (em formato ARGB de 16 bits)
;
; **********************************************************************
escreve_pixel:
	MOV     [DEFINE_LINHA], R1		; seleciona a linha
	MOV     [DEFINE_COLUNA], R2		; seleciona a coluna
	MOV     [DEFINE_PIXEL], R3		; altera a cor do pixel na linha e coluna já selecionadas
	RET
; **********************************************************************
; TESTA_LIMITES - Testa se o boneco chegou aos limites do ecrã e nesse caso
;			   impede o movimento (força R7 a 0)
; Argumentos:	R2 - coluna em que o objeto está
;			R6 - largura do boneco
;			R7 - sentido de movimento do boneco (valor a somar à coluna
;				em cada movimento: +1 para a direita, -1 para a esquerda)
;
; Retorna: 	R7 - 0 se já tiver chegado ao limite, inalterado caso contrário	
; **********************************************************************
testa_limites:
	PUSH	R5
	PUSH	R6
testa_limite_esquerdo:		        ; vê se o boneco chegou ao limite esquerdo
	MOV	    R5, MIN_COLUNA
	CMP	    R2, R5
	JGT	    testa_limite_direito
	CMP	    R7, 0			        ; passa a deslocar-se para a direita
	JGE	    sai_testa_limites
	JMP	    impede_movimento	    ; entre limites. Mantém o valor do R7
testa_limite_direito:		        ; vê se o boneco chegou ao limite direito
	ADD	    R6, R2			        ; posição a seguir ao extremo direito do boneco
	MOV	    R5, MAX_COLUNA
	CMP	    R6, R5
	JLE	    sai_testa_limites	    ; entre limites. Mantém o valor do R7
	CMP	    R7, 0			        ; passa a deslocar-se para a direita
	JGT	    impede_movimento     
	JMP	    sai_testa_limites
impede_movimento:
	MOV	    R7, 0			        ; impede o movimento, forçando R7 a 0
sai_testa_limites:	
	POP	    R6
	POP	    R5
	RET


; **********************************************************************
; Rotinas de interrupção 
; **********************************************************************

; **********************************************************************
; ROT_INT_0 - 	Rotina de atendimento da interrupção 0
;			Assinala o evento na componente 0 da variável
; **********************************************************************
rot_int_0:
    PUSH    R1
    MOV     R1, 1
    MOV     [DEF_MOVE_ASTRO], R1
    POP     R1
    RFE

; **********************************************************************
; ROT_INT_1_sonda - 	Rotina de atendimento da interrupção 1
; **********************************************************************
rot_int_1_sonda:
    MOV     [evento_sondas], R0             ;desbloqueia processo sonda
    RFE
; **********************************************************************
; ROT_INT_2_dec_energy - 	Rotina de atendimento da interrupção 2
;		    Assinala o evento na componente 2 da variável
; **********************************************************************
rot_int_2_dec_energy:
    PUSH    R1
dec_enrg_display:
    MOV     R1, [ENERGY]
    SUB     R1, ENERG_VAR
    JZ      no_energy_left
    MOV     [ENERGY], R1
    CALL    display_decimal_energy
dec_enrg_display_end:
    POP     R1
    RFE
no_energy_left:
    CALL      end_game_no_energy            ; o valor minimo é zero
; **********************************************************************
; ROT_INT_3_panel - 	Rotina de atendimento da interrupção 3
;			Assinala o evento na componente 3 da variável
; **********************************************************************
rot_int_3_panel:
    MOV     [evento_blink_panel], R0        ; desbloqueia processo blink_panel
    RFE 
