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
// format_kill_output

// kill_bltin
// kill -<sinal> <pid>
// se pid não existe
// usuário não tem permissão para enviar sinal para o processo

void get_date_syscall() {
    time_t seconds;
    seconds = time(NULL); // time(2) é listada como syscall na sua manpage
    printf("%ld\n", seconds);
}

// --------------- MAPEAMENTO E EXECUÇÃO DE COMANDOS ---------------

int execute_line(char* line) {    
    // a busca por comando é ineficiente e desorganizada, mas funciona perfeitamente para 6 comandos
    int command_match = -1, i = 0;
    char *commands[] = {"/bin/ls", "/bin/top", "./ep1", "pwd", "date", "kill"};
    int commands_len = sizeof(commands)/sizeof(char*);
    int *command_lengths = (int*) malloc(commands_len * sizeof(int));

    for(int j = 0; j < commands_len; j++) {
        command_lengths[j] = strlen(commands[j]);
    }
    
    int max_len_commands = 8;

    char* buf = calloc(max_len_commands, 1);
    i = 3; // busca começa com tamanho da menor string de comando e vai até tamanho da maior
    while(i <= max_len_commands && command_match == -1) {
        strncpy(buf, line, i);
        for(int j = 0; j < commands_len; j++) {
            if(strcmp(buf, commands[j]) == 0) {
                command_match = j;
            }
        }
        i++;
    }
    
    if(command_match == -1) return -1;

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
            printf("%s\n", get_current_dir_syscall());
            break;
        case 4:
            get_date_syscall();
            break;
        case 5:
            break;
    }

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
