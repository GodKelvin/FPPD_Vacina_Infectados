#include <semaphore.h>
#ifndef VACINA_H
#define VACINA_H

/* ESTRUTURAS UTILIZADAS */
typedef struct Ingrediente Ingrediente;
typedef struct Bancada Bancada;
typedef struct Laboratorio Laboratorio;
typedef struct Infectado Infectado;

struct Ingrediente
{
	/*0 == Nao possui ingrediente
	1 == Possui ingrediente*/
	int disponivel;
	Laboratorio* pertence_lab;

};


struct Bancada
{
	//A bancada tera dois de cada ingrediente
	Ingrediente* virus_morto;
	Ingrediente* injecao;
	Ingrediente* insumo_secreto;

	/*
	Para verificar se determinado ingrediente
	esta disponivel.
	*/
	sem_t *s_virus_morto;
	sem_t *s_injecao;
	sem_t *s_insumo_secreto;
};

struct Laboratorio
{
	pthread_t lab_id_proprio;

	//ID que eu criei
	int lab_id;
	int qtd_renova_estoque;
	int qtd_min_renova_restoque;

	/*Vetor de trabalho compartilhado com os infectados, 
	porem, cada um sabe sua posicao*/
	int *trabalho;
    sem_t *renova_estoque;

	/*Bancada compartilhada tanto com os outros laboratorios
	quanto com os infectados*/
	Bancada* bancada;
	pthread_mutex_t *mutex;
	
};

struct Infectado
{
	pthread_t infec_id_proprio;

	//ID que eu criei
	int infec_id;

	/*Para verificar o ingrediente_infinito:
	1 == virus_morto
	2 == injecao
	3 == insumo_secreto*/
	int ingrediente_infinito;

	int qtd_vacinas_aplicadas;
	int qtd_min_vacinas_aplicadas;

	/*Vetor de trabalho compartilhado com os laboratorios, 
	porem, cada um sabe sua posicao*/
	int *trabalho;
	
	/*Bancada compartilhada tanto com os outros infectados
	quanto com os laboratorios*/
	Bancada* bancada;
	pthread_mutex_t *mutex;

};

/* FUNCOES UTILIZADAS */

//Para verificar se o trabalho de todos terminou
int run_and_work(int *trabalho);

//Thread utilizada pelo infectado
void *run_infectado(void *arg);

//Thread utilizada pelo laboratorio
void *run_laboratorio(void *arg);

#endif