#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>

typedef struct Ingrediente Ingrediente;
typedef struct Bancada Bancada;
typedef struct Laboratorio Laboratorio;
typedef struct Infectado Infectado;

struct Ingrediente
{
	int disponivel;
	//int id_lab_pertence;
	Laboratorio* pertence_lab;

};

//A bancada tera dois de cada ingrediente
struct Bancada
{
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


	/*
	Para ter controle dos ingredientes e informa aos respectivos laboratorios
	a necessidade de renovacao de estoque
	*/
	//
	//sem_t *s_lab_1;
	//sem_t *s_lab_2;
	//sem_t *s_lab_3;
	//

};

/*
	Convencao para ingrediente: 
		1 == virus_morto
		2 == injecao
		3 == insumo_secreto
*/
struct Laboratorio
{
	pthread_t lab_id_proprio;

	//ID que eu criei
	int lab_id;

	//
    Ingrediente* ingrediente_1;
	Ingrediente* ingrediente_2;
    //int ingrediente_2;
	//
	int qtd_renova_estoque;
    sem_t *renova_estoque;

	//Bancada* bancada;
	
};

/*
	Para cada ingrediente:
		0 == Nao possui ingrediente
		1 == Possui ingrediente

	Para verificar o ingrediente_infinito:
		1 == virus_morto
		2 == injecao
		3 == insumo_secreto
*/
//----------------Alterar tamanho das variaveis? (long ing, long long int?)
struct Infectado
{
	pthread_t infec_id_proprio;

	//ID que eu criei
	int infec_id;

	////
    //int virus_morto;
    //int injecao;
    //int insumo_secreto;
	////

	int ingrediente_infinito;
	int qtd_vacinas_aplicadas;
	Bancada* bancada;


	///
    //sem_t *usando_bancada;
	///

	pthread_mutex_t *mutex;

};


void run_infectado(void *arg)
{
	//Captura o infectado
	Infectado* infectado = (Infectado*) arg;

	//Verifica qual ingreidente ele possui, e por consequencia, quais precisa
	//1 == virus morto, 2 == injecao, 3 == insumo secreto
	int possui_ingrediente = infectado->ingrediente_infinito;

	//Precisa dos ingredientes 2 e 3
	if(possui_ingrediente == 1)
	{
		//sem_wait(infectado->bancada->
		//Verificar se possui na bancada os ingredientes que ele precisa
		int possui_ing_2, possui_ing_3;

		//pthread_mutex_lock(&mutex1);
        //pthread_mutex_unlock(&mutex1);

		//pthread_mutex_lock(infectado->mutex)
		sem_getvalue(infectado->bancada->s_injecao, &possui_ing_2);
		sem_getvalue(infectado->bancada->s_insumo_secreto, &possui_ing_3);
		
		if((possui_ing_2 > 0) && (possui_ing_3 > 0))
		{
			//Pegou os dois ingredientes
			sem_wait(infectado->bancada->s_injecao);
			sem_wait(infectado->bancada->s_insumo_secreto);

			//Aplica a vacina
			infectado->qtd_vacinas_aplicadas++;

			//Verificar qual injecao foi pega
			pthread_mutex_lock(infectado->mutex);
			if(infectado->bancada->injecao[0].disponivel)
			{	
				//Informo que nao esta mais disponivel o respectivo ingrediente deste laboratorio
				infectado->bancada->injecao[0].disponivel = 0;
				//Informo ao laboratorio que o respectivo ingrediente foi consumido
				sem_wait(infectado->bancada->injecao[0].pertence_lab->renova_estoque);
			}
			else
			{
				infectado->bancada->injecao[1].disponivel = 0;
				sem_wait(infectado->bancada->injecao[1].pertence_lab->renova_estoque);
			}
			//Verificar qual insumo secreto foi pego
			if(infectado->bancada->insumo_secreto[0].disponivel)
			{
				infectado->bancada->insumo_secreto[0].disponivel = 0;
				sem_wait(infectado->bancada->insumo_secreto[0].pertence_lab->renova_estoque);
			}
			else
			{
				infectado->bancada->insumo_secreto[1].disponivel = 0;
				sem_wait(infectado->bancada->insumo_secreto[1].pertence_lab->renova_estoque);
			}
			pthread_mutex_unlock(infectado->mutex);
		}
	}
	
	/*
	else if(possui_ingrediente == 2)
	{

	}
	else
	{

	}
	*/
}

//Receber numero de tarefas por parametro
int main()
{
	//Quantidade de infectados e laboratorios
	int qtd_infectados = 3;
	int qtd_laboratorios = 3;

	//Quantidade de tarefas (receber por parametro in argv)
	int qtd_tarefas = 5;

	//Criando meus infectados e laboratorios
	Infectado *infectados;
	Laboratorio *laboratorios;

	infectados = malloc(sizeof(Infectado) * qtd_infectados);
	if(infectados == NULL)
	{
		printf("ERRO AO CRIAR INFECTADOS\n");
		return -1;
	}

	laboratorios = malloc(sizeof(Laboratorio) * qtd_laboratorios);
	if(laboratorios == NULL)
	{	
		printf("ERRO AO CRIAR LABORATORIOS\n");
		return -2;
	}

	/*-----Criando os semaforos e mutexes-----*/
	//Semafaros de acesso aos ingredientes
	sem_t s_acesso_injecao, s_acesso_virus_morto, s_acesso_insumo;

	//Semaforos de renovacao dos estoques dos laboratorios
	sem_t s_renova_lab_1, s_renova_lab_2, s_renova_lab_3;

	/*
	Como a bancada terah dois de cada ingrediente disponivel,
	inicia cada acesso com 2;
	*/
	sem_init(&s_acesso_injecao, 0, 2);
	sem_init(&s_acesso_virus_morto, 0, 2);
	sem_init(&s_acesso_insumo, 0, 2);

	/*
	Como de inicio todos os ingredientes estarao abastecidos,
	inicia a renovacao de cada estoque em 0.
	*/
	sem_init(&s_renova_lab_1, 0, 0);
	sem_init(&s_renova_lab_2, 0, 0);
	sem_init(&s_renova_lab_3, 0, 0);
	/*-----Fim criacao de semaforos e mutexes-----*/






	//Liberacao de memoria e destruicao dos semaforos
	free(infectados);
	free(laboratorios);
	sem_destroy(&s_acesso_injecao);
	sem_destroy(&s_acesso_virus_morto);
	sem_destroy(&s_acesso_insumo);
	sem_destroy(&s_renova_lab_1);
	sem_destroy(&s_renova_lab_2);
	sem_destroy(&s_renova_lab_3);

	printf("Hello Galaxy!\n");

    return 0;
}