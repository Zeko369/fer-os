#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define FILENAME "./ispis.txt"
#define SLEEP_TIME_US 10000

// thread store
int processStore;

// IPC store
int outputId;
int *outputStore;

void input_thread(void *x) {
    printf("Pokrenuta ULAZNA DRETVA\n");

    for (int i = 0; i < *((int *) x); i++) {
        sleep(rand() % 5 + 1);

        while (processStore != 0) {
            usleep(SLEEP_TIME_US);
        }

        processStore = rand() % 100 + 1;
        printf("ULAZNA DRETVA: broj %d\n", processStore);
    }

    printf("Završila ULAZNA DRETVA\n");
}

void process_thread(void *x) {
    printf("Pokrenuta RADNA DRETVA\n");

    for (int i = 0; i < *((int *) x); i++) {
        while (processStore == 0 || *outputStore != 0) {
            usleep(SLEEP_TIME_US);
        }

        printf("RADNA DRETVA: pročitan broj %d i povećan na %d\n", processStore, processStore + 1);
        *outputStore = processStore + 1;

        processStore = 0;
    }

    printf("Završila RADNA DRETVA\n");
}

void output_process(int count) {
    printf("Pokrenut IZLAZNI PROCES\n");
    FILE *file = fopen(FILENAME, "a+");

    for (int i = 0; i < count; i++) {
        while (*outputStore == 0) {
            usleep(SLEEP_TIME_US);
        }

        printf("IZLAZNI PROCES: broj upisan u datoteku %d\n", *outputStore);
        fprintf(file, "%d\n", *outputStore);
        *outputStore = 0;
    }

    fclose(file);
    printf("Završio IZLAZNI PROCES\n");
}

void cleanup(int signal) {
    (void) shmdt((char *) outputStore);
    (void) shmctl(outputId, IPC_RMID, NULL);
    exit(0);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Expected 1 argument with number of numbers to generate");
        return 1;
    }

    int count = atoi(argv[1]);
    srand(time(NULL));

    outputId = shmget(IPC_PRIVATE, sizeof(int), 0600);
    if (outputId == -1)
        exit(1);

    outputStore = (int *) shmat(outputId, NULL, 0);
    *outputStore = 0;
    sigset(SIGINT, cleanup);

    if (fork()) {
        output_process(count);
        exit(0);
    }

    if (fork()) {
        pthread_t thr_id[2];

        if (pthread_create(&thr_id[0], NULL, input_thread, &count) != 0) {
            printf("Error starting input thread\n");
            exit(1);
        }
        if (pthread_create(&thr_id[0], NULL, process_thread, &count) != 0) {
            printf("Error starting process thread\n");
            exit(1);
        }

        pthread_join(thr_id[0], NULL);
        pthread_join(thr_id[1], NULL);

        exit(0);
    }

    // Wait for both processes to finish
    (void) wait(NULL);
    (void) wait(NULL);

    return 0;
}
