#include "ints.h"

int install_asm_irq_handler(int irq, void (*irq_func)(void),
			    _go32_dpmi_seginfo *old_irq)
{
	//transforma o IRQ numa posiçao do vector
	int vector_irq = IRQ_VECTOR(irq);
	// variavel para o endereço do novo handler
	_go32_dpmi_seginfo new_irq;
	
	//verifica se a funçao foi bem sucedida
	_go32_dpmi_get_protected_mode_interrupt_vector(vector_irq, old_irq);	
	new_irq.pm_selector = _go32_my_cs( );			//selector do codigo
	new_irq.pm_offset = (unsigned long) irq_func;	//endereço do novo handler
	
	//verifica se a funçao foi bem sucedida
	_go32_dpmi_set_protected_mode_interrupt_vector(vector_irq, &new_irq);
	return 0;
}

int install_c_irq_handler(int irq, void (*irq_func)(void),
			    _go32_dpmi_seginfo *old_irq, _go32_dpmi_seginfo *new_irq)
{
	int vector_irq = IRQ_VECTOR(irq);
	
	//verifica se a funçao foi bem sucedida
	if( (_go32_dpmi_get_protected_mode_interrupt_vector(vector_irq, old_irq)) !=0)
	{
		return -1;
	}
	new_irq->pm_selector = _go32_my_cs();
	new_irq->pm_offset = (unsigned long) irq_func;
	//Prepara a funçao em assembly que chama a funçao em C
	_go32_dpmi_allocate_iret_wrapper(new_irq);
	//verifica se a funçao foi bem sucedida
	if( (_go32_dpmi_set_protected_mode_interrupt_vector(vector_irq, new_irq)) !=0)
	{
		return -1;
	}
	return 0;
}

void reinstall_asm_irq_handler(int irq, _go32_dpmi_seginfo *old_irq)
{
	int vector_irq = IRQ_VECTOR(irq);
	//reposiçao do handler original
    _go32_dpmi_set_protected_mode_interrupt_vector(vector_irq, old_irq);
}

void reinstall_c_irq_handler(int irq, _go32_dpmi_seginfo *old_irq, _go32_dpmi_seginfo *new_irq)
{
	int vector_irq = IRQ_VECTOR(irq);
	_go32_dpmi_set_protected_mode_interrupt_vector(vector_irq, old_irq);
	_go32_dpmi_free_iret_wrapper(new_irq);
}

void mask_pic(int irq)
{
	//associa o IRQ ao bit a mascarar
	int bit = IRQ_MASK_BIT(irq);
	int tmp;
	//interrupçao no PIC-1
	if(irq<=7)
	{
	    tmp = inportb(PIC1_MASK);
	    bit = bit | tmp;
	    outportb(PIC1_MASK, bit);
	}
	//interrupçao no PIC-2
	else
	{
	    tmp = inportb(PIC2_MASK);
	    bit = bit | tmp;
		outportb(PIC2_MASK, bit);
	}
}

void unmask_pic(int irq)
{
	//associa o IRQ ao bit a mascarar
	int bit = (IRQ_MASK_BIT(irq));
	int tmp;
	//interrupçao no PIC-1
	if(irq<=7)
	{
	   tmp = inportb(PIC1_MASK);
	   bit = ~bit & tmp;
	   outportb(PIC1_MASK, bit);
	}
	//interrupçao no PIC-2
	else
	{
	   tmp = inportb(PIC2_MASK);
	   bit = ~bit & tmp;
	   outportb(PIC2_MASK, bit);
	}
}

