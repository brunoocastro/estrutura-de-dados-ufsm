/*
 * ============================================================================
 * Arquivo : 06_round_robin_clp.c
 * Módulo  : 06 — Filas                        (Ementa: Unidade 3.1)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Simular, em tempo discreto (1 tick = 1 ms), um escalonador ROUND-ROBIN de
 *   tarefas de um CLP/controlador usando uma FILA CIRCULAR de prontos:
 *     - quem chega entra no fim da fila;
 *     - a CPU pega a tarefa da frente e a executa por no máximo q ticks
 *       (o quantum);
 *     - se a tarefa não terminou, volta para o FIM da fila (preempção).
 *   A cada tick o programa mostra a CPU, a fila e os eventos; no fim desenha
 *   o diagrama de Gantt e calcula espera, resposta e turnaround de cada
 *   tarefa. Depois compara quanta diferentes com o FCFS (fila sem preempção).
 *
 * EXEMPLO REAL
 *   Tarefas de um controlador: varredura de E/S, malha PID, gravação de log no
 *   cartão SD (longa), resposta a uma requisição Modbus (curta e com prazo:
 *   o mestre desiste após um timeout) e atualização da IHM. Sem preempção, a
 *   tarefa longa de log atrasa a resposta Modbus. RTOS como FreeRTOS usam
 *   round-robin entre tarefas de mesma prioridade; o Linux usa SCHED_RR.
 *
 * COMPLEXIDADE (n = tarefas, T = duração total da simulação em ticks)
 *   Operação                      Tempo   Por quê
 *   enfileirar / desenfileirar    O(1)    fila circular (ver 01_fila_linear_vs_circular)
 *   decisão do escalonador        O(1)    só olha a frente da fila
 *   simulação inteira             O(T·n)  a cada tick verifica as chegadas
 *   Espaço: O(n) para a fila (cada tarefa está nela no máximo uma vez).
 *   Com uma lista de chegadas ordenada, as chegadas custariam O(1) por tick.
 *
 * COMO EXECUTAR
 *   ./run 06_round_robin_clp               (ENTER avança a cada troca de tarefa)
 *   ./run 06_round_robin_clp < /dev/null   (executa direto, sem pausas)
 * ============================================================================
 */
#define _POSIX_C_SOURCE 200809L /* antes de qualquer #include: libera isatty() em -std=c11 */
#include <limits.h>
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

typedef struct {
    const char *nome;
    const char *descricao;
    int chegada; /* tick em que a tarefa fica pronta */
    int duracao; /* ticks de CPU de que precisa */
} Tarefa;

static const Tarefa TAREFAS[] = {
    {"E/S", "varredura de entradas/saídas", 0, 2},
    {"PID", "cálculo da malha PID", 0, 3},
    {"LOG", "gravação de log no cartão SD", 1, 8},
    {"MOD", "resposta a requisição Modbus", 3, 2},
    {"IHM", "atualização da tela da IHM", 5, 3},
};
#define NT ((int)(sizeof TAREFAS / sizeof TAREFAS[0]))
#define MAX_TICKS 64

/* ------------------------------------------------------------------------- */
/* Fila circular de índices de tarefas                                        */
/* ------------------------------------------------------------------------- */

typedef struct {
    int v[NT]; /* basta NT: uma tarefa nunca está duas vezes na fila */
    int ini, fim, tam;
} FilaProntos;

static void fila_iniciar(FilaProntos *f) { f->ini = f->fim = f->tam = 0; }

/* O(1). Devolve 0 se cheia (não acontece aqui, mas o teste evita estouro). */
static int enfileirar(FilaProntos *f, int tarefa) {
    if (f->tam == NT) return 0;
    f->v[f->fim] = tarefa;
    f->fim = (f->fim + 1) % NT;
    f->tam++;
    return 1;
}

/* O(1). Pré-condição: fila não vazia. */
static int desenfileirar(FilaProntos *f) {
    int t = f->v[f->ini];
    f->ini = (f->ini + 1) % NT;
    f->tam--;
    return t;
}

/* O(n): mostra a fila da frente para o fim. */
static void mostrar_fila(const FilaProntos *f) {
    printf("fila: frente->");
    for (int k = 0; k < f->tam; k++) printf("[%s]", TAREFAS[f->v[(f->ini + k) % NT]].nome);
    if (f->tam == 0) printf("(vazia)");
    printf("<-fim");
}

/* ------------------------------------------------------------------------- */
/* Simulação                                                                  */
/* ------------------------------------------------------------------------- */

typedef struct {
    double espera_media, resposta_media, turnaround_medio;
    int resposta_mod; /* resposta da tarefa Modbus (a mais sensível a atraso) */
    int trocas;       /* trocas de contexto */
} Estatisticas;

/*
 * Simula o escalonador com quantum `q` (INT_MAX = FCFS, sem preempção).
 * detalhe 0: só calcula; 1: Gantt + tabela por tarefa; 2: também tick a tick.
 * Convenção no mesmo tick: primeiro entram as CHEGADAS, depois a tarefa que
 * esgotou o quantum vai para o fim (assim quem acabou de chegar não espera
 * uma volta a mais).
 */
static Estatisticas simular(int q, int detalhe) {
    int restante[NT], inicio[NT], termino[NT];
    const char *gantt[MAX_TICKS];
    for (int i = 0; i < NT; i++) {
        restante[i] = TAREFAS[i].duracao;
        inicio[i] = -1;
        termino[i] = -1;
    }
    FilaProntos f;
    fila_iniciar(&f);
    int atual = -1, fatia = 0, ultima = -1, trocas = 0, concluidas = 0, t;
    char qtxt[16]; /* quantum para exibição: "inf" no FCFS */
    if (q == INT_MAX) snprintf(qtxt, sizeof qtxt, "inf");
    else snprintf(qtxt, sizeof qtxt, "%d", q);

    for (t = 0; concluidas < NT && t < MAX_TICKS; t++) {
        int despachou = 0;
        /* 1) chegadas: O(n) por tick */
        for (int i = 0; i < NT; i++) {
            if (TAREFAS[i].chegada != t) continue;
            enfileirar(&f, i);
            if (detalhe == 2) printf("  t=%2d  chega %s -> enfileirar no fim\n", t, TAREFAS[i].nome);
        }
        /* 2) quantum esgotado: preempção, volta para o fim da fila */
        if (atual != -1 && fatia == q) {
            if (f.tam == 0) {
                fatia = 0; /* ninguém esperando: continua sem trocar */
            } else {
                enfileirar(&f, atual);
                if (detalhe == 2)
                    printf("  t=%2d  %s esgotou o quantum (%d) com %d tick(s) restante(s) -> volta ao FIM\n",
                           t, TAREFAS[atual].nome, q, restante[atual]);
                atual = -1;
            }
        }
        /* 3) CPU livre: despacha a frente da fila */
        if (atual == -1 && f.tam > 0) {
            atual = desenfileirar(&f);
            fatia = 0;
            if (inicio[atual] < 0) inicio[atual] = t;
            if (ultima != -1 && ultima != atual) trocas++;
            despachou = 1;
            if (detalhe == 2) printf("  t=%2d  CPU pega a frente da fila: %s\n", t, TAREFAS[atual].nome);
        }
        if (atual == -1) {
            gantt[t] = " . "; /* CPU ociosa */
            continue;
        }
        /* 4) executa 1 tick */
        gantt[t] = TAREFAS[atual].nome;
        restante[atual]--;
        fatia++;
        ultima = atual;
        if (detalhe == 2) {
            printf("  t=%2d  CPU: %s (restam %d, fatia %d/%s)   ", t, TAREFAS[atual].nome,
                   restante[atual], fatia, qtxt);
            mostrar_fila(&f);
            printf("\n");
        }
        if (restante[atual] == 0) {
            termino[atual] = t + 1;
            concluidas++;
            if (detalhe == 2) printf("  t=%2d  %s TERMINOU no fim do tick (t=%d)\n", t,
                                     TAREFAS[atual].nome, t + 1);
            atual = -1;
        }
        if (detalhe == 2 && despachou) pausar();
    }

    Estatisticas e = {0, 0, 0, 0, trocas};
    for (int i = 0; i < NT; i++) {
        int turnaround = termino[i] - TAREFAS[i].chegada;
        e.espera_media += turnaround - TAREFAS[i].duracao;
        e.resposta_media += inicio[i] - TAREFAS[i].chegada;
        e.turnaround_medio += turnaround;
        if (TAREFAS[i].nome[0] == 'M') e.resposta_mod = inicio[i] - TAREFAS[i].chegada;
    }
    e.espera_media /= NT;
    e.resposta_media /= NT;
    e.turnaround_medio /= NT;

    if (detalhe >= 1) {
        printf("\n  Diagrama de Gantt (%s):\n  tick: ", q == INT_MAX ? "FCFS" : "round-robin");
        for (int k = 0; k < t; k++) printf("%-4d", k);
        printf("\n  CPU : ");
        for (int k = 0; k < t; k++) printf("%-4s", gantt[k]);
        /* Cabeçalho literal (printf conta bytes; letras acentuadas ocupam 2) e a
           descrição por último, para que os acentos não desalinhem as colunas. */
        printf("\n\n  tar. chegada duração  início término  espera   resp.  descrição\n");
        for (int i = 0; i < NT; i++) {
            int turnaround = termino[i] - TAREFAS[i].chegada;
            printf("  %-4s %7d %7d %7d %7d %7d %7d  %s\n", TAREFAS[i].nome, TAREFAS[i].chegada,
                   TAREFAS[i].duracao, inicio[i], termino[i], turnaround - TAREFAS[i].duracao,
                   inicio[i] - TAREFAS[i].chegada, TAREFAS[i].descricao);
        }
        printf("  médias: espera = %.1f, resposta = %.1f, turnaround = %.1f ticks; "
               "trocas de contexto = %d\n", e.espera_media, e.resposta_media, e.turnaround_medio,
               e.trocas);
    }
    return e;
}

int main(void) {
    modo_interativo = isatty(STDIN_FILENO);
    printf("ESCALONADOR ROUND-ROBIN COM FILA CIRCULAR (1 tick = 1 ms)\n");
    printf("espera = término - chegada - duração;  resposta = início - chegada\n");
    pausar();

    printf("\n=== ROUND-ROBIN, quantum = 2 ticks (tick a tick) ===\n");
    simular(2, 2);
    pausar();

    printf("\n=== FCFS: mesma fila, mas sem preempção (cada tarefa vai até o fim) ===\n");
    simular(INT_MAX, 1);
    printf("  -> MOD chegou em t=3 e só começou em t=13 (resposta de 10 ms), porque o LOG\n"
           "     ocupou a CPU por 8 ms seguidos: um mestre Modbus com timeout curto poderia\n"
           "     desistir da resposta.\n");
    pausar();

    printf("\n=== COMPARAÇÃO DE QUANTA ===\n\n");
    printf("  quantum   |  espera média |  resposta média |  resposta MOD |   trocas\n");
    printf("  ----------+---------------+-----------------+---------------+---------\n");
    const int quanta[] = {1, 2, 3, 4, INT_MAX};
    for (int i = 0; i < 5; i++) {
        Estatisticas e = simular(quanta[i], 0);
        char rot[16];
        if (quanta[i] == INT_MAX) snprintf(rot, sizeof rot, "FCFS");
        else snprintf(rot, sizeof rot, "%d", quanta[i]);
        printf("  %-9s | %13.1f | %15.1f | %13d | %8d\n", rot, e.espera_media, e.resposta_media,
               e.resposta_mod, e.trocas);
    }
    printf("\n  Quantum pequeno: respostas rápidas, mas muitas trocas de contexto (cada troca\n"
           "  custa tempo real de CPU). Quantum enorme: vira FCFS, a tarefa longa bloqueia as\n"
           "  curtas. Na prática escolhe-se um quantum bem maior que o custo da troca.\n");
    return 0;
}
