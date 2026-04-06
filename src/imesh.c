// implementação do terminal
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
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


// --------------- FUNÇÕES PARA FUNCIONALIDADE BÁSICA DO SHELL ---------------
char* get_username_syscall() {
    struct passwd *result;
    uid_t uid = getuid(); // syscall
    result = getpwuid(uid);
    return result->pw_name;
}

char* get_computer_name_syscall() {
    struct utsname result;
    uname(&result); // syscall
    return strdup(result.nodename);
}

char* get_current_dir_syscall() {
    char* buffer = malloc(PATH_MAX);
    return getcwd(buffer, PATH_MAX); // syscall
}

char* generate_terminal_prompt() {
    // gera prompt no formato "[<nome_usuário>@<nome_computador>:<diretório_atual>]$ "
    char *username, *computer_name, *current_dir, *prompt;

    username = get_username_syscall();
    current_dir = get_current_dir_syscall();
    computer_name = get_computer_name_syscall();
    
    // prompt tem 6 bytes de caracteres hardcoded + \0
    prompt = calloc(7 + strlen(username) + strlen(computer_name) + strlen(current_dir), sizeof(char));

    strcat(prompt, "[");
    strcat(prompt, username);
    strcat(prompt, "@");
    strcat(prompt, computer_name);
    strcat(prompt, ":");
    strcat(prompt, current_dir);
    strcat(prompt, "]");
    strcat(prompt, "$ ");

    free(current_dir);
    return prompt;
}

char** parse_args(char* string) {
    // divide uma string que contém até 9 espaços vazios
    // permite guardar 10 argumentos, nome do comando, e NULL (como pedem as funções exec*)
    char** arg_list = malloc(12 * sizeof(char*)); 
    int i = 0;

    char* token = strtok(string, " ");

    while (token != NULL && i < 10) {
        arg_list[i] = token;
        i++;
        token = strtok(NULL, " ");
    }

    arg_list[i] = NULL;
    return arg_list;
}

// --------------- FUNÇÃO PARA CHAMAR EXECUTÁVEIS ---------------

// função para fork de executável com argumentos
void fork_and_exec(char* file, char* argv[]) {
    pid_t pid = fork();
    if(pid == 0) {
        execve(file, argv, NULL);
    } else {
        pid = wait(NULL);
    }
}

// --------------- FUNÇÕES PARA CHAMAR BUILT-INS COM SYSCALLS ---------------
void kill_syscall(char* argv[]) {
    int exit_code;
    int signal = strtol(argv[1]+1, NULL, 10); //+1 tira o traço da flag, necessário para a syscall
    pid_t pid = (pid_t) strtol(argv[2], NULL, 10);

    exit_code = kill(pid, signal); // kill(2) é listado como syscall na sua manpage
    if(exit_code == 0) return;
    
    if (errno == EPERM) {
        printf("imesh: kill: (%d) - Operation not permitted\n", pid);
    } else if (errno == ESRCH) {
        printf("imesh: kill: (%d) - No such process\n", pid);
    }
}


void get_date_syscall() {
    time_t seconds;
    seconds = time(NULL); // time(2) é listado como syscall na sua manpage
    printf("%ld\n", seconds);
}

// --------------- MAPEAMENTO E EXECUÇÃO DE COMANDOS ---------------

int execute_line(char* line) {    
    // a busca por comando é linear, mas funciona perfeitamente para 6 comandos
    char *commands[] = {"/bin/ls", "/bin/top", "./ep1", "pwd", "date", "kill"};
    int commands_len = sizeof(commands)/sizeof(char*), command_match = -1;

    // MATCH DO COMANDO ----------------------
    char *copy_buffer = malloc(sizeof(line));
    strcpy(copy_buffer, line);
    char *command = strtok(copy_buffer, " ");

    for(int j = 0; j < commands_len; j++) {
        if(strcmp(command, commands[j]) == 0) {
            command_match = j;
        }
    }

    free(copy_buffer);
    if(command_match == -1) return -1;

    // EXECUÇÃO DO COMANDO ENCONTRADO ----------------------

    char** args_list = parse_args(line);   

    switch (command_match) {
        case 0:
            fork_and_exec("/bin/ls", args_list);
            break;
        case 1:
            fork_and_exec("/bin/top", args_list);
            break;
        case 2:
            fork_and_exec("./ep1", args_list);
            break;
        case 3:
            char* cwd = get_current_dir_syscall();     
            printf("%s\n", cwd);
            free(cwd);
            break;
        case 4:
            get_date_syscall();
            break;
        case 5:
            kill_syscall(args_list);
            break;
    }

    free(args_list);
    return 0;
}

// --------------- FUNÇÃO PRINCIPAL ---------------

int main() {
    while(true) {
        char *line, *s, *prompt;

        prompt = generate_terminal_prompt();
        line = readline(prompt);

        if (!line)
            break;

        s = line;

        if (*s) {
            add_history(s);
        }

        if(execute_line(line) == -1) {
            printf("Comando não encontrado\n");
        }
        

        free(prompt);
        free(line);
    }

    return 0;
}
