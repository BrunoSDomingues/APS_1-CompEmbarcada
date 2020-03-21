// vim:tabstop=4:shiftwidth=4:noexpandtab

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
#define LED_PIO PIOC						// periferico que controla o LED
#define LED_PIO_ID ID_PIOC					// ID do periférico PIOC (controla LED)
#define LED_PIO_IDX 8						// ID do LED no PIO
#define LED_PIO_IDX_MASK (1 << LED_PIO_IDX) // Mascara para CONTROLARMOS o LED

// LED do play (LED1)
#define LED1_PIO PIOA
#define LED1_PIO_ID ID_PIOA
#define LED1_PIO_IDX 0
#define LED1_PIO_IDX_MASK (1u << LED1_PIO_IDX)

// LED do pause (LED2)
#define LED2_PIO PIOC
#define LED2_PIO_ID ID_PIOC
#define LED2_PIO_IDX 30
#define LED2_PIO_IDX_MASK (1u << LED2_PIO_IDX)

// LED do next (LED3)
#define LED3_PIO PIOB
#define LED3_PIO_ID ID_PIOB
#define LED3_PIO_IDX 2
#define LED3_PIO_IDX_MASK (1 << LED3_PIO_IDX)

// Botoes

// Botao play (BUTTON 1)
#define BUT1_PIO PIOD
#define BUT1_PIO_ID ID_PIOD
#define BUT1_PIO_IDX 28
#define BUT1_PIO_IDX_MASK (1u << BUT1_PIO_IDX)

// Botao pause (BUTTON 2)
#define BUT2_PIO PIOC
#define BUT2_PIO_ID 12
#define BUT2_PIO_IDX 31
#define BUT2_PIO_IDX_MASK (1u << BUT2_PIO_IDX)

// Botao next (BUTTON 3)
#define BUT3_PIO PIOA
#define BUT3_PIO_ID ID_PIOA
#define BUT3_PIO_IDX 19
#define BUT3_PIO_IDX_MASK (1u << BUT3_PIO_IDX)

// Configuracoes do Buzzer
#define BUZ_PIO PIOC						 // periferico que controla o Buzzer
#define BUZ_PIO_ID ID_PIOC					 // ID do periférico PIOC (controla Buzzer)
#define BUZ_PIO_IDX 13						 // ID do Buzzer no PIO
#define BUZ_PIO_IDX_MASK (1u << BUZ_PIO_IDX) // Mascara para CONTROLARMOS o Buzzer

#define new_song(song, n, t){                   \
		song.notes = n;                         \
		song.tempos = t;                        \
		song.length = sizeof(n) / sizeof(n[0]); \
}

/************************************************************************/
/* structs                                                              */
/************************************************************************/

typedef struct
{
	int *notes;
	int *tempos;
	size_t length;
} song;

/************************************************************************/
/* prototypes                                                           */
/************************************************************************/

void init(void);
void tone(int freq, int dur);
void play(int note, int tempo, int compass);
void next_song(int *choice, int n_songs, song *cur_song, song *songs);
void BUT1_callback();
void BUT3_callback();

/************************************************************************/
/* interrupcoes                                                         */
/************************************************************************/

volatile char BUT1_flag;
volatile char BUT3_flag;

void BUT1_callback() {
	BUT1_flag = 1;
}

void BUT3_callback() {
	BUT3_flag = 1;
}

/************************************************************************/
/* funcoes                                                              */
/************************************************************************/

// Função de inicialização do uC
void init(void)
{
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


	// Interrupt
	pio_handler_set(
		BUT1_PIO_ID,
		BUT1_PIO_ID,
		BUT1_PIO_IDX_MASK,
		PIO_IT_FALL_EDGE,
		BUT1_callback
	);

	pio_handler_set(
		BUT3_PIO_ID,
		BUT3_PIO_ID,
		BUT3_PIO_IDX_MASK,
		PIO_IT_FALL_EDGE,
		BUT3_callback
	);

	NVIC_EnableIRQ(BUT1_PIO_ID);
	NVIC_SetPriority(BUT1_PIO_ID, 1);  // Priority 1

	NVIC_EnableIRQ(BUT3_PIO_ID);
	NVIC_SetPriority(BUT3_PIO_ID, 1);  // Priority 1

	pio_enable_interrupt(BUT1_PIO, BUT1_PIO_IDX_MASK);
	pio_enable_interrupt(BUT3_PIO, BUT3_PIO_IDX_MASK);
}

void tone(int freq, int dur)
{
	// recebe uma frequência em Hertz e uma duração em milisegundos
	int t = 500000 / freq;  // Tempo em us de pausa: 10e6/(2 * freq)
	// 1 loop - 10e6/freq us
	// x loops - 10e3 us
	// x = freq/1000
	int j = (dur * freq) / 1000;
	for (int i = 0; i < j; i++)
	{
		pio_set(PIOC, LED_PIO_IDX_MASK);   // Acende o LED
		pio_set(PIOC, BUZ_PIO_IDX_MASK);   // Coloca som no buzzer
		delay_us(t);                       // Delay por software de t us
		pio_clear(PIOC, LED_PIO_IDX_MASK); // Apaga o LED
		pio_clear(PIOC, BUZ_PIO_IDX_MASK); // Tira som do buzzer
		delay_us(t);
	}
}

void play(int note, int tempo, int compass)
{
	int noteDuration = compass / tempo;

	tone(note, noteDuration);
	int pauseBetweenNotes = noteDuration * 1.30;
	delay_ms(pauseBetweenNotes);
}

void next_song(int *choice, int n_songs, song *cur_song, song *songs)
{
	*choice = (*choice + 1) % n_songs;
	*cur_song = songs[*choice];  // TODO check
	// TODO Mudar algum indicador aqui
}

/************************************************************************/
/* Main                                                                 */
/************************************************************************/

// Funcao principal chamada na inicalizacao do uC.

int main(void)
{
	// inicializa sistema e IOs
	init();

	int n_songs = 2;
	int choice = 0;
	char pause = 1;

	song s1, s2, cur_song;

	new_song(s1, n1, t1);
	new_song(s2, n2, t2);

	song songs[] = {s1, s2};

	size_t i = 0;

	// Botão play/pause  (BUTTON 1)
	// Botão next        (BUTTON 3)

	while (1) {
		if (pause) {
			pmc_sleep(SAM_PM_SMODE_SLEEP_WFI);  // Sleep until interrupt happens
		}

		if (BUT3_flag) {  // Change
			next_song(&choice, n_songs, &cur_song, songs);
			BUT3_flag = 0;
		}

		if (BUT1_flag) {  // Pause or play
			pause = !pause;
			BUT1_flag = 0;
		}

		if (!pause) {
			if (i < cur_song.length) {
				play(cur_song.notes[i], cur_song.tempos[i], 800);
				i++;
			} else {
				pause = 0;
				i = 0;
			}
		}

	}
	return 0;
}
