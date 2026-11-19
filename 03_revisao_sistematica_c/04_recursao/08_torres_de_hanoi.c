/*
 * ============================================================================
 * Arquivo : 08_torres_de_hanoi.c
 * Módulo  : 03 — Revisão sistemática de C     (Ementa: 0.6 Recursividade)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Resolver as Torres de Hanói e DESENHAR os três pinos a cada movimento.
 *   A recursão é a forma natural de pensar o problema:
 *     para mover n discos de A para C (usando B):
 *       1. mova n-1 discos de A para B (usando C)   <- subproblema menor
 *       2. mova o disco maior de A para C            <- 1 movimento
 *       3. mova n-1 discos de B para C (usando A)   <- subproblema menor
 *   O rastro indentado mostra qual chamada está ativa; cada pino é uma PILHA
 *   (só o disco do topo sai), ligação direta com ../../05_pilhas/.
 *
 * EXEMPLO REAL
 *   O mesmo esquema "resolva n-1, faça 1, resolva n-1" aparece em rotinas de
 *   backup em rodízio (esquema "Torre de Hanói" de fitas/discos) e é um
 *   clássico para entender por que algoritmos exponenciais são inviáveis.
 *
 * COMPLEXIDADE (n = número de discos)
 *   Movimentos: M(n) = 2·M(n-1) + 1, M(1) = 1  =>  M(n) = 2^n − 1  -> O(2^n)
 *   Pilha de chamadas: O(n) (profundidade máxima n).
 *   Não existe solução com menos movimentos: o problema em si é exponencial.
 *
 * COMO EXECUTAR
 *   ./run 08_torres_de_hanoi               (pede n de 1 a 6; ENTER avança)
 *   ./run 08_torres_de_hanoi < /dev/null   (n = 3, sem pausas)
 * ============================================================================
 */
#define _POSIX_C_SOURCE 200809L   /* antes de qualquer #include: libera isatty() em -std=c11 */
#include <stdio.h>
#include <unistd.h>

#define MAX_DISCOS 6
#define N_PADRAO 3

static int modo_interativo = 0;   /* 1 quando stdin é um terminal */

/* Pausa até o aluno teclar ENTER; não faz nada se a entrada não for um terminal. */
static void pausar(void) {
    if (!modo_interativo) return;
    printf("    [ENTER] próximo passo...");
    fflush(stdout);
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
}

/* Cada pino é uma pilha de discos: disco[p][0] é o de baixo, altura[p] o topo.
 * MAX_DISCOS limita só o DESENHO; a contagem sem desenho vai até MAX_CONTAGEM. */
#define MAX_CONTAGEM 20
static int disco[3][MAX_CONTAGEM];
static int altura[3];
static int n_discos;
static unsigned long movimentos = 0;
static int desenhar = 1;

static void empilha(int p, int d) { disco[p][altura[p]++] = d; }
static int desempilha(int p) { return disco[p][--altura[p]]; }

/* O(n): desenha um disco de tamanho d (ou só a haste, se d == 0) centrado. */
static void desenha_disco(int d) {
    int largura = 2 * n_discos + 1;
    int cheio = d ? 2 * d - 1 : 1;
    int margem = (largura - cheio) / 2;
    printf("%*s", margem + 2, "");
    for (int i = 0; i < cheio; i++) putchar(d ? '=' : '|');
    printf("%*s", margem + 2, "");
}

/* O(n): desenha os três pinos lado a lado, do nível mais alto para o chão. */
static void desenha_pinos(void) {
    for (int nivel = n_discos - 1; nivel >= 0; nivel--) {
        printf("    ");
        for (int p = 0; p < 3; p++)
            desenha_disco(nivel < altura[p] ? disco[p][nivel] : 0);
        printf("\n");
    }
    int largura = 2 * n_discos + 5;
    printf("    ");
    for (int p = 0; p < 3; p++) {
        for (int i = 0; i < largura; i++) putchar('-');
    }
    printf("\n    ");
    for (int p = 0; p < 3; p++)
        printf("%*c%*s", largura / 2 + 1, 'A' + p, largura - largura / 2 - 1, "");
    printf("\n");
}

/* O(1): move o disco do topo de "de" para o topo de "para". */
static void move(int de, int para, int nivel) {
    int d = desempilha(de);
    empilha(para, d);
    movimentos++;
    if (desenhar) {
        printf("%*smovimento %lu: disco %d de %c para %c\n",
               nivel * 2, "", movimentos, d, 'A' + de, 'A' + para);
        desenha_pinos();
        pausar();
    }
}

/* O(2^n) movimentos, O(n) pilha. */
static void hanoi(int n, int de, int para, int via, int nivel) {
    if (n == 0) return; /* caso base: nada a mover */
    if (desenhar)
        printf("%*shanoi(%d, %c -> %c, via %c)\n", nivel * 2, "", n, 'A' + de, 'A' + para, 'A' + via);
    hanoi(n - 1, de, via, para, nivel + 1);   /* 1. tira os n-1 de cima do caminho */
    move(de, para, nivel);                    /* 2. move o maior */
    hanoi(n - 1, via, para, de, nivel + 1);   /* 3. recoloca os n-1 sobre ele */
}

static void prepara(int n) {
    n_discos = n;
    altura[0] = altura[1] = altura[2] = 0;
    for (int d = n; d >= 1; d--) empilha(0, d); /* maior embaixo */
    movimentos = 0;
}

int main(void) {
    modo_interativo = isatty(STDIN_FILENO);
    int n = N_PADRAO;
    printf("Quantos discos (1 a %d)? ", MAX_DISCOS);
    if (scanf("%d", &n) != 1 || n < 1 || n > MAX_DISCOS) {
        n = N_PADRAO;
        printf("(entrada vazia ou fora da faixa: usando n = %d)\n", n);
    } else {
        int c;
        while ((c = getchar()) != '\n' && c != EOF) { }
    }

    prepara(n);
    printf("\n=== Torres de Hanoi com %d discos: estado inicial ===\n", n);
    desenha_pinos();
    pausar();
    hanoi(n, 0, 2, 1, 0);
    printf("\nConcluido em %lu movimentos; 2^%d - 1 = %lu.\n", movimentos, n, (1UL << n) - 1);

    printf("\n=== Crescimento exponencial (movimentos contados executando, sem desenho) ===\n");
    desenhar = 0;
    printf("%6s | %10s | %10s | %s\n", "discos", "contados", "2^n - 1", "a 1 movimento por segundo");
    printf("-------+------------+------------+---------------------------\n");
    int ns[] = {5, 10, 15, 20};
    for (size_t k = 0; k < sizeof ns / sizeof ns[0]; k++) {
        int nn = ns[k];
        prepara(nn);
        hanoi(nn, 0, 2, 1, 0);  /* executa de verdade, só contando */
        int certo = altura[2] == nn; /* todos os discos chegaram em C? */
        double s = (double)movimentos; /* 1 movimento por segundo */
        char tempo[32];
        if (s < 60) snprintf(tempo, sizeof tempo, "%.0f s", s);
        else if (s < 3600) snprintf(tempo, sizeof tempo, "%.1f min", s / 60);
        else if (s < 86400) snprintf(tempo, sizeof tempo, "%.1f h", s / 3600);
        else snprintf(tempo, sizeof tempo, "%.1f dias", s / 86400);
        printf("%6d | %10lu | %10lu | %s%s\n", nn, movimentos, (1UL << nn) - 1,
               tempo, certo ? "" : "  (ERRO)");
    }
    printf("%6d | %10s | %10s | %s\n", 64, "-", "1,8e19", "~585 bilhoes de anos (a lenda dos monges)");
    printf("\nCada disco a mais DOBRA o trabalho: O(2^n).\n");
    return 0;
}
