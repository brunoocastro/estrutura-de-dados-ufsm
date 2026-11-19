/*
 * ============================================================================
 * Arquivo : 04_deque_circular.c
 * Módulo  : 06 — Filas                        (Ementa: Unidade 3.2)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Fila dupla (deque, "double-ended queue") em vetor circular: insere e
 *   remove nas DUAS pontas, todas as 4 operações em O(1). O vetor é desenhado
 *   a cada passo, com `ini` andando para trás (com volta) no inserir_frente.
 *   Mostra também:
 *     - a armadilha do módulo de número negativo em C: (-1) % 6 == -1;
 *     - que o deque generaliza pilha e fila (usando só um lado de cada vez);
 *     - um uso real: histórico "desfazer" de setpoints de uma IHM com limite
 *       de N entradas (o mais antigo sai pela frente quando enche).
 *   No fim, um menu permite testar as 4 operações livremente.
 *
 * EXEMPLO REAL
 *   Histórico de desfazer/refazer de IHM e editores; janela deslizante de
 *   leituras de sensor (ver 05_janela_deslizante_maximo); escalonadores com
 *   "work stealing" (cada núcleo tira tarefas da sua ponta, outro núcleo
 *   "rouba" da ponta oposta); buffer de rolagem do terminal.
 *
 * COMPLEXIDADE (n = elementos, cap = capacidade)
 *   Operação          Tempo   Por quê
 *   inserir_frente    O(1)    ini = (ini - 1 + cap) % cap; escreve
 *   inserir_fim       O(1)    escreve em fim; fim = (fim + 1) % cap
 *   remover_frente    O(1)    lê em ini;  ini = (ini + 1) % cap
 *   remover_fim       O(1)    fim = (fim - 1 + cap) % cap; lê
 *   acessar i-ésimo   O(1)    dados[(ini + i) % cap]
 *   Espaço: O(cap). Num vetor comum (sem circularidade), inserir na frente
 *   custaria O(n), pois todos teriam de andar uma casa para a direita.
 *
 * COMO EXECUTAR
 *   ./run 04_deque_circular               (ENTER avança; depois abre o menu)
 *   ./run 04_deque_circular < /dev/null   (só a demonstração, sem pausas)
 * ============================================================================
 */
#define _POSIX_C_SOURCE 200809L /* antes de qualquer #include: libera isatty() em -std=c11 */
#include <stdio.h>
#include <unistd.h>

#define CAP 6

static int modo_interativo = 0; /* 1 quando stdin é um terminal */

/* Pausa até o aluno teclar ENTER; não faz nada se a entrada não for um terminal. */
static void pausar(void) {
    if (!modo_interativo) return;
    printf("    [ENTER] próximo passo...");
    fflush(stdout);
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
}

typedef struct {
    int dados[CAP];
    int ini; /* índice do primeiro elemento (frente) */
    int fim; /* índice da próxima casa livre depois do último (trás) */
    int tam; /* desfaz a ambiguidade ini == fim (vazio ou cheio) */
} Deque;

static void dq_iniciar(Deque *d) { d->ini = d->fim = d->tam = 0; }
static int dq_vazio(const Deque *d) { return d->tam == 0; }
static int dq_cheio(const Deque *d) { return d->tam == CAP; }

/*
 * O(1). Recua `ini` uma casa e escreve ali.
 * ARMADILHA: em C, o resto de divisão de número negativo é negativo:
 * (0 - 1) % CAP == -1, um índice inválido. Somar CAP antes do % resolve:
 * (0 - 1 + CAP) % CAP == CAP - 1, a última casa (volta pela esquerda).
 */
static int inserir_frente(Deque *d, int x) {
    if (dq_cheio(d)) return 0;
    d->ini = (d->ini - 1 + CAP) % CAP;
    d->dados[d->ini] = x;
    d->tam++;
    return 1;
}

/* O(1). Igual ao enfileirar da fila circular. */
static int inserir_fim(Deque *d, int x) {
    if (dq_cheio(d)) return 0;
    d->dados[d->fim] = x;
    d->fim = (d->fim + 1) % CAP;
    d->tam++;
    return 1;
}

/* O(1). Igual ao desenfileirar da fila circular. */
static int remover_frente(Deque *d, int *x) {
    if (dq_vazio(d)) return 0;
    *x = d->dados[d->ini];
    d->ini = (d->ini + 1) % CAP;
    d->tam--;
    return 1;
}

/* O(1). Recua `fim` (com a mesma correção do negativo) e lê o último. */
static int remover_fim(Deque *d, int *x) {
    if (dq_vazio(d)) return 0;
    d->fim = (d->fim - 1 + CAP) % CAP;
    *x = d->dados[d->fim];
    d->tam--;
    return 1;
}

/* O(cap): vetor físico com marcadores e, abaixo, a ordem lógica. */
static void desenhar(const Deque *d) {
    printf("      índice: ");
    for (int i = 0; i < CAP; i++) printf("  %2d  ", i);
    printf("\n      vetor : ");
    for (int i = 0; i < CAP; i++) {
        if ((i - d->ini + CAP) % CAP < d->tam) printf("[%4d]", d->dados[i]);
        else printf("[    ]");
    }
    printf("\n              ");
    for (int i = 0; i < CAP; i++) {
        if (i == d->ini && i == d->fim) printf(" i=f  ");
        else if (i == d->ini) printf(" ini  ");
        else if (i == d->fim) printf(" fim  ");
        else printf("      ");
    }
    printf("\n      lógico: frente <");
    for (int k = 0; k < d->tam; k++) printf(" %d", d->dados[(d->ini + k) % CAP]);
    printf(" > trás   (tam = %d)\n", d->tam);
}

/* Operações do roteiro: código + valor. */
typedef enum { IF, IT, RF, RT } Op; /* inserir frente/trás, remover frente/trás */

static void aplicar(Deque *d, Op op, int v, int passo) {
    int x, antes_ini = d->ini, antes_fim = d->fim;
    printf("\n  passo %2d: ", passo);
    switch (op) {
    case IF:
        if (inserir_frente(d, v))
            printf("inserir_frente(%d): ini = (%d - 1 + %d) %% %d = %d; dados[%d] = %d%s\n", v,
                   antes_ini, CAP, CAP, d->ini, d->ini, v,
                   antes_ini == 0 ? "   <-- volta pela ESQUERDA" : "");
        else printf("inserir_frente(%d): RECUSADO (cheio)\n", v);
        break;
    case IT:
        if (inserir_fim(d, v))
            printf("inserir_fim(%d): dados[%d] = %d; fim = (%d + 1) %% %d = %d%s\n", v, antes_fim,
                   v, antes_fim, CAP, d->fim, d->fim == 0 ? "   <-- volta pela DIREITA" : "");
        else printf("inserir_fim(%d): RECUSADO (cheio)\n", v);
        break;
    case RF:
        if (remover_frente(d, &x))
            printf("remover_frente() -> %d; ini = (%d + 1) %% %d = %d\n", x, antes_ini, CAP, d->ini);
        else printf("remover_frente(): deque vazio\n");
        break;
    case RT:
        if (remover_fim(d, &x))
            printf("remover_fim() -> %d; fim = (%d - 1 + %d) %% %d = %d\n", x, antes_fim, CAP, CAP,
                   d->fim);
        else printf("remover_fim(): deque vazio\n");
        break;
    }
    desenhar(d);
    pausar();
}

/* ------------------------------------------------------------------------- */
/* Uso real: histórico de setpoints com limite (desfazer)                     */
/* ------------------------------------------------------------------------- */

/*
 * Cada alteração de setpoint vai para o FIM do deque. Desfazer = remover_fim
 * (pilha). Se o histórico encher, o registro mais ANTIGO é descartado pela
 * FRENTE (fila). Nem uma pilha nem uma fila sozinhas fazem as duas coisas em
 * O(1): por isso o deque.
 */
static void historico_setpoints(void) {
    printf("\n=== USO REAL: histórico 'desfazer' de setpoints de temperatura (limite %d) ===\n", CAP);
    Deque h;
    dq_iniciar(&h);
    const int alteracoes[] = {60, 65, 70, 72, 75, 80, 85, 90};
    for (int i = 0; i < 8; i++) {
        int descartado;
        if (dq_cheio(&h)) {
            remover_frente(&h, &descartado);
            printf("\n  histórico cheio: descarta o mais antigo (%d °C) pela FRENTE\n", descartado);
        }
        inserir_fim(&h, alteracoes[i]);
        printf("  operador ajusta setpoint para %d °C -> inserir_fim(%d)\n", alteracoes[i],
               alteracoes[i]);
    }
    desenhar(&h);
    pausar();
    for (int i = 0; i < 3; i++) {
        int sp;
        remover_fim(&h, &sp);
        printf("\n  [Desfazer] remove %d °C pelo FIM; ", sp);
        if (!dq_vazio(&h)) printf("setpoint volta a %d °C\n", h.dados[(h.fim - 1 + CAP) % CAP]);
        else printf("histórico vazio\n");
        desenhar(&h);
        pausar();
    }
}

/* ------------------------------------------------------------------------- */
/* Menu (termina em EOF ou na opção 0)                                        */
/* ------------------------------------------------------------------------- */

static void menu(Deque *d) {
    int passo = 100;
    for (;;) {
        printf("\nMENU  1) inserir_frente  2) inserir_fim  3) remover_frente  4) remover_fim  0) sair\n"
               "opção: ");
        fflush(stdout);
        int op, v = 0;
        if (scanf("%d", &op) != 1) { printf("\n(fim da entrada)\n"); return; }
        if (op == 0) return;
        if (op < 1 || op > 4) { printf("    opção inválida\n"); continue; }
        if (op <= 2) {
            printf("valor: ");
            fflush(stdout);
            if (scanf("%d", &v) != 1) { printf("\n(fim da entrada)\n"); return; }
        }
        const Op ops[] = {IF, IT, RF, RT};
        aplicar(d, ops[op - 1], v, ++passo);
    }
}

int main(void) {
    modo_interativo = isatty(STDIN_FILENO);
    printf("FILA DUPLA (DEQUE) EM VETOR CIRCULAR, capacidade %d\n", CAP);
    printf("Armadilha do módulo negativo em C: (-1) %% %d = %d  (por isso usamos (i - 1 + %d) %% %d = %d)\n",
           CAP, (-1) % CAP, CAP, CAP, (0 - 1 + CAP) % CAP);

    Deque d;
    dq_iniciar(&d);
    desenhar(&d);
    pausar();

    /* Roteiro que passa pelas duas voltas (esquerda e direita) e enche o deque. */
    const struct { Op op; int v; } roteiro[] = {
        {IT, 10}, {IT, 20}, {IF, 5},  {IF, 1},  {IT, 30}, {IF, 0},  {IT, 40}, {RT, 0}, {RF, 0},
        {RF, 0},  {IT, 50}, {IT, 60}, {RT, 0},  {RF, 0},  {IT, 70}, {IT, 80}, {IT, 90}, {RF, 0},
    };
    int n = (int)(sizeof roteiro / sizeof roteiro[0]);
    for (int p = 0; p < n; p++) aplicar(&d, roteiro[p].op, roteiro[p].v, p + 1);

    printf("\n  Observação: usando só inserir_fim + remover_fim o deque é uma PILHA;\n"
           "  usando só inserir_fim + remover_frente é uma FILA.\n");
    pausar();

    historico_setpoints();

    printf("\nAgora experimente (deque atual do roteiro):\n");
    desenhar(&d);
    menu(&d);
    return 0;
}
