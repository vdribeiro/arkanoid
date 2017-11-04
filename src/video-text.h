#ifndef _VIDEO_TEXT_H_
#define _VIDEO_TEXT_H_

#include <sys/farptr.h>
#include <go32.h>

#include "utypes.h"

/** @defgroup VideoText Video-Text
 * @{
 *
 * Video in text mode definitions
 */

 /** Estrutura Posicao.
*   Guarda a informacao relativa a uma posicao
*/
typedef struct {
	int x;
	int y;
	char attributes;
	char ch;
} posicao ;
 
/** @name Screen and video memory definitions in text mode */
/*@{*/
#define VIDEO_TEXT 0xB8000 ///< Video address in text mode
#define WIDTH 80	   ///< Number of rows
#define HEIGHT 25	   ///< Number of columns
/*@}*/

/** @name Bit meaning in attribute byte
* (see color definitions in system header file conio.h)
*/
/*@{*/
#define BLUE_FOREG  (1 << 0)  ///< Foreground blue bit
#define GREEN_FOREG (1 << 1)  ///< Foreground green bit
#define RED_FOREG   (1 << 2)  ///< Foreground red bit

#define STRONG      (1 << 3)  ///< Foreground intensity bit

#define BLUE_BACK   (1 << 4)  ///< Background blue bit
#define GREEN_BACK  (1 << 5)  ///< Background gren bit
#define RED_BACK    (1 << 6)  ///< Background red bit

#define AQUA_BACK		(BLUE_BACK | GREEN_BACK)
#define PURPLE_BACK		(BLUE_BACK | RED_BACK)
#define YELLOW_BACK		(GREEN_BACK | RED_BACK)
#define LIGHTGRAY_BACK	(BLUE_BACK | GREEN_BACK | RED_BACK)

//#define BLINK       (1 << 7)  ///< already defined in conio.h

#define NORMAL (RED_FOREG | GREEN_FOREG | BLUE_FOREG) ///< handy definition
/*@}*/

/** @name Usefull functions */
/*@{*/

/** Print character ch at position x,y with attributes
* return true if sucessfull
*/
Bool printCharAt(char ch, int x, int y, char attributes);

/** Print character array str at position x,y with attributes
* return true if sucessfull
*/
Bool printStringAt(char *str, int x, int y, char attributes);

/** Print integer num at position x,y with attributes
* No C library function is used.
* return true if sucessfull
*/
Bool printIntAt(int num, int x, int y, char attributes);

/** Draw rectangular frame with left upper corner at position x,y
* with width and height dimensions, decorated with title attributes
* return true if sucessfull
*/
Bool drawFrame(char *title, char attributes, int x, int y, int width, int height);

/** Retorna o caracter de uma dada posicao
*
* @param x Coordenada x
* @param y Coordenada y
* @return Caracter
*/
char getCharAt(int x, int y);

/** Retorna o atributo de uma dada posicao
*
* @param x Coordenada x
* @param y Coordenada y
* @return Atributo
*/
char getAttributesAt(int x, int y);

/** Clear Screen
* return true if sucessfull
*/
Bool clear();

/** Calcula o tamanho de um array de caracteres
*
* @param str Apontador de char
* @return Tamanho
*/
uint stringLength(char *str);

/**@} end of VideoText */

#endif
