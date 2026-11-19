/*
 * ============================================================================
 * Arquivo : 05_erros_classicos_memoria.c
 * Módulo  : 03 — Revisão sistemática de C     (Ementa: 0.7 Alocação dinâmica)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Conhecer os erros clássicos de memória dinâmica SEM executar comportamento
 *   indefinido: cada erro é mostrado como código (texto), com a consequência
 *   e a correção, que é a parte executada. Um pequeno "rastreador" de
 *   alocações (como fazem Valgrind e AddressSanitizer) mostra vazamentos de
 *   verdade: ele lembra todo bloco não liberado e, no relatório final, aponta
 *   a linha que o alocou (e só então o libera, para o programa terminar limpo).
 *     1. vazamento (memory leak)          5. estouro de buffer (+1 do '\0')
 *     2. double free                      6. não testar o retorno de malloc
 *     3. ponteiro pendurado (dangling)    7. free em memória que não é do heap
 *     4. uso após free (use-after-free)
 *
 * EXEMPLO REAL
 *   Um servidor OPC/SCADA que vaza 100 bytes por requisição atende 10 por
 *   segundo: em um mês perdeu ~2,5 GB e cai "sem motivo". Uso após free e
 *   double free são porta de entrada para ataques e falhas intermitentes.
 *
 * COMPLEXIDADE (k = blocos vivos no rastreador)
 *   rastro_malloc: O(k) para achar uma posição livre na tabela;
 *   rastro_free:   O(k) para achar o bloco; relatório: O(k).
 *   (Ferramentas reais usam tabelas hash para O(1).)
 *
 * COMO EXECUTAR
 *   ./run 05_erros_classicos_memoria               (ENTER avança cada passo)
 *   ./run 05_erros_classicos_memoria < /dev/null   (executa direto, sem pausas)
 *   Dica: compile com -fsanitize=address para o compilador vigiar seus erros.
 * ============================================================================
 */
#define _POSIX_C_SOURCE 200809L   /* antes de qualquer #include: libera isatty() em -std=c11 */
#include <stdint.h>
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

/* ===================== Rastreador de alocações ===================== */
#define MAX_BLOCOS 32

typedef struct {
    void *ptr;          /* NULL = posição livre */
    size_t bytes;
    const char *rotulo;
    int linha;
} Bloco;

static Bloco tabela[MAX_BLOCOS];

/* O(k): aloca e anota quem pediu (rótulo e linha do código). */
static void *rastro_malloc(size_t bytes, const char *rotulo, int linha) {
    for (size_t i = 0; i < MAX_BLOCOS; i++) {
        if (tabela[i].ptr == NULL) {
            void *p = malloc(bytes);
            if (p == NULL) return NULL;
            tabela[i] = (Bloco){p, bytes, rotulo, linha};
            return p;
        }
    }
    return NULL; /* tabela cheia: tratado como falta de memória */
}

/* O(k): libera p se ele veio do rastro_malloc e devolve NULL; se p não for
 * conhecido, recusa e devolve o próprio p. Use pela macro RFREE(p), que
 * grava o retorno de volta em p: assim, depois do free, p vira NULL, e um
 * futuro double free vira free(NULL), que é inofensivo. */
static void *rastro_free(void *p) {
    if (p == NULL) {
        printf("      rastro_free: ponteiro ja e NULL -> nada a fazer (sem double free)\n");
        return NULL;
    }
    for (size_t i = 0; i < MAX_BLOCOS; i++) {
        if (tabela[i].ptr == p) {
            free(p);
            tabela[i].ptr = NULL;
            return NULL;
        }
    }
    printf("      rastro_free: endereco desconhecido (nao veio do rastro_malloc): recusado!\n");
    return p;
}

/* O(k): lista os blocos ainda vivos (vazamentos) e então os libera. */
static void relatorio_vazamentos(void) {
    size_t total = 0, blocos = 0;
    for (size_t i = 0; i < MAX_BLOCOS; i++) {
        if (tabela[i].ptr != NULL) {
            printf("      VAZOU: %zu bytes de \"%s\" alocados na linha %d\n",
                   tabela[i].bytes, tabela[i].rotulo, tabela[i].linha);
            total += tabela[i].bytes;
            blocos++;
            free(tabela[i].ptr);   /* o rastreador limpa para o programa sair limpo */
            tabela[i].ptr = NULL;
        }
    }
    printf("      => %zu bloco(s), %zu byte(s) que o programa perdeu de vista\n", blocos, total);
}

#define RMALLOC(bytes, rotulo) rastro_malloc((bytes), (rotulo), __LINE__)
#define RFREE(p) ((p) = rastro_free(p))

/* ===================== Cenários ===================== */

/* COM BUG: no caminho de erro, retorna sem liberar o buffer. */
static int processa_quadro_com_vazamento(int quadro_valido) {
    char *buf = RMALLOC(64, "buffer do quadro (versao com bug)");
    if (buf == NULL) return 0;
    if (!quadro_valido)
        return 0;                    /* <- BUG: esqueceu RFREE(buf) */
    RFREE(buf);
    return 1;
}

/* CORRIGIDO: um único ponto de saída que sempre libera. */
static int processa_quadro_correto(int quadro_valido) {
    char *buf = RMALLOC(64, "buffer do quadro (versao corrigida)");
    if (buf == NULL) return 0;
    int ok = quadro_valido;          /* ...processamento... */
    RFREE(buf);                      /* libera em TODOS os caminhos */
    return ok;
}

int main(void) {
    modo_interativo = isatty(STDIN_FILENO);

    printf("=== 1) VAZAMENTO (memory leak) ===\n");
    printf("    Errado:  char *buf = malloc(64);\n");
    printf("             if (!valido) return 0;   /* sai sem free */\n");
    printf("    Errado:  p = malloc(10); p = malloc(20);   /* o 1o bloco ficou sem dono */\n");
    printf("    Efeito:  a memoria nunca volta; o programa cresce ate faltar RAM.\n");
    printf("    Correto: um free para cada malloc, em TODOS os caminhos de saida.\n\n");
    for (int q = 0; q < 3; q++) processa_quadro_com_vazamento(0); /* 3 quadros inválidos */
    for (int q = 0; q < 3; q++) processa_quadro_correto(0);
    int *p = RMALLOC(10 * sizeof(int), "p, primeiro bloco (sobrescrito)");
    p = RMALLOC(20 * sizeof(int), "p, segundo bloco");  /* perde o primeiro: vazamento */
    RFREE(p);
    printf("    Rastreador depois de 3 quadros com bug, 3 corretos e a sobrescrita de p:\n");
    relatorio_vazamentos();
    pausar();

    printf("\n=== 2) DOUBLE FREE ===\n");
    printf("    Errado:  free(p); ... free(p);\n");
    printf("    Efeito:  corrompe as estruturas internas do malloc; o programa pode\n");
    printf("             abortar ou falhar muito depois, em outro lugar.\n");
    printf("    Correto: free(p); p = NULL;  (free(NULL) nao faz nada)\n\n");
    int *dado = RMALLOC(sizeof(int), "dado");
    RFREE(dado);   /* libera e zera */
    RFREE(dado);   /* segunda vez: inofensiva, pois dado == NULL */
    pausar();

    printf("\n=== 3) PONTEIRO PENDURADO (dangling pointer) ===\n");
    printf("    Errado:  int *a = malloc(...); int *b = a;  free(a); a = NULL;\n");
    printf("             /* b continua com o endereco antigo! */\n");
    printf("    Errado:  int *f(void) { int x = 5; return &x; }  /* x morre no return */\n");
    printf("    Correto: defina UM dono do bloco; zere todas as copias; nunca devolva\n");
    printf("             endereco de variavel local.\n\n");
    int *a = RMALLOC(4 * sizeof(int), "bloco compartilhado");
    if (a != NULL) {
        int *b = a;                               /* segunda cópia do endereço */
        uintptr_t guardado = (uintptr_t)b;        /* lido ANTES do free */
        RFREE(a);
        printf("      a = %p (zerado pelo rastro_free)\n", (void *)a);
        printf("      b ainda guardava 0x%llx: um bloco que ja foi devolvido.\n",
               (unsigned long long)guardado);
        b = NULL;                                 /* correção: zerar a cópia também */
        printf("      correcao aplicada: b = %p\n", (void *)b);
    }
    pausar();

    printf("\n=== 4) USO APOS FREE (use-after-free) ===\n");
    printf("    Errado:  free(no); printf(\"%%d\", no->valor);  /* ou no->prox */\n");
    printf("    Efeito:  pode ate \"funcionar\" no teste e falhar em campo: o bloco ja\n");
    printf("             pode ter sido entregue a outro malloc. O AddressSanitizer\n");
    printf("             acusa \"heap-use-after-free\".\n");
    printf("    Correto: copie o que precisa ANTES do free (classico em listas):\n");
    printf("             prox = no->prox; free(no); no = prox;\n");
    pausar();

    printf("\n=== 5) ESTOURO DE BUFFER: o +1 do '\\0' ===\n");
    const char *nome = "Sensor-TT101";
    printf("    Errado:  char *c = malloc(strlen(nome));     /* falta 1 byte */\n");
    printf("             strcpy(c, nome);                    /* escreve 1 alem */\n");
    printf("    Correto: char *c = malloc(strlen(nome) + 1);\n\n");
    size_t precisa = strlen(nome) + 1;
    char *copia = RMALLOC(precisa, "copia do nome");
    if (copia != NULL) {
        memcpy(copia, nome, precisa);
        printf("      \"%s\": strlen = %zu, alocados = %zu (com o '\\0')\n",
               copia, strlen(copia), precisa);
        RFREE(copia);
    }
    pausar();

    printf("\n=== 6) NAO TESTAR O RETORNO DE malloc ===\n");
    printf("    Errado:  int *v = malloc(n * sizeof *v); v[0] = 1;\n");
    printf("    Efeito:  se faltar memoria, malloc devolve NULL e v[0] derruba o programa.\n");
    printf("    Correto: if (v == NULL) { /* avise, libere o que tiver, saia */ }\n");
    pausar();

    printf("\n=== 7) free EM MEMORIA QUE NAO VEIO DO HEAP ===\n");
    printf("    Errado:  int x; free(&x);           /* x esta na pilha */\n");
    printf("    Errado:  int *v = malloc(...); free(v + 1);   /* meio do bloco */\n");
    printf("    Correto: so passe a free o endereco EXATO devolvido por malloc/calloc/realloc.\n\n");
    int local = 0;
    void *pl = &local;
    RFREE(pl);   /* o rastreador recusa: o endereço não é dele */
    pausar();

    printf("\n=== Relatorio final do rastreador ===\n");
    relatorio_vazamentos();
    printf("\nFerramentas: compile com -fsanitize=address (ou use valgrind no Linux)\n");
    printf("para encontrar esses erros automaticamente nos seus programas.\n");
    return 0;
}
