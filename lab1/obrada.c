#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>

#define N 6

int WAITING[N];
int PRIORITY[N];
int CURRENT_PRIORITY;

int signals[] = {SIGUSR1, SIGUSR2, SIGTERM, SIGSYS, SIGINT};
void forbid_interrupts()
{
  for (int i = 0; i < 5; i++)
    sighold(signals[i]);
}

void allow_interrupts()
{
  for (int i = 0; i < 5; i++)
    sigrelse(signals[i]);
}

void print_line(int index, int c, char d)
{
  for (int i = 0; i < 6; i++)
  {
    if (i > 0)
      printf(" ");

    if (index == i)
      if (d != NULL)
        printf("%2c", d);
      else
        printf("%2d", c);
    else
      printf(" -");
  }

  printf("      O_CEK[");
  for (int i = 0; i < N; i++)
    printf("%d ", WAITING[i]);
  printf("]  TEK_PRIOR=%d  PRIOR[", CURRENT_PRIORITY);
  for (int i = 0; i < N; i++)
    printf("%d ", PRIORITY[i]);
  printf("]\n");
}

void process(int index)
{
  print_line(index, 0, 'P');

  for (int i = 0; i < 5; i++)
  {
    print_line(index, i + 1, NULL);
    sleep(1);
  }

  print_line(index, 0, 'K');
}

void process_interrupt(int sig)
{
  int n = -1;
  forbid_interrupts();

  for (int i = 0; i < 5; i++)
  {
    if (signals[i] == sig)
    {
      n = i + 1;
      print_line(i + 1, 0, 'X');
      break;
    }
  }

  WAITING[n]++;

  // find next
  int x = 0;
  do
  {
    x = 0;
    for (int i = CURRENT_PRIORITY + 1; i < N; i++)
      if (WAITING[i] > 0)
        x = i;

    if (x > 0)
    {
      WAITING[x]--;
      PRIORITY[x] = CURRENT_PRIORITY;
      CURRENT_PRIORITY = x;

      allow_interrupts();
      process(x);
      forbid_interrupts();

      CURRENT_PRIORITY = PRIORITY[x];
      PRIORITY[x] = 0;
    }
  } while (x > 0);

  allow_interrupts();
}

int main(void)
{
  printf("Proces obrade prekida, PID=%ld\n", getpid());
  printf("GP S1 S2 S3 S4 S5\n");

  for (int i = 0; i < 5; i++)
  {
    sigset(signals[i], process_interrupt);
  }

  int i = 1;
  while (1)
  {
    print_line(0, i, NULL);

    sleep(1);
    i++;
  }

  printf("Zavrsio osnovni program\n");
  return 0;
}
