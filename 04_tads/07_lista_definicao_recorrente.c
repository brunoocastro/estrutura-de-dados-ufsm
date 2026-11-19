/*
 * ============================================================================
 * Arquivo : 07_lista_definicao_recorrente.c
 * Módulo  : 04 — Tipos Abstratos de Dados       (Ementa: Unidade 1.3)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Mostrar a DEFINIÇÃO RECORRENTE de um TAD:
 *       uma lista é  VAZIA  ou  um ELEMENTO seguido de uma LISTA.
 *   A definição vira código quase palavra por palavra:
 *     - a struct é auto-referenciada (o nó aponta para "o resto", que é outra
 *       lista);
 *     - cada função tem um CASO BASE (lista vazia) e um CASO RECURSIVO
 *       (elemento + resto). O programa desenha a recursão descendo e voltando
 *       para tamanho, soma, busca e impressão, e compara com a versão
 *       iterativa usada em ../07_listas.
 *
 * EXEMPLO REAL
 *   Rota de um AGV (veículo guiado automático) como lista de trechos: cada nó
 *   guarda a estação de destino e o comprimento do trecho em metros. Somar a
 *   lista = distância total; buscar = "a rota passa pela estação 30?".
 *
 * COMPLEXIDADE (n = número de nós)
 *   Operação      Tempo   Espaço (recursiva)       Espaço (iterativa)
 *   tamanho       O(n)    O(n) pilha de chamadas   O(1)
 *   soma          O(n)    O(n)                     O(1)
 *   busca         O(n)    O(n) no pior caso        O(1)
 *   imprimir      O(n)    O(n)                     O(1)
 *   inserir_inicio O(1)   O(1)                     O(1)
 *   liberar       O(n)    O(n)                     O(1)
 *   Por quê: cada chamada trata 1 nó e delega o resto; são n+1 chamadas (a
 *   última na lista vazia). Cada chamada pendente ocupa um quadro na pilha,
 *   por isso listas muito longas (milhões de nós) pedem a versão iterativa.
 *
 * COMO EXECUTAR
 *   ./run 07_lista_definicao_recorrente               (ENTER avança cada passo)
 *   ./run 07_lista_definicao_recorrente < /dev/null   (executa direto, sem pausas)
 * ============================================================================
 */
#define _POSIX_C_SOURCE 200809L   /* antes de qualquer #include: libera isatty() em -std=c11 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static int modo_interativo = 0;   /* 1 quando stdin é um terminal */

/* Pausa até o aluno teclar ENTER; não faz nada se a entrada não for um terminal. */
static void pausar(void) {
    if (!modo_interativo) return;
    printf("    [ENTER] próximo passo...");
    fflush(stdout);
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
}

/* ##########################################################################
 * O TAD RECORRENTE
 *   Lista = NULL (vazia)  |  ponteiro para No { estacao, metros, resto }
 *   'struct No' precisa do nome (tag) porque o campo 'resto' a referencia
 *   antes do typedef terminar. Um campo 'struct No resto' (sem *) seria
 *   impossível: a struct conteria a si mesma e teria tamanho infinito.
 * ########################################################################## */

typedef struct No {
    int estacao;          /* estação de destino do trecho */
    int metros;           /* comprimento do trecho */
    struct No *resto;     /* a "lista que vem depois": outra Lista */
} No;

typedef No *Lista;        /* uma Lista é só um ponteiro para o primeiro nó */

#define LISTA_VAZIA NULL

/* "Construtor" da definição: elemento seguido de lista. O(1)
 * Devolve a nova lista; em falta de memória devolve NULL sem liberar 'resto'
 * (o chamador continua dono dele e decide o que fazer). */
static Lista inserir_inicio(int estacao, int metros, Lista resto) {
    No *novo = malloc(sizeof *novo);
    if (novo == NULL) return NULL;
    novo->estacao = estacao;
    novo->metros = metros;
    novo->resto = resto;
    return novo;
}

/* ----- desenho -------------------------------------------------------- */

/* Mostra a lista no formato da notação recorrente: (10:12m) -> (20:8m) -> []  O(n) */
static void desenhar(Lista l) {
    for (; l != NULL; l = l->resto) printf("(%d:%dm) -> ", l->estacao, l->metros);
    printf("[]");
}

static void recuo(int nivel) {
    for (int i = 0; i < nivel; i++) printf("   ");
}

/* ----- funções recursivas com rastreamento ----------------------------- */

static int chamadas;   /* contador de chamadas, para comparar com n */

/* tamanho([])        = 0
 * tamanho(x : resto) = 1 + tamanho(resto)                         O(n) */
static int tamanho(Lista l, int nivel) {
    chamadas++;
    recuo(nivel);
    printf("tamanho(");
    desenhar(l);
    printf(")\n");
    if (l == NULL) {
        recuo(nivel);
        printf("-> caso base: lista vazia, devolve 0\n");
        return 0;
    }
    int r = 1 + tamanho(l->resto, nivel + 1);
    recuo(nivel);
    printf("<- 1 + tamanho(resto) = %d\n", r);
    return r;
}

/* soma([])        = 0
 * soma(x : resto) = x.metros + soma(resto)                         O(n) */
static int soma(Lista l, int nivel) {
    chamadas++;
    if (l == NULL) {
        recuo(nivel);
        printf("soma([]) = 0   (caso base)\n");
        return 0;
    }
    recuo(nivel);
    printf("soma a partir da estação %d: %d + soma(resto) ...\n", l->estacao, l->metros);
    int r = l->metros + soma(l->resto, nivel + 1);
    recuo(nivel);
    printf("... = %d m\n", r);
    return r;
}

/* busca([], e)        = falso
 * busca(x : resto, e) = verdadeiro se x.estacao == e, senão busca(resto, e)
 * A recursão PARA cedo quando acha: melhor caso O(1), pior O(n). */
static int busca(Lista l, int estacao, int nivel) {
    chamadas++;
    recuo(nivel);
    if (l == NULL) {
        printf("lista vazia: %d não está na rota\n", estacao);
        return 0;
    }
    if (l->estacao == estacao) {
        printf("estação %d == %d: ACHOU (não precisa olhar o resto)\n", l->estacao, estacao);
        return 1;
    }
    printf("estação %d != %d: procura no resto\n", l->estacao, estacao);
    return busca(l->resto, estacao, nivel + 1);
}

/* Imprime na ordem: trata o nó ANTES da chamada recursiva. O(n) */
static void imprimir(Lista l) {
    if (l == NULL) return;
    printf(" %d", l->estacao);
    imprimir(l->resto);
}

/* Imprime ao contrário: trata o nó DEPOIS da chamada recursiva. O(n)
 * A pilha de chamadas guarda os nós "pendentes" e os devolve em ordem inversa
 * (é uma pilha LIFO de verdade: veja ../05_pilhas). */
static void imprimir_invertido(Lista l) {
    if (l == NULL) return;
    imprimir_invertido(l->resto);
    printf(" %d", l->estacao);
}

/* Libera o resto antes do próprio nó: se liberasse 'l' primeiro, l->resto
 * seria lido de memória já devolvida (uso após free). O(n) */
static void liberar(Lista l) {
    if (l == NULL) return;
    liberar(l->resto);
    free(l);
}

/* ----- versões iterativas, como em ../07_listas ------------------------ */

/* O laço 'while (p != NULL) p = p->resto' é a mesma recursão, sem a pilha. O(n), O(1) espaço */
static int soma_iterativa(Lista l) {
    int s = 0;
    for (No *p = l; p != NULL; p = p->resto) s += p->metros;
    return s;
}

int main(void) {
    modo_interativo = isatty(STDIN_FILENO);
    printf("DEFINIÇÃO RECORRENTE: UMA LISTA É VAZIA OU ELEMENTO + LISTA\n");

    /* PASSO 1 ------------------------------------------------------------ */
    printf("\n== PASSO 1 — Construindo a rota pela definição (de trás para frente) ==\n");
    const int estacoes[] = { 10, 20, 30, 40 };
    const int metros[] = { 12, 8, 15, 5 };
    Lista rota = LISTA_VAZIA;
    printf("  rota = ");
    desenhar(rota);
    printf("   (a lista vazia já é uma lista)\n");
    for (int i = 3; i >= 0; i--) {
        Lista nova = inserir_inicio(estacoes[i], metros[i], rota);
        if (nova == NULL) {
            fprintf(stderr, "sem memória\n");
            liberar(rota);
            return 1;
        }
        rota = nova;
        printf("  rota = (%d:%dm) seguido de [lista anterior] = ", estacoes[i], metros[i]);
        desenhar(rota);
        printf("\n");
        pausar();
    }
    printf("\n  Na memória (cada caixa é um nó; o ponteiro 'resto' é outra lista):\n  ");
    for (No *p = rota; p != NULL; p = p->resto) printf("[%d|%2dm|*]-->", p->estacao, p->metros);
    printf("NULL\n");
    pausar();

    /* PASSO 2 ------------------------------------------------------------ */
    printf("\n== PASSO 2 — tamanho: descendo até o caso base e voltando ==\n");
    chamadas = 0;
    int n = tamanho(rota, 1);
    printf("  tamanho = %d nós, %d chamadas (n + 1: a última é na lista vazia)\n", n, chamadas);
    pausar();

    /* PASSO 3 ------------------------------------------------------------ */
    printf("\n== PASSO 3 — soma: distância total da rota ==\n");
    chamadas = 0;
    int total = soma(rota, 1);
    printf("  distância total = %d m (recursiva, %d chamadas) | %d m (iterativa)\n", total,
           chamadas, soma_iterativa(rota));
    pausar();

    /* PASSO 4 ------------------------------------------------------------ */
    printf("\n== PASSO 4 — busca: para assim que encontra ==\n");
    chamadas = 0;
    int achou = busca(rota, 30, 1);
    printf("  busca(30) = %s em %d chamadas\n\n", achou ? "sim" : "não", chamadas);
    chamadas = 0;
    achou = busca(rota, 99, 1);
    printf("  busca(99) = %s em %d chamadas (pior caso: n + 1)\n", achou ? "sim" : "não",
           chamadas);
    pausar();

    /* PASSO 5 ------------------------------------------------------------ */
    printf("\n== PASSO 5 — A posição da chamada recursiva muda a ordem ==\n");
    printf("  imprimir           (trata nó, depois o resto):");
    imprimir(rota);
    printf("\n  imprimir_invertido (trata o resto, depois o nó):");
    imprimir_invertido(rota);
    printf("\n  Rota de volta do AGV sem montar outra lista: a pilha de chamadas guarda a ordem.\n");
    pausar();

    /* PASSO 6 ------------------------------------------------------------ */
    printf("\n== PASSO 6 — A mesma ideia gera árvores ==\n");
    printf("  lista  = vazia | elemento + UMA lista        -> struct No { ...; No *resto; }\n");
    printf("  árvore = vazia | raiz + subárvore esq + dir  -> struct No { ...; No *esq, *dir; }\n");
    printf("  E as funções seguem o mesmo molde:\n");
    printf("    tamanho(lista)  = 0 se vazia, senão 1 + tamanho(resto)\n");
    printf("    tamanho(árvore) = 0 se vazia, senão 1 + tamanho(esq) + tamanho(dir)\n");
    printf("  Veja 08_arvore_expressao_recorrente.c, ../07_listas e ../09_arvores.\n");
    pausar();

    liberar(rota);
    printf("\nFim da demonstração (todos os nós liberados).\n");
    return 0;
}
