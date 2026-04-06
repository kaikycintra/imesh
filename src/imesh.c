// implementação do terminal
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

// executáveis
char exec_ls[] = "/bin/ls -1aF --color=never";
char exec_top[] = "/bin/top -b -n 1 -p 1";
char exec_ep1[] = "./ep1"; // precisa receber os argumentos

// embutidos (implementados com chamada de sistema específica)
char pwd[] = "pwd";
char date[] = "date +%s";
char kill[] = "kill"; // precisa receber -<sinal> e <pid>

// --------------- FUNÇÕES PARA FUNCIONALIDADE BÁSICA DO SHELL ---------------
// shell precisa ter uma localização

// rolagem de linhas com setas do teclado

// imprime_linha
// chamadas de sistema para o nome
// "[<nome_usuário>@<nome_computador>:<diretório_atual>]$ "<comando>

// --------------- FUNÇÕES PARA CHAMAR EXECUTÁVEIS ---------------

// função para fork de executável com argumentos

// format_kill_output

// --------------- FUNÇÕES PARA CHAMAR BUILT-INS ---------------
// pwd_bltin

// kill_bltin
// se pid não existe
// usuário não tem permissão para enviar sinal para o processo

// date_bltin

// --------------- FUNÇÃO PRINCIPAL ---------------
int main() {
    while(true) {
        sleep(1);
        
    }

    return 0;
}
