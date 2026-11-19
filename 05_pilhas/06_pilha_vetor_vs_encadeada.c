/*
 * ============================================================================
 * Arquivo : 06_pilha_vetor_vs_encadeada.c
 * Módulo  : 05 — Pilhas                        (Ementa: Unidade 2.2)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Ver, lado a lado e passo a passo, as DUAS implementações de pilha
 *   recebendo as mesmas operações:
 *     - VETOR (estática): capacidade fixa 4; o 5º push dá OVERFLOW.
 *     - ENCADEADA (dinâmica): cada push faz um malloc; não há limite fixo.
 *   As duas fazem push/pop em O(1): o vetor mexe só em dados[topo]; a
 *   encadeada mexe só no primeiro nó. Na parte 2, uma tabela com contagens
 *   reais para n = 10 ... 10000 compara com duas alternativas:
 *     - vetor que DOBRA de tamanho quando enche (resolve o overflow; O(1)
 *       amortizado);
 *     - versão INGÊNUA com o topo na posição 0 do vetor: cada push/pop
 *       desloca todos os elementos, O(n) por operação e O(n²) no total.
 *
 * EXEMPLO REAL
 *   Firmware de CLP/microcontrolador costuma usar a versão em vetor (memória
 *   previsível, sem malloc); aplicações de PC/servidor (SCADA, histórico de
 *   IHM) usam a encadeada ou o vetor que dobra, porque não sabem o tamanho
 *   máximo de antemão.
 *
 * COMPLEXIDADE (n = elementos)
 *   Operação         Vetor fixo   Vetor dobra       Encadeada    Topo no índice 0
 *   push             O(1)         O(1) amortizado   O(1)         O(n)
 *   pop / peek       O(1)         O(1)              O(1)         O(n) / O(1)
 *   n push + n pop   O(n)         O(n)              O(n)         O(n²)
 *   memória          C posições   até 2n posições   n nós (valor + ponteiro)
 *   Por quê: "O(1) amortizado" = de vez em quando um push custa O(n) (a
 *   cópia ao dobrar), mas como o tamanho dobra, as cópias somadas ficam
 *   abaixo de 2n; divididas por n pushes, dão custo constante por operação.
 *
 * COMO EXECUTAR
 *   ./run 06_pilha_vetor_vs_encadeada               (ENTER avança cada passo)
 *   ./run 06_pilha_vetor_vs_encadeada < /dev/null   (executa direto)
 * ============================================================================
 */
#define _POSIX_C_SOURCE 200809L /* antes de qualquer #include: libera isatty() em -std=c11 */
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
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
/* Implementação 1: vetor de capacidade fixa                                 */
/* ------------------------------------------------------------------------ */
#define CAP_VETOR 4

typedef struct {
    int dados[CAP_VETOR];
    int n; /* quantidade = próxima posição livre; topo está em dados[n - 1] */
} PilhaVetor;

/* O(1). Retorna 0 em overflow. */
static int vetor_push(PilhaVetor *p, int v) {
    if (p->n == CAP_VETOR) return 0;
    p->dados[p->n++] = v;
    return 1;
}

/* O(1). Retorna 0 em underflow. */
static int vetor_pop(PilhaVetor *p, int *v) {
    if (p->n == 0) return 0;
    *v = p->dados[--p->n];
    return 1;
}

/* ------------------------------------------------------------------------ */
/* Implementação 2: lista encadeada (topo = primeiro nó)                     */
/* ------------------------------------------------------------------------ */
typedef struct No {
    int valor;
    struct No *prox;
} No;

typedef struct {
    No *topo;
    int n;
    long mallocs, frees; /* para conferir que todo malloc teve seu free */
} PilhaEnc;

/* O(1): aloca, aponta para o topo antigo, vira o topo. 0 se malloc falhar. */
static int enc_push(PilhaEnc *p, int v) {
    No *novo = malloc(sizeof *novo);
    if (novo == NULL) return 0;
    p->mallocs++;
    novo->valor = v;
    novo->prox = p->topo;
    p->topo = novo;
    p->n++;
    return 1;
}

/* O(1): desliga o primeiro nó ANTES de liberá-lo. 0 em underflow. */
static int enc_pop(PilhaEnc *p, int *v) {
    if (p->topo == NULL) return 0;
    No *velho = p->topo;
    *v = velho->valor;
    p->topo = velho->prox;
    free(velho);
    p->frees++;
    p->n--;
    return 1;
}

/* O(n): libera o que sobrou. */
static void enc_libera(PilhaEnc *p) {
    int lixo;
    while (enc_pop(p, &lixo)) { }
}

/* ------------------------------------------------------------------------ */
/* Desenho lado a lado: cada implementação preenche uma "coluna" de linhas  */
/* ------------------------------------------------------------------------ */
#define MAX_LINHAS 24
#define LARG_LINHA 48

typedef struct {
    char linha[MAX_LINHAS][LARG_LINHA];
    int n;
} Coluna;

/* Acrescenta uma linha formatada (como printf) à coluna. */
static void col_add(Coluna *c, const char *fmt, ...) {
    if (c->n == MAX_LINHAS) return;
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(c->linha[c->n++], LARG_LINHA, fmt, ap);
    va_end(ap);
}

/* O(C): desenha o vetor inteiro, inclusive posições livres. */
static void desenha_vetor(Coluna *c, const PilhaVetor *p) {
    col_add(c, "VETOR  (cap = %d, n = %d)", CAP_VETOR, p->n);
    col_add(c, "     +------+");
    for (int i = CAP_VETOR - 1; i >= 0; i--) {
        if (i < p->n)
            col_add(c, " [%d] | %4d |%s", i, p->dados[i], i == p->n - 1 ? " <- topo" : "");
        else
            col_add(c, " [%d] |      |%s", i, "");
    }
    col_add(c, "     +------+");
    col_add(c, " capacidade fixa: %d", CAP_VETOR);
}

/* O(n): desenha os nós do topo para a base. */
static void desenha_encadeada(Coluna *c, const PilhaEnc *p) {
    col_add(c, "ENCADEADA  (n = %d, malloc = %ld, free = %ld)", p->n, p->mallocs, p->frees);
    if (p->topo == NULL) {
        col_add(c, "  topo --> NULL");
        return;
    }
    col_add(c, "  topo");
    col_add(c, "   |");
    col_add(c, "   v");
    for (const No *no = p->topo; no != NULL; no = no->prox) {
        if (no->prox != NULL) {
            col_add(c, "  [ %4d | * ]", no->valor);
            col_add(c, "   |");
            col_add(c, "   v");
        } else {
            col_add(c, "  [ %4d | / ] --> NULL", no->valor);
        }
    }
}

static void desenha_lado_a_lado(const PilhaVetor *pv, const PilhaEnc *pe) {
    Coluna a = {.n = 0}, b = {.n = 0};
    desenha_vetor(&a, pv);
    desenha_encadeada(&b, pe);
    int linhas = a.n > b.n ? a.n : b.n;
    for (int i = 0; i < linhas; i++)
        printf("  %-40s%s\n", i < a.n ? a.linha[i] : "", i < b.n ? b.linha[i] : "");
}

/* ------------------------------------------------------------------------ */
/* Parte 1: roteiro passo a passo                                            */
/* ------------------------------------------------------------------------ */
typedef struct {
    char op; /* 'u' = push, 'o' = pop, 'k' = peek */
    int valor;
} Operacao;

static void parte1_passo_a_passo(void) {
    PilhaVetor pv = {.n = 0};
    PilhaEnc pe = {NULL, 0, 0, 0};
    const Operacao roteiro[] = {
        {'u', 10}, {'u', 20}, {'u', 30}, {'u', 40}, {'u', 50}, {'k', 0},
        {'o', 0},  {'o', 0},  {'o', 0},  {'o', 0},  {'o', 0},  {'o', 0},
    };
    const int total = (int)(sizeof roteiro / sizeof roteiro[0]);
    int avisou = 0; /* já explicou a divergência entre as duas pilhas? */

    printf("PARTE 1 — as mesmas operações nas duas implementações\n\n");
    desenha_lado_a_lado(&pv, &pe);
    pausar();

    for (int k = 0; k < total; k++) {
        const Operacao *op = &roteiro[k];
        int v1 = 0, v2 = 0;
        printf("\n---- Passo %d de %d: ", k + 1, total);
        if (op->op == 'u') {
            printf("push(%d) ----\n", op->valor);
            int n_antes = pv.n;
            if (vetor_push(&pv, op->valor))
                printf("  vetor    : dados[%d] = %d; n %d -> %d   (1 escrita, nada é deslocado)\n",
                       n_antes, op->valor, n_antes, pv.n);
            else
                printf("  vetor    : OVERFLOW! as %d posições estão ocupadas; %d foi recusado\n",
                       CAP_VETOR, op->valor);
            if (enc_push(&pe, op->valor))
                printf("  encadeada: malloc(%zu B); novo->prox = topo; topo = novo  "
                       "(sem limite fixo)\n", sizeof(No));
            else
                printf("  encadeada: malloc devolveu NULL (memória esgotada)\n");
        } else if (op->op == 'o') {
            printf("pop() ----\n");
            if (vetor_pop(&pv, &v1))
                printf("  vetor    : n %d -> %d; devolve dados[%d] = %d\n", pv.n + 1, pv.n, pv.n,
                       v1);
            else
                printf("  vetor    : UNDERFLOW! pilha vazia\n");
            if (enc_pop(&pe, &v2))
                printf("  encadeada: topo = topo->prox; free(nó antigo); devolve %d\n", v2);
            else
                printf("  encadeada: UNDERFLOW! topo == NULL\n");
            if (v1 != v2 && !avisou) {
                printf("  ATENÇÃO: os valores diferem porque o vetor recusou o 50 no overflow;\n"
                       "           a encadeada guardou um elemento a mais.\n");
                avisou = 1;
            }
        } else {
            printf("peek() ----\n");
            if (pv.n > 0) printf("  vetor    : topo = dados[%d] = %d (não remove)\n", pv.n - 1,
                                 pv.dados[pv.n - 1]);
            if (pe.topo) printf("  encadeada: topo->valor = %d (não remove)\n", pe.topo->valor);
        }
        desenha_lado_a_lado(&pv, &pe);
        pausar();
    }
    printf("\n  Conferência de memória da encadeada: %ld malloc, %ld free -> %s\n",
           pe.mallocs, pe.frees, pe.mallocs == pe.frees ? "nada vazou" : "VAZAMENTO");
    enc_libera(&pe);
}

/* ------------------------------------------------------------------------ */
/* Parte 2: tabela de crescimento com contagens determinísticas              */
/* ------------------------------------------------------------------------ */

/* "Acesso" = ler ou escrever um elemento. Cada função faz n push seguidos
 * de n pop e devolve quantos acessos foram necessários. */

/* Vetor de capacidade n (topo no fim): exatamente 1 acesso por operação. */
static long custo_vetor_fixo(int n) {
    int *v = malloc((size_t)n * sizeof *v);
    if (v == NULL) return -1;
    long acessos = 0;
    int topo = 0, lixo = 0;
    for (int i = 0; i < n; i++) { v[topo++] = i; acessos++; }
    for (int i = 0; i < n; i++) { lixo += v[--topo]; acessos++; }
    free(v);
    (void)lixo; /* lixo só existe para os pops "usarem" o valor lido */
    return acessos;
}

/* Vetor que começa com 1 posição e DOBRA quando enche. A cópia é feita à mão
 * (em vez de realloc) para podermos contar cada elemento copiado. */
static long custo_vetor_dobra(int n, long *copias) {
    int cap = 1, topo = 0, lixo = 0;
    int *v = malloc(sizeof *v);
    if (v == NULL) return -1;
    long acessos = 0;
    *copias = 0;
    for (int i = 0; i < n; i++) {
        if (topo == cap) {
            int *maior = malloc((size_t)cap * 2 * sizeof *maior);
            if (maior == NULL) { free(v); return -1; }
            for (int j = 0; j < topo; j++) { maior[j] = v[j]; (*copias)++; }
            free(v);
            v = maior;
            cap *= 2;
        }
        v[topo++] = i;
        acessos++;
    }
    for (int i = 0; i < n; i++) { lixo += v[--topo]; acessos++; }
    free(v);
    (void)lixo;
    return acessos + *copias;
}

/* Encadeada: 1 acesso ao valor por operação, mais 1 malloc/free por nó. */
static long custo_encadeada(int n) {
    PilhaEnc p = {NULL, 0, 0, 0};
    long acessos = 0;
    int v;
    for (int i = 0; i < n; i++) { if (!enc_push(&p, i)) break; acessos++; }
    while (enc_pop(&p, &v)) acessos++;
    return acessos;
}

/* INGÊNUA: topo em v[0]. Push abre espaço deslocando todos para a direita;
 * pop fecha o buraco deslocando todos para a esquerda. */
static long custo_topo_no_inicio(int n) {
    int *v = malloc((size_t)n * sizeof *v);
    if (v == NULL) return -1;
    long acessos = 0;
    int qtd = 0, lixo = 0;
    for (int i = 0; i < n; i++) {
        for (int j = qtd; j > 0; j--) { v[j] = v[j - 1]; acessos++; } /* desloca */
        v[0] = i;
        acessos++;
        qtd++;
    }
    for (int i = 0; i < n; i++) {
        lixo += v[0];
        acessos++;
        for (int j = 0; j < qtd - 1; j++) { v[j] = v[j + 1]; acessos++; } /* desloca */
        qtd--;
    }
    free(v);
    (void)lixo;
    return acessos;
}

static void parte2_tabela(void) {
    printf("\n\nPARTE 2 — custo de n push seguidos de n pop (acessos a elementos)\n\n");
    printf("  %6s | %11s | %20s | %11s | %16s\n", "n", "vetor fixo", "vetor dobra (cópias)",
           "encadeada", "topo no índice 0");
    printf("  -------+-------------+----------------------+-------------+-----------------\n");
    const int ns[] = {10, 100, 1000, 10000};
    for (size_t k = 0; k < sizeof ns / sizeof ns[0]; k++) {
        int n = ns[k];
        long copias = 0;
        long dobra = custo_vetor_dobra(n, &copias);
        printf("  %6d | %11ld | %11ld (%6ld) | %11ld | %16ld\n", n, custo_vetor_fixo(n), dobra,
               copias, custo_encadeada(n), custo_topo_no_inicio(n));
    }
    printf("\n  Leitura da tabela:\n"
           "  - vetor fixo e encadeada: exatamente 2n -> O(1) por operação.\n"
           "  - vetor que dobra: 2n + cópias, e as cópias somam menos de 2n -> O(1) amortizado.\n"
           "  - topo no índice 0: cresce como n² (n x10 => custo x100) -> O(n) por operação.\n"
           "  A pilha é eficiente porque SEMPRE trabalha na ponta que não exige deslocar nada.\n"
           "  Memória para n = 10000: vetor %zu B; encadeada %zu B (+ controle do malloc).\n",
           10000 * sizeof(int), 10000 * sizeof(No));
}

int main(void) {
    modo_interativo = isatty(STDIN_FILENO);
    parte1_passo_a_passo();
    pausar();
    parte2_tabela();
    return 0;
}
