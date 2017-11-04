#ifndef _KBC_H
#define _KBC_H

#include <pc.h>
#include <stdio.h>
#include <dos.h>

#include "ints.h"
#include "utypes.h"

/** @defgroup KeyboardController KeyboardController
 * @{
 *
 * Keyboard and Mouse related functions
 */

#define KBC_TIMEOUT 30 ///< maximum amount of milliseconds to wait for KBC response/availability

/** @name The hardware register addresses */
/*@{*/ 
#define CMD_REG   0x64  ///< command register (read only)
#define STAT_REG  0x64  ///< status register  (write only)
#define DATA_REG  0x60  ///< data register    (read/write)
/*@}*/ 

/** @name Bit meanings in the status register */
/*@{*/ 
#define OBF     (1 << 0) ///< Output Buffer Full
#define IBF     (1 << 1) ///< Input Buffer Full
#define AUX     (1 << 5) ///< 1 if data at DATA_REG came from mouse interface
#define PAR_ERR (1 << 7) ///< Parity Error
#define TO_ERR  (1 << 6) ///< Timeout Error
/*@}*/ 

/** @name "Historical" commands to send to DATA_REG, response in DATA_REG */
/*@{*/ 
#define ResetKBC   0xFF ///< Reset the KBC interface
#define DefaultKBD 0xF6 ///< Set default scancode and repeat rate
#define DisableKBD 0xF5 ///< Historical Disable keyboard
#define EnableKBD  0xF4 ///< Historical clear buffer and enable keyboard
#define KBDRate    0xF3 ///< Keyboard repeat rate, one arg, bits 0-4 repeat rate, bits 5-6 start delay
#define WriteLEDS  0xED ///< Turn on/off KBD LEDS, one arg, one bit per led, bits 0-2, 0-off, 1-on
#define SCROLL_LOCK (1 << 0) ///< SCROLL_LOCK led for WriteLEDS command
#define NUM_LOCK    (1 << 1) ///< NUM__LOCK led for WriteLEDS command
#define CAPS_LOCK   (1 << 2) ///< CAPS_LOCK led for WriteLEDS command
/*@}*/ 

/** @name Response codes to commands/data written in DATA_REG */
/*@{*/ 
#define ACK        0xFA ///< Acknowledge OK response
#define Resend     0xFE ///< Resend error response; on multibyte commands the whole command must be resent from the beginning
#define Error      0xFC ///< Error
/*@}*/ 

/** @name Command codes to send to CMD_REG, args, if exists, to DATA_REG, return values in DATA_REG */
/*@{*/ 
#define ReadCommand   0x20 ///< Read Command_Byte
#define WriteCommand  0x60 ///< Write Command_Byte, one arg
#define DisableMouse  0xA7 ///< Disable Mouse Interface 
#define EnableMouse   0xA8 ///< Enable Mouse Interface 
#define TestMouse     0xA9 ///< Test Mouse Interface, returns 0 if OK
#define Test8042      0xAA ///< Test KBC, returns 0x55 if sucess, 0xFC on error
#define TestKBD       0xAB ///< Test mouse, returns 0 if OK
#define NDisableKBD   0xAD ///< New (versus historical) Disable keyboard
#define NEnableKBD    0xAE ///< New (versus historical) Enable keyboard
#define WriteMouse    0xD4 ///< Write Mouse, one arg, see "Commands for the Mouse"
#define ResetPC       0xFE ///< Reset PC (reboot)
/*@}*/ 

/** @name Bit meanings in the Command_Byte
 * (read/write with command ReadCommand/WriteCommand)
*/
/*@{*/ 
#define INT_1    (1 << 0)  ///< Enable generation of interrupts from keyboard
#define INT_12   (1 << 1)  ///< Enable generation of interrupts from mouse
#define EN_KBD   (1 << 4)  ///< Enable keyboard interface
#define EN_MOUSE (1 << 5)  ///< Enable mouse interface
/*@}*/
 
/** @name Commands to the Mouse
 * Prefaced with command WriteMouse
 */
/*@{*/ 
#define MouseReset     0xFF
#define MouseDefaults  0xF6
#define StreamDisable  0xF5
#define StreamEnable 0xF4
#define SampleRate   0xF3
#define ReadDevice   0xF2
#define RemoteMode   0xF0
#define MouseReadData  0xEB
#define EnterStream  0xEA
#define StatusRequest 0xE9
#define Resolution   0xE8
#define Scaling2     0xE7
#define Scaling1     0xE6
/*@}*/ 

/** @name Bit meaning in first byte of mouse data packet
 */
/*@{*/ 
#define YOVF (1 << 7) ///< X overflow
#define XOVF (1 << 6) ///< Y overflow
#define YSGN (1 << 5) ///< Sign of data packet byte 3 (y movement)
#define XSGN (1 << 4) ///< Sign of data packet byte 2 (x movement)
#define MBT  (1 << 2) ///< Middle button
#define RBT  (1 << 1) ///< Right button
#define LBT  (1 << 0) ///< Left button
/*@}*/ 

/** @name Utility macros to inquiry first byte of mouse data packet */
/*@{*/ 
#define LEFT_BUTTON(dt)   (((dt) & LBT) == LBT) ///< 1 if left button pressed
#define RIGHT_BUTTON(dt)  (((dt) & RBT) == RBT) ///< 1 if right button pressed
#define MIDDLE_BUTTON(dt) (((dt) & MBT) == MBT) ///< 1 if middle button pressed

#define Y_OVERFLOW(dt)    (((dt) & XOVF) == XOVF) ///< 1 if overflow in x movement
#define X_OVERFLOW(dt)    (((dt) & YOVF) == YOVF) ///< 1 if overflow in y movement

#define X_SIGN(dt)        (((dt) & XSGN) == XSGN ? -1 : 1) ///< 1 if positive x movement, -1 if negative
#define Y_SIGN(dt)        (((dt) & YSGN) == YSGN ? -1 : 1) ///< 1 if positive y movement, -1 if negative
/*@}*/ 

/** Initializes the KeyBoard Controller. If 'debug' is not 0 every
 * read or write to the KBC, together with the read or writed data is
 * echoed to the screen. In this mode the function name being executed
 * is printed followed by a ':', add=xx means the I/O address written,
 * and data=xx the data read or written.
 *
 *  A 1 is returned if a mouse is found.
 *
 * The function starts to flush any pending data in the KBC, reading
 * the data register until a timeout is received.
 *
 * Then it enables the KBC mouse interface, send a MouseReset to the
 * mouse and checks for the correct answer (0xFA, 0xAA then 0x00).  If
 * the mouse was detected, data streaming is disabled (StreamDisable),
 * SampleRate is set to 100, Resolution is set to 8 counts/mm, Scaling
 * is set to 1:1, the mouse interrupts generation are enabled at the
 * KBC (not at the PIC!), and data streaming enabled
 * (StreamEnable). All this (but StreamEnable) shouldn't be necessary,
 * as MouseReset is supposed to do it properly.
 *
 * The KBDRate is also set to its maximum speed.
 *
 * Uses write_kbc_cmd(), write_kbc_data(), read_kbc() and write_aux()
 *
 * Example Output generated with debug = 1:
 * <code><pre>
read_kbc: timeout
write_kbc: add=64 data=d4
write_kbc: add=60 data=ff
read_kbc: data=fa
read_kbc: data=aa
read_kbc: data= 0
write_kbc: add=60 data=f5
read_kbc: data=fa
write_kbc: add=64 data=a7
write_kbc: add=64 data=d4
write_kbc: add=60 data=f5
read_kbc: data=fa
write_kbc: add=60 data=f3
read_kbc: data=fa
write_kbc: add=60 data= 0
read_kbc: data=fa
write_kbc: add=64 data=d4
write_kbc: add=60 data=f3
read_kbc: data=fa
write_kbc: add=64 data=d4
write_kbc: add=60 data= a
read_kbc: data=fa
write_kbc: add=64 data=d4
write_kbc: add=60 data=e8
read_kbc: data=fa
write_kbc: add=64 data=d4
write_kbc: add=60 data= 1
read_kbc: data=fa
write_kbc: add=64 data=20
read_kbc: data=45
write_kbc: add=64 data=60
write_kbc: add=60 data=47
read_kbc: timeout
write_kbc: add=60 data=f4
read_kbc: data=fa
write_kbc: add=64 data=a8
write_kbc: add=64 data=d4
write_kbc: add=60 data=f4
read_kbc: data=fa
 * </pre></code>
 */
int kbc_init(int debug);

/** Disable the mouse, sending a StreamDisable to the mouse, then
 * disables generation of mouse interrupts at the KBC, sends a
 * DisableMouse to the KBC and finnaly disable mouse interrupts at the
 * PIC2
 */
void mouse_disable(void);

/** Write 'data' to the command register, returning 0 for OK or -1 on
 * timeout.
 * 
 * Uses write_kbc()
 */
int  write_kbc_cmd( unsigned data);
  
/** Write 'data' to the data register, returning ACK, Error or -1 on timeout.
 *
 * If a Resend reply is received from the KBC, it retries up to 3 times to
 * write the data.
 *
 * Uses write_kbc() and read_kbc()
 */
int  write_kbc_data( unsigned data);

/** Send 'cmd' to the mouse, prefacing it with the WriteMouse command
 * to the command register.
 *
 * If a Resend reply is received from the KBC, it retries up to 3 times to
 * write 'cmd', resending the whole sequence.
 *
 * Uses write_kbc_cmd(), write_kbc() and read_kbc()
 */ 
int  write_aux(unsigned cmd);

/** Returns the data read a the data register.
*
*  Waits for OBF on status register up to KBC_TIMEOUT milliseconds,
*  then reads and returns the data read at the data register. Returns
*  -1 on timeout or transmission error.
*/
int  read_kbc(void);

/** Write 'data' at IO address 'adr' , returning -1 on error or timeout.
 *
 * Waits for IBF on status register , than writes data to 'adr',
 * return -1 on timeout.
 */
int  write_kbc(unsigned adr, unsigned data);

/** @} end of KeyboardController */

#endif
