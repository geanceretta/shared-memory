#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>

#define CAPACIDADE_MESA 10
#define GAULESES 4

void retiraJavali(char nome);

struct shmbuf {
    sem_t *mutex, *sem_cozinhar, *sem_servir;
    int javalis_grelhados;
};

struct shmbuf *shmp;

void* process_g(void* unused){
    char nome = 'g';
    while(1){
        retiraJavali(nome);
        sleep((rand()%4)+1);
    }
    pthread_exit(NULL);
}

void* process_e(void* unused){
    char nome = 'e';
    while(1){
        retiraJavali(nome);
        sleep((rand()%4)+1);
    }
    pthread_exit(NULL);
}

void* process_a(void* unused){
    char nome = 'a';
    while(1){
        retiraJavali(nome);
        sleep((rand()%4)+1);
    }
    pthread_exit(NULL);
}

void* process_n(void* unused){
    char nome = 'n';
    while(1){
        retiraJavali(nome);
        sleep((rand()%4)+1);
    }
    pthread_exit(NULL);
}

void retiraJavali(char nome) {
    sem_wait(shmp->mutex);
    if (shmp->javalis_grelhados > 0) {
        printf("Gaules %c comendo o javali %d\n", nome, shmp->javalis_grelhados);
        shmp->javalis_grelhados--;
        sem_post(shmp->mutex);
        
    } else if (shmp->javalis_grelhados == 0) {
        printf("Gaules %c acordando o cozinheiro\n", nome);
        sem_post(shmp->sem_cozinhar);
        sem_post(shmp->mutex);
        sem_wait(shmp->sem_servir);
    } else {
        sem_post(shmp->mutex);
    }
}

int main (int argc, char* argv[]){
    int fd;
    char nomes_gauleses[4] = "gean";
    printf("Associando-se a memoria compartilhada\n");
    fd = shm_open(argv[1], O_RDWR, 0);
    shmp = mmap(NULL, sizeof(struct shmbuf),
                PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    
    printf("Criando threads\n");
    pthread_t thread_g, thread_e, thread_a, thread_n;
    pthread_create(&thread_g, NULL, process_g, NULL);
    pthread_create(&thread_e, NULL, process_e, NULL);
    pthread_create(&thread_a, NULL, process_a, NULL);
    pthread_create(&thread_n, NULL, process_n, NULL);
    
    printf("Aguardando threads finalizarem\n");
    pthread_join(thread_g, NULL);
    pthread_join(thread_e, NULL);
    pthread_join(thread_a, NULL);
    pthread_join(thread_n, NULL);
}
