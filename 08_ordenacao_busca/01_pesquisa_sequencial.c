/*
 * ============================================================================
 * Arquivo : 01_pesquisa_sequencial.c
 * Módulo  : 08 — Ordenação e Busca            (Ementa: Unidade 4.1)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Ver a pesquisa sequencial (linear) andando elemento por elemento, comparar
 *   a versão simples com a versão com SENTINELA e comprovar, contando, que o
 *   custo médio de uma busca bem-sucedida é (n + 1) / 2 ≈ n/2 comparações e o
 *   de uma busca malsucedida é n.
 *
 * EXEMPLO REAL
 *   Um gateway Modbus guarda a lista de tags configuradas pelo integrador na
 *   ordem em que foram cadastradas (NÃO ordenada). Para atender a um pedido
 *   "qual o endereço do registrador da tag TT-101?", o firmware só pode varrer
 *   a lista do início ao fim. O mesmo vale para procurar um contato no
 *   histórico de chamadas do celular ou um nome numa lista de presença.
 *
 * COMPLEXIDADE (n = número de elementos)
 *   Operação                Melhor   Médio        Pior   Espaço
 *   busca simples           O(1)     O(n) ≈ n/2   O(n)   O(1)
 *   busca com sentinela     O(1)     O(n) ≈ n/2   O(n)   O(1) (+1 posição)
 *   Por quê: no melhor caso a chave está na posição 0; no pior ela está no
 *   fim ou não existe (n comparações). Se cada posição é igualmente provável,
 *   a média é (1 + 2 + ... + n) / n = (n + 1) / 2. A sentinela NÃO muda a
 *   ordem de grandeza: ela só elimina o teste "i < n" de cada volta do laço
 *   (metade dos testes), o que importa em microcontroladores e laços internos.
 *   Vantagem da pesquisa sequencial: funciona em dados NÃO ordenados e em
 *   listas encadeadas (sem acesso direto ao meio).
 *
 * COMO EXECUTAR
 *   ./run 01_pesquisa_sequencial               (ENTER avança cada passo)
 *   ./run 01_pesquisa_sequencial < /dev/null   (executa direto, sem pausas)
 * ============================================================================
 */
#define _POSIX_C_SOURCE 200809L   /* antes de qualquer #include: libera isatty() em -std=c11 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

/* Contadores separados: comparar a CHAVE é o trabalho útil; testar o LIMITE
 * (i < n) é o "pedágio" que a sentinela elimina. */
typedef struct {
    long chave;    /* comparações v[i] == chave */
    long limite;   /* testes i < n              */
} Contador;

/* ------------------------------------------------------------------------- */
/* Desenho                                                                    */
/* ------------------------------------------------------------------------- */

/* Desenha v[0..n-1] em células de 5 colunas; a posição `atual` recebe ^i.
 * As posições já examinadas (< atual) aparecem entre parênteses para mostrar
 * a parte do vetor já descartada. O(n). */
static void desenha_vetor(const int v[], int n, int atual) {
    printf("      idx: ");
    for (int k = 0; k < n; k++) printf("%5d", k);
    printf("\n      val: ");
    for (int k = 0; k < n; k++) {
        char cel[16];
        if (k < atual) snprintf(cel, sizeof cel, "(%d)", v[k]);
        else           snprintf(cel, sizeof cel, "%d", v[k]);
        printf("%5s", cel);
    }
    printf("\n           ");
    for (int k = 0; k < n; k++) printf("%5s", k == atual ? "^i" : "");
    printf("\n");
}

/* ------------------------------------------------------------------------- */
/* Algoritmos                                                                 */
/* ------------------------------------------------------------------------- */

/* Pesquisa sequencial simples. Devolve o índice da 1ª ocorrência ou -1.
 * Cada volta faz DOIS testes: i < n (não sair do vetor) e v[i] == chave.
 * Se `mostrar` for 1, desenha cada passo. O(n). */
static int busca_simples(const int v[], int n, int chave, Contador *c, int mostrar) {
    for (int i = 0; ; i++) {
        c->limite++;
        if (!(i < n)) break;                 /* passou do fim: não achou */
        c->chave++;
        if (mostrar) {
            desenha_vetor(v, n, i);
            printf("      passo %d: compara v[%d] = %d com %d -> %s\n", i + 1, i, v[i], chave,
                   v[i] == chave ? "ACHOU" : "diferente, avança");
            pausar();
        }
        if (v[i] == chave) return i;
    }
    if (mostrar) printf("      i chegou a n = %d: chave %d NÃO está no vetor\n", n, chave);
    return -1;
}

/* Pesquisa sequencial com sentinela.
 * Pré-condição: o vetor tem CAPACIDADE para n + 1 elementos (v[n] é livre).
 * Ideia: copiar a chave para v[n]. Agora o laço SEMPRE para (no pior caso na
 * sentinela), então o teste i < n some de dentro do laço; só testamos uma vez,
 * no fim, se paramos num elemento real ou na sentinela.
 * Armadilha: esquecer a posição extra → escrita fora do vetor (o sanitizer
 * acusa). Também não funciona em vetor `const` (precisa escrever em v[n]). O(n). */
static int busca_sentinela(int v[], int n, int chave, Contador *c, int mostrar) {
    v[n] = chave;                            /* planta a sentinela */
    int i = 0;
    while (1) {
        c->chave++;
        if (mostrar) {
            desenha_vetor(v, n + 1, i);
            printf("      passo %d: compara v[%d] = %d com %d -> %s\n", i + 1, i, v[i], chave,
                   v[i] == chave ? (i < n ? "ACHOU" : "parou na SENTINELA") : "diferente, avança");
            pausar();
        }
        if (v[i] == chave) break;
        i++;
    }
    c->limite++;                             /* um único teste de limite, fora do laço */
    return i < n ? i : -1;
}

/* Embaralha v com Fisher–Yates usando um gerador próprio (xorshift32), para
 * que a saída seja IGUAL em qualquer sistema (rand() muda entre bibliotecas). O(n). */
static unsigned estado_rng = 2026u;
static unsigned proximo_rng(void) {
    estado_rng ^= estado_rng << 13;
    estado_rng ^= estado_rng >> 17;
    estado_rng ^= estado_rng << 5;
    return estado_rng;
}
static void embaralha(int v[], int n) {
    for (int i = n - 1; i > 0; i--) {
        int j = (int)(proximo_rng() % (unsigned)(i + 1));
        int t = v[i]; v[i] = v[j]; v[j] = t;
    }
}

/* ------------------------------------------------------------------------- */
/* Exemplo real: tabela de tags Modbus (não ordenada)                         */
/* ------------------------------------------------------------------------- */

typedef struct {
    const char *tag;        /* nome no padrão ISA: TT = transmissor de temperatura... */
    int registrador;        /* endereço Modbus (4xxxx = holding register)            */
    const char *descricao;
} TagModbus;

/* Lista na ordem de CADASTRO (como o integrador digitou), sem ordenação. */
static const TagModbus tags[] = {
    {"PT-201", 40011, "pressão linha de vapor (bar)"},
    {"TT-101", 40001, "temperatura reator 1 (°C)"},
    {"FT-305", 40021, "vazão água de resfriamento (m³/h)"},
    {"LT-110", 40005, "nível tanque pulmão (%)"},
    {"XV-402", 40031, "válvula de bloqueio (aberta/fechada)"},
    {"TT-102", 40002, "temperatura reator 2 (°C)"},
    {"M-501",  40041, "motor bomba P-501 (rpm)"},
    {"AT-220", 40015, "analisador de pH"},
};
static const int n_tags = (int)(sizeof tags / sizeof tags[0]);

/* Procura a tag pelo nome; mostra cada comparação de string. O(n · L), onde
 * L é o tamanho da tag (strcmp compara caractere a caractere). */
static int busca_tag(const char *nome, int mostrar, long *comparacoes) {
    for (int i = 0; i < n_tags; i++) {
        (*comparacoes)++;
        int igual = strcmp(tags[i].tag, nome) == 0;
        if (mostrar)
            printf("      [%d] strcmp(\"%s\", \"%s\") -> %s\n", i, tags[i].tag, nome,
                   igual ? "IGUAL" : "diferente");
        if (igual) return i;
    }
    return -1;
}

/* ------------------------------------------------------------------------- */

static void demo_passo_a_passo(void) {
    printf("=== 1) Pesquisa sequencial simples, passo a passo ===\n\n");
    int v[9] = {42, 17, 8, 93, 25, 61, 4, 77};   /* 8 elementos + 1 posição livre */
    int n = 8;
    Contador c = {0, 0};

    printf("   Procurando 25 (existe):\n");
    int pos = busca_simples(v, n, 25, &c, 1);
    printf("   -> índice %d | comparações de chave = %ld | testes i<n = %ld\n\n", pos, c.chave, c.limite);
    pausar();

    c = (Contador){0, 0};
    printf("   Procurando 50 (NÃO existe) - precisa olhar TODOS os n elementos:\n");
    pos = busca_simples(v, n, 50, &c, 1);
    printf("   -> índice %d | comparações de chave = %ld | testes i<n = %ld (n + 1)\n\n", pos, c.chave, c.limite);
    pausar();

    printf("=== 2) Mesma busca malsucedida COM SENTINELA (v[8] recebe a chave) ===\n\n");
    c = (Contador){0, 0};
    pos = busca_sentinela(v, n, 50, &c, 1);
    printf("   -> índice %d | comparações de chave = %ld | testes de limite = %ld (só 1!)\n\n",
           pos, c.chave, c.limite);
    pausar();
}

/* Mede o custo médio: procura CADA elemento uma vez (busca bem-sucedida) e
 * uma chave ausente (malsucedida). Compara com a fórmula (n + 1) / 2. */
static void demo_custo_medio(void) {
    printf("=== 3) Custo médio medido x fórmula (n + 1)/2 ===\n");
    printf("   Para cada n, o vetor é embaralhado e cada um dos n valores é procurado uma vez.\n\n");
    printf("   %8s | %14s | %10s | %12s | %16s | %16s\n", "n", "média medida", "(n+1)/2", "ausente",
           "testes (simples)", "testes (sentin.)");
    printf("   ---------+----------------+------------+--------------+------------------+-----------------\n");
    const int tamanhos[] = {10, 100, 1000, 10000};
    for (int t = 0; t < 4; t++) {
        int n = tamanhos[t];
        int *v = malloc((size_t)(n + 1) * sizeof *v);   /* +1 para a sentinela */
        if (v == NULL) { printf("   sem memória\n"); return; }
        for (int i = 0; i < n; i++) v[i] = i * 3 + 1;
        embaralha(v, n);

        Contador simples = {0, 0}, sentinela = {0, 0};
        for (int k = 0; k < n; k++) {
            int chave = k * 3 + 1;
            busca_simples(v, n, chave, &simples, 0);
            busca_sentinela(v, n, chave, &sentinela, 0);
        }
        Contador ausente = {0, 0};
        busca_simples(v, n, -5, &ausente, 0);

        printf("   %8d | %14.1f | %10.1f | %12ld | %16ld | %16ld\n", n, (double)simples.chave / n,
               (n + 1) / 2.0, ausente.chave, simples.chave + simples.limite,
               sentinela.chave + sentinela.limite);
        free(v);
    }
    printf("\n   Leitura: a média medida bate com (n+1)/2 -> custo LINEAR: 10x mais dados, 10x mais\n");
    printf("   trabalho. A sentinela corta quase metade dos testes (2n -> n+1), mas continua O(n).\n");
    printf("   Para cair para O(log n) é preciso ORDENAR e usar pesquisa binária (02_pesquisa_binaria).\n\n");
    pausar();
}

static void demo_modbus(void) {
    printf("=== 4) Exemplo real: localizar uma tag na tabela Modbus (não ordenada) ===\n\n");
    printf("   %-3s %-8s %-6s %s\n", "#", "tag", "reg.", "descrição");
    for (int i = 0; i < n_tags; i++)
        printf("   %-3d %-8s %-6d %s\n", i, tags[i].tag, tags[i].registrador, tags[i].descricao);
    printf("\n   Pedido do SCADA: ler a tag \"XV-402\"\n");
    long comp = 0;
    int i = busca_tag("XV-402", 1, &comp);
    if (i >= 0)
        printf("   -> encontrada em %ld comparações: registrador %d (%s)\n\n", comp,
               tags[i].registrador, tags[i].descricao);
    pausar();

    comp = 0;
    printf("   Pedido do SCADA: ler a tag \"TT-999\" (erro de digitação na IHM)\n");
    i = busca_tag("TT-999", 0, &comp);
    printf("   -> %s após %ld comparações (varreu tudo: pior caso)\n\n",
           i < 0 ? "Modbus exception 02 (endereço inválido)" : "encontrada", comp);
    pausar();
}

/* Exploração livre: o aluno digita tags; ENTER vazio ou EOF encerra. */
static void exploracao_livre(void) {
    char linha[64];
    printf("=== Exploração: digite uma tag (ex.: LT-110). ENTER vazio ou Ctrl+D encerra ===\n");
    while (1) {
        printf("   tag> ");
        fflush(stdout);
        if (fgets(linha, sizeof linha, stdin) == NULL) { printf("\n"); break; }   /* EOF */
        linha[strcspn(linha, "\r\n")] = '\0';
        if (linha[0] == '\0') break;
        long comp = 0;
        int i = busca_tag(linha, 1, &comp);
        if (i >= 0) printf("   -> registrador %d, %ld comparações\n", tags[i].registrador, comp);
        else        printf("   -> não encontrada (%ld comparações = n)\n", comp);
    }
}

int main(void) {
    modo_interativo = isatty(STDIN_FILENO);
    printf("PESQUISA SEQUENCIAL (LINEAR)\n\n");
    demo_passo_a_passo();
    demo_custo_medio();
    demo_modbus();
    exploracao_livre();
    return 0;
}
