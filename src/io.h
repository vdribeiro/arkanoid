#ifndef _IO_
#define _IO_H_

#include <stdio.h>
#include <string.h>

/** @defgroup IO Input/Output
 * 
 * Modulo onde são definidas as funções para a leitura
 * e escrita do ficheiro com as pontuacoes.
 * 
 */

/** Estrutura de um jogador para efeitos de pontução
 *
 */
typedef struct{
	char nome[16];
	int pontos;
	int tempo;
} jogador;
 
/**
  * Le o ficheiro de texto com as pontuações
  *
  * @param f - ficheiro a ser lido.
  * @param joga - estrutura jogador
  */
FILE* readFile(FILE* f, jogador* joga);

/**
  * Escreve no ficheiro as pontuções
  *
  * @param f ficheiro a ser escrito.
  * @param joga - estrutura jogador
  */
FILE* writeFile(FILE* f, jogador* joga);


/** 
  * Ordena a estrututa jogador por ordem de pontuação
  *
  * @param joga - estrutura jogador
  * http://www.softpanorama.org/Algorithms/Sorting/insertion_sort.shtml - 17/05/2010
  */
void insertionSort(jogador* joga);

/** @} end of IO*/
#endif
