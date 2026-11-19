/*
 * ============================================================================
 * Arquivo : 02_fila_encadeada.c
 * Módulo  : 06 — Filas                        (Ementa: Unidade 3.1)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Fila FIFO com nós encadeados e DOIS ponteiros: `ini` (de onde se remove)
 *   e `fim` (onde se insere). A cada operação o programa desenha os nós
 *       ini                fim
 *        |                  |
 *        v                  v
 *      [10|*]-->[20|*]-->[30|/]
 *   e narra cada atribuição de ponteiro. Depois mede por que o ponteiro `fim`
 *   é indispensável: sem ele, cada inserção percorre a fila inteira (O(n)).
 *   Ao final há um menu para explorar livremente.
 *
 * EXEMPLO REAL
 *   Fila de lotes aguardando a estufa de cura numa linha de pintura: o número
 *   de lotes não tem limite conhecido de antemão (pode haver parada da estufa)
 *   e a ordem de chegada precisa ser respeitada. A versão encadeada cresce sob
 *   demanda; o vetor circular (01_fila_linear_vs_circular) tem limite fixo.
 *
 * COMPLEXIDADE (n = elementos na fila)
 *   Operação        com ini e fim    só com ini     Espaço
 *   enfileirar      O(1)             O(n)           O(n) total
 *   desenfileirar   O(1)             O(1)           + 1 ponteiro por nó
 *   frente/vazia    O(1)             O(1)
 *   Por quê: inserir no fim exige conhecer o último nó. Com `fim` ele está à
 *   mão; sem `fim` é preciso caminhar de `ini` até o último (n passos).
 *   O preço da flexibilidade: um malloc/free por elemento e o ponteiro extra.
 *
 * COMO EXECUTAR
 *   ./run 02_fila_encadeada               (ENTER avança; depois abre o menu)
 *   ./run 02_fila_encadeada < /dev/null   (só a demonstração, sem pausas)
 * ============================================================================
 */
#define _POSIX_C_SOURCE 200809L /* antes de qualquer #include: libera isatty() em -std=c11 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

typedef struct No {
    int lote;        /* número do lote */
    struct No *prox; /* próximo da fila (em direção ao fim); NULL no último */
} No;

typedef struct {
    No *ini;    /* primeiro nó: o próximo a sair */
    No *fim;    /* último nó: depois dele entra o próximo */
    int tam;
} Fila;

/* O(1). Fila vazia: os DOIS ponteiros valem NULL. */
static void fila_iniciar(Fila *f) {
    f->ini = NULL;
    f->fim = NULL;
    f->tam = 0;
}

/*
 * O(1). Insere no fim. Se `narrar` != 0, explica cada atribuição.
 * Devolve 0 se o malloc falhar (a fila fica intacta).
 */
static int enfileirar(Fila *f, int lote, int narrar) {
    No *novo = malloc(sizeof *novo);
    if (novo == NULL) return 0;
    novo->lote = lote;
    novo->prox = NULL; /* o novo será o último: ninguém depois dele */
    if (narrar) printf("    1) novo = malloc(...); novo->lote = %d; novo->prox = NULL\n", lote);

    if (f->fim == NULL) {
        /* Fila vazia: o novo nó é ao mesmo tempo o primeiro e o último. */
        f->ini = novo;
        if (narrar) printf("    2) fila estava vazia -> ini = novo\n");
    } else {
        f->fim->prox = novo; /* engata o novo depois do último atual */
        if (narrar) printf("    2) fim->prox = novo   (o lote %d passa a apontar para %d)\n",
                           f->fim->lote, lote);
    }
    f->fim = novo;
    if (narrar) printf("    3) fim = novo\n");
    f->tam++;
    return 1;
}

/*
 * O(1). Remove do início. Devolve 0 se a fila estiver vazia.
 * ARMADILHA clássica: ao remover o ÚLTIMO nó, `fim` precisa voltar a NULL.
 * Se esquecer, `fim` fica apontando para memória liberada (ponteiro pendente)
 * e o próximo enfileirar escreve nela: comportamento indefinido.
 */
static int desenfileirar(Fila *f, int *lote, int narrar) {
    if (f->ini == NULL) return 0;
    No *aux = f->ini;           /* guarda o nó para poder liberar depois */
    *lote = aux->lote;
    f->ini = aux->prox;         /* o segundo vira o primeiro */
    if (narrar) printf("    1) aux = ini (lote %d); ini = aux->prox\n", aux->lote);
    if (f->ini == NULL) {
        f->fim = NULL;
        if (narrar) printf("    2) ini ficou NULL -> fila vazia -> fim = NULL  (não esqueça!)\n");
    }
    free(aux);                  /* só agora: antes disso ainda líamos aux->prox */
    if (narrar) printf("    %d) free(aux)\n", f->ini == NULL ? 3 : 2);
    f->tam--;
    return 1;
}

/* O(n): libera todos os nós. Sem isso a memória vaza ao sair. */
static void fila_liberar(Fila *f) {
    int descartado;
    while (desenfileirar(f, &descartado, 0)) { }
}

/*
 * O(n): desenha os nós em uma linha e, acima, as setas de `ini` e `fim`
 * alinhadas com o nó correspondente. A coluna de cada nó é medida enquanto a
 * linha é montada em um buffer.
 */
static void desenhar(const Fila *f) {
    char linha[1024];
    int col_ini = -1, col_fim = -1, pos = 0, mostrados = 0;
    linha[0] = '\0';
    for (const No *p = f->ini; p != NULL && pos < 900; p = p->prox, mostrados++) {
        if (p == f->ini) col_ini = pos;
        if (p == f->fim) col_fim = pos;
        pos += snprintf(linha + pos, sizeof linha - (size_t)pos, "[%d|%s]%s", p->lote,
                        p->prox ? "*" : "/", p->prox ? "-->" : "");
    }
    if (f->ini == NULL) {
        printf("      ini = NULL, fim = NULL   (fila vazia)\n");
        return;
    }
    if (mostrados < f->tam)
        snprintf(linha + pos, sizeof linha - (size_t)pos, "...");

    /* Linha dos rótulos e das setas: 6 espaços de margem + coluna do nó. */
    char rot[1024], seta[1024], ponta[1024];
    int largura = (int)strlen(linha) + 4;
    memset(rot, ' ', (size_t)largura);
    memset(seta, ' ', (size_t)largura);
    memset(ponta, ' ', (size_t)largura);
    rot[largura] = seta[largura] = ponta[largura] = '\0';
    if (col_fim >= 0) {
        memcpy(rot + col_fim, "fim", 3);
        seta[col_fim + 1] = '|';
        ponta[col_fim + 1] = 'v';
    }
    if (col_ini >= 0) {
        if (col_ini == col_fim) memcpy(rot + col_ini, "ini=fim", 7);
        else memcpy(rot + col_ini, "ini", 3);
        seta[col_ini + 1] = '|';
        ponta[col_ini + 1] = 'v';
    }
    printf("      %s\n      %s\n      %s\n      %s\n", rot, seta, ponta, linha);
    printf("      tam = %d   memória dos nós = %d x %zu bytes = %zu bytes\n",
           f->tam, f->tam, sizeof(No), (size_t)f->tam * sizeof(No));
}

/* ------------------------------------------------------------------------- */
/* Comparação: fila só com `ini` (sem ponteiro de fim)                        */
/* ------------------------------------------------------------------------- */

/*
 * O(n): para inserir no fim sem o ponteiro `fim`, caminha de `ini` até o
 * último nó. `visitas` conta quantos nós foram atravessados.
 */
static int enfileirar_sem_fim(No **ini, int lote, long *visitas) {
    No *novo = malloc(sizeof *novo);
    if (novo == NULL) return 0;
    novo->lote = lote;
    novo->prox = NULL;
    if (*ini == NULL) { *ini = novo; return 1; }
    No *p = *ini;
    while (p->prox != NULL) { p = p->prox; (*visitas)++; }
    (*visitas)++; /* contou o último também */
    p->prox = novo;
    return 1;
}

static void tabela_crescimento(void) {
    printf("\n=== TABELA: nós visitados para enfileirar n lotes ===\n\n");
    /* Cabeçalho literal: printf conta bytes, e "ó" ocupa 2 bytes em UTF-8. */
    printf("         n |       só ini (caminha) |              ini + fim\n");
    printf("  ---------+------------------------+-----------------------\n");
    const int tamanhos[] = {10, 100, 1000, 10000};
    for (int t = 0; t < 4; t++) {
        int n = tamanhos[t];
        long visitas = 0;
        No *ini = NULL;
        Fila f;
        fila_iniciar(&f);
        for (int i = 0; i < n; i++) {
            enfileirar_sem_fim(&ini, i, &visitas);
            enfileirar(&f, i, 0); /* 0 visitas: vai direto a f.fim */
        }
        printf("  %8d | %22ld | %22d\n", n, visitas, 0);
        while (ini != NULL) { No *aux = ini; ini = ini->prox; free(aux); }
        fila_liberar(&f);
    }
    printf("\n  Sem `fim`, o total é n(n-1)/2 visitas (O(n) por inserção). Com `fim`: O(1).\n");
}

/* ------------------------------------------------------------------------- */
/* Menu para exploração livre (termina em EOF ou na opção 0)                  */
/* ------------------------------------------------------------------------- */

static void menu(Fila *f) {
    int proximo_lote = 500;
    for (;;) {
        printf("\nMENU  1) chega lote   2) lote entra na estufa (sai da fila)   3) mostrar   0) sair\n"
               "opção: ");
        fflush(stdout);
        int op;
        if (scanf("%d", &op) != 1) {
            /* EOF ou entrada inválida: encerra sem laço infinito. */
            printf("\n(fim da entrada)\n");
            return;
        }
        if (op == 0) return;
        if (op == 1) {
            if (enfileirar(f, proximo_lote, 1)) proximo_lote++;
            else printf("    sem memória\n");
            desenhar(f);
        } else if (op == 2) {
            int lote;
            if (desenfileirar(f, &lote, 1)) printf("    lote %d entrou na estufa\n", lote);
            else printf("    fila vazia: nada a remover\n");
            desenhar(f);
        } else if (op == 3) {
            desenhar(f);
        } else {
            printf("    opção inválida\n");
        }
    }
}

int main(void) {
    modo_interativo = isatty(STDIN_FILENO);
    Fila f;
    fila_iniciar(&f);

    printf("FILA ENCADEADA: lotes aguardando a estufa de cura\n\n");
    printf("Estado inicial:\n");
    desenhar(&f);
    pausar();

    const int chegadas[] = {101, 102, 103};
    for (int i = 0; i < 3; i++) {
        printf("\nchega o lote %d -> enfileirar(%d)\n", chegadas[i], chegadas[i]);
        enfileirar(&f, chegadas[i], 1);
        desenhar(&f);
        pausar();
    }

    int lote;
    printf("\na estufa libera uma vaga -> desenfileirar()\n");
    desenfileirar(&f, &lote, 1);
    printf("    -> lote %d entrou na estufa (era o mais antigo: FIFO)\n", lote);
    desenhar(&f);
    pausar();

    printf("\nchega o lote 104 -> enfileirar(104)\n");
    enfileirar(&f, 104, 1);
    desenhar(&f);
    pausar();

    while (f.tam > 0) {
        printf("\nestufa livre -> desenfileirar()\n");
        desenfileirar(&f, &lote, 1);
        printf("    -> lote %d entrou na estufa\n", lote);
        desenhar(&f);
        pausar();
    }

    tabela_crescimento();
    pausar();

    menu(&f);
    fila_liberar(&f);
    return 0;
}
