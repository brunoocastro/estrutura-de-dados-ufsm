/*
 * ============================================================================
 * Arquivo : 01_fila_linear_vs_circular.c
 * Módulo  : 06 — Filas                        (Ementa: Unidade 3.1)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Mostrar, com o vetor desenhado a cada passo, três formas de guardar uma
 *   fila FIFO (First In, First Out) em um vetor de tamanho fixo:
 *     A) fila linear com DESLOCAMENTO: a frente é sempre o índice 0, então
 *        cada remoção empurra todos os elementos uma casa para a esquerda;
 *     B) fila linear SEM deslocamento: os índices `ini` e `fim` só andam para
 *        a direita, e surge o problema do "falso cheio" (há casas livres, mas
 *        `fim` bateu no final do vetor);
 *     C) fila CIRCULAR: `ini` e `fim` dão a volta com o operador módulo
 *        (wrap-around), e todas as operações ficam O(1) sem desperdício.
 *   No fim, uma tabela de crescimento mede as movimentações de A contra C.
 *
 * EXEMPLO REAL
 *   Fila de peças numa esteira, fila de comandos enviados a um inversor,
 *   buffer de recepção serial de um microcontrolador, fila de mensagens entre
 *   tarefas de um RTOS. Em firmware não se usa malloc a cada peça: o padrão é
 *   justamente o vetor circular de tamanho fixo (versão C deste arquivo).
 *
 * COMPLEXIDADE (n = elementos na fila)
 *   Operação        A) deslocamento   B) linear simples   C) circular   Espaço
 *   enfileirar      O(1)              O(1)                O(1)          O(cap)
 *   desenfileirar   O(n)              O(1)                O(1)
 *   frente/vazia    O(1)              O(1)                O(1)
 *   Por quê: em A, tirar o elemento do índice 0 exige copiar os n-1 restantes;
 *   esvaziar uma fila com n elementos custa (n-1)+(n-2)+...+1 = n(n-1)/2
 *   cópias, O(n²) no total. Em B e C só um índice anda. B, porém, nunca
 *   reaproveita as casas liberadas; C reaproveita graças ao módulo.
 *
 * COMO EXECUTAR
 *   ./run 01_fila_linear_vs_circular               (ENTER avança cada passo)
 *   ./run 01_fila_linear_vs_circular < /dev/null   (executa direto, sem pausas)
 * ============================================================================
 */
#define _POSIX_C_SOURCE 200809L /* antes de qualquer #include: libera isatty() em -std=c11 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define CAP_DEMO 6 /* pequeno de propósito: o wrap-around aparece logo */

static int modo_interativo = 0; /* 1 quando stdin é um terminal */

/* Pausa até o aluno teclar ENTER; não faz nada se a entrada não for um terminal. */
static void pausar(void) {
    if (!modo_interativo) return;
    printf("    [ENTER] próximo passo...");
    fflush(stdout);
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
}

/* ------------------------------------------------------------------------- */
/* Desenho comum às três versões                                             */
/* ------------------------------------------------------------------------- */

/*
 * Desenha o vetor com os índices, as casas ocupadas e os marcadores:
 *   ini = índice do primeiro elemento (o próximo a sair);
 *   fim = índice da PRÓXIMA casa livre (onde o próximo vai entrar).
 * Convenção usada em todo o arquivo. Alguns livros fazem `fim` apontar para o
 * último elemento; as duas funcionam, desde que o código seja coerente.
 * `ocupado[i]` diz se a casa i guarda um elemento válido. `fim` pode valer
 * `cap` na fila linear (fora do vetor); o marcador aparece depois da última casa.
 * O(cap).
 */
static void desenhar(const int *v, const int *ocupado, int cap, int ini, int fim) {
    printf("      índice: ");
    for (int i = 0; i < cap; i++) printf("  %2d  ", i);
    printf("\n      vetor : ");
    for (int i = 0; i < cap; i++) {
        if (ocupado[i]) printf("[%4d]", v[i]);
        else printf("[    ]");
    }
    printf("\n              ");
    for (int i = 0; i <= cap; i++) {
        if (i == ini && i == fim) printf(" i=f  ");
        else if (i == ini) printf(" ini  ");
        else if (i == fim) printf(" fim  ");
        else printf("      ");
    }
    printf("\n");
}

/* ------------------------------------------------------------------------- */
/* A) Fila linear com deslocamento                                           */
/* ------------------------------------------------------------------------- */

typedef struct {
    int *dados;
    int cap;
    int tam;          /* elementos em dados[0..tam-1]; a frente é SEMPRE dados[0] */
    long movimentos;  /* cópias feitas ao deslocar (o custo escondido) */
} FilaDeslocamento;

/* O(1): só reserva o vetor. Devolve 0 se faltar memória. */
static int fd_criar(FilaDeslocamento *f, int cap) {
    f->dados = malloc((size_t)cap * sizeof *f->dados);
    f->cap = cap;
    f->tam = 0;
    f->movimentos = 0;
    return f->dados != NULL;
}

/* O(1): escreve na primeira casa livre, que é dados[tam]. */
static int fd_enfileirar(FilaDeslocamento *f, int x) {
    if (f->tam == f->cap) return 0;
    f->dados[f->tam++] = x;
    return 1;
}

/* O(n): retira dados[0] e copia os tam-1 restantes uma casa para a esquerda. */
static int fd_desenfileirar(FilaDeslocamento *f, int *saida) {
    if (f->tam == 0) return 0;
    *saida = f->dados[0];
    for (int i = 1; i < f->tam; i++) {
        f->dados[i - 1] = f->dados[i];
        f->movimentos++;
    }
    f->tam--;
    return 1;
}

static void fd_desenhar(const FilaDeslocamento *f) {
    int ocupado[CAP_DEMO];
    for (int i = 0; i < f->cap; i++) ocupado[i] = i < f->tam;
    desenhar(f->dados, ocupado, f->cap, 0, f->tam);
    printf("      movimentações acumuladas: %ld\n", f->movimentos);
}

/* ------------------------------------------------------------------------- */
/* B) Fila linear sem deslocamento (o "falso cheio")                         */
/* ------------------------------------------------------------------------- */

typedef struct {
    int *dados;
    int cap;
    int ini; /* primeiro elemento */
    int fim; /* próxima casa livre; elementos válidos em dados[ini..fim-1] */
} FilaLinear;

static int fl_criar(FilaLinear *f, int cap) {
    f->dados = malloc((size_t)cap * sizeof *f->dados);
    f->cap = cap;
    f->ini = 0;
    f->fim = 0;
    return f->dados != NULL;
}

/*
 * O(1). ARMADILHA: o teste de cheio olha só `fim`. Se `ini` já andou, as casas
 * 0..ini-1 estão livres, mas nunca mais serão usadas: "falso cheio".
 */
static int fl_enfileirar(FilaLinear *f, int x) {
    if (f->fim == f->cap) return 0;
    f->dados[f->fim++] = x;
    return 1;
}

/* O(1): só avança `ini`; a casa deixada para trás fica perdida. */
static int fl_desenfileirar(FilaLinear *f, int *saida) {
    if (f->ini == f->fim) return 0;
    *saida = f->dados[f->ini++];
    return 1;
}

static void fl_desenhar(const FilaLinear *f) {
    int ocupado[CAP_DEMO];
    for (int i = 0; i < f->cap; i++) ocupado[i] = i >= f->ini && i < f->fim;
    desenhar(f->dados, ocupado, f->cap, f->ini, f->fim);
    printf("      elementos: %d   casas livres desperdiçadas antes de ini: %d\n",
           f->fim - f->ini, f->ini);
}

/* ------------------------------------------------------------------------- */
/* C) Fila circular                                                          */
/* ------------------------------------------------------------------------- */

typedef struct {
    int *dados;
    int cap;
    int ini; /* primeiro elemento */
    int fim; /* próxima casa livre */
    int tam; /* quantos elementos há na fila */
} FilaCircular;

/*
 * Por que guardar `tam`? Na fila circular, ini == fim acontece tanto com a fila
 * VAZIA quanto com a fila CHEIA. O contador desfaz a ambiguidade. A alternativa
 * clássica é deixar sempre uma casa vazia (cheia quando (fim+1)%cap == ini).
 */
static int fc_criar(FilaCircular *f, int cap) {
    f->dados = malloc((size_t)cap * sizeof *f->dados);
    f->cap = cap;
    f->ini = 0;
    f->fim = 0;
    f->tam = 0;
    return f->dados != NULL;
}

/* O(1): escreve em `fim` e avança com módulo; depois de cap-1 vem 0. */
static int fc_enfileirar(FilaCircular *f, int x) {
    if (f->tam == f->cap) return 0;
    f->dados[f->fim] = x;
    f->fim = (f->fim + 1) % f->cap;
    f->tam++;
    return 1;
}

/* O(1): lê em `ini` e avança com módulo. Nenhum elemento é copiado. */
static int fc_desenfileirar(FilaCircular *f, int *saida) {
    if (f->tam == 0) return 0;
    *saida = f->dados[f->ini];
    f->ini = (f->ini + 1) % f->cap;
    f->tam--;
    return 1;
}

static void fc_desenhar(const FilaCircular *f) {
    int ocupado[CAP_DEMO];
    /* A casa i está ocupada se estiver a menos de `tam` passos de `ini`,
       contando com a volta: (i - ini + cap) % cap < tam. */
    for (int i = 0; i < f->cap; i++) ocupado[i] = (i - f->ini + f->cap) % f->cap < f->tam;
    desenhar(f->dados, ocupado, f->cap, f->ini, f->fim);
    printf("      ordem lógica (frente -> fim):");
    for (int k = 0; k < f->tam; k++) printf(" %d", f->dados[(f->ini + k) % f->cap]);
    printf("   (tam = %d)\n", f->tam);
}

/* ------------------------------------------------------------------------- */
/* Roteiro comum: o mesmo pedido de operações para as três filas             */
/* ------------------------------------------------------------------------- */

/* Valor > 0 = enfileirar esse valor; 0 = desenfileirar. */
static const int roteiro[] = {10, 20, 30, 40, 0, 0, 50, 60, 70, 80, 0, 90};
#define N_ROTEIRO ((int)(sizeof roteiro / sizeof roteiro[0]))

static void demo_deslocamento(void) {
    printf("\n=== A) FILA LINEAR COM DESLOCAMENTO (frente fixa no índice 0) ===\n");
    FilaDeslocamento f;
    if (!fd_criar(&f, CAP_DEMO)) { printf("sem memória\n"); return; }
    fd_desenhar(&f);
    for (int p = 0; p < N_ROTEIRO; p++) {
        int x;
        if (roteiro[p] > 0) {
            if (fd_enfileirar(&f, roteiro[p]))
                printf("\n  passo %2d: enfileirar(%d) -> dados[%d] = %d   (0 movimentações)\n",
                       p + 1, roteiro[p], f.tam - 1, roteiro[p]);
            else
                printf("\n  passo %2d: enfileirar(%d) -> RECUSADO: fila realmente cheia\n",
                       p + 1, roteiro[p]);
        } else {
            long antes = f.movimentos;
            if (fd_desenfileirar(&f, &x))
                printf("\n  passo %2d: desenfileirar() -> saiu %d; deslocou %ld elemento(s) "
                       "para a esquerda\n", p + 1, x, f.movimentos - antes);
        }
        fd_desenhar(&f);
        pausar();
    }
    printf("\n  Resumo A: nenhuma casa desperdiçada, mas %ld cópias só para manter a frente "
           "no índice 0.\n", f.movimentos);
    free(f.dados);
}

static void demo_linear(void) {
    printf("\n=== B) FILA LINEAR SEM DESLOCAMENTO (ini e fim só andam para a direita) ===\n");
    FilaLinear f;
    if (!fl_criar(&f, CAP_DEMO)) { printf("sem memória\n"); return; }
    fl_desenhar(&f);
    int recusados = 0;
    for (int p = 0; p < N_ROTEIRO; p++) {
        int x;
        if (roteiro[p] > 0) {
            if (fl_enfileirar(&f, roteiro[p])) {
                printf("\n  passo %2d: enfileirar(%d) -> dados[%d] = %d; fim = %d\n",
                       p + 1, roteiro[p], f.fim - 1, roteiro[p], f.fim);
            } else {
                recusados++;
                printf("\n  passo %2d: enfileirar(%d) -> RECUSADO: fim == %d (fim do vetor)... "
                       "mas há %d casa(s) livre(s)!  ** FALSO CHEIO **\n",
                       p + 1, roteiro[p], f.cap, f.cap - (f.fim - f.ini));
            }
        } else if (fl_desenfileirar(&f, &x)) {
            printf("\n  passo %2d: desenfileirar() -> saiu %d; ini = %d (casa %d fica "
                   "inutilizada)\n", p + 1, x, f.ini, f.ini - 1);
        }
        fl_desenhar(&f);
        pausar();
    }
    printf("\n  Resumo B: O(1) em tudo, porém %d chegada(s) recusada(s) com espaço sobrando.\n",
           recusados);
    free(f.dados);
}

static void demo_circular(void) {
    printf("\n=== C) FILA CIRCULAR (ini e fim dão a volta com %% cap) ===\n");
    FilaCircular f;
    if (!fc_criar(&f, CAP_DEMO)) { printf("sem memória\n"); return; }
    fc_desenhar(&f);
    for (int p = 0; p < N_ROTEIRO; p++) {
        int x;
        if (roteiro[p] > 0) {
            int pos = f.fim;
            if (fc_enfileirar(&f, roteiro[p])) {
                printf("\n  passo %2d: enfileirar(%d) -> dados[%d] = %d; fim = (%d + 1) %% %d = %d%s\n",
                       p + 1, roteiro[p], pos, roteiro[p], pos, f.cap, f.fim,
                       f.fim == 0 ? "   <-- WRAP-AROUND: volta ao início" : "");
            } else {
                printf("\n  passo %2d: enfileirar(%d) -> RECUSADO: tam == cap (cheia de verdade)\n",
                       p + 1, roteiro[p]);
            }
        } else {
            int pos = f.ini;
            if (fc_desenfileirar(&f, &x))
                printf("\n  passo %2d: desenfileirar() -> saiu %d; ini = (%d + 1) %% %d = %d%s\n",
                       p + 1, x, pos, f.cap, f.ini,
                       f.ini == 0 ? "   <-- WRAP-AROUND" : "");
        }
        fc_desenhar(&f);
        pausar();
    }
    printf("\n  Resumo C: O(1) em tudo, 0 cópias, e as casas liberadas foram reaproveitadas.\n");
    printf("  Note que no passo 10 a fila ficou com ini == fim e tam == cap: sem o contador\n"
           "  `tam` seria impossível distinguir 'cheia' de 'vazia'.\n");
    free(f.dados);
}

/* ------------------------------------------------------------------------- */
/* Tabela de crescimento: enche com n elementos e esvazia                    */
/* ------------------------------------------------------------------------- */

static void tabela_crescimento(void) {
    printf("\n=== TABELA DE CRESCIMENTO: enfileirar n elementos e depois desenfileirar todos ===\n");
    printf("  (contagem exata de cópias extras; o esperado para A é n(n-1)/2)\n\n");
    printf("  %8s | %18s | %18s | %12s\n", "n", "A) deslocamento", "C) circular", "n(n-1)/2");
    printf("  ---------+--------------------+--------------------+-------------\n");
    const int tamanhos[] = {10, 100, 1000, 10000};
    for (int t = 0; t < 4; t++) {
        int n = tamanhos[t], x;
        FilaDeslocamento a;
        FilaCircular c;
        if (!fd_criar(&a, n)) return;
        if (!fc_criar(&c, n)) { free(a.dados); return; }
        long copias_c = 0; /* a fila circular nunca copia: fica 0 de propósito */
        for (int i = 0; i < n; i++) { fd_enfileirar(&a, i); fc_enfileirar(&c, i); }
        for (int i = 0; i < n; i++) { fd_desenfileirar(&a, &x); fc_desenfileirar(&c, &x); }
        printf("  %8d | %18ld | %18ld | %12ld\n", n, a.movimentos, copias_c,
               (long)n * (n - 1) / 2);
        free(a.dados);
        free(c.dados);
    }
    printf("\n  n multiplicado por 10 => cópias de A multiplicadas por ~100 (O(n²) no total,\n"
           "  O(n) por remoção). A circular continua com custo constante por operação.\n");
}

int main(void) {
    modo_interativo = isatty(STDIN_FILENO);
    printf("FILA EM VETOR: linear x circular (capacidade da demonstração = %d)\n", CAP_DEMO);
    printf("Roteiro aplicado às três versões: enf 10, 20, 30, 40; des; des; enf 50, 60, 70, 80;"
           " des; enf 90\n");
    pausar();
    demo_deslocamento();
    demo_linear();
    demo_circular();
    tabela_crescimento();
    return 0;
}
