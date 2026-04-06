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

// command_type (acha tipo de comando da linha)

// execute_line(line, command_type)

// --------------- FUNÇÕES PARA CHAMAR EXECUTÁVEIS ---------------

// função para fork de executável com argumentos

// format_kill_output

// executáveis
char exec_ls[] = "/bin/ls -1aF --color=never";
char exec_top[] = "/bin/top -b -n 1 -p 1";
char exec_ep1[] = "./ep1"; // precisa receber os argumentos

// --------------- FUNÇÕES PARA CHAMAR BUILT-INS COM SYSCALLS ---------------
// kill_bltin
// kill -<sinal> <pid>
// se pid não existe
// usuário não tem permissão para enviar sinal para o processo

// date_bltin
// date +%s

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
            printf("<%s>\n", s);
        }

        free(prompt);
        free(line);
    }

    return 0;
}
