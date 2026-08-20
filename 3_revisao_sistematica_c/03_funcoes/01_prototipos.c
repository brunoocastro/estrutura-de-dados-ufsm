/*
 * PROTOTIPOS, DEFINICAO E RETORNO
 *
 * Um prototipo informa ao compilador o nome, retorno e parametros de uma funcao
 * antes de sua chamada. A definicao contem o corpo executado.
 *
 * gcc -std=c11 -Wall -Wextra -pedantic 01_prototipos.c -o 01_prototipos
 * ./01_prototipos
 */
#include <stdio.h>

/* Declaracoes antecipadas: as funcoes sao definidas depois de main. */
int soma(int a, int b);
void mostra_mensagem(const char *mensagem);

int main(void) {
    int resultado = soma(7, 5);
    mostra_mensagem("Funcoes podem retornar valores ou apenas executar uma acao.");
    printf("soma(7, 5) = %d\n", resultado);
    return 0;
}

int soma(int a, int b) {
    return a + b;
}

void mostra_mensagem(const char *mensagem) {
    printf("mensagem: %s\n", mensagem);
}
