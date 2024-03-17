#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define NUM_TRENS 30
#define MAX_TRENS 30

sem_t semaforo;
pthread_mutex_t mutex;
pthread_cond_t cond;

typedef struct Trem {
    int id_trem;
    char prioridade[6];
    char entrada[3];
    char saida[3];
    int chegada;
} Trem;

Trem fila_trens[MAX_TRENS];
int qtd_trens = 0;
int contador_chegada = 0;

int prioridade(const char* p1) {
    if (strcmp(p1, "ALTA") == 0) return 3;
    if (strcmp(p1, "MEDIA") == 0) return 2;
    if (strcmp(p1, "BAIXA") == 0) return 1;
    return 0;
}

void fila_espera(Trem* trem) {
    int posicao_insercao = qtd_trens;

    // Encontre a posição de inserção, que é logo após o último trem com a mesma entrada
    for (int i = 0; i < qtd_trens; i++) {
        if (strcmp(fila_trens[i].entrada, trem->entrada) == 0) {
            posicao_insercao = i + 1;
        }
    }

    // Mova os trens para frente para criar espaço para o novo trem
    for (int j = qtd_trens; j > posicao_insercao; j--) {
        fila_trens[j] = fila_trens[j - 1];
    }

    // Insira o novo trem na posição encontrada
    fila_trens[posicao_insercao] = *trem;
    qtd_trens++;
}

Trem* proximo_trem() {
    // Se não houver trens, retorne NULL
    if (qtd_trens == 0) {
        return NULL;
    }

    // Inicializa os índices dos trens candidatos para cada entrada
    int indice_A1 = -1;
    int indice_B1 = -1;

    // Encontra o primeiro trem em cada entrada
    for (int i = 0; i < qtd_trens; i++) {
        if (strcmp(fila_trens[i].entrada, "A1") == 0 && indice_A1 == -1) {
            indice_A1 = i;
        }
        if (strcmp(fila_trens[i].entrada, "B1") == 0 && indice_B1 == -1) {
            indice_B1 = i;
        }
    }

    // Se não houver trens em uma das entradas, retorna o primeiro da outra entrada
    if (indice_A1 == -1) return &fila_trens[indice_B1];
    if (indice_B1 == -1) return &fila_trens[indice_A1];
    
    // Se o primeiro trem em qualquer fila estiver atrás de outro trem na mesma fila, ele não pode cruzamento ainda.
    if ((indice_A1 > 0 && strcmp(fila_trens[indice_A1 - 1].entrada, "A1") == 0) ||
        (indice_B1 > 0 && strcmp(fila_trens[indice_B1 - 1].entrada, "B1") == 0)) {
        return NULL;
    }

    // Comparar prioridades e ordem de chegada
    int prioridade_A1 = prioridade(fila_trens[indice_A1].prioridade);
    int prioridade_B1 = prioridade(fila_trens[indice_B1].prioridade);

    if (prioridade_A1 > prioridade_B1 || 
       (prioridade_A1 == prioridade_B1 && fila_trens[indice_A1].chegada < fila_trens[indice_B1].chegada)) {
        return &fila_trens[indice_A1];
    } else {
        return &fila_trens[indice_B1];
    }
}

void remover_trem_aguardando(Trem* trem) {
    int encontrado = 0;
    for (int i = 0; i < qtd_trens; i++) {
        if (fila_trens[i].id_trem == trem->id_trem) {
            encontrado = 1;
        }
        if (encontrado && i < qtd_trens - 1) {
            fila_trens[i] = fila_trens[i + 1];
        }
    }
    if (encontrado) {
        qtd_trens--;
    }
}

void* cruzamento(void* arg) {
    Trem* trem = (Trem*)arg;

    pthread_mutex_lock(&mutex);
    trem->chegada = contador_chegada++;
    fila_espera(trem);
    pthread_mutex_unlock(&mutex);
    printf("Trem %d, de prioridade %s, está chegando no cruzamento por %s.\n", trem->id_trem, trem->prioridade, trem->entrada);
    
    while (1) {
        pthread_mutex_lock(&mutex);
        Trem* proximo = proximo_trem();
       if (proximo && proximo->id_trem == trem->id_trem) {
            sem_wait(&semaforo);
            remover_trem_aguardando(trem);
            printf("Trem %d, de prioridade %s, está cruzando em direção à saída %s.\n", trem->id_trem, trem->prioridade, trem->saida);
            pthread_mutex_unlock(&mutex);
            sleep(rand() % 3 + 1);
            printf("Trem %d, de prioridade %s, cruzou a saída %s.\n", trem->id_trem, trem->prioridade, trem->saida);
            sem_post(&semaforo);
            break;
        }
        else{
        pthread_mutex_unlock(&mutex);
        usleep(100000);
        }
    }

    pthread_mutex_lock(&mutex);
    pthread_cond_broadcast(&cond);
    pthread_mutex_unlock(&mutex);

    return NULL;
}

int main() {
    pthread_t threads[NUM_TRENS];
    Trem trem_data[NUM_TRENS];

    sem_init(&semaforo, 0, 1);
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);

    char* prioridades[] = {"ALTA", "MEDIA", "BAIXA"};
    char* entradas[] = {"A1", "B1"};
    char* saidas[] = {"A2", "B2"};
    srand(time(NULL));

    for (int i = 0; i < NUM_TRENS; ++i) {
        trem_data[i].id_trem = i;
        strcpy(trem_data[i].prioridade, prioridades[rand() % 3]);
        strcpy(trem_data[i].entrada, entradas[rand() % 2]);
        strcpy(trem_data[i].saida, saidas[rand() % 2]);
        trem_data[i].chegada = contador_chegada++;

        if (pthread_create(&threads[i], NULL, cruzamento, &trem_data[i])) {
            perror("Falha ao criar a thread do trem");
            return 1;
        }
        sleep(1); // Garantir ordem de chegada
    }

    for (int i = 0; i < NUM_TRENS; ++i) {
        pthread_join(threads[i], NULL);
    }

    sem_destroy(&semaforo);
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);

    return 0;
}
