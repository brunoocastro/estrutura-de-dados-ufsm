/*
 * ============================================================================
 * Arquivo : 08_arvore_expressao_recorrente.c
 * Módulo  : 04 — Tipos Abstratos de Dados       (Ementa: Unidade 1.3)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Levar a definição recorrente da lista para a árvore:
 *       uma expressão é  um NÚMERO,  ou uma VARIÁVEL,
 *                        ou  (EXPRESSÃO  operador  EXPRESSÃO).
 *   Isso é uma árvore binária: folhas são números/variáveis, nós internos são
 *   operadores com duas subárvores. O programa monta a lei de controle
 *   proporcional  u = kp × (sp − pv) + bias  (com kp = 2), desenha a árvore,
 *   mostra a avaliação recursiva descendo e voltando, conta nós e altura,
 *   imprime em notação infixa e pós-fixa e usa a árvore para simular uma
 *   malha de nível.
 *
 * EXEMPLO REAL
 *   Blocos de cálculo configuráveis em IHMs/supervisórios e o próprio
 *   compilador de Texto Estruturado (ST) de um CLP: a expressão digitada pelo
 *   engenheiro vira uma árvore como esta e é avaliada a cada ciclo.
 *
 * COMPLEXIDADE (n = número de nós, h = altura da árvore)
 *   Operação         Tempo   Espaço (pilha de chamadas)
 *   avaliar          O(n)    O(h)
 *   contar nós       O(n)    O(h)
 *   altura           O(n)    O(h)
 *   imprimir/posfixa O(n)    O(h)
 *   liberar          O(n)    O(h)
 *   Por quê: cada nó é visitado exatamente uma vez; a pilha só guarda o
 *   caminho da raiz até o nó atual (no máximo h chamadas pendentes). Em
 *   árvores equilibradas h ≈ log2 n; numa árvore "degenerada" h = n (vira
 *   lista) — este é o assunto da AVL em ../09_arvores.
 *
 * COMO EXECUTAR
 *   ./run 08_arvore_expressao_recorrente               (ENTER avança cada passo)
 *   ./run 08_arvore_expressao_recorrente < /dev/null   (executa direto, sem pausas)
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
 * O TAD RECORRENTE "Expressão"
 * ########################################################################## */

typedef enum { NUMERO, VARIAVEL, OPERADOR } TipoNo;

/* Variáveis da malha de controle (o "ambiente" da avaliação). */
enum { VAR_SP, VAR_PV, VAR_BIAS, NUM_VARS };
static const char *NOME_VAR[NUM_VARS] = { "sp", "pv", "bias" };

typedef struct Expr {
    TipoNo tipo;
    double valor;            /* usado se tipo == NUMERO */
    int var;                 /* usado se tipo == VARIAVEL */
    char op;                 /* '+', '-', '*', '/' se tipo == OPERADOR */
    struct Expr *esq, *dir;  /* subárvores: NULL nas folhas */
} Expr;

/* Construtores: um por "caso" da definição. Cada um aloca 1 nó. O(1)
 * Se faltar memória, devolvem NULL e liberam as subárvores recebidas, para
 * que expressões aninhadas como op('+', num(1), num(2)) não vazem memória. */
static void liberar(Expr *e);

static Expr *novo_no(TipoNo tipo) {
    Expr *e = malloc(sizeof *e);
    if (e == NULL) return NULL;
    e->tipo = tipo;
    e->valor = 0.0;
    e->var = 0;
    e->op = '?';
    e->esq = e->dir = NULL;
    return e;
}

static Expr *numero(double v) {
    Expr *e = novo_no(NUMERO);
    if (e) e->valor = v;
    return e;
}

static Expr *variavel(int v) {
    Expr *e = novo_no(VARIAVEL);
    if (e) e->var = v;
    return e;
}

static Expr *operacao(char op, Expr *esq, Expr *dir) {
    Expr *e = (esq && dir) ? novo_no(OPERADOR) : NULL;
    if (e == NULL) {
        liberar(esq);
        liberar(dir);
        return NULL;
    }
    e->op = op;
    e->esq = esq;
    e->dir = dir;
    return e;
}

/* Pós-ordem: libera os filhos antes do pai (senão perderia os ponteiros). O(n) */
static void liberar(Expr *e) {
    if (e == NULL) return;
    liberar(e->esq);
    liberar(e->dir);
    free(e);
}

/* ----- funções recursivas ---------------------------------------------- */

/* nos(vazia) = 0;  nos(raiz) = 1 + nos(esq) + nos(dir)               O(n) */
static int nos(const Expr *e) {
    return e == NULL ? 0 : 1 + nos(e->esq) + nos(e->dir);
}

/* altura(vazia) = 0;  altura(raiz) = 1 + max(altura(esq), altura(dir)) O(n) */
static int altura(const Expr *e) {
    if (e == NULL) return 0;
    int a = altura(e->esq), b = altura(e->dir);
    return 1 + (a > b ? a : b);
}

/* Infixa com parênteses: (esq op dir). Ordem: esquerda, raiz, direita. O(n) */
static void imprimir_infixa(const Expr *e) {
    if (e == NULL) return;
    if (e->tipo == NUMERO)   { printf("%g", e->valor); return; }
    if (e->tipo == VARIAVEL) { printf("%s", NOME_VAR[e->var]); return; }
    printf("(");
    imprimir_infixa(e->esq);
    printf(" %c ", e->op);
    imprimir_infixa(e->dir);
    printf(")");
}

/* Pós-fixa: esquerda, direita, raiz. Sem parênteses; é o formato que uma
 * PILHA avalia (veja a avaliação RPN e a conversão infixa -> pós-fixa em ../05_pilhas). O(n) */
static void imprimir_posfixa(const Expr *e) {
    if (e == NULL) return;
    imprimir_posfixa(e->esq);
    imprimir_posfixa(e->dir);
    if (e->tipo == NUMERO)        printf("%g ", e->valor);
    else if (e->tipo == VARIAVEL) printf("%s ", NOME_VAR[e->var]);
    else                          printf("%c ", e->op);
}

/* Desenho "deitado": direita em cima, raiz no meio, esquerda embaixo.
 * Gire a cabeça 90° para a esquerda para ver a árvore em pé. O(n) */
static void desenhar(const Expr *e, int nivel) {
    if (e == NULL) return;
    desenhar(e->dir, nivel + 1);
    for (int i = 0; i < nivel; i++) printf("        ");
    if (e->tipo == NUMERO)        printf("%g\n", e->valor);
    else if (e->tipo == VARIAVEL) printf("%s\n", NOME_VAR[e->var]);
    else                          printf("[%c]\n", e->op);
    desenhar(e->esq, nivel + 1);
}

/* Avalia a expressão. Caso base: folha (número ou variável). Caso
 * recursivo: avalia as duas subárvores e aplica o operador. O(n), O(h) pilha.
 * 'narrar' liga o desenho da recursão; 'nivel' controla o recuo. */
static double avaliar(const Expr *e, const double amb[NUM_VARS], int narrar, int nivel) {
    if (e->tipo == NUMERO) {
        if (narrar) printf("%*s%g  (folha: número)\n", 4 * nivel, "", e->valor);
        return e->valor;
    }
    if (e->tipo == VARIAVEL) {
        if (narrar)
            printf("%*s%s = %g  (folha: variável)\n", 4 * nivel, "", NOME_VAR[e->var],
                   amb[e->var]);
        return amb[e->var];
    }
    if (narrar) {
        printf("%*s[%c] avalia ", 4 * nivel, "", e->op);
        imprimir_infixa(e);
        printf("\n");
    }
    double a = avaliar(e->esq, amb, narrar, nivel + 1);
    double b = avaliar(e->dir, amb, narrar, nivel + 1);
    double r = 0.0;
    switch (e->op) {
    case '+': r = a + b; break;
    case '-': r = a - b; break;
    case '*': r = a * b; break;
    case '/': r = b != 0.0 ? a / b : 0.0; break;   /* simplificação: divisão por zero -> 0 */
    }
    if (narrar) printf("%*s[%c] devolve %g %c %g = %g\n", 4 * nivel, "", e->op, a, e->op, b, r);
    return r;
}

/* Barra horizontal de 0 a 100 % com a letra S na posição do setpoint. O(40) */
static void barra(double pct, double setpoint) {
    int k = (int)(pct / 100.0 * 40.0 + 0.5);
    int s = (int)(setpoint / 100.0 * 40.0 + 0.5);
    if (k < 0) k = 0;
    if (k > 40) k = 40;
    putchar('|');
    for (int i = 0; i < 40; i++) putchar(i == s ? 'S' : i < k ? '#' : ' ');
    putchar('|');
}

int main(void) {
    modo_interativo = isatty(STDIN_FILENO);
    printf("DEFINIÇÃO RECORRENTE DE ÁRVORE: EXPRESSÃO DE UM CONTROLADOR P\n");

    /* PASSO 1 ------------------------------------------------------------ */
    printf("\n== PASSO 1 — Montando u = kp × (sp − pv) + bias, com kp = 2, pela definição ==\n");
    printf("  erro   = (sp - pv)              <- operador com duas folhas\n");
    printf("  termoP = (2 * erro)             <- operador com um número e uma subárvore\n");
    printf("  u      = (termoP + bias)        <- a raiz\n");
    Expr *u = operacao('+',
                       operacao('*', numero(2.0),
                                operacao('-', variavel(VAR_SP), variavel(VAR_PV))),
                       variavel(VAR_BIAS));
    if (u == NULL) {
        fprintf(stderr, "sem memória\n");
        return 1;
    }
    printf("\n  Árvore deitada (raiz à esquerda; gire a cabeça para a esquerda):\n\n");
    desenhar(u, 1);
    printf("\n  Em pé:            [+]\n");
    printf("                  /     \\\n");
    printf("               [*]      bias\n");
    printf("              /   \\\n");
    printf("             2    [-]\n");
    printf("                 /   \\\n");
    printf("               sp     pv\n");
    pausar();

    /* PASSO 2 ------------------------------------------------------------ */
    printf("\n== PASSO 2 — Medidas recursivas ==\n");
    printf("  nós    = 1 + nós(esq) + nós(dir)            = %d\n", nos(u));
    printf("  altura = 1 + max(altura(esq), altura(dir))  = %d\n", altura(u));
    printf("  infixa  : ");
    imprimir_infixa(u);
    printf("\n  pós-fixa: ");
    imprimir_posfixa(u);
    printf("  <- a ordem em que uma pilha avaliaria\n");
    pausar();

    /* PASSO 3 ------------------------------------------------------------ */
    printf("\n== PASSO 3 — Avaliação recursiva com sp = 60 %%, pv = 45 %%, bias = 10 ==\n");
    double amb[NUM_VARS] = { 0 };
    amb[VAR_SP] = 60.0;
    amb[VAR_PV] = 45.0;
    amb[VAR_BIAS] = 10.0;
    double saida = avaliar(u, amb, 1, 1);
    printf("  u = %g %%  (abertura da válvula de entrada)\n", saida);
    pausar();

    /* PASSO 4 ------------------------------------------------------------ */
    printf("\n== PASSO 4 — A mesma árvore avaliada a cada ciclo de uma malha de nível ==\n");
    printf("  Modelo simplificado do tanque: pv(k+1) = pv(k) + 0,2·u − 0,2·25 (vazão de saída).\n");
    printf("  Controlador P puro: o nível estabiliza ABAIXO do setpoint (erro de regime).\n\n");
    printf("  ciclo     pv      u    nível (S = setpoint 60 %%)\n");
    amb[VAR_PV] = 20.0;
    for (int k = 0; k <= 12; k++) {
        double uk = avaliar(u, amb, 0, 0);
        if (uk < 0.0) uk = 0.0;           /* válvula satura em 0..100 % */
        if (uk > 100.0) uk = 100.0;
        printf("  %5d  %5.1f  %5.1f   ", k, amb[VAR_PV], uk);
        barra(amb[VAR_PV], amb[VAR_SP]);
        printf("\n");
        amb[VAR_PV] += 0.2 * uk - 0.2 * 25.0;
    }
    printf("\n  Mudar a lei de controle = montar outra árvore; o avaliador não muda.\n");
    printf("  Custo por ciclo: O(n) com n = %d nós.\n", nos(u));
    pausar();

    liberar(u);
    printf("\nFim da demonstração (árvore liberada em pós-ordem).\n");
    return 0;
}
