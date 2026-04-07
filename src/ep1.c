// Implementação do simulador de processos
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>

#define MAX_TASK_NUM 50

// --------------- LÓGICA DE ENTRADA ---------------
typedef struct {
    char name[32];
    double t0;
    double deadline;
    double dt;

    // dados a serem salvos durante execução
    double time_left;
    double tf;
    bool active;
    bool is_finished;
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
        
        queue[count].time_left = queue[count].dt;
        queue[count].tf = 0;
        queue[count].active = false;
        queue[count].is_finished = false;

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

// --------------- SEÇÃO PRINCIPAL ---------------

// arg1, algoritmo do escalonador, de 0 a 2
// arg2, arquivo trace de entrada
// arg3, arquivo a ser criado para saída
int main(int argc, char **argv) {
    int num_tasks = 0;
    Task* tasks = tasks_from_trace(argv[2], &num_tasks);

    if (tasks != NULL) {
        show_tasks(tasks, num_tasks);
        free(tasks);
    }
    return 0;
}