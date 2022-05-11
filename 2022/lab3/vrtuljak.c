#define _XOPEN_SOURCE 500

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

#define TIME 4
#define CAROUSEL_MAX_PEOPLE 5
#define DEFAULT_PEOPLE 13

sem_t *enter_sem;
sem_t *enter_sem_confirm;

sem_t *exit_sem;
sem_t *exit_sem_confirm;

void init_semaphore(sem_t **sem);
void cleanup(int sig);

void carousel_process() {
  while(1) {
    for(int i =0 ;i < CAROUSEL_MAX_PEOPLE; i++) {
      sem_post(enter_sem);
    }
    for(int i = 0; i < CAROUSEL_MAX_PEOPLE; i++) {
      sem_wait(enter_sem_confirm);
      printf("Enter: %d/%d\n", i + 1, CAROUSEL_MAX_PEOPLE);
    }

    printf("---------------- Carousel is starting ----------------\n");
    for (int i = 0; i < TIME; i++) {
      printf("....\n");
      sleep(1);
    }
    printf("---------------- Carousel has stopped  ----------------\n");

    for(int i = 0; i < CAROUSEL_MAX_PEOPLE; i++) {
      sem_post(exit_sem);
    }
    for(int i = 0; i < CAROUSEL_MAX_PEOPLE; i++) {
      sem_wait(exit_sem_confirm);
      printf("Exit: %d/%d\n", CAROUSEL_MAX_PEOPLE - i, CAROUSEL_MAX_PEOPLE);
    }

    printf("---------------- Carousel is empty     ----------------\n\n");
  }
}

void person_process() {
  sem_wait(enter_sem);
  sem_post(enter_sem_confirm);

  sem_wait(exit_sem);
  sem_post(exit_sem_confirm);
}

int main(void) {
  // Initialize semaphores in process
  init_semaphore(&enter_sem);
  init_semaphore(&enter_sem_confirm);
  init_semaphore(&exit_sem);
  init_semaphore(&exit_sem_confirm);

  sigset(SIGINT, cleanup);

  if(fork() == 0) {
    carousel_process();
    exit(0);
  }

  for(int i = 0; i < DEFAULT_PEOPLE; i++) {
    if(fork() == 0) {
      person_process();
      exit(0);
    }
  }

  for(int i = 0; i < DEFAULT_PEOPLE; i++) {
    wait(NULL);
  }

  wait(NULL);
  cleanup(-1);

  return 0;
}

void init_semaphore(sem_t **sem) {
  int id = shmget(IPC_PRIVATE, sizeof(sem_t), IPC_CREAT | 0644);
  *sem = (sem_t *)shmat(id, NULL, 0);
  shmctl(id, IPC_RMID, NULL);
  sem_init(*sem, 1, 0);
}

void cleanup(int sig) {
  sem_destroy(enter_sem);
  sem_destroy(enter_sem_confirm);
  sem_destroy(exit_sem);
  sem_destroy(exit_sem_confirm);

  shmdt(enter_sem);
  shmdt(enter_sem_confirm);
  shmdt(exit_sem);
  shmdt(exit_sem_confirm);

  exit(0);
}
