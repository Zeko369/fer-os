#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

long current = 0;

#define APPEND 0
#define OVERWRITE 1

#define STATUS_FILE "status.txt"
#define OUTPUT_FILE "obrada.txt"

/**
 * @param filename
 * @param value value to write
 * @param mode use 0 for append, 1 for overwrite
 */
void printToFile(char *filename, int value, int mode)
{
  FILE *file = fopen(filename, mode == APPEND ? "a+" : "w");
  fprintf(file, "%d", value);
  if (mode == APPEND)
  {
    fprintf(file, "\n");
  }

  fclose(file);
}

void userCallback()
{
  printf("Currently processing: %ld\n", current);
}

void terminateCallback()
{
  printf("\nTerminating...\n");
  printToFile(STATUS_FILE, current, OVERWRITE);
  exit(0);
}
void interruptCallback()
{
  exit(0);
}

int main()
{
  struct sigaction act;
  act.sa_handler = userCallback;
  sigemptyset(&act.sa_mask);
  sigaddset(&act.sa_mask, SIGTERM);
  act.sa_flags = 0;
  sigaction(SIGUSR1, &act, NULL);

  act.sa_handler = terminateCallback;
  sigemptyset(&act.sa_mask);
  sigaction(SIGTERM, &act, NULL);

  act.sa_handler = interruptCallback;
  sigaction(SIGINT, &act, NULL);

  // load first integer from file obrada.txt
  FILE *f = fopen(STATUS_FILE, "r");
  fscanf(f, "%ld", &current);
  fclose(f);

  if (current == 0)
  {
    f = fopen(OUTPUT_FILE, "r");
    while (fscanf(f, "%ld", &current) != EOF)
      ;
    fclose(f);

    current = (long)sqrt(current);

    if (current == 0)
    {
      current = 1;
    }
  }

  printf("Starting with PID %ld\n", (long)getpid());
  printToFile(STATUS_FILE, 0, OVERWRITE);

  while (1)
  {
    long tmp = (long)pow(current, 2);
    printToFile(OUTPUT_FILE, tmp, APPEND);
    current++;

    sleep(5);
  }

  return 0;
}
