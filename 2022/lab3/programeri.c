#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

// HARD CODED OPTIONS
// #define RANDOM_ORDER 1
#define PROGRAMER_THREADS 14

// // RANDOM OPTIONS
// #define RANDOM_ORDER 1
// #define PROGRAMER_THREADS 20

#define COND_VALUE 2

#define MS 0
#define LINUX 1
#define GET_NAME(type) (type == MS ? "MS" : "Linux")

pthread_mutex_t mutex;
pthread_cond_t cond[2];

int outside[2] = {0, 0};
int inside[2] = {0, 0};
int already_ate = 0;

void enter_restaurant(int type) {
  pthread_mutex_lock(&mutex);

  outside[type]++;
  while(inside[1-type] > 0 || (already_ate > COND_VALUE && outside[1-type] > 0)) {
    pthread_cond_wait(&cond[type], &mutex);
  }
  inside[type]++;
  outside[type]--;

  if(outside[1-type] > 0) {
    already_ate++;
  }

  pthread_mutex_unlock(&mutex);
}

void exit_restaurant(int type) {
  pthread_mutex_lock(&mutex);

  inside[type]--;
  if(inside[type] == 0) {
    already_ate = 0;
    pthread_cond_broadcast(&cond[1-type]);
  }

  pthread_mutex_unlock(&mutex);
}

void *programer_thread(void *arg) {
  int type = *(int *)arg;

  enter_restaurant(type);
  printf("%s: Entered restaurant\n", GET_NAME(type));
  sleep(rand() % 5 + 1);
  exit_restaurant(type);
  printf("%s: Left\n", GET_NAME(type));

  return NULL;
}

int main() {
  srand((unsigned)time(NULL));

  pthread_t threads[PROGRAMER_THREADS];

  #ifdef RANDOM_ORDER
  int types[PROGRAMER_THREADS];
  for(int i = 0; i < PROGRAMER_THREADS; i++) {
    types[i] = rand() % 2;
  }
  #else
  int types[PROGRAMER_THREADS] = {MS, LINUX, LINUX, LINUX, MS, MS, MS, MS, MS, MS, MS, LINUX, LINUX, LINUX};
  #endif

  pthread_mutex_init(&mutex, NULL);
  pthread_cond_init (&cond[0], NULL);
  pthread_cond_init (&cond[1], NULL);

  for(int i = 0; i < PROGRAMER_THREADS; i++) {
    pthread_create(&threads[i], NULL, programer_thread, &types[i]);
  }

  for (int i = 0; i < 10; i++) {
    pthread_join (threads[i], NULL);
  }

  pthread_mutex_destroy(&mutex);
  pthread_cond_destroy(&cond[0]);
  pthread_cond_destroy(&cond[1]);

  return 0;
}
