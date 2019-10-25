#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>

#define CAPACIDADE_MESA 10

struct shmbuf {
    sem_t *mutex, *sem_cozinhar, *sem_servir;
    int javalis_grelhados;
};

struct shmbuf *shmp;

void* process_c(void* unused){
    while(1){
        printf("Cozinheiro dormindo ate ser acordado por um gaules...\n");
        sem_wait(shmp->sem_cozinhar);
        printf("Cozinheiro aguardando receber os pratos vazios...\n");
        sem_wait(shmp->mutex);
        for (int i = 0; i < CAPACIDADE_MESA; i++) {
            shmp->javalis_grelhados++;
            printf("Cozinheiro: grelhando javali %d/%d\n", shmp->javalis_grelhados, CAPACIDADE_MESA);
            sleep((rand()%4)+1);
        }
        sem_post(shmp->mutex);
        sem_post(shmp->sem_servir);
    }
    pthread_exit(NULL);
}

int main (int argc, char* argv[]){
    printf("Desconectando semaforos fantasma\n");
    // Unlink named semaphores
    sem_unlink("/mutex");
    sem_unlink("/sem_cozinhar");
    sem_unlink("/sem_servir");
    
    int fd;
    printf("Iniciando memoria compartilhada\n");
    shm_unlink(argv[1]);
    fd = shm_open(argv[1], O_CREAT|O_EXCL|O_RDWR, S_IRUSR|S_IWUSR);
    ftruncate(fd, sizeof(struct shmbuf));
    shmp = mmap(NULL, sizeof(struct shmbuf),
                PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    
    printf("Iniciando semaforos\n");
    // Semaphores initialization
    //sem_init(&shmp->sem, 1, 1);
    shmp->mutex = sem_open("/mutex", O_CREAT, 0644, 1);
    shmp->sem_cozinhar = sem_open("/sem_cozinhar", O_CREAT, 0644, 0);
    shmp->sem_servir = sem_open("/sem_servir", O_CREAT, 0644, 0);
    
    if (argc > 2)
        shmp->javalis_grelhados = atoi(argv[2]);
    
    printf("Criando thread\n");
    // Thread creation
    pthread_t thread_c;
    pthread_create(&thread_c, NULL, process_c, NULL);
    
    printf("Aguardando finalizacao das threads\n");
    // Wait all threads to finish
    pthread_join(thread_c, NULL);
    
    printf("Fechando semaforos \n");
    // Close named semaphores
    sem_close(shmp->mutex);
    sem_close(shmp->sem_cozinhar);
    sem_close(shmp->sem_servir);
    
    printf("Desconectando semaforos\n");
    // Unlink named semaphores
    sem_unlink("/mutex");
    sem_unlink("/sem_cozinhar");
    sem_unlink("/sem_servir");
}
