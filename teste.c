#include <semaphore.h>
#include <stdio.h>

int main()
{
    printf("HELLO GALAXY!\n");
    sem_t s1, s2;

    sem_init(&s1, 0, 1);
    sem_init(&s2, 0, 1);

    //NEGAR O SEMAFORO
    //
    if(!sem_wait(&s1) && !sem_wait(&s2))
    {
        int value1, value2;
        printf("SAPORRA FUNCIONA\n");
        sem_getvalue(&s1, &value1);
        sem_getvalue(&s2, &value2);

        printf("%d, %d\n", value1, value2);
    }

    printf("FODEU\n");

    sem_destroy(&s1);
    sem_destroy(&s2);
    return 0;
}