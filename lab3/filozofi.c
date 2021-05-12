#include <signal.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <pthread.h>

#define N 5

int cond = 1;

char philosopher[N];
int chopstick[N];

pthread_t threads[N];

pthread_mutex_t monitor;
pthread_cond_t queue[N];

void print_line(int n)
{
    for (int i = 0; i < N; i++)
    {
        printf("%c ", philosopher[i]);
    }
    printf("(%d)\n", n + 1);
}

void think(int n)
{
    sleep(3);
}

void eat(int n)
{
    pthread_mutex_lock(&monitor);
    philosopher[n] = 'o';

    while (chopstick[(n + 1) % N] == 0 || chopstick[n] == 0)
    {
        pthread_cond_wait(&queue[n], &monitor);
    }

    chopstick[n] = 0;
    chopstick[(n + 1) % N] = 0;
    philosopher[n] = 'X';

    print_line(n);
    pthread_mutex_unlock(&monitor);

    sleep(2);

    pthread_mutex_lock(&monitor);
    chopstick[n] = 1;
    chopstick[(n + 1) % 5] = 1;
    philosopher[n] = 'O';

    pthread_cond_signal(&queue[(n - 1) % N]);
    pthread_cond_signal(&queue[(n + 1) % N]);

    print_line(n);
    pthread_mutex_unlock(&monitor);
}

void *Philosopher(void *i)
{
    int n = *((int *)i) - 1;
    while (cond)
    {
        think(n);
        eat(n);
    }
}

void end()
{
    cond = 0;
    for (int i = 0; i < N; i++)
    {
        pthread_cancel(threads[i]);
    }

    exit(0);
}

int main(void)
{
    sigset(SIGINT, end);
    pthread_mutex_init(&monitor, NULL);

    for (int i = 0; i < N; i++)
    {
        chopstick[i] = 1;
        philosopher[i] = '0';
        pthread_cond_init(&queue[i], NULL);
    }

    for (int i = 0; i < N; i++)
    {
        if (pthread_create(&threads[i], NULL, Philosopher, (void *)&i) != 0)
        {
            printf("Err starting thread\n");
            exit(1);
        }
    }

    for (int i = 0; i < N; i++)
    {
        pthread_join(threads[i], NULL);
    }

    return 0;
}