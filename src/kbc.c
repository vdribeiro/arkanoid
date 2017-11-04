#include "kbc.h"

static int de=0;

int kbc_init(int debug)
{
	de=debug;
	int tmp=read_kbc();
	while(tmp!=-1)
	{
        tmp=read_kbc();
	}
    //habilita interface do rato
	write_kbc_cmd(EnableMouse);
	//Reinicializa rato
	write_aux(MouseReset);
	//aguarda 500 ms
	delay(500);
	//verifica bytes
    if((read_kbc()!=0xAA) || (read_kbc()!=0x00))
		return -1;
	//envia o StreamDisable para  o rato
	write_aux(StreamDisable);
	//envia SampleRate para o rato
	write_aux(SampleRate);
	//100 samples/sec
	write_aux(100); 
	//envia Resolution para o rato
	write_aux(Resolution);
	//8 counts/mm
	write_aux(0x03); 
	//envia Scaling para o rato(Scaling 1:1)
	write_aux(Scaling1);
	//lê command_byte do KBC que devolve um valor
	write_kbc_cmd(ReadCommand);
	//lê esse valor do KBC para command_byte
    Byte le=read_kbc();
    Byte command_byte=0;
	//testa le
	if(le==0xFF)
		//habilita  a interface do KBD, rato e geraçao das interrupçoes 1 e 12
		command_byte=((((command_byte|INT_12) | INT_1) | EN_KBD) | EN_MOUSE);
	else
	{
		//habilita apenas interrupçao 12
		command_byte=le | INT_12;
	}
	//envia WriteCommand para o KBC
	write_kbc_cmd(WriteCommand);
	write_kbc_data(command_byte);
	//envia StreamEnable para o KBC
	write_aux(StreamEnable);
	//envia KBDRate para o KBC
	write_kbc_data(KBDRate);
	//programa ritmo
    write_kbc_data(0x1F);
	de=0;
    return 1;
}

int write_aux(unsigned cmd)
{
	int tentativas = 3;
	int read;
	
	while(tentativas>0)
	{
		//escreve no kbc writeMouse(comando para o rato) e cmd que recebe como argumento e testa se da erro
		if(write_kbc(CMD_REG,WriteMouse)== -1)
			return -1;
		else if(write_kbc(DATA_REG, cmd) == -1)	
				return -1;
		else
		{
			//le KBC
			read = read_kbc();
			//ainda nao respondeu
			if(read==Resend)
				tentativas--;
			//recebeu correctamente os dados
			if(read==ACK)
				return ACK;
			//Erro
			if(read==Error)
				return Error;
		}
	}
	//fim das tentativas
	return -1;
}

int write_kbc_data(unsigned data)
{
	int tentativas = 3;
	int read;
	
	while(tentativas>0)
	{
		//escreve no DATA_REG DATA
		write_kbc(DATA_REG,data);
		//le KBC
		read = read_kbc();
		//ainda nao respondeu
		if(read==Resend)
			tentativas--;
		//recebeu correctamente os dados
		if(read==ACK)
		{
			if(de==1)
				printf("write_kbc: add=60 data=%2x\n",data);
			return ACK;
		}
		//Erro
		if(read==Error)
			return Error;
	}
	//fim das tentativas
	return -1;		
}

int  write_kbc_cmd(unsigned data)
{
	return write_kbc(CMD_REG,data);
}

int  read_kbc(void)
{
	int data, stat, i=0;
	while(i < KBC_TIMEOUT)
	{
		stat = inportb(STAT_REG);
		//ciclo enquanto buffer de saida cheio
		if((stat & OBF) !=0)
		{
			data = inportb(DATA_REG);
			//ou erro de paridade ou timeout
			if((stat & (PAR_ERR | TO_ERR)) == 0)
			{
				if(de==1)
					printf("read_kbc: data=%2x\n",data);
				return data;
			}
			else
			{
				if(de==1)
					printf("read_kbc: erro\n");
				return -1;
			}
		}
		//pausa de 1ms
		delay(1);
		i++;
        }
        if(de==1)
			printf("read_kbc: timeout\n");
		return -1;
}

int write_kbc(unsigned adr, unsigned data)
{
	int stat;
	int time = KBC_TIMEOUT;
	while(time!=0)
	{
		stat = inportb(STAT_REG);
		//ciclo enquanto buffer de entrada vazio
		if((stat & IBF) == 0)
		{
			if(de==1)
				printf("write_kbc: add=%2x data=%2x\n",adr,data);
			outportb(adr,data);
			return 0;
		}
		time--;
		//pausa de 1ms
		delay(1);
	}
	//terminou o tempo
	return -1;
}

void mouse_disable(void)
{
	//envia o StreamDisable para  o rato
	write_aux(StreamDisable);
	//inibe o rato	
	write_kbc_cmd(DisableMouse);
	//lê command_byte do KBC que devolve um valor
	write_kbc_cmd(ReadCommand);
	//lê esse valor do KBC para read
	int read=read_kbc();   
  
	if(read != -1)
	{
		//desabilita interrupçao 12 do rato no kbc
		read &= ~(INT_12);
	}
	else
	{
		//default do kbc
		read=0x65;
	}
	//escreve command_byte no KBC
	write_kbc(CMD_REG,WriteCommand);
	//escreve read no DATA_REG
	write_kbc(DATA_REG,read);
	//desabilita interrupçao 12 do rato no PIC2
	mask_pic(MOUSE_IRQ);
}
