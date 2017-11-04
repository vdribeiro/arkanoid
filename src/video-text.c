#include "video-text.h"

Bool printCharAt(char ch, int x, int y, char attributes)
{
	ulong offset = (y-1)*(WIDTH*2) + (x-1)*2;
	
	if(x<1||y<1||x>WIDTH||y>HEIGHT)
		return false;
		
	_farpokeb(_dos_ds, VIDEO_TEXT + offset, ch);
	_farpokeb(_dos_ds, VIDEO_TEXT + offset + 1, attributes);
	return true;
}

Bool printStringAt(char *str, int x, int y, char attributes)
{
	if(x<1||y<1||x>WIDTH||y>HEIGHT)
		return false;
		
	if ((y==HEIGHT) && ((x-WIDTH) < stringLength(str)))
		return false;
		
	while(*str)
	{
		printCharAt(*(str++), x, y, attributes);
		x++;
		if (x>WIDTH) {
			x=1;
			y++;
		}
	}
	
	return true;
}

Bool printIntAt(int num, int x, int y, char attributes)
{
	int i=0, j=0;
	char n[15], nf[15];
	
	if(x<1||y<1||x>WIDTH||y>HEIGHT)
		return false;
	
	//numero positivo ou negativo
	if(num<0) {
		j=1;
		num*=-1;
		nf[0]='-';
	} else {
		j=0;
	}
	
	//passa de inteiro para char (ao contrario)
	i=0;
	while(num!=0) {
		n[i]=((num%10) + 0x30);
		num/=10;
		i++;
	}
	//n[i]='\0';
	i--;
	
	//passa para o array por ordem
	while(i>=0) {
		nf[j++]=n[i--];
	}
	
	//Se o numero for zero
    if(j==0) {
		nf[0] = '0';
		j++;
	}
	
	//escreve o 'escape' na ultima posicao 
	nf[j]='\0';
	
	return printStringAt(nf, x, y, attributes);
}

Bool drawFrame(char *title, char attributes, int x, int y, int width, int height)
{
	int xp=x,yp=y, pos;
	int widthx=x+width-1;
	int heighty=y+height-1;
	
	if(x<1||y<1||x>WIDTH||y>HEIGHT||widthx>WIDTH||heighty>HEIGHT)
		return false;
	
	//cantos
	printCharAt(0xC9, x, y, attributes);
	printCharAt(0xBB, widthx, y, attributes);
	printCharAt(0xC8, x, heighty, attributes);
	printCharAt(0xBC, widthx, heighty, attributes);
	
	//barras horizontais
	x++;
	while (x<widthx) {
		printCharAt(0xCD, x, y, attributes);
		printCharAt(0xCD, x, heighty, attributes);
		x++;
	}
	x=xp;
	
	//barras verticais
	y++;
	while (y<heighty) {
		printCharAt(0xBA, x, y, attributes);
		printCharAt(0xBA, widthx, y, attributes);
		y++;
	}
	y=yp;
	
	//titulo centrado
	pos=(x+(widthx-x)/2)-(stringLength(title)/2);
	printStringAt(title, pos, y, attributes);
	
	return true;
}

char getCharAt(int x, int y)
{
	ulong offset = y*(WIDTH*2) + x*2;
	char ch = _farpeekb(_dos_ds, VIDEO_TEXT + offset);
	return ch;
}

char getAttributesAt(int x, int y)
{
	ulong offset = y*(WIDTH*2) + x*2;
	char ch = _farpeekb(_dos_ds, VIDEO_TEXT + offset + 1);
	return ch;
}

Bool clear()
{
	int i, j;
	for(j=0; j<HEIGHT; j++)
		for(i=0; i<WIDTH; i++)
			printCharAt(' ', i, j, 0x00);
	return true;
}

uint stringLength(char *str)
{
	uint tamanho=0;
	while(*(str++))
	{
		tamanho++;
	}
	return tamanho;
}
