#include <pthread.h>

//Inclusao da minha biblioteca
#include "vacina.h"

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
			//Verificando se possui injecao
			if(!sem_trywait(infectado->bancada->s_injecao))
			{
				//Se conseguiu pegar os dois ingredientes
				if(!sem_trywait(infectado->bancada->s_insumo_secreto))
				{
					//Vai consegui pegar os dois ingredientes
					//Verificar qual injecao foi pega
					pthread_mutex_lock(infectado->mutex);
					if(infectado->bancada->injecao[0].disponivel)
					{	
						//Informo que nao esta mais disponivel o respectivo ingrediente deste laboratorio
						infectado->bancada->injecao[0].disponivel = 0;
						//Informo ao laboratorio que o respectivo ingrediente foi consumido
						sem_post(infectado->bancada->injecao[0].pertence_lab->renova_estoque);
					}
					else if(infectado->bancada->injecao[1].disponivel)
					{
						infectado->bancada->injecao[1].disponivel = 0;
						sem_post(infectado->bancada->injecao[1].pertence_lab->renova_estoque);
					}

					//Verificar qual insumo secreto foi pego
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

					if(infectado->qtd_vacinas_aplicadas >= infectado->qtd_min_vacinas_aplicadas)
					{
						//Este infectado ja trabalhou o suficiente
						infectado->trabalho[0] = 1;
					}
					//Se nao precisar mais de trabalhar, informa a todos os labs
					if(!run_and_work(infectado->trabalho))
					{
						sem_post(infectado->bancada->virus_morto[0].pertence_lab->renova_estoque);
						sem_post(infectado->bancada->virus_morto[1].pertence_lab->renova_estoque);
						sem_post(infectado->bancada->injecao[0].pertence_lab->renova_estoque);
						sem_post(infectado->bancada->injecao[1].pertence_lab->renova_estoque);
						sem_post(infectado->bancada->insumo_secreto[0].pertence_lab->renova_estoque);
						sem_post(infectado->bancada->insumo_secreto[1].pertence_lab->renova_estoque);
					}
				}
				else
				{
					//Informo que soltei a injecao, pois nao tem insumo_secreto
					sem_post(infectado->bancada->s_injecao);
				}
			}
		}
	}

	//Precisa dos ingredientes 1 e 3 (virus morto e insumo secreto)
	else if(infectado->ingrediente_infinito == 2)
	{
		while(run_and_work(infectado->trabalho))
		{
			if(!sem_trywait(infectado->bancada->s_virus_morto))
			{
				if(!sem_trywait(infectado->bancada->s_insumo_secreto))
				{
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

					//Verificar qual insumo secreto foi pego
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

					if(infectado->qtd_vacinas_aplicadas >= infectado->qtd_min_vacinas_aplicadas)
					{
						//Este infectado ja trabalhou o suficiente
						infectado->trabalho[1] = 1;
					}
					//Se nao precisar mais de trabalhar, informa a todos os labs
					if(!run_and_work(infectado->trabalho))
					{
						sem_post(infectado->bancada->virus_morto[0].pertence_lab->renova_estoque);
						sem_post(infectado->bancada->virus_morto[1].pertence_lab->renova_estoque);
						sem_post(infectado->bancada->injecao[0].pertence_lab->renova_estoque);
						sem_post(infectado->bancada->injecao[1].pertence_lab->renova_estoque);
						sem_post(infectado->bancada->insumo_secreto[0].pertence_lab->renova_estoque);
						sem_post(infectado->bancada->insumo_secreto[1].pertence_lab->renova_estoque);
					}
				}
				else
				{
					//Informo que soltei o virus, pois nao tem insumo secreto
					sem_post(infectado->bancada->s_virus_morto);
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
			
			if(!sem_trywait(infectado->bancada->s_virus_morto))
			{
				if(!sem_trywait(infectado->bancada->s_injecao))
				{
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

					//Verificar qual injecao secreto foi pega
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
					
					//Se nao precisar mais de trabalhar, informa a todos os labs
					if(!run_and_work(infectado->trabalho))
					{
						sem_post(infectado->bancada->virus_morto[0].pertence_lab->renova_estoque);
						sem_post(infectado->bancada->virus_morto[1].pertence_lab->renova_estoque);
						sem_post(infectado->bancada->injecao[0].pertence_lab->renova_estoque);
						sem_post(infectado->bancada->injecao[1].pertence_lab->renova_estoque);
						sem_post(infectado->bancada->insumo_secreto[0].pertence_lab->renova_estoque);
						sem_post(infectado->bancada->insumo_secreto[1].pertence_lab->renova_estoque);
					}
					
				}
				else
				{
					//Informo que soltei o virus morto, pois nao tem injecao
					sem_post(infectado->bancada->s_virus_morto);
				}
			}
		}
	}
	
	return 0;
}

void *run_laboratorio(void *arg)
{
	Laboratorio* laboratorio = (Laboratorio*) arg;

	//Verificar id do laboratorio
	if(laboratorio->lab_id == 0)
	{

		while(run_and_work(laboratorio->trabalho))
		{
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
			
			//Verificando se deve esperar ou avisar a todos os labs que acabou
			if(!run_and_work(laboratorio->trabalho))
			{
				sem_post(laboratorio->bancada->virus_morto[0].pertence_lab->renova_estoque);
				sem_post(laboratorio->bancada->virus_morto[1].pertence_lab->renova_estoque);
				sem_post(laboratorio->bancada->injecao[0].pertence_lab->renova_estoque);
				sem_post(laboratorio->bancada->injecao[1].pertence_lab->renova_estoque);
				sem_post(laboratorio->bancada->insumo_secreto[0].pertence_lab->renova_estoque);
				sem_post(laboratorio->bancada->insumo_secreto[1].pertence_lab->renova_estoque);
			}
			else
			{
				sem_wait(laboratorio->renova_estoque);
				sem_wait(laboratorio->renova_estoque);
			}
		}
	}
	else if(laboratorio->lab_id == 1)
	{
		while(run_and_work(laboratorio->trabalho))
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

			//Verificando se deve esperar ou avisar a todos os labs que acabou
			if(!run_and_work(laboratorio->trabalho))
			{
				sem_post(laboratorio->bancada->virus_morto[0].pertence_lab->renova_estoque);
				sem_post(laboratorio->bancada->virus_morto[1].pertence_lab->renova_estoque);
				sem_post(laboratorio->bancada->injecao[0].pertence_lab->renova_estoque);
				sem_post(laboratorio->bancada->injecao[1].pertence_lab->renova_estoque);
				sem_post(laboratorio->bancada->insumo_secreto[0].pertence_lab->renova_estoque);
				sem_post(laboratorio->bancada->insumo_secreto[1].pertence_lab->renova_estoque);
			}
			else
			{
				sem_wait(laboratorio->renova_estoque);
				sem_wait(laboratorio->renova_estoque);
			}
			
		}
	}
	else
	{
		while(run_and_work(laboratorio->trabalho))
		{
			//Estocar os produtos DESTE lab
			pthread_mutex_lock(laboratorio->mutex);
			laboratorio->bancada->injecao[1].disponivel = 1;
			laboratorio->bancada->insumo_secreto[1].disponivel = 1;
			pthread_mutex_unlock(laboratorio->mutex);

			//Avisar que foi estocado
			sem_post(laboratorio->bancada->s_injecao);
			sem_post(laboratorio->bancada->s_insumo_secreto);

			//Renovou estoque
			laboratorio->qtd_renova_estoque++;

			if(laboratorio->qtd_renova_estoque >= laboratorio->qtd_min_renova_restoque)
			{
				laboratorio->trabalho[5] = 1;
			}
			
			//Verificando se deve esperar ou avisar a todos os labs que acabou
			if(!run_and_work(laboratorio->trabalho))
			{

				sem_post(laboratorio->bancada->virus_morto[0].pertence_lab->renova_estoque);
				sem_post(laboratorio->bancada->virus_morto[1].pertence_lab->renova_estoque);
				sem_post(laboratorio->bancada->injecao[0].pertence_lab->renova_estoque);
				sem_post(laboratorio->bancada->injecao[1].pertence_lab->renova_estoque);
				sem_post(laboratorio->bancada->insumo_secreto[0].pertence_lab->renova_estoque);
				sem_post(laboratorio->bancada->insumo_secreto[1].pertence_lab->renova_estoque);
			}
			else
			{
				sem_wait(laboratorio->renova_estoque);
				sem_wait(laboratorio->renova_estoque);
			}
		}
	}

	return 0;
}