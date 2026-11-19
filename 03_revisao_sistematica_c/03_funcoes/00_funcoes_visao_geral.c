/*
 * ============================================================================
 * Arquivo : 00_funcoes_visao_geral.c
 * Módulo  : 03 — Revisão sistemática de C     (Ementa: 0.6 Funções)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Panorama de funções: protótipos, variável global x local, passagem por
 *   valor (cópia) e "por referência" simulada com ponteiros. O clássico
 *   troca_por_valor (não funciona) contra troca_por_ponteiro (funciona).
 *
 * EXEMPLO REAL
 *   Uma rotina de calibração que precisa devolver ganho E offset: como a
 *   função só retorna um valor, os outros resultados saem por ponteiros
 *   (calibrar(leituras, n, &ganho, &offset)).
 *
 * COMPLEXIDADE
 *   Cada função deste arquivo é O(1). Uma chamada de função também custa O(1):
 *   empilha parâmetros e endereço de retorno num novo quadro (frame) da pilha.
 *
 * COMO EXECUTAR
 *   ./run 00_funcoes_visao_geral
 *   ./run 00_funcoes_visao_geral < /dev/null   (usa x = 10 e y = 20)
 * ============================================================================
 */
#include <stdio.h>

/* Protótipos: permitem chamar as funções antes de suas definições. */
void soma_por_valor(int a, int b);
void soma_por_ponteiro(const int *a, const int *b, int *resultado);
void troca_por_valor(int a, int b);
void troca_por_ponteiro(int *a, int *b);

/* Variável global: existe durante toda a execução do programa. Útil aqui
 * para contar chamadas, mas em código real prefira passar dados por
 * parâmetro (globais dificultam testar e entender quem altera o quê). */
int chamadas = 0;

int main(void) {
    int x = 10;
    int y = 20;
    int resultado;

    printf("=== Funcoes e passagem de parametros ===\n");
    printf("Informe dois inteiros: ");
    if (scanf("%d %d", &x, &y) != 2) {
        /* Entrada vazia ou inválida: segue com valores de exemplo. */
        x = 10;
        y = 20;
        printf("(usando o exemplo x = %d, y = %d)\n", x, y);
    }

    printf("\nAntes das chamadas: x = %d, y = %d\n", x, y);
    soma_por_valor(x, y);
    soma_por_ponteiro(&x, &y, &resultado);
    printf("Soma escrita em resultado por ponteiros: %d\n", resultado);

    troca_por_valor(x, y);
    printf("Depois de troca_por_valor: x = %d, y = %d  (nada mudou)\n", x, y);
    troca_por_ponteiro(&x, &y);
    printf("Depois de troca_por_ponteiro: x = %d, y = %d  (trocados)\n", x, y);
    printf("Funcoes chamadas: %d\n", chamadas);
    return 0;
}

/* O(1). Os parâmetros a e b são cópias: alterar as cópias não altera o chamador. */
void soma_por_valor(int a, int b) {
    int soma_local = a + b; /* Variável local: só existe nesta função. */
    chamadas++;
    printf("soma_por_valor: %d + %d = %d\n", a, b, soma_local);
}

/* O(1). C não possui passagem por referência nativa; ponteiros simulam esse
 * efeito. const nos dois primeiros = só leitura; resultado = saída. */
void soma_por_ponteiro(const int *a, const int *b, int *resultado) {
    chamadas++;
    *resultado = *a + *b;
    printf("soma_por_ponteiro: acessou os valores nos enderecos %p e %p\n",
           (const void *)a, (const void *)b);
}

/* O(1). Troca apenas as cópias locais: o chamador não percebe nada. */
void troca_por_valor(int a, int b) {
    int temporario = a;
    a = b;
    b = temporario;
    chamadas++;
    printf("troca_por_valor: trocou apenas as copias locais (%d, %d)\n", a, b);
}

/* O(1). Recebe os ENDEREÇOS e troca o conteúdo lá: o chamador vê a troca. */
void troca_por_ponteiro(int *a, int *b) {
    int temporario = *a;
    *a = *b;
    *b = temporario;
    chamadas++;
    printf("troca_por_ponteiro: alterou os valores nos enderecos %p e %p\n",
           (void *)a, (void *)b);
}
