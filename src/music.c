#include "music.h"

#define SPEAKER_DISABLE 0xFD	/* mascara para desactivar altifalante */

float frequency[] =
	{16.35, 17.32, 18.35, 19.45, 20.6, 21.83, 23.12, 24.5, 25.96, 27.5, 29.14, 30.87,
	32.7, 34.65, 36.71, 38.89, 41.2, 43.65, 46.25, 49, 51.91, 55, 58.27, 61.74,
	65.41, 69.3, 73.42, 77.78, 82.41, 87.31, 92.5, 98, 103.83, 110, 116.54, 123.47,
	130.81, 138.59, 146.83, 155.56, 164.81, 174.61, 185, 196, 207.65, 220, 233.08, 246.94,
	261.63, 277.18, 293.66, 311.13, 329.63, 349.23, 369.99, 392, 415.3, 440, 466.16, 493.88,
	523.25, 554.37, 587.33, 622.25, 659.26, 698.46, 739.99, 783.99, 830.61, 880, 932.33, 987.77,
	1046.5, 1108.73, 1174.66, 1244.51, 1318.51, 1396.91, 1479.98, 1567.98, 1661.22, 1760, 1864.66, 1975.53,
	2093, 2217.46, 2349.32, 2489.02, 2637.02, 2793.83, 2959.96, 3135.96, 3322.44, 3520, 3729.31, 3951.07,
	4186.01, 4434.92, 4698.64, 4978.03};

void speaker_on()
{
	unsigned char data=inportb(SPEAKER_CTRL); /*ler o que esta no porto 61*/
	outportb(SPEAKER_CTRL,data|SPEAKER_ENABLE); /*alterar o bit 1 do porto 61 para 1*/
}

void speaker_off()
{ 
	unsigned char data=inportb(SPEAKER_CTRL); /*ler o que esta no porto 61*/	
	outportb(SPEAKER_CTRL,data & SPEAKER_DISABLE); /*alterar o bit 1 do porto 61 para 0*/
}

void programa_timer(int note)
{
	unsigned char data=inportb(SPEAKER_CTRL); /*ler o que esta no porto 61*/		
	outportb(SPEAKER_CTRL, data | TIMER2_ENABLE); /*alterar o bit 0 do porto 61*/
	int freq= TIMER_CLK / frequency[note]; /*calculo do divisor*/
	outportb(TIMER_CTRL,TIMER2_PONG); /*programar timer 2*/
	outportb(TIMER_2,LSB(freq)); /*colocar divisor*/
	outportb(TIMER_2,MSB(freq));
}

void play_song(Song* s)
{
	unsigned char data=inportb(SPEAKER_CTRL); /*ler o que esta no porto 61*/		
	outportb(SPEAKER_CTRL, data | TIMER2_ENABLE); /*alterar o bit 0 do porto 61*/
	
	unsigned int k;
	for(k=0;k<s->lenght;k++)
	{
		int freq= TIMER_CLK / frequency[s->notes[k]]; /*calculo do divisor*/
		outportb(TIMER_CTRL,TIMER2_PONG); /*programar timer 2*/
		outportb(TIMER_2,LSB(freq)); /*colocar divisor*/
		outportb(TIMER_2,MSB(freq));
		speaker_on(); /*ligar speaker*/
		delay(s->time[k]);
		speaker_off();
		delay(s->pause); /*desligar speaker*/
	}
}
