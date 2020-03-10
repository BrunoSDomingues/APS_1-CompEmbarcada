/**
 * 5o Semestre - Engenharia da Computacao
 * APS 1 - Musical
 */

/************************************************************************/
/* includes                                                             */
/************************************************************************/

#include "asf.h"
#include "notas.h"

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
