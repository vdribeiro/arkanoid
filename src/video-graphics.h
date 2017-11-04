#ifndef _VIDEO_GRAPHICS_H_
#define _VIDEO_GRAPHICS_H_

#include <dpmi.h>
#include <go32.h>
#include <sys\nearptr.h>
#include <sys/farptr.h>

//#include "codepage.h"
#include "utypes.h"

/** @defgroup video Video 
 * @{
 *
 * Entering/leaving/utility video functions
 */

extern int HRES;  /**< global variable with the horizontal resolution in pixels */
extern int VRES;  /**< global variable with the vertical resolution in pixels */
extern ulong VIDEO_MEM; 

/** @name Screen and video memory definitions*/
/*@{*/
#define AH 0x4F
/*@{*/

#define MODE_640x480 0x101
#define MODE_800x600 0x103
#define MODE_1024x768 0x105
#define MODE_1280x1024 0x107
#define sgn(x) ((x<0)?-1:((x>0)?1:0));

/** Obtem endereco da memoria de video
*/
ulong get_videoadd(int mode);

/** Enter graphics mode, enabling near pointers and mapping video physical memory
 * to program virtual address.
 *
 * Returns a generic pointer pointing to video memory address or NULL on error. 
 * "mode" specifies the VESA graphics mode to use, and
 * the mapping information is returned through "map".
 *
 * Also initializes two global variables, VRES and HRES,
 */
char *enter_graphics(int mode, __dpmi_meminfo *map);

/** Unmap video memory, disable near pointer and returns to text mode
 */
void leave_graphics(__dpmi_meminfo map);

/** Draws a pixel of color "color" at screen coordinates x, y at memory address "base"
 */
void set_pixel(int x, int y, int color, char *base);

/** Returns the pixel color at screen coordinates x, y at memory address "base"
 */
int get_pixel(int x, int y, char *base);

/** Set graphics memory at memory address "base" to "color".
 */
void clear_screen(char color, char *base);

/** Draw a line of color "color" between point (xi,yi) and (xf,yf) at memory address "base"
*/
void draw_line(int xi, int yi, int xf, int yf, int color, char *base);

/** Draw the character "c" in graphics mode starting at point (x,y) at memory address "base"
*/
void drawCharAt(char c, int x, int y, int fore_color, int back_color,
				int char_scale, char *video_base, char *table);

/** Draw a string "str" in graphics mode starting at point (x,y) at memory address "base"
*/
void drawStringAt(char* str, int x, int y, int fore_color, int back_color,
				int char_scale, char *video_base, char *table);

/** Draw a number "num" in graphics mode starting at point (x,y) at memory address "base"
*/
void drawIntAt(int num, int x, int y, int fore_color, int back_color,
				int char_scale, char *video_base, char *table);

/** Draw a group of lines creating a rectangle of color "color" between point (xi,yi) and (xf,yf) at memory address "base"
*/
void drawRectangle(int xi, int yi, int xf, int yf, int color, char *base);

/** @} end of video */ 

#endif
