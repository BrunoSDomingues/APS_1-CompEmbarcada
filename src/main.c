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
#include "gfx_mono_ug_2832hsweg04.h"
#include "gfx_mono_text.h"
#include "sysfont.h"

/************************************************************************/
/* defines                                                              */
/************************************************************************/

// Compasso base
#define TEMPO					800

// LED do SAME70
#define LED_PIO					PIOC					// periferico que controla o LED
#define LED_PIO_ID				ID_PIOC					// ID do periférico PIOC (controla LED)
#define LED_PIO_IDX				8						// ID do LED no PIO
#define LED_PIO_IDX_MASK		(1 << LED_PIO_IDX)		// Mascara para CONTROLARMOS o LED

// Botoes

// Botao play/pause (BUTTON 1)
#define BUT1_PIO				PIOD
#define BUT1_PIO_ID				ID_PIOD
#define BUT1_PIO_IDX			28
#define BUT1_PIO_IDX_MASK		(1u << BUT1_PIO_IDX)
#define BUT1_PRIORITY			6

// Botao previous (BUTTON 2)
#define BUT2_PIO				PIOC
#define BUT2_PIO_ID				12
#define BUT2_PIO_IDX			31
#define BUT2_PIO_IDX_MASK		(1u << BUT2_PIO_IDX)
#define BUT2_PRIORITY			4

// Botao next (BUTTON 3)
#define BUT3_PIO				PIOA
#define BUT3_PIO_ID				ID_PIOA
#define BUT3_PIO_IDX			19
#define BUT3_PIO_IDX_MASK		(1u << BUT3_PIO_IDX)
#define BUT3_PRIORITY			4

// Configuracoes do Buzzer
#define BUZ_PIO PIOC						 // periferico que controla o Buzzer
#define BUZ_PIO_ID ID_PIOC					 // ID do periférico PIOC (controla Buzzer)
#define BUZ_PIO_IDX 13						 // ID do Buzzer no PIO
#define BUZ_PIO_IDX_MASK (1u << BUZ_PIO_IDX) // Mascara para CONTROLARMOS o Buzzer

#define new_song(song, n, t)                    \
	{                                           \
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
	char *title;
} song;

/************************************************************************/
/* prototypes                                                           */
/************************************************************************/

void init(void);
void tone(int freq, int dur);
void play(int note, int tempo, int compass);
void next_song(int *choice, int n_songs, song *cur_song, song *songs);
void prev_song(int *choice, int n_songs, song *cur_song, song *songs);
void write_song_title(char* status);
void clear_display();
void BUT1_callback();
void BUT2_callback();
void BUT3_callback();

/************************************************************************/
/* interrupcoes                                                         */
/************************************************************************/

volatile char BUT1_flag = 0;
volatile char BUT2_flag = 0;
volatile char BUT3_flag = 0;

void BUT1_callback(void)
{
	BUT1_flag = 1;
}

void BUT2_callback(void)
{
	BUT2_flag = 1;
}

void BUT3_callback(void)
{
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

	//Inicializa LED como saida
	pio_set_output(LED_PIO, LED_PIO_IDX_MASK, 0, 0, 0);

	// Inicializa PIO dos botoes
	pmc_enable_periph_clk(BUT1_PIO_ID);
	pmc_enable_periph_clk(BUT2_PIO_ID);
	pmc_enable_periph_clk(BUT3_PIO_ID);

	// configura pinos ligado aos botoes como entrada com um pull-up.
	pio_configure(BUT1_PIO, PIO_INPUT, BUT1_PIO_IDX_MASK, PIO_PULLUP);
	pio_configure(BUT2_PIO, PIO_INPUT, BUT2_PIO_IDX_MASK, PIO_PULLUP);
	pio_configure(BUT3_PIO, PIO_INPUT, BUT3_PIO_IDX_MASK, PIO_PULLUP);

	NVIC_EnableIRQ(BUT1_PIO_ID);
	NVIC_SetPriority(BUT1_PIO_ID, BUT1_PRIORITY);

	NVIC_EnableIRQ(BUT2_PIO_ID);
	NVIC_SetPriority(BUT2_PIO_ID, BUT2_PRIORITY);

	NVIC_EnableIRQ(BUT3_PIO_ID);
	NVIC_SetPriority(BUT3_PIO_ID, BUT3_PRIORITY);

	pio_enable_interrupt(BUT1_PIO, BUT1_PIO_IDX_MASK);
	pio_enable_interrupt(BUT2_PIO, BUT2_PIO_IDX_MASK);
	pio_enable_interrupt(BUT3_PIO, BUT3_PIO_IDX_MASK);

	// Incializacao do buzzer
	pmc_enable_periph_clk(BUZ_PIO_ID);
	pio_set_output(BUZ_PIO, BUZ_PIO_IDX_MASK, 0, 0, 0);

	// Interrupt
	pio_handler_set(BUT1_PIO, BUT1_PIO_ID, BUT1_PIO_IDX_MASK, PIO_IT_FALL_EDGE, BUT1_callback);
	pio_handler_set(BUT2_PIO, BUT2_PIO_ID, BUT2_PIO_IDX_MASK, PIO_IT_FALL_EDGE, BUT2_callback);
	pio_handler_set(BUT3_PIO, BUT3_PIO_ID, BUT3_PIO_IDX_MASK, PIO_IT_FALL_EDGE, BUT3_callback);
}

void tone(int freq, int dur)
{
	// recebe uma frequência em Hertz e uma duração em milisegundos
	int t = 500000 / freq; // Tempo em us de pausa: 10e6/(2 * freq)
	// 1 loop - 10e6/freq us
	// x loops - 10e3 us
	// x = freq/1000
	int j = (dur * freq) / 1000;
	for (int i = 0; i < j; i++)
	{
		pio_set(PIOC, LED_PIO_IDX_MASK);   // Acende o LED
		pio_set(PIOC, BUZ_PIO_IDX_MASK);   // Coloca som no buzzer
		delay_us(t);					   // Delay por software de t us
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
	*cur_song = songs[*choice];
}

void prev_song(int *choice, int n_songs, song *cur_song, song *songs)
{
	if (*choice == 0) {
		*choice = n_songs - 1;
		} else {
		*choice = *choice - 1;
	}
	*cur_song = songs[*choice];
}

void write_song_title(char* status) {
	gfx_mono_draw_string(status, 10, 10, &sysfont);
}

void clear_display() {
	gfx_mono_draw_string("		", 10, 10, &sysfont);
}

/************************************************************************/
/* Main                                                                 */
/************************************************************************/

// Funcao principal chamada na inicalizacao do uC.

int main(void)
{
	// inicializa sistema e IOs
	board_init();
	sysclk_init();
	init();
	delay_init();

	const int n_songs = 3;
	int choice = 0;
	unsigned char pause = 1;

	song s1, s2, s3, cur_song;

	new_song(s1, n1, t1);
	s1.title = &"Mario 1";
	
	new_song(s2, n2, t2);
	s2.title = &"Mario 2";
	
	new_song(s3, n3, t3);
	s3.title = &"Take on Me";

	song songs[] = {s1, s2, s3};

	size_t i = 0;
	cur_song = songs[0];
	
	// Botão play/pause  (BUTTON 1)
	BUT1_flag = 0;
	// Botão previous	 (BUTTON 2)
	BUT2_flag = 0;
	// Botão next        (BUTTON 3)
	BUT3_flag = 0;
	
	gfx_mono_ssd1306_init();
	write_song_title(songs[choice].title);

	while (1)
	{
		if (pause) pmc_sleep(SAM_PM_SMODE_SLEEP_WFI); // Sleep until interrupt happens
		
		if (BUT2_flag){ // Change
			BUT2_flag = 0;
			prev_song(&choice, n_songs, &cur_song, songs);
			i = 0;
			clear_display();
			write_song_title(songs[choice].title);
		}

		if (BUT3_flag){ // Change
			BUT3_flag = 0;
			next_song(&choice, n_songs, &cur_song, songs);
			i = 0;
			clear_display();
			write_song_title(songs[choice].title);
		}

		if (BUT1_flag){ // Pause or play
			if (pause == 1) pause = 0;
			else pause = 1;
			BUT1_flag = 0;
		}

		if (!pause){
			if (i < cur_song.length){
				play(cur_song.notes[i], cur_song.tempos[i], TEMPO);
				i++;
			}
			else {
				pause = 0;
				i = 0;
			}
		}
	}
	return 0;
}