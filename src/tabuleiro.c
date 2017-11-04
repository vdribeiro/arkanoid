#include "tabuleiro.h"

void drawTabuleiro(int VLIM, int HLIM, char *video_mem){

				/*draw_line(10, 10, 10, VLIM-10, 150, video_mem);
				draw_line(HLIM-10, 10, HLIM-10, VLIM-10, 150, video_mem);
				draw_line(10, 10, HLIM-10, 10, 150, video_mem);*/
				
				//barra esquerda
				drawRectangle(20, 30, 40, VLIM-10, 150, video_mem);
				//barra direita
				drawRectangle(HLIM-40, 30, HLIM-20, VLIM-10, 150, video_mem);
				//barra topo
				drawRectangle(20, 30, HLIM-20, 50, 150, video_mem);
				
				//drawRectangle((HLIM/2)-50, VLIM-20, (HLIM/2)+50, VLIM-10, 300, video_mem);

}

int colisao_bloco(Sprite *bloco, int velocidade, Sprite *bola)
{
	//  Bx,By(4)___(8)___(3)Bx,By+
	//              |                       |
	//          (5|                        |7)
	//              |__________|
	// Bx,By+(1)       (6)       (2)Bx+,By+
	
	int Bx = bloco->x;
	int By = bloco->y;

	int x = bola->x;
	int y = bola->y;
	
	/*if( (x+BOLA_TAM > Bx) && (x < Bx+BLOC_LRG) && (y < By + BLOC_ALT) && (y+BOLA_TAM > By )){
		if (bola->yspeed>0) {
			bola->yspeed = -velocidade;
		} else {
			bola->yspeed = velocidade;
		}
		
		
		return 1;
		}*/
		
		
		if( (x+BOLA_TAM > Bx) && (x < Bx+BLOC_LRG) && (y < By + BLOC_ALT) && (y+BOLA_TAM > By )){
			
				if ((x > Bx+BLOC_LRG/2 + 10) || (x+BOLA_TAM < Bx+BLOC_LRG/2 - 10)) {
				if (bola->xspeed>0) {
					bola->xspeed = -velocidade;
				} else {
					bola->xspeed = velocidade;
				}
			}
			else
			if ((y+BOLA_TAM < By + BLOC_ALT/2) || (y < By + BLOC_ALT)) {
				if (bola->yspeed>0) {
					bola->yspeed = -velocidade;
				} else {
					bola->yspeed = velocidade;
				}
			}
			
			return 1;
		}
	
/*
	//detectar embates nos lados
	if (y <= By+BLOC_ALT && x+BOLA_TAM > Bx && x < Bx+BLOC_LRG){
		bola->yspeed = - bola->yspeed; // inv y POS6
		return 1;
		}
	if (x+BOLA_TAM >= Bx && y <= By+BLOC_ALT && y+BOLA_TAM >= By){
		bola->xspeed = -bola->xspeed; //inv x POS5
		return 1;
		}
	if (x <= Bx+BLOC_LRG && y <= By+BLOC_ALT && y+BOLA_TAM >= By){
		bola->xspeed = -bola->xspeed; //inv x POS7
		return 1;
		}
	if (y+BOLA_TAM >= By && x+BOLA_TAM >= Bx && x <= Bx+BLOC_LRG){
		bola->yspeed = - bola->yspeed; // inv y POS8
		return 1;
		}
	
	//detectar embates nos vertices
	if (x+BOLA_TAM == Bx && y == By+BLOC_ALT){
		bola->xspeed = -bola->xspeed; //inv x,y POS1
		bola->yspeed = -bola->yspeed;
		return 1;
		}
	if (x == Bx+BOLA_TAM && y == By+BLOC_ALT){
		bola->xspeed = -bola->xspeed; //inv x,y POS2
		bola->yspeed = -bola->yspeed;
		return 1;
		}
	if (x == Bx+BOLA_TAM && y+BOLA_TAM == By){
		bola->xspeed = -bola->xspeed; //inv x,y POS3
		bola->yspeed = -bola->yspeed;
		return 1;
		}
	if (x+BOLA_TAM == Bx && y+BOLA_TAM == By){
		bola->xspeed = -bola->xspeed; //inv x,y POS POS4
		bola->yspeed = -bola->yspeed;
		return 1;
		}*/
	return 0;
}
