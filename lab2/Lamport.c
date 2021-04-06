#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdatomic.h>
#include <pthread.h>

#define MAX_THREADS 10000

int store;
int n_threads;
int n_increase;
pthread_t thr_id[MAX_THREADS];
atomic_int ENTER[MAX_THREADS], COUNT[MAX_THREADS];

int max_number()
{
  int m = -1;
  for (int i = 0; i < n_threads; i++)
    if (COUNT[i] > m)
      m = COUNT[i];

  return m;
}

void take_mutex(int i)
{
  ENTER[i] = 1;
  COUNT[i] = max_number() + 1;
  ENTER[i] = 0;

  for (int j = 0; j < n_threads; j++)
  {
    while (ENTER[j] != 0)
      ;
    while (COUNT[j] != 0 && (COUNT[j] < COUNT[i] || (COUNT[j] == COUNT[i] && j < i)))
      ;
  }
}

void release_mutex(int i)
{
  COUNT[i] = 0;
}

void *increase(void *index)
{
  int id = -1;
  pthread_t thread = pthread_self();

  for (int i = 0; i < n_threads; i++)
    if (thr_id[i] != NULL)
      if (thread->__sig == thr_id[i]->__sig)
        id = i;

  if (id < 0)
  {
    printf("Error initializing a thread");
    exit(1);
  }

  for (int i = 0; i < n_increase; i++)
  {
    take_mutex(id);
    store++;
    release_mutex(id);
  }
}

int main(int argc, char *argv[])
{
  if (argc != 3)
  {
    printf("Wrong number of arguments\n");
    exit(1);
  }

  long num_threads = strtol(argv[1], NULL, 10);
  long num_increase = strtol(argv[2], NULL, 10);

  if (num_threads > INT_MAX || num_threads < INT_MIN || num_increase < INT_MIN || num_increase > INT_MAX)
  {
    printf("Conversion error\n");
    exit(1);
  }

  n_threads = num_threads;
  n_increase = num_increase;

  store = 0;

  for (int i = 0; i < n_threads; i++)
  {
    ENTER[i] = 0;
    COUNT[i] = 0;

    thr_id[i] = NULL;

    int tmp = i;
    if (pthread_create(&thr_id[i], NULL, increase, (void *)&tmp) != 0)
    {
      printf("Error starting thread\n");
      exit(1);
    }
  }

  for (int i = 0; i < n_threads; i++)
  {
    pthread_join(thr_id[i], NULL);
  }

  printf("%d\n", store);

  return 0;
}