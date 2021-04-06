#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

int id;
int *store;

void increase(int n)
{
  for (int i = 0; i < n; i++)
  {
    *store = *store + 1;
  }
}

void erase(int sig)
{
  (void)shmdt((char *)store);
  (void)shmctl(id, IPC_RMID, NULL);
  exit(0);
}

int main(int argc, char *argv[])
{
  if (argc != 3)
  {
    printf("Wrong number of arguments\n");
    exit(1);
  }

  long num_processes = strtol(argv[1], NULL, 10);
  long num_increase = strtol(argv[2], NULL, 10);

  if (num_processes > INT_MAX || num_processes < INT_MIN || num_increase < INT_MIN || num_increase > INT_MAX)
  {
    printf("Conversion error\n");
    exit(1);
  }

  int n_processes = num_processes;
  int n_increase = num_increase;

  id = shmget(IPC_PRIVATE, sizeof(int), 0600);
  if (id == -1)
    exit(1);

  store = (int *)shmat(id, NULL, 0);
  *store = 0;
  sigset(SIGINT, erase);

  for (int i = 0; i < n_processes; i++)
  {
    if (fork() == 0)
    {
      increase(n_increase);
      exit(0);
    }
  }

  for (int i = 0; i < n_processes; i++)
    (void)wait(NULL);

  printf("%d\n", *store);
  return 0;
}