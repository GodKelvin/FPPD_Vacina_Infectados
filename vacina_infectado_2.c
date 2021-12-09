#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>

typedef struct Ingrediente Ingrediente;
typedef struct Bancada Bancada;
typedef struct Laboratorio Laboratorio;
typedef struct Infectado Infectado;

struct Ingrediente
{
	int disponivel;
	Laboratorio* pertence_lab;

};

/*A bancada tera dois de cada ingrediente.
0 == Nao possui ingrediente
1 == Possui ingrediente*/
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
	Bancada* bancada;
	pthread_mutex_t *mutex;
	
};

/*Para verificar o ingrediente_infinito:
1 == virus_morto
2 == injecao
3 == insumo_secreto*/
//----------------Alterar tamanho das variaveis? (long ing, long long int?)
struct Infectado
{
	pthread_t infec_id_proprio;

	//ID que eu criei
	int infec_id;
	int ingrediente_infinito;
	int qtd_vacinas_aplicadas;
	int qtd_min_vacinas_aplicadas;

	/*Vetor de trabalho compartilhado com os laboratorios, 
	porem, cada um sabe sua posicao*/
	int *trabalho;
	Bancada* bancada;
	pthread_mutex_t *mutex;

};

int run_and_work(int *trabalho)
{
	for(int i = 0; i < 6; i++)
	{
		//Alguem ainda nao trabalhou o suficiente
		if(trabalho[i] == 0) return 1;
	}
	return 0;
}


void *run_infectado(void *arg)
{
	//Captura o infectado
	Infectado* infectado = (Infectado*) arg;

	//Verifica qual ingrediente ele possui, e por consequencia, quais precisa
	//1 == virus morto, 2 == injecao, 3 == insumo secreto
	//Precisa dos ingredientes 2 e 3 (injecao e insumo secreto)
	if(infectado->ingrediente_infinito == 1)
	{
		while(run_and_work(infectado->trabalho))
		{
			//Verificar se possui na bancada os ingredientes que ele precisa
			int qtd_injecao, qtd_insumo_secreto;

			//Verificar se pelo menos possui um ingrediente de cada disponivel
			pthread_mutex_lock(infectado->mutex);
			sem_getvalue(infectado->bancada->s_injecao, &qtd_injecao);
			sem_getvalue(infectado->bancada->s_insumo_secreto, &qtd_insumo_secreto);
			pthread_mutex_unlock(infectado->mutex);
			if((qtd_injecao > 0) && (qtd_insumo_secreto > 0))
			{
				//Pegou os dois ingredientes
				sem_wait(infectado->bancada->s_injecao);
				sem_wait(infectado->bancada->s_insumo_secreto);

				//Verificar qual injecao foi pega
				pthread_mutex_lock(infectado->mutex);
				if(infectado->bancada->injecao[0].disponivel)
				{	
					//Informo que nao esta mais disponivel o respectivo ingrediente deste laboratorio
					infectado->bancada->injecao[0].disponivel = 0;
					//Informo ao laboratorio que o respectivo ingrediente foi consumido
					sem_post(infectado->bancada->injecao[0].pertence_lab->renova_estoque);
				}
				else
				{
					infectado->bancada->injecao[1].disponivel = 0;
					sem_post(infectado->bancada->injecao[1].pertence_lab->renova_estoque);
				}
				pthread_mutex_unlock(infectado->mutex);

				//Verificar qual insumo secreto foi pego
				pthread_mutex_lock(infectado->mutex);
				if(infectado->bancada->insumo_secreto[0].disponivel)
				{
					infectado->bancada->insumo_secreto[0].disponivel = 0;
					sem_post(infectado->bancada->insumo_secreto[0].pertence_lab->renova_estoque);
				}
				else
				{
					infectado->bancada->insumo_secreto[1].disponivel = 0;
					sem_post(infectado->bancada->insumo_secreto[1].pertence_lab->renova_estoque);
				}
				pthread_mutex_unlock(infectado->mutex);

				//Aplica a vacina
				infectado->qtd_vacinas_aplicadas++;

				if(infectado->qtd_vacinas_aplicadas > infectado->qtd_min_vacinas_aplicadas)
				{
					//Este infectado ja trabalhou o suficiente
					infectado->trabalho[0] = 1;
				}

				//if(!run_and_work(infectado->trabalho))
			}
		}
	}

	//Precisa dos ingredientes 1 e 3 (virus morto e insumo secreto)
	else if(infectado->ingrediente_infinito == 2)
	{
		while(run_and_work(infectado->trabalho))
		{
			
			//Verificar se possui na bancada os ingredientes que ele precisa
			int qtd_virus_morto, qtd_insumo_secreto;

			pthread_mutex_lock(infectado->mutex);
			sem_getvalue(infectado->bancada->s_virus_morto, &qtd_virus_morto);
			sem_getvalue(infectado->bancada->s_insumo_secreto, &qtd_insumo_secreto);
			pthread_mutex_unlock(infectado->mutex);
			if((qtd_virus_morto > 0) && (qtd_insumo_secreto > 0))
			{
				//Pegou os dois ingredientes
				sem_wait(infectado->bancada->s_virus_morto);
				sem_wait(infectado->bancada->s_insumo_secreto);

				//Verificar qual virus_morto foi pego
				pthread_mutex_lock(infectado->mutex);
				if(infectado->bancada->virus_morto[0].disponivel)
				{	
					infectado->bancada->virus_morto[0].disponivel = 0;
					//Informo ao laboratorio que o respectivo ingrediente foi consumido
					sem_post(infectado->bancada->virus_morto[0].pertence_lab->renova_estoque);
				}
				else
				{
					infectado->bancada->virus_morto[1].disponivel = 0;
					sem_post(infectado->bancada->virus_morto[1].pertence_lab->renova_estoque);
				}
				pthread_mutex_unlock(infectado->mutex);

				//Verificar qual insumo secreto foi pego
				pthread_mutex_lock(infectado->mutex);
				if(infectado->bancada->insumo_secreto[0].disponivel)
				{
					infectado->bancada->insumo_secreto[0].disponivel = 0;
					sem_post(infectado->bancada->insumo_secreto[0].pertence_lab->renova_estoque);
				}
				else
				{
					infectado->bancada->insumo_secreto[1].disponivel = 0;
					sem_post(infectado->bancada->insumo_secreto[1].pertence_lab->renova_estoque);
				}
				pthread_mutex_unlock(infectado->mutex);

				//Aplica a vacina
				infectado->qtd_vacinas_aplicadas++;

				if(infectado->qtd_vacinas_aplicadas > infectado->qtd_min_vacinas_aplicadas)
				{
					//Este infectado ja trabalhou o suficiente
					infectado->trabalho[1] = 1;
				}
			}
		}
	}
	//Precisa dos ingredientes 1 e 2 (virus morto e injecao)
	//else if(infectado->ingrediente_infinito == 3)
	else
	{
		while(run_and_work(infectado->trabalho))
		{
			
			//Verificar se possui na bancada os ingredientes que ele precisa
			int qtd_virus_morto, qtd_injecao;

			pthread_mutex_lock(infectado->mutex);
			sem_getvalue(infectado->bancada->s_virus_morto, &qtd_virus_morto);
			sem_getvalue(infectado->bancada->s_injecao, &qtd_injecao);
			pthread_mutex_unlock(infectado->mutex);
			if((qtd_virus_morto > 0) && (qtd_injecao > 0))
			{
				//Pegou os dois ingredientes
				sem_wait(infectado->bancada->s_virus_morto);
				sem_wait(infectado->bancada->s_injecao);

				//Verificar qual virus_morto foi pego
				pthread_mutex_lock(infectado->mutex);
				if(infectado->bancada->virus_morto[0].disponivel)
				{	
					//Informo que nao esta mais disponivel o respectivo ingrediente deste laboratorio
					infectado->bancada->virus_morto[0].disponivel = 0;
					//Informo ao laboratorio que o respectivo ingrediente foi consumido
					sem_post(infectado->bancada->virus_morto[0].pertence_lab->renova_estoque);
				}
				else
				{
					infectado->bancada->virus_morto[1].disponivel = 0;
					sem_post(infectado->bancada->virus_morto[1].pertence_lab->renova_estoque);
				}
				pthread_mutex_unlock(infectado->mutex);
				//Verificar qual injecao secreto foi pega
				pthread_mutex_lock(infectado->mutex);
				if(infectado->bancada->injecao[0].disponivel)
				{
					infectado->bancada->injecao[0].disponivel = 0;
					sem_post(infectado->bancada->injecao[0].pertence_lab->renova_estoque);
				}
				else
				{
					infectado->bancada->injecao[1].disponivel = 0;
					sem_post(infectado->bancada->injecao[1].pertence_lab->renova_estoque);
				}
				pthread_mutex_unlock(infectado->mutex);

				//Aplica a vacina
				infectado->qtd_vacinas_aplicadas++;

				if(infectado->qtd_vacinas_aplicadas >= infectado->qtd_min_vacinas_aplicadas)
				{
					//Este infectado ja trabalhou o suficiente
					infectado->trabalho[2] = 1;
				}
			}
		}
	}
	
	return 0;
}


//Iniciar qtd_renova_estoque em 1?
void *run_laboratorio(void *arg)
{
	Laboratorio* laboratorio = (Laboratorio*) arg;

	//Verificar id do laboratorio
	if(laboratorio->lab_id == 0)
	{

		while(run_and_work(laboratorio->trabalho))
		{

			//pthread_mutex_lock(laboratorio->mutex);
			if(!laboratorio->bancada->virus_morto[0].disponivel && !laboratorio->bancada->injecao[0].disponivel)
			{
				//Estocar os produtos DESTE lab
				pthread_mutex_lock(laboratorio->mutex);
				laboratorio->bancada->virus_morto[0].disponivel = 1;
				laboratorio->bancada->injecao[0].disponivel = 1;
				pthread_mutex_unlock(laboratorio->mutex);

				//Avisar que foi estocado
				sem_post(laboratorio->bancada->s_virus_morto);
				sem_post(laboratorio->bancada->s_injecao);

				//Renovou estoque
				laboratorio->qtd_renova_estoque++;

				if(laboratorio->qtd_renova_estoque >= laboratorio->qtd_min_renova_restoque)
				{
					laboratorio->trabalho[3] = 1;
				}
				sem_wait(laboratorio->renova_estoque);
				sem_wait(laboratorio->renova_estoque);
			}
			//pthread_mutex_unlock(laboratorio->mutex);
		}
	}
	else if(laboratorio->lab_id == 1)
	{
		while(run_and_work(laboratorio->trabalho))
		{
			//pthread_mutex_lock(laboratorio->mutex);
			if(!laboratorio->bancada->insumo_secreto[0].disponivel && !laboratorio->bancada->virus_morto[1].disponivel)
			{
				//Estocar os produtos DESTE lab
				pthread_mutex_lock(laboratorio->mutex);
				laboratorio->bancada->insumo_secreto[0].disponivel = 1;
				laboratorio->bancada->virus_morto[1].disponivel = 1;
				pthread_mutex_unlock(laboratorio->mutex);

				//Avisar que foi estocado
				sem_post(laboratorio->bancada->s_insumo_secreto);
				sem_post(laboratorio->bancada->s_virus_morto);

				//Renovou estoque
				laboratorio->qtd_renova_estoque++;

				if(laboratorio->qtd_renova_estoque >= laboratorio->qtd_min_renova_restoque)
				{
					laboratorio->trabalho[4] = 1;
				}
			}
			//pthread_mutex_unlock(laboratorio->mutex);
			
			sem_wait(laboratorio->renova_estoque);
			sem_wait(laboratorio->renova_estoque);
		}
	}
	else
	{
		while(run_and_work(laboratorio->trabalho))
		{
			//pthread_mutex_lock(laboratorio->mutex);
			if(!laboratorio->bancada->injecao[1].disponivel && !laboratorio->bancada->insumo_secreto[1].disponivel)
			{

				//Estocar os produtos DESTE lab
				pthread_mutex_lock(laboratorio->mutex);
				laboratorio->bancada->injecao[1].disponivel = 1;
				laboratorio->bancada->insumo_secreto[1].disponivel = 1;
				pthread_mutex_unlock(laboratorio->mutex);

				//Avisar que foi estocado
				sem_post(laboratorio->bancada->s_injecao);
				sem_post(laboratorio->bancada->s_insumo_secreto);
				//pthread_mutex_unlock(laboratorio->mutex);

				//Renovou estoque
				laboratorio->qtd_renova_estoque++;

				if(laboratorio->qtd_renova_estoque >= laboratorio->qtd_min_renova_restoque)
				{
					laboratorio->trabalho[5] = 1;
				}
				sem_wait(laboratorio->renova_estoque);
				sem_wait(laboratorio->renova_estoque);
			}
			//pthread_mutex_unlock(laboratorio->mutex);
			
		}
	}

	return 0;
}



//Receber numero de tarefas por parametro
int main()
{
	//Quantidade de tarefas (receber por parametro in argv)
	int num_trabalho_minimo = 1;
	//Quantidade de infectados, laboratorios e ingredientes
	int qtd_infectados = 3;
	int qtd_laboratorios = 3;
	int qtd_ingredientes = 6;

	/*-----ALOCANDO AS RESPECTIVAS MEMORIAS-----*/
	Infectado *infectados;		//TAM 3
	Laboratorio *laboratorios;	//TAM 3
	Bancada *bancada;			//TAM 1
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
	pthread_mutex_t mutex_acesso_ingrediente, mutex_reestocando_ingrediente;
	pthread_mutex_init(&mutex_acesso_ingrediente, NULL);
	pthread_mutex_init(&mutex_reestocando_ingrediente, NULL);

	//Semafaros de acesso aos ingredientes
	sem_t s_acesso_injecao, s_acesso_virus_morto, s_acesso_insumo;

	//Semaforos de renovacao dos estoques dos laboratorios
	sem_t s_renova_lab_1, s_renova_lab_2, s_renova_lab_3;

	/* Iniciando com 0 pois os laboratorios irao informar
	quando estocarem os ingredientes */
	sem_init(&s_acesso_injecao, 0, 0);
	sem_init(&s_acesso_virus_morto, 0, 0);
	sem_init(&s_acesso_insumo, 0, 0);


	/* Iniciando com 0 pois os laboratorios irao informar
	quando estocarem os ingredientes */
	sem_init(&s_renova_lab_1, 0, 0);
	sem_init(&s_renova_lab_2, 0, 0);
	sem_init(&s_renova_lab_3, 0, 0);
	/*-----FIM DA CRIACAO DE SEMAFOROS E MUTEXES-----*/

	/*-----ASSOCIANDO AS ESTRUTURAS CRIADAS-----*/
	int i, j;
	for(i = 0; i < qtd_laboratorios; i++)
	{
		laboratorios[i].lab_id = i;
		laboratorios[i].qtd_renova_estoque = 0;
		laboratorios[i].bancada = bancada;
		laboratorios[i].mutex = &mutex_reestocando_ingrediente;
        laboratorios[i].qtd_min_renova_restoque = num_trabalho_minimo;
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
		infectados[i].mutex = &mutex_acesso_ingrediente;
        infectados[i].qtd_min_vacinas_aplicadas = num_trabalho_minimo;
		//infectados[i].pegou_ingrediente = 0;
	}

	for(i = 0, j = 0; i < qtd_ingredientes; i++)
	{
		ingredientes[i].disponivel = 0;
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
	//Informando a bancada quais ingredientes possuem o que.
	//Ingredientes que possuem virus morto
	//Laboratorios 1 & 2 (posicao 0 e 1)
	bancada->virus_morto[0] = ingredientes[0];
	bancada->virus_morto[1] = ingredientes[2];

	//Ingredientes que possuem injecao
	//Laboratorios 1 & 3 (posicao 0 e 1)
	bancada->injecao[0] = ingredientes[1];
	bancada->injecao[1] = ingredientes[4];

	//Ingredientes que possuem insumo_secreto
	//Laboratorios 2 & 3 posicao (0 e 1)
	bancada->insumo_secreto[0] = ingredientes[3];
	bancada->insumo_secreto[1] = ingredientes[5];

	int trabalho_minimo[6];
	if(num_trabalho_minimo < 1)
	{
		//Entao todos ja realizaram o trabalho minimo (0x)
		for(i = 0; i < 6; i++)
		{
			trabalho_minimo[i] = 1;
		}
	}
	else
	{
		for(i = 0; i < 6; i++)
		{
			trabalho_minimo[i] = 0;
		}
	}

	/*Associando os respectivos infectados e laboratorios 
	a suas posicoes no vetor de trabalho minimo */
	infectados[0].trabalho = trabalho_minimo;
	infectados[1].trabalho = trabalho_minimo;
	infectados[2].trabalho = trabalho_minimo;

	laboratorios[0].trabalho = trabalho_minimo;
	laboratorios[1].trabalho = trabalho_minimo;
	laboratorios[2].trabalho = trabalho_minimo;

	/*-----FIM DA ASSOCIACAO DAS ESTRUTURAS-----*/


	//printf("TESTES\n");
	//printf("ING DISPONIVEL: %d, %d\n", bancada->virus_morto[0].disponivel, laboratorios->bancada->virus_morto[0].disponivel);

	printf("Rodando as threads\n");
    for(i = 0; i < qtd_laboratorios; i++)
    {
		//if(pthread_create(&laboratorios[i].lab_id_proprio, NULL, run_laboratorio, &laboratorios[i]) != 0)
        if(pthread_create(&laboratorios[i].lab_id_proprio, NULL, run_laboratorio, &laboratorios[i]) != 0)
        {
            perror("Pthread Create Lab falhou\n");
            return -2;
        }
    }

	for(i = 0; i < qtd_infectados; i++)
	{
		if(pthread_create(&infectados[i].infec_id_proprio, NULL, run_infectado, &infectados[i]) != 0)
		{
			perror("Pthread Create Infec falhou\n");
			return -1;
		}
	}
	


	
	//Esperando as threads terminarem
	for(i = 0; i < qtd_infectados; i++)
	{
		if(pthread_join(infectados[i].infec_id_proprio, NULL) != 0)
		{
			printf("ERROR NO JOIN DOS INFECS\n");
			return -3;
		}
	}
	

	for(i = 0; i < qtd_laboratorios; i++)
	{
		if(pthread_join(laboratorios[i].lab_id_proprio, NULL) != 0)
		{
			printf("ERROR NO JOIN DOS LABS\n");
			return -3;
		}
	}

	for(i = 0; i < qtd_infectados; i++)
	{
		printf("Infectado %d: %d\n", infectados[i].infec_id, infectados[i].qtd_vacinas_aplicadas);
	}

	for(i = 0; i < qtd_laboratorios; i++)
	{
		printf("Laboratorio %d: %d\n", laboratorios[i].lab_id, laboratorios[i].qtd_renova_estoque);
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