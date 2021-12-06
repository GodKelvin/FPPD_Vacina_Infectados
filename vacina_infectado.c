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
    //Ingrediente* ingrediente_1;
	//Ingrediente* ingrediente_2;
    //int ingrediente_2;
	//
	int qtd_renova_estoque;
    sem_t *renova_estoque;

	Bancada* bancada;
	
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


void *run_infectado(void *arg)
{
	//Captura o infectado
	Infectado* infectado = (Infectado*) arg;

	//Verifica qual ingrediente ele possui, e por consequencia, quais precisa
	//1 == virus morto, 2 == injecao, 3 == insumo secreto
	//Precisa dos ingredientes 2 e 3 (injecao e insumo secreto)
	if(infectado->ingrediente_infinito == 1)
	{
		//Verificar se possui na bancada os ingredientes que ele precisa
		int qtd_injecao, qtd_insumo_secreto;

		//Verificar se pelo menos possui um ingrediente de cada disponivel
		//---VERIFICAR SE ESSES DOIS MUTEXES DE GETVALUE SAO NECESSARIOS-------------------
		pthread_mutex_lock(infectado->mutex);
		sem_getvalue(infectado->bancada->s_injecao, &qtd_injecao);
		sem_getvalue(infectado->bancada->s_insumo_secreto, &qtd_insumo_secreto);
		if((qtd_injecao > 0) && (qtd_insumo_secreto > 0))
		{
			//Pegou os dois ingredientes
			sem_wait(infectado->bancada->s_injecao);
			sem_wait(infectado->bancada->s_insumo_secreto);
			pthread_mutex_unlock(infectado->mutex);
			//Aplica a vacina
			infectado->qtd_vacinas_aplicadas++;

			//Verificar qual injecao foi pega
			pthread_mutex_lock(infectado->mutex);
			if(infectado->bancada->injecao[0].disponivel)
			{	
				//Informo que nao esta mais disponivel o respectivo ingrediente deste laboratorio
				infectado->bancada->injecao[0].disponivel = 0;
				//Informo ao laboratorio que o respectivo ingrediente foi consumido
				//sem_wait(infectado->bancada->injecao[0].pertence_lab->renova_estoque);
				sem_post(infectado->bancada->injecao[0].pertence_lab->renova_estoque);
			}
			else
			{
				infectado->bancada->injecao[1].disponivel = 0;
				//sem_wait(infectado->bancada->injecao[1].pertence_lab->renova_estoque);
				sem_post(infectado->bancada->injecao[0].pertence_lab->renova_estoque);
			}
			//Verificar qual insumo secreto foi pego
			if(infectado->bancada->insumo_secreto[0].disponivel)
			{
				infectado->bancada->insumo_secreto[0].disponivel = 0;
				//sem_wait(infectado->bancada->insumo_secreto[0].pertence_lab->renova_estoque);
				sem_post(infectado->bancada->injecao[0].pertence_lab->renova_estoque);
			}
			else
			{
				infectado->bancada->insumo_secreto[1].disponivel = 0;
				//sem_wait(infectado->bancada->insumo_secreto[1].pertence_lab->renova_estoque);
				sem_post(infectado->bancada->injecao[0].pertence_lab->renova_estoque);
			}
			pthread_mutex_unlock(infectado->mutex);
		}
		//Se nao tiver os dois ingredientes disponiveis
		else
		{
			pthread_mutex_unlock(infectado->mutex);
		}
		
	}

	//Precisa dos ingredientes 1 e 3 (virus morto e insumo secreto)
	else if(infectado->ingrediente_infinito == 2)
	{
		//Verificar se possui na bancada os ingredientes que ele precisa
		int qtd_virus_morto, qtd_insumo_secreto;

		pthread_mutex_lock(infectado->mutex);
		sem_getvalue(infectado->bancada->s_virus_morto, &qtd_virus_morto);
		sem_getvalue(infectado->bancada->s_insumo_secreto, &qtd_insumo_secreto);

		if((qtd_virus_morto > 0) && (qtd_insumo_secreto > 0))
		{
			//Pegou os dois ingredientes
			sem_wait(infectado->bancada->s_virus_morto);
			sem_wait(infectado->bancada->s_insumo_secreto);
			pthread_mutex_unlock(infectado->mutex);

			//Aplica a vacina
			infectado->qtd_vacinas_aplicadas++;

			//Verificar qual virus_morto foi pega
			pthread_mutex_lock(infectado->mutex);
			if(infectado->bancada->virus_morto[0].disponivel)
			{	
				//Informo que nao esta mais disponivel o respectivo ingrediente deste laboratorio
				infectado->bancada->virus_morto[0].disponivel = 0;
				//Informo ao laboratorio que o respectivo ingrediente foi consumido
				sem_wait(infectado->bancada->virus_morto[0].pertence_lab->renova_estoque);
			}
			else
			{
				infectado->bancada->virus_morto[1].disponivel = 0;
				sem_wait(infectado->bancada->virus_morto[1].pertence_lab->renova_estoque);
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
		else
		{
			pthread_mutex_unlock(infectado->mutex);
		}
	}
	//ELSE?
	//Precisa dos ingredientes 1 e 2 (virus morto e injecao)
	else if(infectado->ingrediente_infinito == 3)
	{
		//Verificar se possui na bancada os ingredientes que ele precisa
		//---REFATORAR PARA "POSSUI_INJECAO // ...."
		int qtd_virus_morto, qtd_injecao;

		pthread_mutex_lock(infectado->mutex);
		sem_getvalue(infectado->bancada->s_virus_morto, &qtd_virus_morto);
		sem_getvalue(infectado->bancada->s_injecao, &qtd_injecao);

		if((qtd_virus_morto > 0) && (qtd_injecao > 0))
		{
			//Pegou os dois ingredientes
			sem_wait(infectado->bancada->s_virus_morto);
			sem_wait(infectado->bancada->s_injecao);
			pthread_mutex_unlock(infectado->mutex);
			//Aplica a vacina
			infectado->qtd_vacinas_aplicadas++;

			//Verificar qual virus_morto foi pega
			pthread_mutex_lock(infectado->mutex);
			if(infectado->bancada->virus_morto[0].disponivel)
			{	
				//Informo que nao esta mais disponivel o respectivo ingrediente deste laboratorio
				infectado->bancada->virus_morto[0].disponivel = 0;
				//Informo ao laboratorio que o respectivo ingrediente foi consumido
				sem_wait(infectado->bancada->virus_morto[0].pertence_lab->renova_estoque);
			}
			else
			{
				infectado->bancada->virus_morto[1].disponivel = 0;
				sem_wait(infectado->bancada->virus_morto[1].pertence_lab->renova_estoque);
			}
			//Verificar qual injecao secreto foi pega
			if(infectado->bancada->injecao[0].disponivel)
			{
				infectado->bancada->injecao[0].disponivel = 0;
				sem_wait(infectado->bancada->injecao[0].pertence_lab->renova_estoque);
			}
			else
			{
				infectado->bancada->injecao[1].disponivel = 0;
				sem_wait(infectado->bancada->injecao[1].pertence_lab->renova_estoque);
			}
			pthread_mutex_unlock(infectado->mutex);
		}
		else
		{
			pthread_mutex_unlock(infectado->mutex);
		}
	}
	
	return NULL;
}


//Iniciar qtd_renova_estoque em 1?
void *run_laboratori0(void *arg)
{
	Laboratorio* laboratorio = (Laboratorio*) arg;

	//Verificar id do laboratorio
	if(laboratorio->id_lab == 1)
	{
		sem_wait(laboratorio->renova_estoque);
		sem_wait(laboratorio->renova_estoque);
		//Renovou estoque
		laboratorio->qtd_renova_estoque++;

		//Os ingredientes gerados por esse laboratorio
		laboratorio->bancada->virus[0].disponivel = 1
		laboratorio->bancada->injecao[1].disponivel = 1

		//post nos semaforos de bancada de acordo com o ingrediente que produz
	}
}



//Receber numero de tarefas por parametro
int main()
{
	//Quantidade de infectados, laboratorios e ingredientes
	int qtd_infectados = 3;
	int qtd_laboratorios = 3;
	int qtd_ingredientes = 6;

	//Quantidade de tarefas (receber por parametro in argv)
	int qtd_tarefas = 5;

	/*-----ALOCANDO AS RESPECTIVAS MEMORIAS-----*/
	Infectado *infectados;		//TAM 3
	Laboratorio *laboratorios;	//TAM 3
	Bancada* bancada;			//TAM 1
	Ingrediente *ingredientes;	//TAM 6 (2 pra cada laboratorio)

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

	bancada = malloc(sizeof(Bancada));
	if(bancada == NULL)
	{
		printf("ERRO AO CRIAR BANCADA\n");
		return -3;
	}

	ingredientes = malloc(sizeof(Ingrediente) * qtd_ingredientes);
	if(ingredientes == NULL)
	{
		printf("ERRO AO CRIAR INGREDIENTES\n");
		return -4;
	}
	/*-----FIM DA ALOCACAO DE MEMORIA-----*/


	/*-----CRIANDO SEMAFOROS E MUTEX-----*/
	pthread_mutex_t mutex;
	pthread_mutex_init(&mutex, NULL);

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
	inicia a renovacao de cada estoque em 2 (ou 0?).
	*/
	sem_init(&s_renova_lab_1, 0, 0);
	sem_init(&s_renova_lab_2, 0, 0);
	sem_init(&s_renova_lab_3, 0, 0);

	//sem_init(&s_renova_lab_1, 0, 2);
	//sem_init(&s_renova_lab_2, 0, 2);
	//sem_init(&s_renova_lab_3, 0, 2);
	/*-----FIM DA CRIACAO DE SEMAFOROS E MUTEXES-----*/

	/*-----ASSOCIANDO AS ESTRUTURAS CRIADAS-----*/
	int i, j;
	for(i = 0; i < qtd_laboratorios; i++)
	{
		laboratorios[i].lab_id = i;
		laboratorios[i].qtd_renova_estoque = 0;
		laboratorios[i].bancada = bancada;
	}

	//Associando os respectivos semaforos aos respectivos labs
	laboratorios[0].renova_estoque = &s_renova_lab_1;
	laboratorios[1].renova_estoque = &s_renova_lab_2;
	laboratorios[2].renova_estoque = &s_renova_lab_3;

	for(i = 0; i < qtd_infectados; i++)
	{
		infectados[i].infec_id = i;
		infectados[i].ingrediente_infinito = i+1;
		infectados[i].qtd_vacinas_aplicadas = 0;
		infectados[i].bancada = bancada;
		infectados[i].mutex = &mutex;
	}

	for(i = 0, j = 0; i < qtd_ingredientes; i++)
	{
		ingredientes[i].disponivel = 1;
		ingredientes[i].pertence_lab = &laboratorios[j];

		if(i % 2 != 0) j++;
	}
	
	/*---CRIACAO DOS ESPACOS DOS INGREDIENTES NA BANCADA---*/
	bancada->virus_morto = malloc(sizeof(Ingrediente) * 2);
	bancada->injecao = malloc(sizeof(Ingrediente) * 2);
	bancada->insumo_secreto = malloc(sizeof(Ingrediente) * 2);
	/*---FIM DA CRIACAO DOS ESPACOS DOS INGREDIENTES NA BANCADA---*/

	/*---ASSOCIANDO OS SEMAFOROS---*/
	bancada->s_injecao = &s_acesso_injecao;
	bancada->s_virus_morto = &s_acesso_virus_morto;
	bancada->s_insumo_secreto = &s_acesso_insumo;
	//Informando a bancada quais ingredientes possuem o que
	//Ingredientes que possuem virus morto
	//Laboratorios 1 & 2
	bancada->virus_morto[0] = ingredientes[0];
	bancada->virus_morto[1] = ingredientes[2];

	//Ingredientes que possuem injecao
	//Laboratorios 1 & 3
	bancada->injecao[0] = ingredientes[1];
	bancada->injecao[1] = ingredientes[4];

	//Ingredientes que possuem insumo_secreto
	//Laboratorios 2 & 3
	bancada->insumo_secreto[0] = ingredientes[3];
	bancada->insumo_secreto[1] = ingredientes[5];

	/*-----FIM DA ASSOCIACAO DAS ESTRUTURAS-----*/


	printf("TESTES\n");
	printf("ING DISPONIVEL: %d, %d\n", bancada->virus_morto[0].disponivel, laboratorios->bancada->virus_morto[0].disponivel);

	//Teste de create
	for(i = 0; i < qtd_infectados; i++)
	{
		//printf("OPA\n");
		if(pthread_create(&infectados[i].infec_id_proprio, NULL, run_infectado, &infectados[i]) != 0)
		{
			perror("Pthread Create falhou\n");
			exit(1);
		}
	}

	for(i = 0; i < qtd_infectados; i++)
    {
        if(pthread_join(infectados[i].infec_id_proprio, NULL) != 0)
        {
            perror("Pthread_join falhou\n");
            exit(1);
        }
    }
	



	//Liberacao de memoria e destruicao dos semaforos
	free(infectados);
	free(laboratorios);
	free(bancada->virus_morto);
	free(bancada->injecao);
	free(bancada->insumo_secreto);
	free(bancada);
	free(ingredientes);
	sem_destroy(&s_acesso_injecao);
	sem_destroy(&s_acesso_virus_morto);
	sem_destroy(&s_acesso_insumo);
	sem_destroy(&s_renova_lab_1);
	sem_destroy(&s_renova_lab_2);
	sem_destroy(&s_renova_lab_3);

	printf("Hello Galaxy!\n");

    return 0;
}