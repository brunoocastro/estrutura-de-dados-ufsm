/*
 * ============================================================================
 * Arquivo : 01_fatorial_passos.c
 * Módulo  : 03 — Revisão sistemática de C     (Ementa: 0.6 Recursividade)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Ver a PILHA DE CHAMADAS funcionando: cada chamada de fatorial(n) cria um
 *   quadro (frame) com sua própria cópia de n. Os quadros se empilham na
 *   descida (até o caso base) e se desempilham na subida, quando cada um
 *   finalmente faz sua multiplicação. A cada evento a pilha é desenhada.
 *
 * EXEMPLO REAL
 *   Toda chamada de função em C (recursiva ou não) usa essa pilha: num
 *   microcontrolador com 2 KB de RAM, saber quantos quadros cabem é questão de
 *   o firmware funcionar ou travar (veja 11_estouro_de_pilha.c).
 *
 * COMPLEXIDADE (n = valor de entrada)
 *   Tempo: O(n)  -> n + 1 chamadas, cada uma faz O(1) trabalho próprio.
 *   Pilha: O(n)  -> no fundo da recursão há n + 1 quadros vivos ao mesmo tempo.
 *   A versão com laço (for) faz o mesmo em O(n) tempo e O(1) de memória.
 *
 * COMO EXECUTAR
 *   ./run 01_fatorial_passos               (pede n; ENTER avança cada passo)
 *   ./run 01_fatorial_passos < /dev/null   (usa n = 4, sem pausas)
 * ============================================================================
 */
#define _POSIX_C_SOURCE 200809L   /* antes de qualquer #include: libera isatty() em -std=c11 */
#include <stdio.h>
#include <unistd.h>

#define MAX_QUADROS 16
#define N_PADRAO 4

static int modo_interativo = 0;   /* 1 quando stdin é um terminal */

/* Pausa até o aluno teclar ENTER; não faz nada se a entrada não for um terminal. */
static void pausar(void) {
    if (!modo_interativo) return;
    printf("    [ENTER] próximo passo...");
    fflush(stdout);
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
}

/* Cópia "didática" da pilha real: guardamos o n e o estado de cada quadro
 * só para poder desenhá-los. A pilha verdadeira é gerenciada pelo compilador. */
static unsigned int pilha_n[MAX_QUADROS];
static int topo = -1; /* -1 = pilha vazia (só main) */

/* O(profundidade): desenha a pilha com o topo em cima. Cada linha da caixa
 * é montada com snprintf para que todas tenham a mesma largura. */
static void desenha_pilha(void) {
    char linha[64];
    printf("        pilha (topo em cima):\n");
    printf("        +------------------------------------------+\n");
    for (int i = topo; i >= 0; i--) {
        unsigned int n = pilha_n[i];
        if (n == 0)
            snprintf(linha, sizeof linha, "fatorial(n=0): caso base, retorna 1");
        else
            snprintf(linha, sizeof linha, "fatorial(n=%u): aguarda %u * fatorial(%u)", n, n, n - 1);
        printf("        | %-40s |\n", linha);
    }
    printf("        | %-40s |\n", "main");
    printf("        +------------------------------------------+\n");
}

/* O(n) tempo e pilha. "nivel" controla a indentação do rastro. */
static unsigned long fatorial(unsigned int n, int nivel) {
    pilha_n[++topo] = n; /* empilha (o compilador faz isso de verdade) */
    printf("%*sENTRA fatorial(%u)  -> empilha quadro (%d quadro(s) de fatorial)\n",
           nivel * 4, "", n, topo + 1);
    desenha_pilha();
    pausar();

    unsigned long resultado;
    if (n == 0) {
        resultado = 1; /* caso base: para de descer */
        printf("%*sCASO BASE: fatorial(0) = 1\n", nivel * 4, "");
    } else {
        unsigned long parcial = fatorial(n - 1, nivel + 1); /* desce */
        resultado = n * parcial;                            /* só agora multiplica */
        printf("%*sVOLTA a fatorial(%u): %u * %lu = %lu\n",
               nivel * 4, "", n, n, parcial, resultado);
    }

    topo--; /* desempilha: o quadro deixa de existir, com seu n */
    printf("%*sSAI fatorial(%u) = %lu -> desempilha\n", nivel * 4, "", n, resultado);
    desenha_pilha();
    pausar();
    return resultado;
}

int main(void) {
    modo_interativo = isatty(STDIN_FILENO);
    unsigned int n = N_PADRAO;

    /* unsigned long tem pelo menos 32 bits: 12! = 479001600 cabe, 13! não. */
    printf("Informe um inteiro entre 0 e 12: ");
    if (scanf("%u", &n) != 1 || n > 12) {
        n = N_PADRAO;
        printf("(entrada vazia ou fora da faixa: usando n = %u)\n", n);
    } else {
        int c;
        while ((c = getchar()) != '\n' && c != EOF) { } /* descarta o resto da linha */
    }

    printf("\n=== Rastreamento de fatorial(%u) ===\n", n);
    unsigned long r = fatorial(n, 0);
    printf("\nResultado: %u! = %lu\n", n, r);
    printf("Chamadas: %u | maior profundidade: %u quadros de fatorial ao mesmo tempo.\n",
           n + 1, n + 1);
    printf("Tempo O(n) e memoria de pilha O(n). Um laco for faria o mesmo com O(1) de memoria.\n");
    return 0;
}
