// Implementação do simulador de processos
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

// --------------- LÓGICA DE ENTRADA ---------------
// nome deadline t0 dt
// nome é string com 32 caracteres alocados
// deadline é instante de tempo em que gostaríamos que o processo acabe, >=t0+dt
// t0 é quando o processo chega no sistema
// dt é o tempo de processamento



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
    printf("%s\n", argv[1]);
    printf("%s\n", argv[2]);
    printf("%s\n", argv[3]);

    read_trace_file();
}