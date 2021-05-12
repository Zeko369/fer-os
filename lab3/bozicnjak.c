#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdatomic.h>
#include <unistd.h>
#include <semaphore.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <time.h>

int id;

atomic_int *br_sobova;
atomic_int *br_patuljaka;

sem_t *santa;
sem_t *w;
sem_t *reindeer;
sem_t *help;

void die() { exit(0); }

void Reindeer()
{
    sigset(SIGINT, die);
    sem_wait(w);

    (*br_sobova)++;
    printf("Reindeer at gate (%d)\n", *br_sobova);

    if (*br_patuljaka == 10)
    {
        sem_post(santa);
    }

    sem_post(w);
    sem_wait(reindeer);
}

void Elf()
{
    sigset(SIGINT, die);
    sem_wait(w);

    (*br_patuljaka)++;
    printf("Elf at gate (%d)\n", *br_patuljaka);

    if (*br_patuljaka == 3)
    {
        sem_post(santa);
    }

    sem_post(w);
    sem_wait(help);
}

int NorthPoleProcess()
{
    sigset(SIGINT, die);
    while (1)
    {
        sleep(rand() % 5 + 1);
        if (rand() % 2 > 0 && *br_sobova < 10)
        {
            if (fork() == 0)
            {
                Reindeer();
                exit(0);
            }
        }

        if (rand() % 2 > 0)
        {
            if (fork() == 0)
            {
                Elf();
                exit(0);
            }
        }
    }
}

void SantaProcess()
{
    sigset(SIGINT, die);
    while (1)
    {
        printf("Santa sleep\n");
        sem_wait(santa);
        sem_wait(w);

        printf("Santa up\n");
        if (*br_sobova == 10 && br_patuljaka > 0)
        {
            // final
            sem_post(w);
            printf("Christmass 🥳🎄\n");
            sleep(2);
            sem_wait(w);

            for (int i = 0; i < 10; i++)
            {
                sem_post(reindeer);
            }

            *br_sobova = 0;
        }

        if (*br_sobova == 10)
        {
            printf("Reindeers(10) at gate\n");
            sem_post(w);

            printf("Reindeers eating\n");
            sleep(2);
            sem_wait(w);
        }

        while (*br_patuljaka >= 3)
        {
            printf("Elves(>=3) at gate\n");
            sem_post(w);

            printf("Santa debugging\n");
            sleep(2);
            sem_wait(w);

            // could/should have been a for
            sem_post(help);
            sem_post(help);
            sem_post(help);

            *br_patuljaka -= 3;
        }
        sem_post(w);
    }
}

int main(void)
{
    srand((int)time(NULL));

    size_t size = sizeof(sem_t) * 4 + sizeof(atomic_int) * 2;
    id = shmget(IPC_PRIVATE, size, 0600);
    if (id == -1)
    {
        exit(1);
    }

    br_sobova = (atomic_int *)(shmat(id, NULL, 0) + sizeof(sem_t) * 4);
    br_patuljaka = (atomic_int *)(br_sobova + sizeof(atomic_int));

    santa = (sem_t *)shmat(id, NULL, 0);
    w = (sem_t *)(santa + sizeof(sem_t));
    reindeer = (sem_t *)(santa + sizeof(sem_t) * 2);
    help = (sem_t *)(santa + sizeof(sem_t) * 3);

    shmctl(id, IPC_RMID, NULL);

    sem_init(santa, 1, 0);
    sem_init(w, 1, 1);

    sem_init(reindeer, 1, 0);
    sem_init(help, 1, 0);

    if (fork() == 0)
    {
        NorthPoleProcess();
        exit(0);
    }

    if (fork() == 0)
    {
        SantaProcess();
        exit(0);
    }

    // both processes
    (void)wait(NULL);
    (void)wait(NULL);

    sem_destroy(santa);
    sem_destroy(w);
    sem_destroy(reindeer);
    sem_destroy(help);

    shmdt(santa);
    shmdt(w);
    shmdt(reindeer);
    shmdt(help);

    return 0;
}
