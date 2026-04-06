// esse arquivo não expõe uma API completa do imesh.c
// no entanto, ele expõe funcionalidades que poderiam ser expandidas e reaproveitadas por outros terminais
#ifndef IMESH_H
#define IMESH_H

#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <sys/utsname.h>
#include <linux/limits.h>
#include <sys/types.h>
#include <pwd.h>
#include <sys/wait.h>
#include <time.h>
#include <signal.h>
#include <errno.h>

// syscalls para comandos do terminal
char* get_current_dir_syscall(void);
void get_date_syscall(void);
void kill_syscall(char* argv[]);

// funções para funcionalidade do terminal
char* generate_terminal_prompt(void);
char** parse_args(char* string);
void fork_and_exec(char* file, char* argv[]);
int execute_line(char* line);

#endif