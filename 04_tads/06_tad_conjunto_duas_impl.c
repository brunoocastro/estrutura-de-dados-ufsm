/*
 * ============================================================================
 * Arquivo : 06_tad_conjunto_duas_impl.c
 * Módulo  : 04 — Tipos Abstratos de Dados       (Ementa: Unidade 1.2)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Ver na prática a frase "um TAD separa O QUE faz de COMO faz". A mesma
 *   interface Conjunto (inserir, contem, remover, minimo) recebe DUAS
 *   implementações:
 *     A) vetor NÃO ordenado: insere no fim; busca olhando um por um;
 *     B) vetor ORDENADO    : busca binária; insere/remove deslocando elementos.
 *   O código do usuário é escrito UMA vez e roda com as duas. Contadores de
 *   comparações e movimentos mostram o preço de cada escolha, passo a passo
 *   com n pequeno e numa tabela de crescimento até n = 10000.
 *
 * EXEMPLO REAL
 *   Lista de crachás RFID autorizados numa catraca/portaria de fábrica, ou
 *   lista de endereços de escravos Modbus/Profinet ativos num gateway:
 *   consultas a todo momento ("este crachá pode entrar?"), cadastros raros.
 *   Quem consulta muito ganha com B; quem insere muito e quase não consulta
 *   pode preferir A.
 *
 * COMPLEXIDADE (n = elementos no conjunto)
 *   Operação     A: não ordenado          B: ordenado
 *   contem       O(n) comparações         O(log n) comparações
 *   inserir      O(n) (checa repetido)    O(log n) comp. + O(n) movimentos
 *                + O(1) movimento
 *   remover      O(n) comp. + O(1) mov.   O(log n) comp. + O(n) movimentos
 *                (troca com o último)
 *   minimo       O(n)                     O(1) (primeira posição)
 *   listar em ordem  O(n log n) (ordenar) O(n)
 *   Espaço: O(n) nas duas (vetor dinâmico que dobra de capacidade).
 *
 * COMO EXECUTAR
 *   ./run 06_tad_conjunto_duas_impl               (ENTER avança cada passo)
 *   ./run 06_tad_conjunto_duas_impl < /dev/null   (executa direto, sem pausas)
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
 * INTERFACE  (conjunto.h)
 *   Em C "puro" a troca de implementação costuma ser feita no link
 *   (compila conjunto_vetor.c OU conjunto_ordenado.c). Como aqui tudo está em
 *   um arquivo, a interface vira uma tabela de ponteiros para função: o
 *   usuário chama impl->contem(...) sem saber qual código vai rodar.
 * ########################################################################## */

typedef struct {
    int *dados;
    size_t n, cap;
    unsigned long comparacoes;   /* instrumentação didática */
    unsigned long movimentos;    /* escritas de elementos no vetor */
} Conjunto;

typedef struct {
    const char *nome;
    int  (*inserir)(Conjunto *c, int x);        /* 1 se inseriu, 0 se já existia/sem memória */
    int  (*contem)(Conjunto *c, int x);
    int  (*remover)(Conjunto *c, int x);        /* 1 se removeu */
    int  (*minimo)(Conjunto *c, int *saida);    /* 0 se vazio */
} ImplConjunto;

void conjunto_iniciar(Conjunto *c);
void conjunto_liberar(Conjunto *c);
/* Escreve "{ 3 7 [19] }" em buf, marcando 'destaque' com []. O(n) */
void conjunto_formatar(const Conjunto *c, int destaque, char *buf, size_t cap);

/* ##########################################################################
 * IMPLEMENTAÇÃO COMUM
 * ########################################################################## */

void conjunto_iniciar(Conjunto *c) {
    c->dados = NULL;
    c->n = c->cap = 0;
    c->comparacoes = c->movimentos = 0;
}

void conjunto_liberar(Conjunto *c) {
    free(c->dados);
    conjunto_iniciar(c);
}

/* Garante espaço para mais 1 elemento. Dobrar a capacidade torna o custo
 * AMORTIZADO de cada inserção O(1) (soma 1 + 2 + 4 + ... + n < 2n cópias). */
static int garantir_espaco(Conjunto *c) {
    if (c->n < c->cap) return 1;
    size_t nova = c->cap ? c->cap * 2 : 4;
    int *p = realloc(c->dados, nova * sizeof *p);   /* temporário: se falhar, não perde dados */
    if (p == NULL) return 0;
    c->dados = p;
    c->cap = nova;
    return 1;
}

void conjunto_formatar(const Conjunto *c, int destaque, char *buf, size_t cap) {
    size_t usado = 0;
    int k = snprintf(buf, cap, "{");
    if (k > 0) usado = (size_t)k;
    for (size_t i = 0; i < c->n && usado < cap; i++) {
        k = snprintf(buf + usado, cap - usado, c->dados[i] == destaque ? " [%d]" : " %d",
                     c->dados[i]);
        if (k > 0) usado += (size_t)k;
    }
    if (usado < cap) snprintf(buf + usado, cap - usado, " }");
}

/* ##########################################################################
 * IMPLEMENTAÇÃO A — vetor NÃO ordenado
 * ########################################################################## */

/* Busca sequencial: devolve o índice ou -1. O(n) */
static long a_indice(Conjunto *c, int x) {
    for (size_t i = 0; i < c->n; i++) {
        c->comparacoes++;
        if (c->dados[i] == x) return (long)i;
    }
    return -1;
}

static int a_contem(Conjunto *c, int x) { return a_indice(c, x) >= 0; }

static int a_inserir(Conjunto *c, int x) {
    if (a_indice(c, x) >= 0) return 0;       /* conjunto não tem repetidos: O(n) */
    if (!garantir_espaco(c)) return 0;
    c->dados[c->n++] = x;                    /* no fim: O(1) */
    c->movimentos++;
    return 1;
}

/* Truque da ordem livre: põe o último no buraco em vez de deslocar todos. */
static int a_remover(Conjunto *c, int x) {
    long i = a_indice(c, x);
    if (i < 0) return 0;
    c->dados[i] = c->dados[--c->n];
    c->movimentos++;
    return 1;
}

static int a_minimo(Conjunto *c, int *saida) {   /* O(n): precisa olhar todos */
    if (c->n == 0) return 0;
    int m = c->dados[0];
    for (size_t i = 1; i < c->n; i++) {
        c->comparacoes++;
        if (c->dados[i] < m) m = c->dados[i];
    }
    *saida = m;
    return 1;
}

static const ImplConjunto IMPL_NAO_ORDENADO = { "A: vetor não ordenado", a_inserir, a_contem,
                                                a_remover, a_minimo };

/* ##########################################################################
 * IMPLEMENTAÇÃO B — vetor ORDENADO (invariante: dados[0] < dados[1] < ...)
 * ########################################################################## */

/* Busca binária: devolve a posição onde x está ou onde DEVERIA entrar.
 * *achou = 1 se x está lá. O(log n): a faixa [ini, fim) cai à metade por volta. */
static size_t b_posicao(Conjunto *c, int x, int *achou) {
    size_t ini = 0, fim = c->n;
    *achou = 0;
    while (ini < fim) {
        size_t meio = ini + (fim - ini) / 2;   /* evita overflow de (ini+fim)/2 */
        c->comparacoes++;
        if (c->dados[meio] == x) { *achou = 1; return meio; }
        if (c->dados[meio] < x) ini = meio + 1;
        else                    fim = meio;
    }
    return ini;
}

static int b_contem(Conjunto *c, int x) {
    int achou;
    b_posicao(c, x, &achou);
    return achou;
}

static int b_inserir(Conjunto *c, int x) {
    int achou;
    size_t p = b_posicao(c, x, &achou);
    if (achou || !garantir_espaco(c)) return 0;
    for (size_t i = c->n; i > p; i--) {       /* abre espaço: desloca à direita, O(n) */
        c->dados[i] = c->dados[i - 1];
        c->movimentos++;
    }
    c->dados[p] = x;
    c->movimentos++;
    c->n++;
    return 1;
}

static int b_remover(Conjunto *c, int x) {
    int achou;
    size_t p = b_posicao(c, x, &achou);
    if (!achou) return 0;
    for (size_t i = p; i + 1 < c->n; i++) {   /* fecha o buraco mantendo a ordem, O(n) */
        c->dados[i] = c->dados[i + 1];
        c->movimentos++;
    }
    c->n--;
    return 1;
}

static int b_minimo(Conjunto *c, int *saida) {   /* O(1): a invariante já ordenou */
    if (c->n == 0) return 0;
    *saida = c->dados[0];
    return 1;
}

static const ImplConjunto IMPL_ORDENADO = { "B: vetor ordenado", b_inserir, b_contem, b_remover,
                                            b_minimo };

/* ##########################################################################
 * USUÁRIO DO TAD — escrito uma vez, roda com qualquer implementação
 * ########################################################################## */

/* Executa uma operação pela interface e mostra o estado e o custo DAQUELE passo.
 * 'op' escolhe a função da tabela; o usuário não sabe qual implementação roda. */
static void passo(const ImplConjunto *impl, Conjunto *c, char op, int x) {
    unsigned long c0 = c->comparacoes, m0 = c->movimentos;
    const char *nome = "?";
    int r = 0;
    switch (op) {
    case 'i': nome = "inserir"; r = impl->inserir(c, x); break;
    case 'c': nome = "contem";  r = impl->contem(c, x);  break;
    case 'r': nome = "remover"; r = impl->remover(c, x); break;
    case 'm': nome = "minimo";  r = impl->minimo(c, &x); break;
    }
    char estado[96];
    conjunto_formatar(c, x, estado, sizeof estado);
    printf("  %-8s %3d  %-3s  %-30s %6lu %6lu\n", nome, x, r ? "sim" : "---", estado,
           c->comparacoes - c0, c->movimentos - m0);
}

static void roteiro_passo_a_passo(const ImplConjunto *impl) {
    Conjunto c;
    conjunto_iniciar(&c);
    printf("\n== %s ==\n", impl->nome);
    printf("  %-8s %3s  %-3s  %-30s %6s %6s\n", "operação", "x", "ok", "estado", "comp.", "mov.");
    const char ops[] = { 'i', 'i', 'i', 'i', 'i', 'i', 'c', 'c', 'r', 'm' };
    const int  xs[]  = { 42,  7,   19,  3,   25,  7,   25,  50,  7,   0 };
    for (size_t i = 0; i < sizeof ops; i++) {
        passo(impl, &c, ops[i], xs[i]);
        pausar();
    }
    printf("  TOTAL: %lu comparações, %lu movimentos\n", c.comparacoes, c.movimentos);
    conjunto_liberar(&c);
    pausar();
}

/* Gerador pseudoaleatório determinístico (LCG): mesma sequência sempre. */
static unsigned long semente = 12345;
static unsigned long aleatorio(void) {
    semente = semente * 1103515245UL + 12345UL;
    return (semente / 65536UL) % 32768UL;
}

/* Cenário "catraca": cadastra n crachás distintos e faz n consultas. */
static void medir(const ImplConjunto *impl, int n, unsigned long *ins_c, unsigned long *ins_m,
                  unsigned long *cons_c) {
    Conjunto c;
    conjunto_iniciar(&c);
    int *ids = malloc((size_t)n * sizeof *ids);
    if (ids == NULL) { *ins_c = *ins_m = *cons_c = 0; return; }
    for (int i = 0; i < n; i++) ids[i] = 1000 + 7 * i;            /* distintos */
    semente = 2024;
    for (int i = n - 1; i > 0; i--) {                             /* embaralha (Fisher-Yates) */
        unsigned long r = aleatorio() * 32768UL;   /* duas chamadas em comandos separados: */
        r += aleatorio();                           /* ordem de avaliação bem definida */
        int j = (int)(r % (unsigned long)(i + 1));
        int t = ids[i]; ids[i] = ids[j]; ids[j] = t;
    }
    for (int i = 0; i < n; i++) impl->inserir(&c, ids[i]);
    *ins_c = c.comparacoes;
    *ins_m = c.movimentos;
    c.comparacoes = 0;
    for (int i = 0; i < n; i++) impl->contem(&c, ids[(i * 31) % n]);  /* todas existem */
    *cons_c = c.comparacoes;
    free(ids);
    conjunto_liberar(&c);
}

static void tabela_crescimento(void) {
    printf("\n== PASSO 3 — Crescimento: cadastrar n crachás e depois fazer n consultas ==\n");
    printf("  %6s | %-31s | %-31s\n", "", IMPL_NAO_ORDENADO.nome, IMPL_ORDENADO.nome);
    printf("  %6s | %10s %10s %9s | %10s %10s %9s\n", "n", "ins.comp", "ins.mov", "cons.comp",
           "ins.comp", "ins.mov", "cons.comp");
    for (int n = 10; n <= 10000; n *= 10) {
        unsigned long ac, am, aq, bc, bm, bq;
        medir(&IMPL_NAO_ORDENADO, n, &ac, &am, &aq);
        medir(&IMPL_ORDENADO, n, &bc, &bm, &bq);
        printf("  %6d | %10lu %10lu %9lu | %10lu %10lu %9lu\n", n, ac, am, aq, bc, bm, bq);
    }
    printf("\n  Leitura da tabela:\n");
    printf("  - consultas: A cresce ~n²/2 (n consultas × n/2), B cresce ~n·log2(n);\n");
    printf("    com n = 10000, B faz centenas de vezes menos comparações.\n");
    printf("  - inserções: A paga em COMPARAÇÕES (checar repetido), B paga em MOVIMENTOS\n");
    printf("    (deslocar ~n/2 elementos); as duas são O(n) por inserção.\n");
    printf("  - Muitas consultas, poucas inserções (catraca) -> B. Se a regra de não repetir\n");
    printf("    sumisse (lista de eventos), A inseriria em O(1) e seria a melhor.\n");
    pausar();
}

int main(void) {
    modo_interativo = isatty(STDIN_FILENO);
    printf("TAD CONJUNTO: MESMA INTERFACE, DUAS IMPLEMENTAÇÕES\n");
    printf("\n== PASSO 1 e 2 — O mesmo roteiro executado pelas duas implementações ==\n");
    printf("  (inserir 42, 7, 19, 3, 25 e 7 de novo; consultar 25 e 50; remover 7; mínimo)\n");
    printf("  [x] marca o elemento envolvido; comp./mov. = custo daquele passo.\n");
    pausar();
    roteiro_passo_a_passo(&IMPL_NAO_ORDENADO);
    roteiro_passo_a_passo(&IMPL_ORDENADO);
    tabela_crescimento();
    printf("\nFim da demonstração.\n");
    return 0;
}
