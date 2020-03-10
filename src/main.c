/**
 * 5o Semestre - Engenharia da Computacao
 * APS 1 - Musical
 */

/************************************************************************/
/* includes                                                             */
/************************************************************************/

#include "asf.h"

/************************************************************************/
/* defines                                                              */
/************************************************************************/

// Configuracoes do Led
#define LED_PIO           PIOC                 // periferico que controla o LED
#define LED_PIO_ID        12                  // ID do periférico PIOC (controla LED)
#define LED_PIO_IDX       8                    // ID do LED no PIO
#define LED_PIO_IDX_MASK  (1 << LED_PIO_IDX)   // Mascara para CONTROLARMOS o LED

// Configuracoes do Buzzer
#define BUZ_PIO           PIOC                 // periferico que controla o Buzzer
#define BUZ_PIO_ID        ID_PIOC                  // ID do periférico PIOC (controla Buzzer)
#define BUZ_PIO_IDX       13                    // ID do Buzzer no PIO
#define BUZ_PIO_IDX_MASK  (1u << BUZ_PIO_IDX)   // Mascara para CONTROLARMOS o Buzzer

// Configuracoes do botao play
#define PLAY_PIO			PIOA
#define PLAY_PIO_ID			ID_PIOA
#define PLAY_PIO_IDX		19
#define PLAY_PIO_IDX_MASK	(1u << PLAY_PIO_IDX)

// Configuracoes do botao troca musica
#define NEXT_PIO			PIOB
#define NEXT_PIO_ID			ID_PIOB
#define NEXT_PIO_IDX		2
#define NEXT_PIO_IDX_MASK	(1u << NEXT_PIO_IDX)

// Notas musicais

#define NOTE_B0  31
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978

/************************************************************************/
/* constants                                                            */
/************************************************************************/

const int s1[] = {
	NOTE_E7, NOTE_E7, 0, NOTE_E7,
	0, NOTE_C7, NOTE_E7, 0,
	NOTE_G7, 0, 0,  0,
	NOTE_G6, 0, 0, 0,

	NOTE_C7, 0, 0, NOTE_G6,
	0, 0, NOTE_E6, 0,
	0, NOTE_A6, 0, NOTE_B6,
	0, NOTE_AS6, NOTE_A6, 0,

	NOTE_G6, NOTE_E7, NOTE_G7,
	NOTE_A7, 0, NOTE_F7, NOTE_G7,
	0, NOTE_E7, 0, NOTE_C7,
	NOTE_D7, NOTE_B6, 0, 0,

	NOTE_C7, 0, 0, NOTE_G6,
	0, 0, NOTE_E6, 0,
	0, NOTE_A6, 0, NOTE_B6,
	0, NOTE_AS6, NOTE_A6, 0,

	NOTE_G6, NOTE_E7, NOTE_G7,
	NOTE_A7, 0, NOTE_F7, NOTE_G7,
	0, NOTE_E7, 0, NOTE_C7,
	NOTE_D7, NOTE_B6
};
const int t1[] = {
	12, 12, 12, 12,
	12, 12, 12, 12,
	12, 12, 12, 12,
	12, 12, 12, 12,

	12, 12, 12, 12,
	12, 12, 12, 12,
	12, 12, 12, 12,
	12, 12, 12, 12,

	9, 9, 9,
	12, 12, 12, 12,
	12, 12, 12, 12,
	12, 12, 12, 12,

	12, 12, 12, 12,
	12, 12, 12, 12,
	12, 12, 12, 12,
	12, 12, 12, 12,

	9, 9, 9,
	12, 12, 12, 12,
	12, 12, 12, 12,
	12, 12
};

const int s2[] = {
	NOTE_C4, NOTE_C5, NOTE_A3, NOTE_A4,
	NOTE_AS3, NOTE_AS4, 0,
	0,
	NOTE_C4, NOTE_C5, NOTE_A3, NOTE_A4,
	NOTE_AS3, NOTE_AS4, 0,
	0,
	NOTE_F3, NOTE_F4, NOTE_D3, NOTE_D4,
	NOTE_DS3, NOTE_DS4, 0,
	0,
	NOTE_F3, NOTE_F4, NOTE_D3, NOTE_D4,
	NOTE_DS3, NOTE_DS4, 0,
	0, NOTE_DS4, NOTE_CS4, NOTE_D4,
	NOTE_CS4, NOTE_DS4,
	NOTE_DS4, NOTE_GS3,
	NOTE_G3, NOTE_CS4,
	NOTE_C4, NOTE_FS4, NOTE_F4, NOTE_E3, NOTE_AS4, NOTE_A4,
	NOTE_GS4, NOTE_DS4, NOTE_B3,
	NOTE_AS3, NOTE_A3, NOTE_GS3,
	0, 0, 0
};
const int t2[] = {
	12, 12, 12, 12,
	12, 12, 6,
	3,
	12, 12, 12, 12,
	12, 12, 6,
	3,
	12, 12, 12, 12,
	12, 12, 6,
	3,
	12, 12, 12, 12,
	12, 12, 6,
	6, 18, 18, 18,
	6, 6,
	6, 6,
	6, 6,
	18, 18, 18, 18, 18, 18,
	10, 10, 10,
	10, 10, 10,
	3, 3, 3
};


/************************************************************************/
/* variaveis globais                                                    */
/************************************************************************/

/************************************************************************/
/* prototypes                                                           */
/************************************************************************/

void init(void);
void tone(int freq, int dur);
void play(int note, int tempo, int compass);

/************************************************************************/
/* interrupcoes                                                         */
/************************************************************************/

/************************************************************************/
/* funcoes                                                              */
/************************************************************************/

// Função de inicialização do uC
void init(void){
	// Initialize the board clock
	sysclk_init();

	// Desativa WatchDog Timer
	WDT->WDT_MR = WDT_MR_WDDIS;
	
	// Ativa o PIO na qual o LED foi conectado
	// para que possamos controlar o LED e os botões
	pmc_enable_periph_clk(LED_PIO_ID);
	pmc_enable_periph_clk(BUZ_PIO_ID);
	
	//Inicializa PC8 como saída
	pio_set_output(LED_PIO, LED_PIO_IDX_MASK, 0, 0, 0);
	pio_set_output(BUZ_PIO, BUZ_PIO_IDX_MASK, 0, 0, 0);
	
	// Inicializa PIO do botao
	pmc_enable_periph_clk(PLAY_PIO_ID);
	pmc_enable_periph_clk(NEXT_PIO_ID);
	
	// configura pino ligado ao botão como entrada com um pull-up.
	pio_set_input(PLAY_PIO_ID, PLAY_PIO_IDX_MASK, PIO_DEFAULT);
	pio_pull_up(PLAY_PIO_ID, PLAY_PIO_IDX_MASK, 1);
	
	pio_set_input(NEXT_PIO_ID, NEXT_PIO_IDX_MASK, PIO_DEFAULT);
	pio_pull_up(NEXT_PIO_ID, NEXT_PIO_IDX_MASK, 1);
}

void tone(int freq, int dur){
	// recebe uma frequência em Hertz e uma duração em milisegundos
	int t = 500000/freq; // Tempo em us de pausa: 10e6/(2 * freq)
	// 1 loop - 10e6/freq us
	// x loops - 10e3 us
	// x = freq/1000
	int j = (dur * freq)/1000;
	for(int i = 0; i <= j; i++){
		pio_set(PIOC, LED_PIO_IDX_MASK);	// Acende o LED
		pio_set(PIOC, BUZ_PIO_IDX_MASK);      // Coloca som no buzzer
		delay_us(t);                        // Delay por software de t us
		pio_clear(PIOC, LED_PIO_IDX_MASK);	// Apaga o LED
		pio_clear(PIOC, BUZ_PIO_IDX_MASK);    // Tira som do buzzer
		delay_us(t);
	}
}

void play(int note, int tempo, int compass){
	int noteDuration = compass/tempo;
		
	tone(note, noteDuration);
	int pauseBetweenNotes = noteDuration * 1.30;
	delay_ms(pauseBetweenNotes);
		
	tone(0, noteDuration);
}

/************************************************************************/
/* Main                                                                 */
/************************************************************************/

// Funcao principal chamada na inicalizacao do uC.
typedef struct {
	int* songs[2];
	int* tempos[2];
	int lengths[2];
} musica;

int main(void){
	// inicializa sistema e IOs
	init();
	
	musica m;
	m.songs[0] = &s1;
	m.songs[1] = &s2;
	m.lengths[0] = sizeof(s1)/sizeof(int);
	m.lengths[1] = sizeof(s2)/sizeof(int);
	m.tempos[0] = &t1;
	m.tempos[1] = &t2;
	int choice = 0;

	// super loop
	// aplicacoes embarcadas não devem sair do while(1).
	while (1){
		if (pio_get(PLAY_PIO, PIO_INPUT, PLAY_PIO_IDX_MASK) == 0){
			
			musica:;
			int len = m.lengths[choice];
			int* s = m.songs[choice];
			int* t = m.tempos[choice];
			for (int i = 0; i < len; i++){
				if (pio_get(NEXT_PIO, PIO_INPUT, NEXT_PIO_IDX_MASK) == 0){
					choice = !choice;
					delay_ms(300);
					goto musica;
				}
				play(s[i],t[i],800);
			}
		}
	}
	return 0;
}
