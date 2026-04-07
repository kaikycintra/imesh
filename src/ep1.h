// esse arquivo não expõe uma API completa do ep1.c
// no entanto, ele expõe funcionalidades que poderiam ser expandidas e reaproveitadas por outros escalonadores
#ifndef EP1_H
#define EP1_H
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>
#include <pthread.h>

#define MAX_TASK_NUM 50
#define WORK_UNIT 100 // intervalo de verificação de preempção em ms, equivalente a unidade de trabalho
#define WORKERS 1
#define RR_QUANTUM 500 // em ms

typedef struct {
    char name[32];
    double t0;
    double deadline;
    double dt;
    
    // dados a serem salvos durante execução
    double tf;
    double running_time;

    // estado da task
    double time_left;
    bool running;
    bool started;
    bool finished;
} Task;
Task* global_tasks = NULL;

Task* tasks_from_trace(char* filename, int* num_tasks);
void show_output(char* filename);

int get_next_sjf_task(Task* tasks, int num_tasks, double current_time);
void sjf_scheduling(int* busy_workers);
void* sjf_thread(void* arg);

int get_next_rr_task(Task* tasks, int num_tasks, double current_time);
void rr_scheduling(int* busy_workers);
void* rr_thread(void* arg);

#endif