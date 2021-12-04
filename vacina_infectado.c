#include <stdio.h>
#include <sempahore.h>

/*
Convencionando que:
	virus morto 	== 1
	injecao         == 2
	insumo secreto  == 3
*/
typedef struct Infectado
{
    int virus_morto;
    int injecao;
    int insumo_secreto;
	int insumo_infinito;
	int quantidade_vacinas_aplicadas;
    sem_t usando_bancada;


}Infectado;

typedef struct Laboratorio
{
    int ingrediente_1;
    int ingrediente_2;
	int qtd_renova_estoque;
    sem_t renova_estoque;


}Laboratorio;


typedef struct Ingrediente
{
	//VM == 1, INJECAO == 2, IS == 3
	int qual_ingrediente;
	Laboratorio* pertence;

}Ingrediente;

typedef struct Bancada
{
	int virus_morto;
	int injecao;
	int insumo_secreto;


}Bancada;

int main()
{

    return 0;
}