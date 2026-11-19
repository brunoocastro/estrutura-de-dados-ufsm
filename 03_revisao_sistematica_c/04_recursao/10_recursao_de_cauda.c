/*
 * ============================================================================
 * Arquivo : 10_recursao_de_cauda.c
 * Módulo  : 03 — Revisão sistemática de C     (Ementa: 0.6 Recursividade)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Comparar três versões de soma(n) = 1 + 2 + ... + n:
 *     1. recursão comum:   return n + soma(n - 1);
 *        cada quadro fica ESPERANDO para fazer "n + ..." na volta;
 *     2. recursão de cauda: return soma_cauda(n - 1, acumulado + n);
 *        a chamada recursiva é a ÚLTIMA ação; nada fica pendente, o resultado
 *        parcial viaja no parâmetro "acumulado";
 *     3. laço (while): a tradução mecânica da versão de cauda, com O(1) de pilha.
 *   A pilha é desenhada com o trabalho pendente de cada quadro, e os endereços
 *   de uma variável local mostram a pilha crescendo de verdade.
 *
 * EXEMPLO REAL
 *   Firmware de CLPs e microcontroladores costuma PROIBIR recursão (normas
 *   como MISRA C) porque a pilha é pequena e o pior caso precisa ser
 *   conhecido. Saber converter recursão de cauda em laço resolve isso.
 *
 * COMPLEXIDADE (n = valor de entrada)
 *   Versão              Tempo   Pilha
 *   recursão comum      O(n)    O(n)  (n + 1 quadros vivos no fundo)
 *   recursão de cauda   O(n)    O(n) em C sem otimização; O(1) se o compilador
 *                               fizer "eliminação de chamada de cauda" (-O2),
 *                               o que o padrão C NÃO garante
 *   laço                O(n)    O(1)
 *
 * COMO EXECUTAR
 *   ./run 10_recursao_de_cauda               (ENTER avança cada passo)
 *   ./run 10_recursao_de_cauda < /dev/null   (executa direto, sem pausas)
 * ============================================================================
 */
#define _POSIX_C_SOURCE 200809L   /* antes de qualquer #include: libera isatty() em -std=c11 */
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#define MAX_DESENHO 16

static int modo_interativo = 0;   /* 1 quando stdin é um terminal */
static int mostrar = 1;
static uintptr_t endereco_base = 0;  /* endereço da local no primeiro quadro */
static unsigned maior_profundidade = 0;

/* Quadros "didáticos" para desenhar a pilha. */
static char pendente[MAX_DESENHO][80];
static int topo = -1;

/* Pausa até o aluno teclar ENTER; não faz nada se a entrada não for um terminal. */
static void pausar(void) {
    if (!modo_interativo) return;
    printf("    [ENTER] próximo passo...");
    fflush(stdout);
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
}

static void desenha_pilha(void) {
    for (int i = topo; i >= 0; i--)
        printf("        | %-44s |\n", pendente[i]);
    printf("        | %-44s |\n", "main");
    printf("        +----------------------------------------------+\n");
}

/* Distância (em bytes) entre a local deste quadro e a do primeiro quadro.
 * Converter para uintptr_t evita subtrair ponteiros de objetos diferentes. */
static long distancia(const void *local) {
    uintptr_t a = (uintptr_t)local;
    return a > endereco_base ? (long)(a - endereco_base) : (long)(endereco_base - a);
}

/* 1) Recursão comum: O(n) tempo e pilha. O "n + ..." acontece DEPOIS que a
 * chamada interna volta, por isso o quadro precisa continuar vivo. */
static unsigned long soma(unsigned n, unsigned prof) {
    volatile int marcador = 0; /* local só para medir onde o quadro está */
    if (prof == 0) endereco_base = (uintptr_t)&marcador;
    if (prof > maior_profundidade) maior_profundidade = prof;
    if (mostrar) {
        topo++;
        if (n > 0)
            snprintf(pendente[topo], sizeof pendente[0], "soma(%u): pendente  %u + soma(%u)",
                     n, n, n - 1);
        else
            snprintf(pendente[topo], sizeof pendente[0], "soma(0): caso base, retorna 0");
        printf("    ENTRA soma(%u) | quadro a %ld bytes do primeiro\n", n, distancia((const void *)&marcador));
        desenha_pilha();
        pausar();
    }
    unsigned long r = (n == 0) ? 0 : n + soma(n - 1, prof + 1);
    if (mostrar) {
        topo--;
        printf("    SAI soma(%u) = %lu (fez a soma pendente na volta)\n", n, r);
    }
    (void)marcador;
    return r;
}

/* 2) Recursão de cauda: o resultado parcial vai no parâmetro. Na volta não há
 * nada a fazer além de repassar o valor. Ainda assim, SEM otimização, cada
 * chamada cria um quadro novo (é o que os endereços mostram). */
static unsigned long soma_cauda(unsigned n, unsigned long acumulado, unsigned prof) {
    volatile int marcador = 0;
    if (prof == 0) endereco_base = (uintptr_t)&marcador;
    if (prof > maior_profundidade) maior_profundidade = prof;
    if (mostrar) {
        snprintf(pendente[++topo], sizeof pendente[0],
                 "soma_cauda(%u, acum=%lu): nada pendente", n, acumulado);
        printf("    ENTRA soma_cauda(%u, %lu) | quadro a %ld bytes do primeiro\n",
               n, acumulado, distancia((const void *)&marcador));
        desenha_pilha();
        pausar();
    }
    (void)marcador;
    if (n == 0) {
        if (mostrar) {
            printf("    CASO BASE: a resposta (%lu) ja esta pronta no acumulador\n", acumulado);
            topo = -1; /* na volta, cada quadro só repassa o valor */
        }
        return acumulado;
    }
    return soma_cauda(n - 1, acumulado + n, prof + 1); /* última ação: cauda */
}

/* 3) Laço: a tradução direta da versão de cauda.
 *    parâmetros viram variáveis; a chamada de cauda vira "atualiza e repete". */
static unsigned long soma_laco(unsigned n) {
    unsigned long acumulado = 0;
    while (n > 0) {          /* soma_cauda(n, acumulado):              */
        acumulado += n;      /*   acumulado = acumulado + n            */
        n--;                 /*   n = n - 1   e "chama de novo" (repete) */
    }
    return acumulado;        /* caso base: devolve o acumulador        */
}

/* Executa as três versões sem desenhar e mostra quantos quadros cada uma
 * empilhou ao mesmo tempo (profundidade máxima + 1). */
static void mede(unsigned n) {
    mostrar = 0;
    maior_profundidade = 0;
    unsigned long r1 = soma(n, 0);
    unsigned prof1 = maior_profundidade;
    maior_profundidade = 0;
    unsigned long r2 = soma_cauda(n, 0, 0);
    unsigned long r3 = soma_laco(n);
    printf("%8u | %14lu | %11u | %11u | %5d | %s\n", n, r1, prof1 + 1,
           maior_profundidade + 1, 1, (r1 == r2 && r2 == r3) ? "iguais" : "DIFERENTES");
}

int main(void) {
    modo_interativo = isatty(STDIN_FILENO);
    const unsigned n = 4;

    printf("=== 1) Recursao comum: soma(%u) ===\n", n);
    unsigned long r = soma(n, 0);
    printf("  resultado %lu: cada quadro esperou a volta para somar.\n", r);
    pausar();

    printf("\n=== 2) Recursao de cauda: soma_cauda(%u, 0) ===\n", n);
    r = soma_cauda(n, 0, 0);
    printf("  resultado %lu: os quadros so repassam o valor na volta.\n", r);
    printf("  Como nada fica pendente, o compilador PODE reaproveitar o mesmo quadro\n");
    printf("  (compile com -O2 e compare os enderecos), mas o padrao C nao garante.\n");
    pausar();

    printf("\n=== 3) Laco: soma_laco(%u) = %lu, usando um unico quadro ===\n", n, soma_laco(n));
    pausar();

    printf("\n=== 4) Quadros simultaneos na pilha (medidos executando) ===\n");
    printf("%8s | %14s | %11s | %11s | %5s | %s\n",
           "n", "soma", "comum", "cauda", "laco", "resultados");
    printf("---------+----------------+-------------+-------------+-------+-----------\n");
    for (unsigned k = 10; k <= 10000; k *= 10)
        mede(k);
    printf("\nA recursao (comum ou de cauda, sem otimizacao) empilha n + 1 quadros;\n");
    printf("o laco usa sempre 1. Com n grande, so o laco e seguro (11_estouro_de_pilha.c).\n");
    return 0;
}
