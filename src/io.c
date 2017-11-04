#include "io.h"

/**
  * Le o ficheiro de texto com as pontuações
  *
  * @param f - ficheiro a ser lido.
  * @param joga - estrutura jogador
  */
FILE* readFile(FILE* f, jogador* joga)
{
	if(f!=NULL)
	{
		int i = 0;
		while(fscanf(f, "%s %i %i", joga[i].nome, &joga[i].pontos, &joga[i].tempo)!=EOF) {
			i++;
		}
	}
	return f;
}

/**
  * Escreve no ficheiro as pontuções
  *
  * @param f Ficheiro a ser escrito.
  * @param joga - estrutura jogador
  */
FILE* writeFile(FILE* f, jogador* joga)
{
	rewind(f);
	int i = 0;
	while(i < 10)
	{
		fprintf(f, "%s %i %i\n", joga[i].nome, joga[i].pontos, joga[i].tempo);
		i++;
	}
	return f;
}

/** 
  * Ordena a estrututa jogador por ordem de pontuação
  *
  * @param joga - estrutura jogador
  * http://www.softpanorama.org/Algorithms/Sorting/insertion_sort.shtml - 17/05/2010
  */
void insertionSort(jogador* joga){
	
	int i, j, pontos, tempo;
	char name[16];
	for (i=1; i < 10; i++)
	{
		pontos = joga[i].pontos;
		tempo = joga[i].tempo;
		strcpy(name, joga[i].nome);
		j = i; // index of the end of sorted region
		while ((j > 0) && (joga[j-1].pontos < pontos))
		{
			strcpy(joga[j].nome, joga[j-1].nome);
			joga[j].pontos = joga[j-1].pontos;
			joga[j].tempo = joga[j-1].tempo;
			j = j - 1;
		}
		joga[j].pontos = pontos;
		joga[j].tempo = tempo;
		strcpy(joga[j].nome,name);
	}
}
