#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

int store;

void *increase(void *x)
{
  for (int i = 0; i < *((int *)x); i++)
  {
    store++;
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

  int n_threads = num_threads;
  int n_increase = num_increase;

  pthread_t thr_id[n_threads];
  store = 0;

  for (int i = 0; i < n_threads; i++)
  {
    if (pthread_create(&thr_id[i], NULL, increase, &n_increase) != 0)
    {
      printf("Greska pri stvaranju dretve!\n");
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