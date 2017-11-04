#ifndef _ARKANOID_H_
#define _ARKANOID_H_

//ficheiro header do main que faz a 'linkagem' com todas as bibliotecas usadas

#include <stdio.h>
#include <stdlib.h>		
#include <string.h>
#include <dos.h>
#include <time.h>

#include "utypes.h"
#include "io.h"
#include "tabuleiro.h"
#include "video-text.h"
#include "video-graphics.h"
#include "ints.h"
#include "timer.h"
#include "music.h"
#include "kbc.h"
#include "rtc.h"

 /** 
  *   Enumeracao das entradas utilizadas na maquina de estados da musica
  */
enum Estados{INICIO, PLAYING, PAUSE, FIM, REPEAT};

//codigos das teclas para o teclado

#define ESC			0x01
#define ESQUERDA 	0x1e
#define DIREITA 	0x20
#define ESPACO	 	0x39


#endif
