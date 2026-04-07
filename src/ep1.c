// Implementação do simulador de processos
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>
#include <pthread.h>

#define MAX_TASK_NUM 50
#define WORK_UNIT 100 // intervalo de verificação de preempção em ms, equivalente a unidade de trabalho
#define WORKERS 1

// --------------- VARIÁVEIS GLOBAIS DAS THREADS ---------------

pthread_mutex_t task_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  available = PTHREAD_COND_INITIALIZER;
static int work = 0;
static int die = 0;

int next_tasks[WORKERS];
Task* global_tasks = NULL;

// --------------- LÓGICA DE ENTRADA ---------------
typedef struct {
    char name[32];
    double t0;
    double deadline;
    double dt;

    // dados a serem salvos durante execução
    double tf;

    // estado da task
    double time_left;
    bool running;
    bool started;
    bool preempted;
    bool finished;
} Task;

Task* tasks_from_trace(char* filename, int* num_tasks) {
    FILE *fp = fopen(filename, "r");
    int count = 0;
    Task* queue = malloc(MAX_TASK_NUM * sizeof(Task));

    // cada linha tem: nome deadline t0 dt
    // nome é string com 32 caracteres alocados
    // deadline é instante de tempo em que gostaríamos que o processo acabe, >=t0+dt
    // t0 é quando o processo chega no sistema
    // dt é o tempo de processamento
    while(fscanf(fp, "%31s %lf %lf %lf", 
        queue[count].name, 
        &queue[count].deadline, 
        &queue[count].t0, 
        &queue[count].dt) == 4) {
    
        queue[count].tf = 0;
        queue[count].time_left = queue[count].dt;
        queue[count].running = false;
        queue[count].started = false;
        queue[count].preempted = false;
        queue[count].finished = false;

        count++;
    }

    fclose(fp);
    *num_tasks = count;
    return queue;
}

void show_tasks(Task* tasks, int num_tasks) {
    for(int i=0; i < num_tasks; i++) {
        printf("task: %s | deadline: %.1f | t0: %.1f | dt: %.1f\n", 
            tasks[i].name, tasks[i].deadline, tasks[i].t0, tasks[i].dt);
    }
}

// --------------- LÓGICA DE SAÍDA ---------------

// cumpriu nome tf tr
// cumpriu vale 1 se tf<= deadline
// nome é o identificador do processo
// tf é quando terminou a execução
// tr é o tempo de relógio, tf-t0

// --------------- ALGORITMOS DO ESCALONADOR ---------------

// shortest job first
// round robin
// escalonamento com prioridade

// retorna o índice da próxima task a ser executada no sjf
int get_next_sjf_task(Task* tasks, int num_tasks, double current_time) {
    int best_index = -1;
    double min_dt = 1e9;

    for (int i = 0; i < num_tasks; i++) {
        if (tasks[i].t0 <= current_time && !tasks[i].finished && !tasks[i].running) {
            if (tasks[i].dt < min_dt) {
                min_dt = tasks[i].dt;
                best_index = i;
            }
        }
    }
    return best_index;
}

void* sleeper_thread(void* arg) {
    while(true) {
        Task* t = NULL;

        pthread_mutex_lock(&task_lock);
        while(!die) {  // enquanto main(produtor) não para as threads
            for(int i = 0; i < WORKERS; i++) {
                int index = next_tasks[i];
                if(global_tasks[index].running && !global_tasks[index].started) {
                    t = &global_tasks[i];
                    t->started = true;
                    break;
                    // pega uma thread marcada para rodar pelo escalonador
                }
            }
            if (t || die) break;
            pthread_cond_wait(&available, &task_lock);
        }
        pthread_mutex_unlock(&task_lock);

        if(die) break; // 'mata' worker

        if(t) {
            while (t->time_left > 0) {
                usleep(WORK_UNIT * 1000); 
                t->time_left -= (double)WORK_UNIT / 1000.0;
            }

            pthread_mutex_lock(&task_lock);
            t->finished = true;
            t->running = false;
            //t->tf = time();
            pthread_mutex_unlock(&task_lock);
        }
    }
    return NULL;
}

// --------------- SEÇÃO PRINCIPAL ---------------

// argv[1], algoritmo do escalonador, de 0 a 2
    // 0 é SJF, 1 é RR, 2 é prioridade
// argv[2], arquivo trace de entrada
// argv[3], arquivo a ser criado para saída
int main(int argc, char **argv) {
    int num_tasks = 0, sched_method = atoi(argv[1]);
    Task* tasks = tasks_from_trace(argv[2], &num_tasks);

    if (tasks != NULL) {
        
        // inicializa workers

        // loop do scheduler
        time_t start = time(NULL);
        while(true) {
    
            // adiciona processo pelo timer

            switch(sched_method) {
                case 0:
                    sjf_sort(tasks, num_tasks);
                    // pthread_cond_broadcast()
                    break;
                case 1:
                    break;
                case 2:
                    break;
                
            }
        }
        free(tasks);
    }
    return 0;
}