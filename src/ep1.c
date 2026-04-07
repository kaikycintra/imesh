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
static int die = 0;

int total_tasks = 0;
double current_sim_time;

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
Task* global_tasks = NULL;

// --------------- LÓGICA DE ENTRADA ---------------

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
void show_output(char* filename) {
    FILE *out_fp = fopen(filename, "w");
    
    for (int i = 0; i < total_tasks; i++) {
        int cumpriu = (global_tasks[i].tf <= global_tasks[i].deadline) ? 1 : 0;
        double tr = global_tasks[i].tf - global_tasks[i].t0;
    
        fprintf(out_fp, "%d %s %.1f %.1f\n", 
                cumpriu, 
                global_tasks[i].name, 
                global_tasks[i].tf, 
                tr);
    }
    
    fclose(out_fp);
}

// --------------- ALGORITMOS DO ESCALONADOR ---------------

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

// shortest job first
void sjf_scheduling(int* busy_workers) {
    int next_task = get_next_sjf_task(global_tasks, total_tasks, current_sim_time);
    if (next_task != -1) {
        global_tasks[next_task].running = true; // marca a task para execução
        busy_workers++;
        pthread_cond_signal(&available); // Acorda um worker
    } // índice -1 = não há task na fila
}

void* sleeper_thread(void* arg) {
    while(true) {
        Task* t = NULL;

        pthread_mutex_lock(&task_lock);
        while(!die) {  // enquanto main(produtor) não para as threads
            for(int i = 0; i < total_tasks; i++) {
                if(global_tasks[i].running && !global_tasks[i].started) {
                    t = &global_tasks[i];
                    t->started = true;
                    break;
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
            t->tf = current_sim_time;
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
    global_tasks = tasks_from_trace(argv[2], &num_tasks);
    total_tasks = num_tasks;

    // inicializa workers com sleeper_threads
    pthread_t workers[WORKERS];
    for (int i = 0; i < WORKERS; i++) {
        pthread_create(&workers[i], NULL, sleeper_thread, NULL);
    }

    // loop de execução produtor consumidor
    int tasks_finished = 0;
    while (tasks_finished < num_tasks) {
        pthread_mutex_lock(&task_lock);

        int busy_workers = 0;
        tasks_finished = 0;
        for (int i = 0; i < num_tasks; i++) {
            if (global_tasks[i].running) busy_workers++;
            if (global_tasks[i].finished) tasks_finished++;
        }

        while (busy_workers < WORKERS) {
            if(sched_method == 0) {
                sjf_scheduling(&busy_workers);
            } else if(sched_method == 1) {
                printf("oi");
            } else if(sched_method == 2) {
                printf("oi");
            }
        }
        
        pthread_mutex_unlock(&task_lock);

        // avança tempo global
        usleep(WORK_UNIT * 1000);
        current_sim_time += (double)WORK_UNIT / 1000.0;
    }

    die = 1;
    pthread_cond_broadcast(&available); // faz broadcast com die para encerrar workers
    for (int i = 0; i < WORKERS; i++) pthread_join(workers[i], NULL);

    show_output(argv[3]);
    
    free(global_tasks);
    return 0;
}