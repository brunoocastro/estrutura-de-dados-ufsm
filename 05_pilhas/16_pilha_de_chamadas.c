/*
 * ============================================================================
 * Arquivo : 16_pilha_de_chamadas.c
 * Módulo  : 05 — Pilhas                        (Ementa: Unidade 2.1)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Mostrar que toda chamada de função usa uma PILHA: a pilha de chamadas
 *   (call stack). Cada chamada empilha um QUADRO (stack frame) com os
 *   parâmetros, as variáveis locais e o ponto de retorno; o return desempilha.
 *   Parte 1: fatorial(4) recursivo, desenhando os quadros a cada chamada e
 *            retorno, e mostrando o endereço REAL do parâmetro n em cada quadro.
 *   Parte 2: o mesmo cálculo SEM recursão, com uma pilha explícita fazendo o
 *            papel da pilha de chamadas (ida = push, volta = pop).
 *   Parte 3: quanto espaço cada quadro ocupa e por que recursão profunda
 *            causa "stack overflow".
 *
 * EXEMPLO REAL
 *   Depuradores (gdb, IDEs) mostram a pilha de chamadas ("call stack") ao
 *   parar num breakpoint. Em microcontroladores e CLPs a pilha tem poucos
 *   KB; por isso normas como MISRA C (regra 17.2) proíbem recursão em
 *   software embarcado crítico, e algoritmos recursivos são reescritos com
 *   pilha explícita de tamanho conhecido (como na Parte 2).
 *
 * COMPLEXIDADE (n = argumento do fatorial)
 *   Versão            Tempo   Espaço
 *   recursiva         O(n)    O(n) quadros na pilha de chamadas
 *   pilha explícita   O(n)    O(n) posições na pilha do programa
 *   laço simples      O(n)    O(1)   (possível porque o fatorial é simples)
 *   Por quê: fatorial(n) só termina depois de fatorial(n-1), que só termina
 *   depois de fatorial(n-2)... ficam n quadros "esperando" ao mesmo tempo.
 *
 * COMO EXECUTAR
 *   ./run 16_pilha_de_chamadas               (ENTER avança cada passo)
 *   ./run 16_pilha_de_chamadas < /dev/null   (executa direto)
 * ============================================================================
 */
#define _POSIX_C_SOURCE 200809L /* antes de qualquer #include: libera isatty() em -std=c11 */
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

static int modo_interativo = 0; /* 1 quando stdin é um terminal */

/* Pausa até o aluno teclar ENTER; não faz nada se a entrada não for um terminal. */
static void pausar(void) {
    if (!modo_interativo) return;
    printf("    [ENTER] próximo passo...");
    fflush(stdout);
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
}

/* ------------------------------------------------------------------------ */
/* Parte 1: espelho da pilha de chamadas real                                */
/* ------------------------------------------------------------------------ */
#define MAX_QUADROS 16

typedef struct {
    int n;               /* parâmetro (n < 0 representa o quadro de quem chamou) */
    uintptr_t end_n;     /* endereço real do parâmetro naquela chamada */
    const char *estado;  /* o que o quadro está fazendo */
} Quadro;

static Quadro espelho[MAX_QUADROS]; /* cópia didática do que a CPU empilha */
static int altura = 0;
static int altura_max = 0;

/* O(altura): desenha os quadros, topo em cima, com a distância em bytes do
 * endereço de n até o de parte1 (quem chamou). Na maioria das plataformas (x86, ARM) a pilha
 * de chamadas cresce para endereços MENORES, por isso as distâncias aumentam
 * à medida que a recursão aprofunda. */
static void desenha_quadros(const char *evento) {
    printf("\n  %s\n", evento);
    printf("    +------------------------------------------------------+\n");
    for (int i = altura - 1; i >= 0; i--) {
        const Quadro *q = &espelho[i];
        uintptr_t ref = espelho[0].end_n;
        long dist = q->end_n <= ref ? (long)(ref - q->end_n) : -(long)(q->end_n - ref);
        if (q->n < 0)
            printf("    | parte1()        %-27s %+6ld B |%s\n", q->estado, dist,
                   i == altura - 1 ? " <- topo" : "");
        else
            printf("    | fatorial(n=%d)   %-27s %+6ld B |%s\n", q->n, q->estado, dist,
                   i == altura - 1 ? " <- topo" : "");
    }
    printf("    +------------------------------------------------------+\n");
    printf("    altura = %d quadro(s)   (B = distância do endereço local até o de parte1)\n",
           altura);
}

/* O(n) tempo e O(n) quadros. Empilha seu quadro no espelho ao entrar e o
 * remove ao sair, exatamente como a CPU faz com a pilha real. */
static long fatorial(int n) {
    if (altura == MAX_QUADROS) return -1; /* proteção do espelho didático */
    espelho[altura] = (Quadro){n, (uintptr_t)&n, n <= 1 ? "caso base: devolve 1" : "executando"};
    altura++;
    if (altura > altura_max) altura_max = altura;
    char titulo[64];
    snprintf(titulo, sizeof titulo, "CHAMADA fatorial(%d): push de um quadro novo", n);
    desenha_quadros(titulo);
    pausar();

    long resultado;
    if (n <= 1) {
        resultado = 1;
    } else {
        espelho[altura - 1].estado = "esperando fatorial(n-1)";
        resultado = n * fatorial(n - 1); /* o quadro fica parado aqui até voltar */
    }

    snprintf(titulo, sizeof titulo, "RETORNO de fatorial(%d) = %ld: pop do quadro", n,
             resultado);
    altura--;
    if (altura > 0 && espelho[altura - 1].n >= 0)
        espelho[altura - 1].estado = "recebeu, vai multiplicar";
    desenha_quadros(titulo);
    pausar();
    return resultado;
}

static void parte1(void) {
    int marcador = -1; /* variável local deste quadro, usada como referência */
    printf("PARTE 1 — fatorial(4) recursivo e a pilha de chamadas\n");
    altura = 0;
    espelho[altura++] = (Quadro){marcador, (uintptr_t)&marcador,
                                 "esperando fatorial(4)"};
    long r = fatorial(4);
    printf("\n  fatorial(4) = %ld. Altura máxima: %d quadros (n + 1 contando parte1).\n", r,
           altura_max);
}

/* ------------------------------------------------------------------------ */
/* Parte 2: a mesma conta com pilha explícita                                */
/* ------------------------------------------------------------------------ */
static void desenha_explicita(const int pilha[], int topo, long acumulado) {
    for (int i = topo - 1; i >= 0; i--)
        printf("          | n = %2d |%s\n", pilha[i], i == topo - 1 ? " <- topo" : "");
    printf("          +--------+   altura = %d   acumulado = %ld\n", topo, acumulado);
}

static void parte2(int n) {
    int pilha[MAX_QUADROS];
    int topo = 0;
    long acumulado = 1;

    printf("\n\nPARTE 2 — fatorial(%d) com pilha EXPLÍCITA (sem recursão)\n", n);
    printf("  IDA: cada 'chamada' vira um push do argumento.\n");
    for (int k = n; k >= 1 && topo < MAX_QUADROS; k--) {
        pilha[topo++] = k;
        printf("\n    push(%d)\n", k);
        desenha_explicita(pilha, topo, acumulado);
        pausar();
    }
    printf("\n  VOLTA: cada 'retorno' vira um pop, multiplicando o acumulado.\n");
    while (topo > 0) {
        int k = pilha[--topo];
        acumulado *= k;
        printf("\n    pop() -> %d   acumulado = acumulado * %d\n", k, k);
        desenha_explicita(pilha, topo, acumulado);
        pausar();
    }
    printf("\n  fatorial(%d) = %ld, com a mesma ordem de push/pop da versão recursiva.\n"
           "  Vantagem: a pilha tem tamanho conhecido e o overflow vira um teste simples\n"
           "  (topo < MAX_QUADROS) em vez de travar o programa.\n",
           n, acumulado);
}

/* ------------------------------------------------------------------------ */
/* Parte 3: tamanho de um quadro e stack overflow                            */
/* ------------------------------------------------------------------------ */
static uintptr_t endereco_no_fundo = 0;

/* Desce `n` níveis e anota o endereço de uma variável local no último. */
static long soma(int n) {
    volatile int local = n; /* volatile: impede o compilador de eliminar a variável */
    if (n == 0) {
        endereco_no_fundo = (uintptr_t)&local;
        return 0;
    }
    return local + soma(n - 1);
}

static void parte3(void) {
    const int profundidade = 1000;
    volatile int referencia = 0;
    uintptr_t topo_ref = (uintptr_t)&referencia;
    long r = soma(profundidade);
    long bytes = (long)(topo_ref > endereco_no_fundo ? topo_ref - endereco_no_fundo
                                                      : endereco_no_fundo - topo_ref);
    long por_quadro = bytes / (profundidade + 1);

    printf("\n\nPARTE 3 — quanto custa cada quadro?\n");
    printf("  soma(%d) = %ld usou %d+1 quadros e ~%ld bytes de pilha de chamadas,\n"
           "  ou seja, ~%ld bytes por quadro nesta máquina/compilador.\n",
           profundidade, r, profundidade, bytes, por_quadro);
    if (por_quadro > 0) {
        printf("  Com esse tamanho de quadro:\n"
               "    pilha típica de PC (8 MB)          -> ~%ld níveis de recursão\n"
               "    microcontrolador com 2 KB de pilha -> ~%ld níveis\n",
               8L * 1024 * 1024 / por_quadro, 2048L / por_quadro);
    }
    printf("  Passar desse limite é o STACK OVERFLOW: o programa é abortado (ou, num\n"
           "  sistema embarcado sem proteção, sobrescreve outros dados silenciosamente).\n"
           "  (Os números variam com compilador, otimização e sanitizers.)\n");
}

int main(void) {
    modo_interativo = isatty(STDIN_FILENO);
    parte1();
    pausar();
    parte2(4);
    parte3();
    return 0;
}
