#include "video-graphics.h"

int HRES = 0, VRES = 0;
ulong VIDEO_MEM = 0;

ulong get_videoadd(int mode)
{
	__dpmi_regs regs;
	ulong dosbuf=0, address=0;

	dosbuf = __tb & 0xFFFFF; /* dosbuf é a área de transferência */
	dosmemput(&address, 4, dosbuf+40); /*apaga bytes 40 a 43 dessa área*/

	regs.x.ax = 0x4F01; /* VBE get mode info */
	regs.x.di = dosbuf & 0xF; /* LSB da área de transferência */
	regs.x.es = (dosbuf >> 4) & 0xFFFF; /*MSBda área de transferência*/
	regs.x.cx = mode; /*informacao sobre o modo*/
	
	__dpmi_int(0x10, &regs); /* DOS int 10h */
	if (regs.h.ah) { return 0; } /* tratamento de erro, modo não suportado */

	dosmemget(dosbuf+40, 4, &address); /* lê o endereço da memória vídeo */
	if (!address) { return 0; } /* tratamento de erro, modo não suportado */
	
	return address;
} 

char *enter_graphics(int mode, __dpmi_meminfo *map)
{
	char *video_mem; /* apontador genérico para aceder à memória gráfica */
	__dpmi_regs regs; /*__dpmi_regs é uma estrutura que representaos registos do CPU */ 
	
	//Resolucao
	switch(mode)
	{
		case 0x101:
			HRES = 640;
			VRES = 480;
			break;
		case 0x103:
			HRES = 800;
			VRES = 600;
			break;
		case 0x105:
			HRES = 1024;
			VRES = 768;
			break;
		case 0x107:
			HRES = 1280;
			VRES = 1024;
			break;
		default:
			HRES = 640;
			VRES = 480;
			break;
	}
	
	//Entrar no modo grafico
	regs.x.ax = 0x4F02; /* registo AX do CPU com valor 0x4f02 */
	regs.x.bx = 0x4000|(mode&0xFFF); /* registo BX do CPU com valor 0x4mode */
	__dpmi_int(0x10, &regs); /* gera interrupção de software 0x10, activando o modo gráfico */
	
	//Mapeamento da memoria video
	__djgpp_nearptr_enable(); /* permite acesso à memória física usando apontadores */
	map->address = get_videoadd(mode); /* endereço físico da memória vídeo, usar o valor correcto */
	map->size = HRES*VRES; /* tamanho do bloco de memória, usar apenas o necessário */
	__dpmi_physical_address_mapping(map); /* video_mem contém o endereço genérico a usar */
	video_mem = (char*)(map->address + __djgpp_conventional_base);
	
	return video_mem;
}

void leave_graphics(__dpmi_meminfo map)
{
	__dpmi_regs regs; /*__dpmi_regs é uma estrutura que representaos registos do CPU */ 
	
	//Libertacao do mapeamento da memoria
	__dpmi_free_physical_address_mapping(&map);
	__djgpp_nearptr_disable();
	
	//Regressar ao modo texto
	regs.x.ax = 0x0003; /* registo AX do CPU com valor 0x03 */
	__dpmi_int(0x10, &regs); /* gera interrupcao software 0x10, entrando no modo texto */
	
	return;
}

void set_pixel(int x, int y, int color, char *base)
{
	if(x>HRES||y>VRES||x<1||y<1)
		return;
		
	*(base + x + y*HRES) = color;
}

int get_pixel(int x, int y, char *base)
{
	int color=0;
	
	if(x>HRES||y>VRES||x<1||y<1)
		return -1;
		
	color = *(base + x + y*HRES);
	return color;
}

void clear_screen(char color, char *base)
{	
	int x=0,y=0;
	for(y=0;y!=VRES;y++)
	{
		for(x=0;x!=HRES;x++)
		{
			set_pixel(x,y,color,base);
		}
	}
}

void draw_line(int xi, int yi, int xf, int yf, int color, char *base)
{
	int x=0, y=0, i=0;// m=1, j=0, ince=0, incne=0, d=0;
	//int dx=0, dy=0, sdx=0, sdy=0, px=0, py=0;
	int dx=0,dy=0,sdx=0,sdy=0,dxabs=0,dyabs=0,px=0,py=0;
	
	if(xi>HRES||yi>VRES||xi<1||yi<1||xf>HRES||yf>VRES||xf<1||yf<1||xi>xf||yi>yf)
		return;

	if (xi==xf)	{
		for (i=yi;i!=yf;i++)
		{
			set_pixel(xi,i,color,base);
		}
	} else if (yi==yf) {
		for (i=xi;i!=xf;i++)
		{
			set_pixel(i,yi,color,base);
		}
	} else if ((xi==yi)&&(xf==yf)) {
		for (i=xi;i!=xf;i++)
		{
			set_pixel(i,i,color,base);
		}
	} else {
		dx=xf-xi;      
		dy=yf-yi;   
		dxabs=dx;
		dyabs=dy;
		sdx=sgn(dx);
		sdy=sgn(dy);
		x=dyabs>>1;
		y=dxabs>>1;
		px=xi;
		py=yi;

		if (dxabs>=dyabs)
		{
			for(i=0;i<dxabs;i++)
			{
			  y+=dyabs;
			  if (y>=dxabs)
			  {
				y-=dxabs;
				py+=sdy;
			  }
			  px+=sdx;
			  set_pixel(px,py,color,base);
			}
		}
		else
		{
			for(i=0;i<dyabs;i++)
			{
			  x+=dxabs;
			  if (x>=dyabs)
			  {
				x-=dyabs;
				px+=sdx;
			  }
			  py+=sdy;
			  set_pixel(px,py,color,base);
			}
		}
		
		/*dx=xf-xi;      
		dy=yf-yi;     
		sdx=sgn(dx);
		sdy=sgn(dy);
		x=dy;
		y=dx;
		px=xi;
		py=yi;

		if (dx>=dy) 
		{
			for(i=0;i<dx;i++)
			{
			  y+=dy;
			  if (y>=dx)
			  {
				y-=dx;
				py+=sdy;
			  }
			  px+=sdx;
			  set_pixel(px,py,color,base);
			}
		}
		else 
		{
			for(i=0;i<dy;i++)
			{
			  x+=dx;
			  if (x>=dy)
			  {
				x-=dy;
				px+=sdx;
			  }
			  py+=sdy;
			  set_pixel(px,py,color,base);
			}
		}*/
		
		/*x=xf-xi;
		y=yf-yi;
		m=y/x;
		
		ince=2*y;
		incne=2*y-2*x;
		d=2*y-x;
		
		if (m<1) {
			j=yi;
			ince=0;
			incne=x/y+1;
			for (i=xi;i<=xf;i++)
			{
				set_pixel(i,j,color,base);
				
				if (ince==incne) {
					j++;
					ince=0;
				} else {
					ince++;
				}
			}
		} else {
			j=xi;
			ince=0;
			incne=y/x+1;
			for (i=yi;i<=yf;i++)
			{
				set_pixel(j,i,color,base);
				if (ince==incne) {
					j++;
					ince=0;
				} else {
					ince++;
				}
			}
		}
		
		for (i=xi;i!=xf;i++)
		{
			y=m*i;
			set_pixel(i,y,color,base);
		}*/
	}
	/*
	//obter o comprimento da linha em x
	int x_size = xf - xi;
	if(x_size < 0) x_size = -x_size;
	
	//obter o comprimento da linha em y
	int y_size = yf - yi;
	if(y_size < 0) y_size = -y_size;
	
	int i;
	if(x_size > y_size) //dimensao maior e a horizontal
	{
		if(xf - xi < 0) //orientar a recta no sentido positivo de x
		{
			int aux = xi;
			xi = xf;
			xf = aux;
			aux = yi;
			yi = yf;
			yf = aux;
		}
		
		for(i = 0; i <= x_size; i++)
		{
			//regra de 3 simples; se x_size esta para y_size, entao i esta para y_point
			double y_point = (double)y_size * (double)i / (double)x_size;
			
			//desenhar pixel
			if(yf - yi < 0) set_pixel(xi + i, yi - (int)(y_point + 0.5), color, base);
			else set_pixel(xi + i, yi + (int)(y_point + 0.5), color, base);
		}
	}
		
	else //dimensao maior e a vertical
	{
		if(yf - yi < 0) //orientar a recta no sentido positivo de y
		{
			int aux = xi;
			xi = xf;
			xf = aux;
			aux = yi;
			yi = yf;
			yf = aux;
		}
		
		for(i = 0; i <= y_size; i++)
		{
			//regra de 3 simples; se y_size esta para x_size, entao i esta para x_point
			double x_point = (double)x_size * (double)i / (double)y_size;
			
			//desenhar pixel
			if(xf - xi < 0) set_pixel(xi - (int)(x_point + 0.5), yi + i, color, base);
			else set_pixel(xi + (int)(x_point + 0.5), yi + i, color, base);
		}
	}		
	*/
}

void drawCharAt(char c, int x, int y, int fore_color, int back_color, int char_scale, char* video_base, char* table)
{
	int CHR_H = 1;
	char* char_def = table + c * CHR_H;

	int i, j;
	Byte mask = 0x80;
	int color;
	int x_act, y_act, sx, sy;

	for (j = 0, y_act = y; j < CHR_H; j++, y_act += char_scale)
	{
		for (i = 0, x_act = x; i < 8; i++, x_act += char_scale)
		{
			color = (*char_def & (mask >> i)) ? fore_color : back_color;
			for(sx = 0; sx < char_scale; sx++)
					for(sy = 0; sy < char_scale; sy++)
						set_pixel(x_act + sx, y_act + sy, color, video_base);
		}
		char_def++;
	}
}	


void drawStringAt(char* str, int x, int y, int fore_color, int back_color, int char_scale, char *video_base, char *table)
{
	int i = 0;
	while(str[i] != 0)
	{
		drawCharAt(str[i], x + 8 * char_scale * i, y, fore_color, back_color, char_scale, video_base, table);
		i++;
	}
}
		
		
void drawIntAt(int num, int x, int y, int fore_color, int back_color,
				int char_scale, char *video_base, char *table)
{
	int i = 0;
	
	if(num < 0)
	{
		drawCharAt('-', x, y, fore_color, back_color, char_scale, video_base, table);
		i++;
		num = 0 - num;
	}
	
	int ordem = 1;
	while(num / ordem >= 10) ordem *= 10;
	
	while(true)
	{
		int digito = num / ordem;
		drawCharAt(digito + '0', x + 8 * char_scale * i, y, fore_color, back_color, char_scale, video_base, table);
		num -= digito * ordem;
		if(ordem == 1) break;
		ordem /= 10;
		i++;
	}
}

void drawRectangle(int xi, int yi, int xf, int yf, int color, char *base){
	
	while(yi != yf){
		draw_line(xi, yi, xf, yi, color, base);
		yi++;
	}
}

/** @} end of video */ 
