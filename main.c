#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

//Inclusao da minha biblioteca
#include "vacina.h"

//Receber numero de tarefas por parametro
int main(int argc, char *argv[])
{

	if(argc <= 1)
	{
		printf("Forneca a quantidade de trabalho minima\n");
		return 0;
	}
	//Quantidade de tarefas (receber por parametro in argv)
	int num_trabalho_minimo = atoi(argv[1]);

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
	pthread_mutex_t mutex_acesso_ingrediente;
	pthread_mutex_init(&mutex_acesso_ingrediente, NULL);

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
		laboratorios[i].mutex = &mutex_acesso_ingrediente;
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

	/*Informando a bancada quais ingredientes possuem o que*/
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


	//Criacao das threads
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
	

	//Imprimindo os resultados na tela
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

    return 0;
}