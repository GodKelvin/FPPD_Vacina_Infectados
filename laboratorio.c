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