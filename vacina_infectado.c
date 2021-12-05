#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>


/*
	Convencao para ingrediente: 
		1 == virus_morto
		2 == injecao
		3 == insumo_secreto
*/
typedef struct Laboratorio
{
	//ID proprio
	pthread_t lab_id;

	//ID que eu criei
	int lab_num;

    int ingrediente_1;
    int ingrediente_2;
	int qtd_renova_estoque;
    sem_t *renova_estoque;
	
}Laboratorio;

typedef struct Ingrediente
{
	//VM == 1, INJECAO == 2, IS == 3
	int qual_ingrediente;
	Laboratorio* pertence;

}Ingrediente;


//A bancada tera dois de cada ingrediente
typedef struct Bancada
{
	Ingrediente* virus_morto;
	Ingrediente* injecao;
	Ingrediente* insumo_secreto;

	/*
	Para ter controle dos ingredientes e informa aos respectivos laboratorios
	a necessidade de renovacao de estoque
	*/
	sem_t *s_lab_1;
	sem_t *s_lab_2;
	sem_t *s_lab_3;

}Bancada;

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
typedef struct Infectado
{
	//ID proprio
	pthread_t infec_id;

	//ID que eu criei
	int infec_num;

    int virus_morto;
    int injecao;
    int insumo_secreto;
	int ingrediente_infinito;
	int quantidade_vacinas_aplicadas;
	Bancada* bancada;
    sem_t *usando_bancada;
	pthread_mutex_t pegando_ingrediente;

}Infectado;



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