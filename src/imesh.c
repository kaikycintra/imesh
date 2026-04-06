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

// --------------- FUNÇÕES PARA CHAMAR EXECUTÁVEIS ---------------

// função para fork de executável com argumentos
void fork_and_exec(char* file, char* argv[]) {
    pid_t pid = fork();
    if(pid == 0) {
        execve(file, argv, NULL);
    } else {
        pid = wait(NULL);
    }
}

// format_kill_output

void exec_ls() {
    char ls_path[] = "/bin/ls";
    char *ls_params[] = {"ls", "-1aF", "--color=never", NULL}; 
    fork_and_exec(ls_path, ls_params);
}

// executáveis
char exec_top[] = "/bin/top -b -n 1 -p 1";
char exec_ep1[] = "./ep1"; // precisa receber os argumentos

// --------------- FUNÇÕES PARA CHAMAR BUILT-INS COM SYSCALLS ---------------
// kill_bltin
// kill -<sinal> <pid>
// se pid não existe
// usuário não tem permissão para enviar sinal para o processo

// date_bltin
// date +%s

// --------------- MAPEAMENTO E EXECUÇÃO DE COMANDOS ---------------

int execute_line(char* line) {    
    // a busca por comando é ineficiente e desorganizada, mas funciona perfeitamente para 6 comandos
    int command_match = -1, i = 0;
    char *commands[] = {"/bin/ls", "/bin/top", "./ep1", "pwd", "date", "kill"};
    int max_len_commands = 8;
    int commands_len = sizeof(commands)/sizeof(char*);

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

    switch (command_match) {
        case 0:
            exec_ls();
            break;
        case 1:
            break;
        case 2:
            break;
        case 3:
            break;
        case 4:
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
