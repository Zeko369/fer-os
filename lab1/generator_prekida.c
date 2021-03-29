#include <stdio.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

int pid = 0;

void stop_fn(int sig)
{
  kill(pid, SIGKILL);
  exit(0);
}

int signals[4] = {SIGINT, SIGSTOP, SIGCONT, SIGTERM};
char signal_names[4][8] = {"SIGINT", "SIGSTOP", "SIGCONT", "SIGTERM"};

int main(int argc, char *argv[])
{
  if (argc != 2)
  {
    printf("You need to pass exactily one argument (PID of process to bombard with SIGs)\n");
    return 1;
  }

  pid = atoi(argv[1]);
  sigset(SIGINT, stop_fn);

  srand(time(NULL));
  while (1)
  {
    int t = 3 + rand() % 3;
    printf("Sleeping: %d\n", t);
    sleep(t);

    int index = rand() % 4;
    printf("Sending %s\n", signal_names[index]);
    kill(pid, signals[index]);
  }

  return 0;
}
