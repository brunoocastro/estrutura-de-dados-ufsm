/*
 * ============================================================================
 * Arquivo : 06_lista_sentinela.c
 * Módulo  : 07 — Listas encadeadas           (Ementa: Unidade 1.3)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Mostrar por que um NÓ SENTINELA (nó-cabeça "fantasma", que não guarda
 *   dado útil) elimina os casos especiais das listas encadeadas:
 *     Parte 1: lista simples SEM sentinela → inserir/remover no início exige
 *              um "if (anterior == NULL)" para mexer na variável inicio.
 *     Parte 2: lista simples COM sentinela → todo nó útil tem um anterior
 *              (no mínimo a sentinela), então o código é um só.
 *     Parte 3: lista DUPLA CIRCULAR com sentinela → nem NULL existe mais:
 *              inserir = 4 escritas, remover = 2 escritas, zero "if".
 *     Parte 4: sentinela como "guarda" na busca → o laço deixa de testar o fim.
 *   Os contadores mostram quantas vezes o ramo especial foi executado.
 *
 * EXEMPLO REAL
 *   - FreeRTOS (RTOS usado em CLPs compactos e microcontroladores): cada lista
 *     de tarefas prontas tem um marcador de fim (xListEnd) que funciona como
 *     sentinela de uma lista dupla circular.
 *   - Kernel Linux: struct list_head, a lista dupla circular com nó-cabeça
 *     usada em drivers e no escalonador.
 *   - Menos "if" em código embarcado = menos caminhos para testar e tempo de
 *     execução mais previsível (importante em tarefas de tempo real).
 *
 * COMPLEXIDADE (n = nós úteis)
 *   Operação                     Sem sentinela   Com sentinela   Dupla circ. c/ sent.
 *   inserir ordenado             O(n)            O(n)            O(n)
 *   remover por valor            O(n)            O(n)            O(n)
 *   inserir/remover dado o nó    —               —               O(1)
 *   casos especiais no código    início/vazia    nenhum          nenhum (nem NULL)
 *   memória extra                0               1 nó            1 nó
 *   A sentinela não muda o Big O: ela muda a QUANTIDADE DE CASOS que o código
 *   (e quem o testa) precisa considerar. Na busca com guarda, o número de
 *   testes por passo cai de 2 para 1 (mesmo O(n), constante menor).
 *
 * COMO EXECUTAR
 *   ./run 06_lista_sentinela               (ENTER avança cada passo)
 *   ./run 06_lista_sentinela < /dev/null   (executa direto, sem pausas)
 * ============================================================================
 */
#define _POSIX_C_SOURCE 200809L   /* antes de qualquer #include: libera isatty() em -std=c11 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct No {
    int valor;
    struct No *prox;
} No;

static int modo_interativo = 0;   /* 1 quando stdin é um terminal */
static unsigned casos_especiais = 0;   /* vezes que um ramo "especial" executou */

/* Pausa até o aluno teclar ENTER; não faz nada se a entrada não for um terminal. */
static void pausar(void) {
    if (!modo_interativo) return;
    printf("    [ENTER] próximo passo...");
    fflush(stdout);
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
}

static void titulo(const char *t) {
    printf("============================================================================\n");
    printf("%s\n", t);
    printf("============================================================================\n");
}

/* Desenha a partir de um primeiro nó; 'rotulo' é o que aponta para ele. O(n). */
static void desenha_simples(const char *rotulo, const No *primeiro, const No *sentinela) {
    printf("    %s", rotulo);
    if (sentinela != NULL)
        printf("[ S|*]-->");
    for (const No *p = primeiro; p != NULL; p = p->prox)
        printf("[%2d|*]-->", p->valor);
    printf("NULL\n");
}

/* ======================================================================== */
/* PARTE 1 — SEM SENTINELA: o primeiro nó é diferente dos outros            */
/* ======================================================================== */

/* O(n). Precisa de No ** (ou de devolver o novo início) porque às vezes
 * altera 'inicio'; e precisa de um if para decidir QUEM aponta para o novo. */
static int sem_insere_ordenado(No **inicio, int valor) {
    No *novo = malloc(sizeof *novo);
    if (novo == NULL) return 0;
    novo->valor = valor;

    No *anterior = NULL;
    No *atual = *inicio;
    while (atual != NULL && atual->valor < valor) {
        anterior = atual;
        atual = atual->prox;
    }
    novo->prox = atual;
    if (anterior == NULL) {           /* CASO ESPECIAL: vazia ou novo é o menor */
        *inicio = novo;
        casos_especiais++;
        printf("  insere %2d → caso especial: não há anterior, muda 'inicio'\n", valor);
    } else {
        anterior->prox = novo;
        printf("  insere %2d → caso geral: anterior->prox = novo\n", valor);
    }
    return 1;
}

/* O(n). Mesmo problema na remoção: remover o primeiro altera 'inicio'. */
static int sem_remove(No **inicio, int valor) {
    No *anterior = NULL;
    No *atual = *inicio;
    while (atual != NULL && atual->valor != valor) {
        anterior = atual;
        atual = atual->prox;
    }
    if (atual == NULL) return 0;
    if (anterior == NULL) {           /* CASO ESPECIAL: remover o primeiro */
        *inicio = atual->prox;
        casos_especiais++;
        printf("  remove %2d → caso especial: muda 'inicio'\n", valor);
    } else {
        anterior->prox = atual->prox;
        printf("  remove %2d → caso geral: anterior->prox = atual->prox\n", valor);
    }
    free(atual);
    return 1;
}

/* ======================================================================== */
/* PARTE 2 — COM SENTINELA: todo nó útil tem um anterior                    */
/* ======================================================================== */

/* A sentinela é um No comum (pode até ficar na pilha, sem malloc). Seu valor
 * nunca é lido. sentinela.prox é o primeiro nó útil; lista vazia: prox == NULL. */

/* O(n). Sem if de caso especial: 'anterior' começa NA sentinela, nunca é NULL.
 * Basta No * (não No **): a sentinela nunca muda de endereço. */
static int com_insere_ordenado(No *sentinela, int valor) {
    No *novo = malloc(sizeof *novo);
    if (novo == NULL) return 0;
    novo->valor = valor;

    No *anterior = sentinela;
    while (anterior->prox != NULL && anterior->prox->valor < valor)
        anterior = anterior->prox;
    novo->prox = anterior->prox;
    anterior->prox = novo;            /* sempre o mesmo caminho */
    printf("  insere %2d → anterior->prox = novo (anterior = %s)\n", valor,
           anterior == sentinela ? "a sentinela" : "um nó útil");
    return 1;
}

/* O(n). Também sem caso especial. */
static int com_remove(No *sentinela, int valor) {
    No *anterior = sentinela;
    while (anterior->prox != NULL && anterior->prox->valor != valor)
        anterior = anterior->prox;
    if (anterior->prox == NULL) return 0;
    No *alvo = anterior->prox;
    anterior->prox = alvo->prox;
    free(alvo);
    printf("  remove %2d → anterior->prox = alvo->prox (anterior = %s)\n", valor,
           anterior == sentinela ? "a sentinela" : "um nó útil");
    return 1;
}

static void libera_simples(No *primeiro) {
    while (primeiro != NULL) {
        No *seguinte = primeiro->prox;
        free(primeiro);
        primeiro = seguinte;
    }
}

/* Roteiro igual para as duas versões: inserções e remoções que caem no
 * início, no meio e no fim. */
static const int insercoes[] = {30, 10, 50, 20, 5};
static const int remocoes[]  = {5, 30, 50};
enum { N_INS = sizeof insercoes / sizeof insercoes[0],
       N_REM = sizeof remocoes / sizeof remocoes[0] };

static int parte1_e_2(void) {
    titulo("PARTE 1 — lista simples SEM sentinela");
    No *inicio = NULL;
    casos_especiais = 0;
    for (int k = 0; k < N_INS; k++) {
        if (!sem_insere_ordenado(&inicio, insercoes[k])) { libera_simples(inicio); return 0; }
        desenha_simples("inicio-->", inicio, NULL);
        pausar();
    }
    for (int k = 0; k < N_REM; k++) {
        sem_remove(&inicio, remocoes[k]);
        desenha_simples("inicio-->", inicio, NULL);
        pausar();
    }
    unsigned sem = casos_especiais;
    printf("  Ramos especiais executados: %u (cada um é um caminho a mais para testar)\n\n", sem);
    libera_simples(inicio);
    pausar();

    titulo("PARTE 2 — lista simples COM sentinela (mesmo roteiro)");
    No sentinela = {0, NULL};
    casos_especiais = 0;
    printf("  Mesmo vazia, a lista já tem um nó (a sentinela):\n");
    desenha_simples("sentinela:", sentinela.prox, &sentinela);
    for (int k = 0; k < N_INS; k++) {
        if (!com_insere_ordenado(&sentinela, insercoes[k])) {
            libera_simples(sentinela.prox);
            return 0;
        }
        desenha_simples("sentinela:", sentinela.prox, &sentinela);
        pausar();
    }
    for (int k = 0; k < N_REM; k++) {
        com_remove(&sentinela, remocoes[k]);
        desenha_simples("sentinela:", sentinela.prox, &sentinela);
        pausar();
    }
    printf("  Ramos especiais executados: %u (antes eram %u). O código não tem o if.\n\n",
           casos_especiais, sem);
    libera_simples(sentinela.prox);   /* a sentinela não foi alocada: não damos free nela */
    pausar();
    return 1;
}

/* ======================================================================== */
/* PARTE 3 — LISTA DUPLA CIRCULAR COM SENTINELA                              */
/* ======================================================================== */

typedef struct NoD {
    int valor;
    struct NoD *ant, *prox;
} NoD;

/* Vazia: a sentinela aponta para si mesma nos dois sentidos.
 * Assim, TODO nó (inclusive a sentinela) sempre tem ant e prox válidos. */
static void dupla_inicializa(NoD *s) {
    s->ant = s;
    s->prox = s;
}

/* Insere 'novo' imediatamente antes de 'pos'. O(1), 4 escritas, nenhum if.
 *   inserir no início  = insere_antes(s->prox, novo)
 *   inserir no fim     = insere_antes(s, novo)      (antes da sentinela = último) */
static void insere_antes(NoD *pos, NoD *novo) {
    novo->prox = pos;
    novo->ant = pos->ant;
    pos->ant->prox = novo;
    pos->ant = novo;
}

/* Remove 'n' da lista. O(1), 2 escritas, nenhum if.
 * Pré-condição: n não é a sentinela (remover a sentinela destruiria a lista). */
static void desliga(NoD *n) {
    n->ant->prox = n->prox;
    n->prox->ant = n->ant;
}

static NoD *dupla_novo(int valor) {
    NoD *n = malloc(sizeof *n);
    if (n != NULL) {
        n->valor = valor;
        n->ant = n->prox = NULL;
    }
    return n;
}

/* Desenha +->[S]<->[10]<->[20]<-+ com a volta por baixo. O(n). */
static void desenha_dupla(const NoD *s) {
    int largura = 4;   /* "[ S]" */
    printf("    +->[ S]");
    for (const NoD *p = s->prox; p != s; p = p->prox) {
        printf("<->[%2d]", p->valor);
        largura += 7;
    }
    printf("<-+\n");
    printf("    +--");
    for (int k = 0; k < largura; k++) putchar('-');
    printf("--+   (s->ant = último, último->prox = s)\n");
}

/* Libera os nós úteis; a sentinela está na pilha. O(n). */
static void dupla_libera(NoD *s) {
    NoD *p = s->prox;
    while (p != s) {
        NoD *seguinte = p->prox;
        free(p);
        p = seguinte;
    }
    dupla_inicializa(s);
}

static int parte3(void) {
    titulo("PARTE 3 — lista DUPLA CIRCULAR com sentinela: zero if, zero NULL");
    NoD s;
    dupla_inicializa(&s);
    printf("  Vazia: s.ant == s.prox == &s\n");
    desenha_dupla(&s);
    pausar();

    NoD *n20 = dupla_novo(20), *n10 = dupla_novo(10), *n30 = dupla_novo(30);
    if (n20 == NULL || n10 == NULL || n30 == NULL) {
        free(n20); free(n10); free(n30);
        return 0;
    }
    printf("  insere_antes(&s, [20])     (no fim; numa lista vazia, também é o início)\n");
    insere_antes(&s, n20);
    desenha_dupla(&s);
    pausar();
    printf("  insere_antes(s.prox, [10]) (no início)\n");
    insere_antes(s.prox, n10);
    desenha_dupla(&s);
    pausar();
    printf("  insere_antes(&s, [30])     (no fim)\n");
    insere_antes(&s, n30);
    desenha_dupla(&s);
    pausar();

    printf("  desliga([20]) (meio)  → mesmas 2 linhas de código\n");
    desliga(n20); free(n20);
    desenha_dupla(&s);
    pausar();
    printf("  desliga([10]) (início) → mesmas 2 linhas de código\n");
    desliga(n10); free(n10);
    desenha_dupla(&s);
    pausar();
    printf("  desliga([30]) (fim, e a lista fica vazia) → mesmas 2 linhas de código\n");
    desliga(n30); free(n30);
    desenha_dupla(&s);
    printf("  Compare com 04_lista_dupla_alarmes_ihm.c, em que remover precisa de 2 if\n"
           "  (o nó é o primeiro? é o último?) porque as pontas apontam para NULL.\n\n");
    pausar();
    dupla_libera(&s);   /* já vazia; fica como hábito */
    return 1;
}

/* ======================================================================== */
/* PARTE 4 — SENTINELA COMO GUARDA NA BUSCA                                  */
/* ======================================================================== */

/* Busca comum: 2 testes por passo (chegou ao fim? achou?). O(n). */
static const NoD *busca_comum(const NoD *s, int valor, unsigned *testes) {
    const NoD *p = s->prox;
    while (1) {
        (*testes)++;
        if (p == s) return NULL;          /* teste 1: fim */
        (*testes)++;
        if (p->valor == valor) return p;  /* teste 2: achou */
        p = p->prox;
    }
}

/* Busca com guarda: copia o valor procurado para a sentinela; a busca SEMPRE
 * acha algo, então o teste de fim sai do laço. 1 teste por passo. O(n). */
static const NoD *busca_com_guarda(NoD *s, int valor, unsigned *testes) {
    s->valor = valor;                     /* a guarda garante a parada */
    const NoD *p = s->prox;
    while ((*testes)++, p->valor != valor)
        p = p->prox;
    return p == s ? NULL : p;             /* achou só a guarda? então não existe */
}

static int parte4(void) {
    titulo("PARTE 4 — sentinela como guarda: busca com 1 teste por passo em vez de 2");
    enum { N = 1000 };
    NoD s;
    dupla_inicializa(&s);
    for (int k = 0; k < N; k++) {
        NoD *n = dupla_novo(k);
        if (n == NULL) { dupla_libera(&s); return 0; }
        insere_antes(&s, n);
    }
    int alvos[] = {0, 500, 999, -1};
    printf("    %8s | %14s | %16s\n", "procura", "testes comum", "testes c/ guarda");
    printf("    ---------+----------------+-----------------\n");
    for (size_t k = 0; k < sizeof alvos / sizeof alvos[0]; k++) {
        unsigned t1 = 0, t2 = 0;
        const NoD *r1 = busca_comum(&s, alvos[k], &t1);
        const NoD *r2 = busca_com_guarda(&s, alvos[k], &t2);
        printf("    %8d | %14u | %16u   %s\n", alvos[k], t1, t2,
               r1 == r2 ? (r1 != NULL ? "achou" : "não existe") : "ERRO");
    }
    printf("  Mesmo O(n), mas metade dos testes: em laços críticos (tempo real) isso conta.\n"
           "  A mesma ideia vale para busca sequencial em vetor (veja ../08_ordenacao_busca/).\n");
    dupla_libera(&s);
    return 1;
}

int main(void) {
    modo_interativo = isatty(STDIN_FILENO);
    if (!parte1_e_2() || !parte3() || !parte4()) {
        fprintf(stderr, "Sem memória para a demonstração.\n");
        return EXIT_FAILURE;
    }
    printf("\nResumo: a sentinela custa 1 nó e remove os casos especiais de início, fim e\n"
           "lista vazia. Outra forma de remover o caso do início é o ponteiro para o elo\n"
           "(No **), mostrado em 02_lista_passo_a_passo.c.\n");
    return EXIT_SUCCESS;
}
