#include "sprite.h"

extern int HRES, VRES;

static char *video_buffer = NULL;

/** Reads a xpm-like sprite defined in "map" (look at pixmap.h for
 * examples). Returns the address of the allocated memory where the
 * sprite was read. Updates "width" and "heigh" with the sprite
 * dimension.  Return NULL on error.
 * Assumes that VRES and HRES (the screen vertical and horizontal resolution)
 * are externaly defined.
 * 
 * Usage example, using the defined sprite in pixmap.h:
 * <pre>
 *   #include "pixmap.h" // defines  pic1, pic2, etc 
 *   int wd, hg;
 *   char *sprite = read_xpm(pic1, &wd, &hg);
 * </pre>
*/

char *read_xpm(char *map[], int *wd, int *ht)
{
	__attribute__((unused)) static char read_xpm_jcard;

	int width, height, colors;
	char sym[256];
	int  col;
	int i, j;
	char *pix, *pixtmp, *tmp, *line;
	char symbol;

	// read width, height, colors
	if (sscanf(map[0],"%d %d %d", &width, &height, &colors) != 3) {
		printf("read_xpm: incorrect width, height, colors\n");
		return NULL;
	}
	#ifdef DEBUG
	printf("%d %d %d\n", width, height, colors);
	#endif
	if (width > HRES || height > VRES || colors > 256) {
		printf("read_xpm: incorrect width, height, colors\n");
		return NULL;
	}

	*wd = width;
	*ht = height;

	for (i=0; i<256; i++)
	sym[i] = 0;

	// read symbols <-> colors 
	for (i=0; i<colors; i++) {
		if (sscanf(map[i+1], "%c %d", &symbol, &col) != 2) {
			printf("read_xpm: incorrect symbol, color at line %d\n", i+1);
			return NULL;
		}
	#ifdef DEBUG
		printf("%c %d\n", symbol, col);
	#endif
		if (col > 256) {
			printf("read_xpm: incorrect color at line %d\n", i+1);
			return NULL;
		}
		sym[col] = symbol;
	}

	// allocate pixmap memory
	pix = pixtmp = malloc(width*height);

	// parse each pixmap symbol line
	for (i=0; i<height; i++) {
		line = map[colors+1+i];
	#ifdef DEBUG
		printf("\nparsing %s\n", line);
	#endif
		for (j=0; j<width; j++) {
			tmp = memchr(sym, line[j], 256);  // find color of each symbol
			if (tmp == NULL) {
				printf("read_xpm: incorrect symbol at line %d, col %d\n", colors+i+1, j);
				return NULL;
			}
			*pixtmp++ = tmp - sym; // pointer arithmetic! back to books :-)
	#ifdef DEBUG
			printf("%c:%d ", line[j], tmp-sym);
	#endif
		}
	}

	return pix;
}

Sprite* create_sprite(char *pic[])
{
	Sprite* sprt = create_unbuffered_sprite(pic);
	if(!sprt) return NULL;
	sprt->bgmap = malloc(sprt->width * sprt->height);
	return sprt;
}

Sprite* create_unbuffered_sprite(char *pic[])
{
	Sprite* sprt = malloc(sizeof(Sprite));
	if(!sprt) return NULL;
	
	int wd, ht;
	sprt->map = read_xpm(pic, &wd, &ht);
	if(!sprt->map) { free(sprt); return NULL; }
	
	sprt->x = sprt->y = sprt->xspeed = sprt->yspeed = 0;
	sprt->width = wd; sprt->height = ht;
	sprt->bgmap = NULL;
	return sprt;
}

Sprite* capture_screen(int x_ini, int y_ini, int width, int height, char* base)
{
	Sprite* sprt = malloc(sizeof(Sprite));
	if(!sprt) return NULL;
	
	char* map = malloc(width * height);
	if(!map) { free(sprt); return NULL; }

	int i, k;
	for(i = 0; i < height; i++)
		for(k = 0; k < width; k++)
			*(map + width*i + k) = get_pixel(x_ini + k, y_ini + i, base);

	sprt->map = map;
	
	sprt->x = x_ini;
	sprt->y = y_ini;
	sprt->xspeed = sprt->yspeed = 0;
	sprt->width = width; sprt->height = height;
	sprt->bgmap = NULL;
	return sprt;
}

void draw_sprite(Sprite* sprt, char *base)
{
	char* dSprt = sprt->map;
	char* bgMap = sprt->bgmap;
	
	int i,k;
	for(i=0; i < sprt->height; i++)
		for(k=0; k < sprt->width; k++)
		{
			if(bgMap) *(bgMap + sprt->width*i + k) = get_pixel(sprt->x+k, sprt->y+i, base); //guarda o valor antigo
			if(*dSprt != 0) set_pixel(sprt->x+k,sprt->y+i, *dSprt, base);  //actualiza com o valor actual
			dSprt++;
		}
}

void draw_sprite_rotated(Sprite *sprt, char *base, int angle)
{
	char* dSprt = sprt->map;
	char* bgMap = sprt->bgmap;
	
	while(angle < 0) angle += 360;
	while(angle >= 360) angle -= 360;

	int i, k;
	if(bgMap)
	{
		//guarda uma copia do background onde vai ser desenhada a sprite
		for(i=0; i < sprt->height; i++)
			for(k=0; k < sprt->width; k++)
				if (angle==0 || angle==180 || angle==360) //caso as dimensoes vertical e horizontal da sprite se mantenham
					*(bgMap + sprt->width*i + k) = get_pixel(sprt->x+k, sprt->y+i, base);
				else
					*(bgMap + sprt->width*k + i) = get_pixel(sprt->x+k, sprt->y+i, base);
	}
			
	//desenhar a sprite no sitio certo conforme o angulo		
	for(i=0; i < sprt->height; i++)
		for(k=0; k < sprt->width; k++)
		{
			if(*dSprt != 0)
				switch(angle)
				{
					case 90:	
						set_pixel(sprt->x+i,sprt->y+k, *dSprt, base);//actualiza com o valor actual
						break;
					case 180:
						set_pixel(sprt->x+sprt->width-k,sprt->y+sprt->height-i, *dSprt, base); //actualiza com o valor actual
						break;
					case 270:
						set_pixel(sprt->x+sprt->height-i,sprt->y+sprt->width-k, *dSprt, base); //actualiza com o valor actual
						break;
					default: //angulo 0 e 360
						set_pixel(sprt->x+k,sprt->y+i, *dSprt, base);  //actualiza com o valor actual
						break;
				}
					
			dSprt++;
		}
}

void draw_sprite_scaled(Sprite *sprt, char *base, int width, int height)
{
	char *dSprt = sprt->map;
	char* bgMap = sprt->bgmap;
	if(bgMap)
	{
		bgMap = realloc(bgMap, width * height);
		sprt->bgmap = bgMap;
	}
	
	double dx = (double)sprt->width / width;
	double dy = (double)sprt->height / height;
	
	int i,k;
	for(i = 0; i < height; i++)
		for(k = 0; k < width; k++)
		{
			if(bgMap) *(bgMap + width*i + k) = get_pixel(sprt->x+k, sprt->y+i, base); //guarda o valor antigo
			
			int map_x = dx*k + 0.5;
			int map_y = dy*i + 0.5;
			int color = *(dSprt + sprt->width*map_y + map_x);
			if(color != 0) set_pixel(sprt->x+k,sprt->y+i, color, base);  //actualiza com o valor mapeado
		}
}

void draw_sprite_rotated_scaled(Sprite *sprt, char *base, int angle, int width, int height)
{
	char* dSprt = sprt->map;
	char* bgMap = sprt->bgmap;
	if(bgMap)
	{
		bgMap = realloc(bgMap, width * height);
		sprt->bgmap = bgMap;
	}

	while(angle < 0) angle += 360;
	while(angle >= 360) angle -= 360;
	double dx = (double)sprt->width / width;
	double dy = (double)sprt->height / height;

	int i, k;

	if(bgMap)
	{
		//guarda uma copia do background onde vai ser desenhada a sprite
		for(i=0; i < height; i++)
			for(k=0; k < width; k++)
				if (angle==0 || angle==180) //caso as dimensoes vertical e horizontal da sprite se mantenham
					*(bgMap + width*i + k) = get_pixel(sprt->x+k, sprt->y+i, base);
				else
					*(bgMap + width*i + k) = get_pixel(sprt->x+i, sprt->y+k, base);
	}
	


	//desenhar a sprite no sitio certo conforme o angulo		
	for(i=0; i < height; i++)
		for(k = 0; k < width; k++)
		{
			int map_x = dx*k + 0.5;
			int map_y = dy*i + 0.5;
			int color = *(dSprt + sprt->width*map_y + map_x);
			if(color != 0)
				switch(angle)
				{
					case 90:
						set_pixel(sprt->x+i,sprt->y+k, color, base);//actualiza com o valor actual
						break;
					case 180:
						set_pixel(sprt->x+width-k,sprt->y+height-i, color, base); //actualiza com o valor actual
						break;
					case 270:
						set_pixel(sprt->x+height-i,sprt->y+width-k, color, base); //actualiza com o valor actual
						break;
					default: //angulo 0 e 360
						set_pixel(sprt->x+k,sprt->y+i, color, base);  //actualiza com o valor actual
						break;
				}
		}
}

void draw_mosaic(Sprite *sprt, char *base)
{
	int x_original = sprt->x;
	int y_original = sprt->y;
	sprt->x = sprt->y = 0;

	int n_sprites_x = HRES / sprt->width;
	int n_sprites_y = VRES / sprt->height;
	
	int i,k;
	for(i = 0; i <= n_sprites_y; i++)
	{
		for(k = 0; k <= n_sprites_x; k++)
		{

			draw_sprite(sprt, base);
			sprt->x += sprt->width;
		}
		sprt->x = 0;
		sprt->y +=sprt->height;
	}

	sprt->x = x_original;
	sprt->y = y_original;
}

void delete_sprite(Sprite *sprt, char *base)
{
	char* bgMap = sprt->bgmap;
	if(!bgMap) return;

	int i, k;
	for(i=0; i < sprt->height; i++)
		for(k=0; k < sprt->width; k++)
			set_pixel(sprt->x+k,sprt->y+i, *(bgMap + sprt->width*i + k), base);
		
}

void delete_sprite_rotated(Sprite *sprt, char *base, int angle)
{
	char* bgMap = sprt->bgmap;
	if(!bgMap) return;
	
	while(angle < 0) angle += 360;
	while(angle >= 360) angle -= 360;

	int i, k;
	for(i=0; i < sprt->height; i++)
		for(k=0; k < sprt->width; k++)
			if (angle==0 || angle==180) //caso as dimensoes vertical e horizontal da sprite se mantenham
				set_pixel(sprt->x+k,sprt->y+i, *(bgMap + sprt->width*i + k), base);
			else
				set_pixel(sprt->x+i,sprt->y+k, *(bgMap + sprt->width*i + k), base);
		
}

void delete_sprite_scaled(Sprite *sprt, char *base, int width, int height)
{
	char* bgMap = sprt->bgmap;
	if(!bgMap) return;
	
	int i, k;
	for(i = 0; i < height; i++)
		for(k = 0; k < width; k++)
			set_pixel(sprt->x+k,sprt->y+i, *(bgMap + width*i + k), base);

	bgMap = realloc(bgMap, sprt->width * sprt->height);
	sprt->bgmap = bgMap;		
}

void destroy_sprite(Sprite *sprt)
{
	if (!sprt) return;	
	free(sprt->map);
	free(sprt->bgmap);
	free(sprt);
}

void animate_sprite(Sprite *sprt, char *base)
{
	move_sprite(sprt, sprt->x + sprt->xspeed, sprt->y + sprt->yspeed, base);
}

void animate_sprite_rotated(Sprite *sprt, char *base, int angle)
{
	if (sprt->bgmap != NULL)
	delete_sprite_rotated(sprt, base, angle);
	sprt->x += sprt->xspeed;
	sprt->y += sprt->yspeed;
	draw_sprite_rotated(sprt, base, angle);
	//delay(20);
}

void move_sprite(Sprite *sprt, int x, int y, char *base)
{
	int xi = (x > sprt->x) ? sprt->x : x;
	int yi = (y > sprt->y) ? sprt->y : y;
	int xf = (x > sprt->x) ? x + sprt->width : sprt->x + sprt->width;
	int yf = (y > sprt->y) ? y + sprt->height : sprt->y + sprt->height;
	
	video_buffer = realloc(video_buffer, HRES * VRES);
	flip_buffer_partial(video_buffer, base, xi, yi, xf - xi, yf - yi);
	
	delete_sprite(sprt, video_buffer);
	sprt->x = x;
	sprt->y = y;
	draw_sprite(sprt, video_buffer);
	
	flip_buffer_partial(base, video_buffer, xi, yi, xf - xi, yf - yi);
}

void flip_buffer(char* dest, char* src)
{
	memcpy(dest, src, HRES*VRES);
}

void flip_buffer_partial(char* dest, char* src, int xi, int yi, int width, int height)
{
	if(xi + width >= HRES)
		width = HRES - xi;
	if(xi < 0) {
		width += xi;
		xi = 0;
	}
	if(yi + height >= VRES)
		height = VRES - yi;
	if(yi < 0) {
		height += yi;
		yi = 0;
	}

	int i;
	for (i = 0; i < height; i++)
		memcpy(dest + (yi+i)*HRES + xi, src + (yi+i)*HRES + xi, width);
}
