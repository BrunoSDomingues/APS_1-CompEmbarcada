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

// LEDs

// LED do SAME70
#define LED_PIO           PIOC                 // periferico que controla o LED
#define LED_PIO_ID        ID_PIOC              // ID do periférico PIOC (controla LED)
#define LED_PIO_IDX       8                    // ID do LED no PIO
#define LED_PIO_IDX_MASK  (1 << LED_PIO_IDX)   // Mascara para CONTROLARMOS o LED

// LED do play (LED1)
#define LED1_PIO			PIOA
#define LED1_PIO_ID			ID_PIOA
#define LED1_PIO_IDX		0
#define LED1_PIO_IDX_MASK	(1u << LED1_PIO_IDX)

// LED do pause (LED2)
#define LED2_PIO			PIOC
#define LED2_PIO_ID			ID_PIOC
#define LED2_PIO_IDX		30
#define LED2_PIO_IDX_MASK	(1u << LED2_PIO_IDX)

// LED do next (LED3)
#define LED3_PIO			PIOB
#define LED3_PIO_ID			ID_PIOB
#define LED3_PIO_IDX		2
#define LED3_PIO_IDX_MASK	(1 << LED3_PIO_IDX)

// Botoes

// Botao play (BUTTON 1)
#define BUT1_PIO			PIOD
#define BUT1_PIO_ID			ID_PIOD
#define BUT1_PIO_IDX		28
#define BUT1_PIO_IDX_MASK	(1u << BUT1_PIO_IDX)

// Botao pause (BUTTON 2)
#define BUT2_PIO			PIOC
#define BUT2_PIO_ID			12
#define BUT2_PIO_IDX		31
#define BUT2_PIO_IDX_MASK	(1u << BUT2_PIO_IDX)

// Botao next (BUTTON 3)
#define BUT3_PIO			PIOA
#define BUT3_PIO_ID			ID_PIOA
#define BUT3_PIO_IDX		19
#define BUT3_PIO_IDX_MASK	(1u << BUT3_PIO_IDX)


// Configuracoes do Buzzer
#define BUZ_PIO           PIOC                  // periferico que controla o Buzzer
#define BUZ_PIO_ID        ID_PIOC               // ID do periférico PIOC (controla Buzzer)
#define BUZ_PIO_IDX       13                    // ID do Buzzer no PIO
#define BUZ_PIO_IDX_MASK  (1u << BUZ_PIO_IDX)   // Mascara para CONTROLARMOS o Buzzer

/************************************************************************/
/* structs                                                              */
/************************************************************************/

typedef struct {
	int* songs[2];
	int* tempos[2];
	int lengths[2];
} musica;

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
	pmc_enable_periph_clk(LED1_PIO_ID);
	
	//Inicializa leds e buzzer como saida
	pio_set_output(LED_PIO, LED_PIO_IDX_MASK, 0, 0, 0);
	pio_set_output(BUZ_PIO, BUZ_PIO_IDX_MASK, 0, 0, 0);
	pio_set_output(LED1_PIO, LED1_PIO_IDX_MASK, 1, 0, 0);
	
	// Inicializa PIO dos botoes
	pmc_enable_periph_clk(BUT1_PIO_ID);
	pmc_enable_periph_clk(BUT3_PIO_ID);
	
	// configura pinos ligado aos botoes como entrada com um pull-up.
	pio_set_input(BUT1_PIO_ID, BUT1_PIO_IDX_MASK, PIO_DEFAULT);
	pio_pull_up(BUT1_PIO_ID, BUT1_PIO_IDX_MASK, 1);
	
	pio_set_input(BUT3_PIO_ID, BUT3_PIO_IDX_MASK, PIO_DEFAULT);
	pio_pull_up(BUT3_PIO_ID, BUT3_PIO_IDX_MASK, 1);
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
		if (pio_get(BUT1_PIO, PIO_INPUT, BUT1_PIO_IDX_MASK) == 0){
			pio_set(PIOA, LED1_PIO_IDX_MASK);
			delay_ms(100);
			pio_clear(PIOA, LED1_PIO_IDX_MASK);
			
			musica:;
			int len = m.lengths[choice];
			int* s = m.songs[choice];
			int* t = m.tempos[choice];
			for (int i = 0; i < len; i++){
				if (pio_get(BUT3_PIO, PIO_INPUT, BUT3_PIO_IDX_MASK) == 0){
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
