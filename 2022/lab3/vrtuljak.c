#define _XOPEN_SOURCE 500

#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>

#define TIME 4
#define CAROUSEL_MAX_PEOPLE 5
#define DEFAULT_PEOPLE 9

// #define DEBUG 1

sem_t *free_space;
sem_t *people_entered;
sem_t *people_exited;
sem_t *people_exited_full;

void error_wrapped_sem_post(sem_t *sem) {
    if (sem_post(sem) == -1) {
        perror("sem_post");
        exit(EXIT_FAILURE);
    }
}

void error_wrapped_sem_wait(sem_t *sem) {
    if (sem_wait(sem) == -1) {
        perror("sem_wait");
        exit(EXIT_FAILURE);
    }
}

sem_t *error_wrapped_sem_init(char *name, int value) {
    sem_unlink(name);
    sem_t *tmp = sem_open(name, O_CREAT, 0644, value);
    if (tmp == SEM_FAILED) {
        perror("sem_open");
        exit(1);
    }

    return tmp;
}

void error_wrapped_sem_close(sem_t *sem) {
    if (sem_close(sem) == -1) {
        perror("sem_close");
        exit(EXIT_FAILURE);
    }
}

void print_all_semaphores() {
#ifdef DEBUG
    sem_t *semaphores[] = {free_space, people_entered, people_exited, people_exited_full};
    for (int i = 0; i < 4; i++) {
        int tmp = -1;
        if (sem_getvalue(semaphores[i], &tmp) == -1) {
            perror("sem_getvalue");
            exit(EXIT_FAILURE);
        }
        printf("%d=%d, ", i, tmp);
    }
    printf("\n");
#endif
}

void cleanup(int sig) {
    printf("Closing...\n");
    error_wrapped_sem_close(free_space);
    error_wrapped_sem_close(people_entered);
    error_wrapped_sem_close(people_exited);
    error_wrapped_sem_close(people_exited_full);

    exit(0);
}

void *carousel_thread(void *arg) {
    while (1) {
        print_all_semaphores();
        for (int i = 0; i < CAROUSEL_MAX_PEOPLE; i++) {
            error_wrapped_sem_post(free_space);
        }

        print_all_semaphores();
        for (int i = 0; i < CAROUSEL_MAX_PEOPLE; i++) {
            error_wrapped_sem_wait(people_entered);
            // printf("enter: %d/%d\n", i + 1, CAROUSEL_MAX_PEOPLE);
        }
        print_all_semaphores();

        sleep(1);

        printf("---------------- Carousel is starting ----------------\n");
        for (int i = 0; i < TIME; i++) {
            printf("....\n");
            sleep(1);
        }
        printf("---------------- Carousel is stopped  ----------------\n");

        print_all_semaphores();
        for (int i = 0; i < CAROUSEL_MAX_PEOPLE; i++) {
            error_wrapped_sem_post(people_exited);
        }
        print_all_semaphores();
        for (int i = 0; i < CAROUSEL_MAX_PEOPLE; i++) {
            error_wrapped_sem_wait(people_exited_full);
            // printf("exit: %d/%d\n", CAROUSEL_MAX_PEOPLE - i - 1, CAROUSEL_MAX_PEOPLE);
        }
        print_all_semaphores();

        printf("Off-boarding done\n");
    }
};

void *person_thread(void *arg) {
    int *id = (int *) arg;

    printf("Person %d is in line for the carousel\n", *id);
    error_wrapped_sem_wait(free_space);
    printf("Person %d is in the carousel\n", *id);
    error_wrapped_sem_post(people_entered);

    error_wrapped_sem_wait(people_exited);
    printf("Person %d exited the carousel\n", *id);
    error_wrapped_sem_post(people_exited_full);
}

int main(int argc, char *argv[]) {
    free_space = error_wrapped_sem_init("/free_space", 0);
    people_entered = error_wrapped_sem_init("/people_entered", 0);
    people_exited = error_wrapped_sem_init("/people_exited", 0);
    people_exited_full = error_wrapped_sem_init("/people_exited_full", 0);

    sigset(SIGINT, cleanup);

    pthread_t carousel_thread_id;
    pthread_t person_thread_id[DEFAULT_PEOPLE];

    int tmp[DEFAULT_PEOPLE];
    for (int i = 0; i < DEFAULT_PEOPLE; i++) {
        tmp[i] = i;

        if (pthread_create(&person_thread_id[i], NULL, person_thread, (void *) &tmp[i])) {
            printf("Error creating person thread\n");
            return 1;
        };
    }

    sleep(1);
    printf("Starting carousel\n");

    if (pthread_create(&carousel_thread_id, NULL, carousel_thread, NULL)) {
        printf("Error creating carousel thread\n");
        return 1;
    }

    for (int i = 0; i < DEFAULT_PEOPLE; i++) {
        pthread_join(person_thread_id[i], NULL);
    }
    pthread_join(carousel_thread_id, NULL);

    cleanup(-1);

    return 0;
}

