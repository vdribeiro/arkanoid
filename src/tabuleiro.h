#ifndef _TABULEIRO_H_
#define _TABULEIRO_H_

#include "utypes.h"
#include "video-graphics.h"
#include "sprite.h"
#include "pixmap.h"

/** @defgroup Tabuleiro
 * @{
 *
 * Tabuleiro do jogo onde se detecta colisões da bola com os blocos
 */


/**
 * Valores Padrão para as dimensoes dos sprites da bola e dos blocos
 */

#define BLOC_LRG	50 	///< Largura do Bloco
#define BLOC_ALT	10 	///< Altura do Bloco
#define BOLA_TAM	31 	///< Tamanho da Bola (a Largura e Altura são iguais)

/**
* Desenha o tabuleiro consoante o valor de VLIM e HLIM 
* (altura e largura em pixeis respectivamente)
*/
void drawTabuleiro(int VLIM, int HLIM, char *video_mem);

/**detector de colisoes*/
int colisao_bloco(Sprite *bloco, int velocidade, Sprite *bola);

/** @} end of Tabuleiro */

#endif


