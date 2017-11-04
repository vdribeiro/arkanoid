#include "arkanoid.h"

//variavel para o scancode do teclado
volatile unsigned char scd = 0;

//variaveis para o rato
volatile unsigned char indice = 0;
volatile char array_bytes[3];

//Rotinas em assembly
void handler_teclado(void);
void handler_rato(void);

//musica
//La Bamba
int notas[] = {Do5, Mi5, Do5, Fa5, La5, Sol5, Sol4, Si4, Re5, Fa5, Fa5, Mi5, Re5};
int tempos[] = {500, 300, 300, 500, 300, 800, 300, 300, 300, 300, 300, 300, 300};
Song song = {sizeof(tempos)/sizeof(int), 10, notas, tempos};
int state = FIM;

//rotinas do rtc
void rtc_intc(void)
{	
	static int n_note = 0, time_elapsed = 0;
	
	outportb(RTC_ADDR_REG, RTC_STAT_C);
	Byte causa = inportb(RTC_DATA_REG);
	
	if (causa & RTC_PF)
		time_elapsed++;
	
	/*maquina de estados que inicia ou para a musica
	de acordo com o estado especificado
	e repete a musica quando esta acaba*/
	switch(state)
	{
		case INICIO :
			state = PLAYING;
			programa_timer(song.notes[n_note]);
			speaker_on();
			break;
		case PLAYING: 
			if (song.time[n_note] < time_elapsed)
			{
				state = PAUSE; 
				time_elapsed = 0;
				n_note++; 
				speaker_off();
			}
			break;
		case PAUSE:
			if (song.pause < time_elapsed) 
				time_elapsed = 0;
			if (n_note > song.lenght) 
				state = REPEAT;
			else
			{
				state = PLAYING;
				programa_timer(song.notes[n_note]);
				speaker_on();
			}
			break;
		case REPEAT:
			time_elapsed = 0;
			n_note = 0;
			state = INICIO;
            break;
		case FIM:
			speaker_off();
			time_elapsed = 0;
			n_note = 0;
			break;
	}	
	outportb(PIC1_CMD, EOI);
	outportb(PIC2_CMD, EOI);
}

//apresentacao
void arkaprint(){
	printf("                      M`                                              MM   \n");
	printf("      MM              M                                               Mh   \n");
	printf("     MNM              M                                    NM         M`   \n");
	printf("    s .M             .M      M                             N          M    \n");
	printf("    h  Md            /M    MM                                         M    \n");
	printf("   M   sM     sM M M md M`M    M M Mo   Mo M NM    M M`Md  M    /M MM M    \n");
	printf("  MMMMMMM     hNM    MoN       M   Mh   M`M  `M   M    -M  M`  .M    MM    \n");
	printf(" sN     MM    ms     M- M/     M/  M-   Mm   sM  -M    `M  M   Mo   /-M    \n");
	printf(" M      .M    MM     MM  M`   M-  mM`   Mh   mM   M    N-  M`  MM  M /M    \n");
	printf("Mh       MMMM Mh     M`   MMMdMMMd NM N M    sMMM /MN M    MMMd.MMM  `MMM  \n");
	
/*
	printf("                       **\n");
	printf("      **               **      **	  	**\n");
	printf("     ***               **    ** 	   ***                          **\n");
	printf("    * **               **  ** 	    * **\n");
	printf("   *  **       ***     ****		     *  **    **       ***   ***    *** *******\n");
	printf("  *   **     **   **   ****			  *   **    ****     **   ** **   **   **   **\n");
	printf(" *******     **  **    ** **	   *******    ** **    **  **  **   **   **   **\n");
	printf(" **    **    ****      **   **  **     **   **  **   **  **  **   **   **   **\n");
	printf("**      **   **  **    **	  ** **       ** **     ** **  **  **   **   **   **\n");
	printf("**       *****     ******		 ****       ****	       ***  ***     *** ******* \n"); */
}

//menu e submenus
void menu() {
	//printf("\n ARKANOID\n\n");
	arkaprint();
	printf(" \n ----- Menu de Opcoes -----\n\n");
	printf(" 1 - Jogar\n");
	printf(" 2 - Instrucoes\n");
	printf(" 3 - Opcoes\n");
	printf(" 4 - Pontuacoes\n");
	printf(" 0 - Sair\n");
	printf(" \n");
	printf(" Escolha a sua opcao: ");
}

void instrucoes() {
	printf(" \n ----- Instrucoes -----\n\n");
	printf(" O jogo e formado por uma bola, uma raquete e blocos coloridos.\n");
	printf(" A bola e atirada pressionando espaco e comeca a andar\n");
	printf(" pelo ecra reflectindo sempre que bate num obstaculo.\n");
	printf(" Com a raquete tenta-se bater na bola, movimentando-se para os\n");
	printf(" lados com as teclas A-esquerda e D-direita.\n");
	printf(" A bola ao bater nos blocos faz somar pontos e o nivel\n");
	printf(" termina quando nao houver mais blocos ou perder as vidas.\n");
	printf(" \n Prima enter para continuar\n");
}

void submenu() {
	printf(" \n ----- Opcoes -----\n\n");
	printf(" 1 - Musica e efeitos sonoros on/off\n");
	printf(" 2 - Velocidade da bola\n");
	printf(" ? - Tecla secreta\n");
	printf(" 0 - Retroceder\n");
	printf(" \n");
	printf(" Escolha a sua opcao: ");
}			

//MAIN
int main(int argc, char *argv[])
{

	//pode receber no maximo 1 argumento
	//(podera ser util - numero de blocos por exemplo)
	if (argc > 2)
		return 0;
		
	//Variaveis
	//---------------------------------------------------------------------
	
	//modo gráfico
	__dpmi_meminfo map;
	char *video_mem;
	int mode=0x101; //resolucao
	int HLIM=640, VLIM=480; //limites do ecra
	
	//interrupções
	_go32_dpmi_seginfo old_irq_kbd, old_irq_mouse, old_irq_rtc, new_irq_rtc;
	
	//controlo da musica (1-on/0-off)
	int sfx=1;
	
	//-musicas
	//notas intro - toca antes de mostrar o menu
	int notas_intro[] = {Sol4, La4, Si4};
	int tempos_intro[] = {300, 300, 300};
	Song intro = {sizeof(tempos_intro)/sizeof(int), 10, notas_intro, tempos_intro };
	
	//musica final (game over) - toca quando jogo termina
	int notas_final[] = {La5, Mi5, Mi5, Fas5, Mi5, Sols5, La5};
	int tempos_final[] = {500, 300, 300, 300, 800, 300, 300};
	Song final = {sizeof(tempos_final)/sizeof(int), 5, notas_final, tempos_final };
	
	//musica ao perder uma vida
	int notas_vida[] = {Do6, La5, Fa5, Re5};
	int tempos_vida[] = {100, 100, 100, 100};
	Song vida = {sizeof(tempos_vida)/sizeof(int), 3, notas_vida, tempos_vida };
	
	//som ao bater num obstaculo
	int notas_bate[] = {Sol6};
	int tempos_bate[] = {5};
	Song bate = {sizeof(tempos_bate)/sizeof(int), 10, notas_bate, tempos_bate };
	
	//Musica do Dragon Ball GT (Easter Egg)
	int notas_gt[] = {Sol5, Sol5, Mi5, Fa5, Sol5, La5, Sol5, Fa5, Mi5, Re5, Mi5, Mi5, Do5, Re5, Mi5, Fa5, Mi5, Re5, Do5, Si4, La4, Do5, Fa5, La5, Sol5, Mi5, Fa5, Mi5, Re5, Do5, Re5, Re5, Do5, Si4, Do5};
	int tempos_gt[] = {500, 500, 300, 300, 300, 300, 500, 500, 500, 800, 500, 500, 300, 300, 300, 300, 500, 500, 500, 800, 500, 500, 800, 500, 1000, 1000, 500, 500, 500, 500, 500, 800, 500, 500, 1000};
	Song DBGT = {sizeof(tempos_gt)/sizeof(int), 10, notas_gt, tempos_gt};
	
	//-tempo e delay
	//clock do sistema
	clock_t ti, tf; 
	double tempo_gasto;
	//variaveis para delay
	time_t start;
	time_t current;
	
	//REAL TIME CLOCK
	RTC_TIME tempo_init, tempo_current, tempo_end;
	Byte c;
	float tempo_inicial = 0, tempo = 0, tempo_corrente = 0, tempo_fim = 0;
	
	//jogador
	jogador joga[10]; //estrutura para pontuacoes
	jogador j; //jogador actual
	int vidas=3; //tem 3 vidas
	
	//-tabuleiro
	double velocidade = 5; //velocidade da bola
	//variaveis auxiliares para colisoes
	int posBx = 0;
	int posBy = 0;
	int nblocos=10;
	int existe[50];
	int dez=0;
	int bcnt=0;
	
	//sprites
	Sprite *bola;
	Sprite *raquete;
	Sprite **blocos;
	
	//ficheiro de pontuações
	FILE * f;
	
	//menu
	int opcao=-1, op=-1;
	int com=0;
	
	//incremento
	int i=0;
	
	//Corpo do Programa
	//---------------------------------------------------------------------
	
	//inicializa o vector
	while (i<10) {
		joga[i].pontos=0;
		joga[i].tempo=0;
		strcpy(joga[i].nome, "Nome");
		i++;
	}
	
	//abrir ficheiro para leitura com as pontuações, colocar no array e ordenar
	f = fopen("pontuacoes.txt", "r");
	if(f == NULL)
		return 1;
	readFile(f, joga);
	fclose(f);
	insertionSort(joga); //ordena
	
	//inicia a musica do menu - intro com 3 notas
	play_song(&intro);
	
	//inicializacao do handler RTC para a musica fluir com o menu
	mask_pic(RTC_IRQ);
	install_c_irq_handler(RTC_IRQ,&rtc_intc,&old_irq_rtc,&new_irq_rtc);
	c = read_rtcv(RTC_STAT_B);
	c = c | RTC_PIE | RTC_AIE | RTC_UIE;
	write_rtcv(RTC_STAT_B , c);
	unmask_pic(RTC_IRQ);
	
	//inicia musica
	state = INICIO;
	
	/*teste de mute (descartado)
	if (argv[1] == 'm') {
		state = FIM;
		sfx=0;
	}*/
	
	//menu
	do {
		menu();
		scanf("%i", &opcao);
		switch (opcao)
		{
			case 1:
				printf("\n Escolha uma das seguintes resolucoes:\n");
				printf(" 1-640x480\n");
				printf(" 2-800x600\n");
				printf(" 3-1024x768\n");
				printf(" 4-1280x1024\n");
				scanf("%i", &op);
				/*De acordo com a resolucao sao definidos os limites
				e centrados os objectos do jogo*/
				switch (op)
				{
					case 1:
						mode=0x101;
						HLIM=640;
						VLIM=480;
						break;
					case 2:
						mode=0x103;
						HLIM=800;
						VLIM=600;
						break;
					case 3: 
						mode=0x105;
						HLIM=1024;
						VLIM=768;
						break;
					case 4: 
						mode=0x107;
						HLIM=1280;
						VLIM=1024;
						break;
					default: 
						mode=0x101;
						HLIM=640;
						VLIM=480;
						break;
				}
				
				//termina musica e (re)inicia handlers
				state = FIM;
				speaker_off();
				reinstall_c_irq_handler(RTC_IRQ,&old_irq_rtc,&new_irq_rtc);
				
				//teclado e rato
				mask_pic(KBD_IRQ);
				mask_pic(MOUSE_IRQ);
				install_asm_irq_handler(KBD_IRQ,handler_teclado,&old_irq_kbd);
				if(kbc_init(0) != 1)
				{
					printf(" ERRO. Não detectou rato.");
					//erro frequente, decidiu-se implementar so teclado
			       	mouse_disable();
				}
				install_asm_irq_handler(MOUSE_IRQ,handler_rato,&old_irq_mouse);
				unmask_pic(MOUSE_IRQ);
				unmask_pic(KBD_IRQ);
				
				//limpa o ecra e entra em modo grafico
				clear();
				video_mem=enter_graphics(mode, &map);
				
				//leitura do tempo inicial
				mask_pic(RTC_IRQ);
				rtc_read_time(&tempo_init);
				tempo_inicial = tempo_init.hour*3600 + tempo_init.min*60 + tempo_init.sec;
				unmask_pic(RTC_IRQ);
				
				//desenha tabuleiro e centra posicoes dos blocos
				drawTabuleiro(VLIM, HLIM, video_mem);
				posBx = (HLIM/2)-300; 
				posBy = 100;
				
				//inicializa variaveis relevantes ao jogo
				vidas=3;
				j.pontos=0;
				nblocos=50; //numero de blocos
				//contadores auxiliares
				dez=0;
				bcnt=0;
				com=0;
				
				//cria sprites
				bola = create_sprite(bol); //bola
				raquete = create_sprite(bar); //raquete
				//cria, posiciona e desenha blocos
				for (i=0;i<nblocos;i++) {
					blocos[i]=create_sprite(block);
					blocos[i]->x = posBx + (dez+1)*BLOC_LRG;
					existe[i]=1; //se bloco existe - 1; 0 - nao existe
					dez++; 
					blocos[i]->y = posBy;
					draw_sprite(blocos[i], video_mem);
					//linhas de dez blocos
					if (dez==10) {
						dez=0;
						posBx=(HLIM/2)-300;
						posBy = posBy + 10;
					}
				}
				
				//posiciona bola e raquete
				bola->x = HLIM/2 - BOLA_TAM;
				bola->y = VLIM-65;
				raquete->x = HLIM/2 - 50 ;
				raquete->y = VLIM - 30;
				
				//bola e raquete comecam paradas
				bola->xspeed = 0;
				bola->yspeed = 0;
				raquete->xspeed = 0;
				raquete->yspeed = 0;
				
				//desenha sprites da bola e raquete
				draw_sprite(bola, video_mem);
				draw_sprite(raquete, video_mem);				
				
				//comeca jogo
				scd = 0; //teclas a zero
				mask_pic(RTC_IRQ);
				//clock para controlar velocidade da bola
				ti = clock();
				/*jogo acaba se for pressionada tecla ESCAPE,
				perder todas as vidas ou destruir todos os blocos*/
				while ((scd != ESC) && (vidas>0) && (bcnt<nblocos)) {
					
					//leitura do tempo corrente
					rtc_read_time(&tempo_current);
					tempo=tempo_current.hour*3600 + tempo_current.min*60 + tempo_current.sec;
					//printf("\r Tempo: %i s\n", tempo-tempo_inicial);
					
					//detectar colisoes da bola
					if (bola->y > VLIM-100) {
						//na raquete
						if ((bola->x+BOLA_TAM > raquete->x) && ((bola->x < raquete->x+100)) && (bola->y+BOLA_TAM > raquete->y) && (bola->y+BOLA_TAM < raquete->y+10)) {
								//bola->yspeed = -bola->yspeed;
								bola->yspeed = -velocidade;
								if (sfx==1) play_song(&bate);
						}
						
						//no fundo
						if (bola->y >= VLIM - 10)
						{
							//perde vida
							com=0;
							clear_screen(0,video_mem);
							drawTabuleiro(VLIM, HLIM, video_mem);
							for (i=0;i<nblocos;i++) {
								if (existe[i]==1)
									draw_sprite(blocos[i], video_mem);
							}
							if (sfx==1) play_song(&vida);
							vidas--;
							bola->xspeed = 0;
							bola->yspeed = 0;
							bola->x = HLIM/2 - BOLA_TAM;
							bola->y = VLIM-65;
							raquete->x = HLIM/2 - 50 ;
							raquete->y = VLIM - 30;
							draw_sprite(bola, video_mem);
							draw_sprite(raquete, video_mem);	
						}
						
					}
					
					//limites do cenario
					if (bola->x < 40) {
						//bola->xspeed = -bola->xspeed;
						bola->xspeed = velocidade;
						if (sfx==1) play_song(&bate);
					}
					if (bola->y < 50) { 
						//bola->yspeed = -bola->yspeed;
						bola->yspeed = velocidade;
						if (sfx==1) play_song(&bate);
					}
					if (bola->x + 32 > HLIM - 40) {
						//bola->xspeed = -bola->xspeed;
						bola->xspeed = -velocidade;
						if (sfx==1) play_song(&bate);
					}
					
					//nos blocos
					if (bola->y<VLIM/2) {
						dez=0;
						for (i = 0; i < nblocos; i++) {
							if (existe[i]==1) {
								if ( colisao_bloco(blocos[i], velocidade, bola) == 1 ) {
									delete_sprite(blocos[i], video_mem);
									bcnt++;
									existe[i]=0;
									if (sfx==1) play_song(&bate);
									j.pontos=j.pontos+10;	
									
									/*
									Redesenhar sempre que existe uma colisao -
									- foi retirado por se considerar que faz
									"piscar" demasiado o ecra.
									
									clear_screen(0,video_mem);
									drawTabuleiro(VLIM, HLIM, video_mem);
									for (k=0;k<nblocos;k++) {
										if (existe[k]==1)
											draw_sprite(blocos[k], video_mem);
											
									draw_sprite(raquete, video_mem);
									
									}*/
									
								}
							}
						}
					}

					tf = clock();
					tempo_gasto = ( (double) (tf - ti) ) / CLOCKS_PER_SEC;
					/*anima a bola a cada 1/10000 segundos
					para conseguir um bom racio de velocidade,
					nem muito rapido, nem muito lento,
					apesar da variavel 'velocidade' so mexer
					com o speed do sprite da bola*/
					if (tempo_gasto>=0.0001) {
						animate_sprite(bola, video_mem);
						ti = clock();
					}
					/*aqui o animate_sprie acelerava demasiado a bola
					porque o ciclo while e executado em nanosegundos*/
					//animate_sprite(bola, video_mem);
					
					//teclas
					switch (scd)
					{
						case ESQUERDA:
							//printf("Tecla a");
							if (raquete->x > 40) {
								raquete->xspeed = -1;
								animate_sprite(raquete, video_mem);
							}
							break;
						case DIREITA:
							//printf("Tecla d");
							if (raquete->x + 100 < HLIM - 40) {
								raquete->xspeed = 1;
								animate_sprite(raquete, video_mem);
							}
							break;
						case ESPACO:
							//larga a bola
							if (com==0) {
								bola->xspeed = velocidade;
								bola->yspeed = -velocidade;
								com=1;
							}
							break;
						default: 
							
						break;
					}
					
					//scd=0;
				}
				unmask_pic(RTC_IRQ);
				//printf("Video memory address %c\n", video_mem);
				
				//leitura do tempo final
				mask_pic(RTC_IRQ);
				rtc_read_time(&tempo_end);
				tempo_fim = tempo_end.hour*3600 + tempo_end.min*60 + tempo_end.sec;
				unmask_pic(RTC_IRQ);
				//calculo do tempo de jogo com RTC
				j.tempo=tempo_fim-tempo_inicial;
				
				//Sai do modo gráfico
				leave_graphics(map);
				
				//repoe handlers
				mask_pic(KBD_IRQ);
				mask_pic(MOUSE_IRQ);
				reinstall_asm_irq_handler(KBD_IRQ,&old_irq_kbd);    
				reinstall_asm_irq_handler(MOUSE_IRQ,&old_irq_mouse);
				unmask_pic(MOUSE_IRQ);
				unmask_pic(KBD_IRQ);
				
				//mostra estatisticas
				printf(" Tempo: %i\n", j.tempo);
				printf(" Pontos: %i\n", j.pontos);
				
				//toca musica final (caso o sfx esteja activo)
				if (sfx==1) play_song(&final);
				
				//pede o nome e escreve no ficheiro a pontução
				printf(" Introduza o seu nome (limite maximo de 15 caracteres): ");
				scanf("%s", j.nome);
				getchar();
				
				i=0;
				while ((i<10) && (joga[i].tempo!=0) && (joga[i].pontos!=0)) {
					i++;
				}
				
				if (i<10) {
					joga[i].tempo=j.tempo;
					joga[i].pontos=j.pontos;
					strcpy(joga[i].nome, j.nome);
					insertionSort(joga);
				}				
				
				f = fopen("pontuacoes.txt", "w");
				if(f == NULL)
					return 1;
				writeFile(f, joga);
				fclose(f);
				
				//inicializacao do handler RTC para a musica do menu
				mask_pic(RTC_IRQ);
				install_c_irq_handler(RTC_IRQ,&rtc_intc,&old_irq_rtc,&new_irq_rtc);
				c = read_rtcv(RTC_STAT_B);
				c = c | RTC_PIE | RTC_AIE | RTC_UIE;
				write_rtcv(RTC_STAT_B , c);
				unmask_pic(RTC_IRQ);
				
				//inicia a musica do menu
				if (sfx==1) {
					play_song(&intro);
					state = INICIO;
				} else {
					state = FIM;
				}
				
				break;
			case 2:
				instrucoes();
				getchar();
				getchar();
				//clear();
				break;
			case 3:
				//clear();
				submenu();
				do {
					scanf("%i", &op);
					switch (op)
					{
						case 1:
							//musica on / off
							if (sfx==0) {
								sfx=1;
								printf(" SFX ON\n");
								state = INICIO;
								printf(" \n");
								printf(" Escolha a sua opcao: ");
							} else {
								sfx=0;
								printf(" SFX OFF\n");
								state = FIM;
								printf(" \n");
								printf(" Escolha a sua opcao: ");
							}
							break;
						case 2:
							//inserir numero de pixeis que avanca de cada vez
							printf(" Insira velocidade\n");
							scanf("%lf", &velocidade);
							clear();
							submenu();
							break;
						case 7:
							//pequena surpresa...
							printf(" Parabens! Achou a tecla secreta!\n");
							state=PAUSE;
							play_song(&DBGT);
							printf(" Prima Enter\n");
							getchar();
							state=INICIO;
							clear();
							break;
						case 0:
							printf(" OK\n");
							break;
						default:
							clear();
							printf(" Opcao invalida. Por favor escolha uma nova opcao.\n");
							submenu();
							break;
					}
				} while (op !=0);
				//clear();
				break;
			case 4:
				printf(" \n ----- Pontuacoes -----\n\n");
				printf(" Nome            Pontos   Tempo\n");
				int p=0;
				for (p=0; p<10;p++) {
					printf(" %s %i %i\n", joga[p].nome, joga[p].pontos, joga[p].tempo);
				}
				printf(" \n Prima enter para continuar\n");
				getchar();
				getchar();
				//clear();
				break;
			case 0: 
				printf(" Saiu\n");
				break;
			default:
				printf(" Opcao invalida. Por favor escolha uma nova opcao.\n");
				clear();
				break;
		}
	} while (opcao !=0);
	
	//para musica
	state = FIM;
	speaker_off();
	
	//delay de 1 segundo
	time(&start);
	do{
		time(&current);
	}while(difftime(current,start) < 1.0);
	
	//fim
	return 0;
	
}
