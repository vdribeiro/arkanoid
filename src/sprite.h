#ifndef _SPRITE_H_
#define _SPRITE_H_

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "video-graphics.h"


/** @defgroup sprite Sprite
 * @{
 *
 * Sprite related functions
 */

/** A Sprite is an "object" that contains all needed information to
 * create, animate, and destroy a pixmap.  The functions assume that
 * the background is BLACK and they take into account collision with
 * other graphical objects or the screen limits. 
 */
typedef struct {
	int x, y;            ///< current sprite position
	int width, height;   ///< sprite dimensions
	int xspeed, yspeed;  ///< sprite current speed in the x and y direction
	char *map;           ///< the sprite pixmap (use read_xpm())
	char *bgmap;		   ///<the sprite background map
} Sprite;

#define MAX_SPEED 5    /**< each sprite maximum speed in pixels/frame */
#define RIGHT_HIT 2    /**< collision with right block (WHITE)  */
#define LEFT_HIT 3     /**< collision with left block (WHITE)   */
#define MOUSE_HIT 4    /**< collision with mouse (LIGHTMAGENTA) */

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
char *read_xpm(char *map[], int *width, int *height);

/** Creates with 0 speed and position, a new sprite with pixmap "pic", in
 * memory whose address is "base".
 * Returns NULL on invalid pixmap.
 */
Sprite* create_sprite(char *pic[]);
Sprite* create_unbuffered_sprite(char *pic[]);
Sprite* capture_screen(int x_ini, int y_ini, int width, int height, char* base);

/** draws a sprite in graphics memory
* the sprite's background color isn't drawn 
*/
void draw_sprite(Sprite *sprt, char *base);
void update_sprite(Sprite* sprt, char* base);

/** draws a sprite in graphics memory
* rotating the sprite with a certain angle
* the sprite's background color isn't drawn 
*/
void draw_sprite_rotated(Sprite *sprt, char *base, int angle);

/** draws a sprite in graphics memory
* scaling the sprite with a certain angle
* the sprite's background color isn't drawn 
*/
void draw_sprite_scaled(Sprite *sprt, char *base, int width, int height);

void draw_sprite_rotated_scaled(Sprite *sprt, char *base, int angle, int width, int height);

/** repeats the sprite 'sprt' all over the background */
void draw_mosaic(Sprite* sprt, char *base);

/** Removes the sprite from the screen if it hasn't been modified */
void delete_sprite(Sprite *sprt, char *base);

/** Removes the sprite from the screen if it hasn't been rotated */
void delete_sprite_rotated(Sprite *sprt, char *base, int angle);

/** Removes the sprite from the screen if it hasn't been scaled */
void delete_sprite_scaled(Sprite *sprt, char *base, int width, int height);

/** Move the Sprite acording to the speed of 'sprt'*/
void animate_sprite(Sprite *sprt, char *base);

/** Move the Sprite acording to the speed of 'sprt' and also rotate it by the 'angle' */
void animate_sprite_rotated(Sprite *sprt, char *base, int angle);

/** moves the sprite 'sprt' to the position (x,y) */
void move_sprite(Sprite *sprt, int x, int y, char *base);

/** The "sprt" sprite is erased from memory whose address is "base"
 * and used resources released.
 */
void destroy_sprite(Sprite *sprt);

/** Move in memory whose address is 'base', the 'sprt' cursor, a
 * standard sprite, from its current position to a new position
 * 'xstep' and 'ystep' away.  
 * The cursor is erased from its present position in xor-mode and draw
 * in the new position also oin xor-mode.
 * The first time this function is called it only draws the cursor at the
 * current position.
 */
void move_cursor(Sprite *fig, int xstep, int ystep, char *base);

/** Copies the graphics memory buffer from the src to dest. */
void flip_buffer(char* dest, char* src);

/** Copies a portion of the graphics memory buffer from the src to dest,
* 'xi' and 'yi' are the starting point of the rectangle and width and height defines it. 
*/
void flip_buffer_partial(char* dest, char* src, int xi, int yi, int width, int height);


/** @} end of sprite */

#endif
