#define _XOPEN_SOURCE 500

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>

int is_pause = 0;
unsigned long number = 1000000001;
unsigned long last_p = 1000000001;

int prime(unsigned long n)
{
  unsigned long i, max;

  if ((n & 1) == 0)
    return 0;

  max = sqrt(n);
  for (i = 3; i <= max; i += 2)
    if ((n % i) == 0)
      return 0;

  return 1;
}

void print_job(int sig)
{
  printf("zadnji prosti broj = %lu\n", last_p);
}

void stop(int sig)
{
  print_job(-1);
  exit(0);
}

void toggle()
{
  is_pause = !is_pause;
}

int main(void)
{
  sigset(SIGINT, toggle);
  sigset(SIGQUIT, stop);

  struct itimerval t;
  sigset(SIGALRM, print_job);

  t.it_value.tv_sec = 5;
  t.it_value.tv_usec = 0;
  t.it_interval.tv_sec = 5;
  t.it_interval.tv_usec = 0;

  setitimer(ITIMER_REAL, &t, NULL);

  while (1)
  {
    if (prime(number))
      last_p = number;

    number++;

    while (is_pause)
      pause();
  }
}
