#ifndef _INTS_H_
#define _INTS_H_

#include <dos.h>
#include <go32.h>
#include <dpmi.h>

/** @defgroup interrupts Interrupts
 * @{
 *
 * Interrupt related functions
 */

/** @name PIC registers address */
/*@{*/ 
#define EOI       0x20  ///< End Of Interrupt Command */
#define PIC1_CMD  0x20  ///< PIC1 register address to send EOI */
#define PIC2_CMD  0xA0  ///< PIC2 register address to send EOI */
#define PIC1_MASK 0x21  ///< PIC1 register address to read/write interrupt mask */
#define PIC2_MASK 0xA1  ///< PIC2 register address to read/write interrupt mask */
/*@}*/ 


/** @name PC Interrupts */
/*@{*/
#define T0_IRQ    0  ///< Timer 0       
#define KBD_IRQ   1  ///< keyboard       
#define COM2_IRQ  3  ///< serial port 2 
#define COM1_IRQ  4  ///< serial port 1  
#define FLP_IRQ   6  ///< floppy         
#define LPT_IRQ   7  ///< parallel port  
#define RTC_IRQ   8  ///< realtime clock 
#define MOUSE_IRQ 12 ///< mouse          
#define FPU_IRQ   13 ///< Floating point 
#define DISK1_IRQ 14 ///< Hard disk controller 1   
#define DISK2_IRQ 15 ///< Hard disk controller 2   
/*@}*/ 

/** @name Some usefull definitions */
/*@{*/
/** Gives mask bit associated with a given irq.
 * <em>Warning</em>, if irq > 7 the result must be applied to PIC2
 */
#define IRQ_MASK_BIT(irq) ((irq) < 8 ? 1 << (irq) : (1 << (irq)) >> 8)

/** gives the vector associated with a given IRQ */
#define IRQ_VECTOR(irq) ((irq) < 8 ? (irq) + 0x08 : (irq) - 8 + 0x70)
/*@}*/

/** Installs the ASM written function 'irq_func' as the interrupt
 * handler associated with interrupt 'irq', updating the 'old_irq'
 * structure with the current interrupt handler.  Returns 0 on success
 * or -1 on error, in which case the original interrupt handler
 * remains installed.
 * 
 * See reinstall_asm_irq_handler() to reinstall the original handler.
 */
int install_asm_irq_handler(int irq, void (*irq_func)(void),
			    _go32_dpmi_seginfo *old_irq);

/** Installs the C written 'irq_func' interrupt handler at the
 * interrupt handler associated with interrupt 'irq', updating the
 * old_irq structure with the current interrupt handler.  Returns 0 on
 * success or -1 on error, in which case the original interrupt
 * handler remains installed.
 * 
 * See reinstall_c_irq_handler() to reinstall the original handler.
 */
int install_c_irq_handler(int irq, void (*irq_func)(void),
			    _go32_dpmi_seginfo *old_irq, _go32_dpmi_seginfo *new_irq);

/** Reinstalls the interrupt handler specified in 'old_irq' for interrupt
 * 'irq'. See install_asm_irq_handler().
 */
void reinstall_asm_irq_handler(int irq, _go32_dpmi_seginfo *old_irq);

/** Reinstalls the interrupt handler specified in 'old_irq' for interrupt
 * 'irq'. See install_c_irq_handler().
 */
void reinstall_c_irq_handler(int irq, _go32_dpmi_seginfo *old_irq,_go32_dpmi_seginfo *new_irq);

/** Mask the bit corresponding to 'irq', disabling its interrupts.
 * The correct PIC is used.
 */
void mask_pic(int irq);

/** Unmask the bit corresponding to 'irq', enabling its interrupts.
 * The correct PIC is used.
*/
void unmask_pic(int irq);

/** @} end of interrupts */

#endif
