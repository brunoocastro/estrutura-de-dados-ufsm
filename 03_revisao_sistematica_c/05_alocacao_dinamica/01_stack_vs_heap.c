/*
 * ============================================================================
 * Arquivo : 01_stack_vs_heap.c
 * Módulo  : 03 — Revisão sistemática de C     (Ementa: 0.7 Alocação dinâmica)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Ver onde cada tipo de variável mora, imprimindo endereços reais:
 *     - dados estáticos: globais e static (existem o programa inteiro);
 *     - PILHA (stack): variáveis locais e parâmetros (nascem e morrem com a
 *       chamada da função; gerenciadas automaticamente);
 *     - HEAP: blocos de malloc/calloc/realloc (vivem até o free; tamanho
 *       decidido em execução; gerenciados por VOCÊ).
 *   O programa monta um "mapa da memória" ordenado por endereço e mostra que
 *   quadros de pilha são reaproveitados, enquanto blocos do heap sobrevivem
 *   ao retorno da função que os criou.
 *
 * EXEMPLO REAL
 *   Um supervisório que só descobre quantos sensores existem ao ler o arquivo
 *   de configuração precisa do heap; já o contador de um laço de controle
 *   deve ser local (pilha): rápido e liberado sozinho.
 *
 * COMPLEXIDADE
 *   Criar/destruir local na pilha: O(1) (o compilador só move o ponteiro de pilha).
 *   malloc/free: O(1) amortizado na prática, mas bem mais caros que a pilha
 *   (procuram um bloco livre, guardam metadados) e exigem free manual.
 *   Ordenar o mapa (qsort de k itens): O(k log k).
 *
 * COMO EXECUTAR
 *   ./run 01_stack_vs_heap               (ENTER avança cada passo)
 *   ./run 01_stack_vs_heap < /dev/null   (executa direto, sem pausas)
 * ============================================================================
 */
#define _POSIX_C_SOURCE 200809L   /* antes de qualquer #include: libera isatty() em -std=c11 */
#include <stdint.h>
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

int global_inicializada = 42;   /* dados estáticos (.data) */
int global_zerada;              /* dados estáticos (.bss): começa em 0 */

/* ---------- Mapa da memória: lista de (nome, região, endereço) ---------- */
typedef struct {
    const char *nome;
    const char *regiao;
    uintptr_t endereco;
} Item;

static Item mapa[16];
static size_t itens = 0;

static void registra(const char *nome, const char *regiao, const void *p) {
    if (itens < sizeof mapa / sizeof mapa[0])
        mapa[itens++] = (Item){nome, regiao, (uintptr_t)p};
}

static int compara(const void *a, const void *b) {
    uintptr_t x = ((const Item *)a)->endereco, y = ((const Item *)b)->endereco;
    return (x > y) - (x < y); /* evita overflow de "x - y" */
}

/* ---------- Funções que criam quadros de pilha em profundidades diferentes ---------- */
static void nivel3(void) {
    int local3 = 3;
    registra("local3 (dentro de nivel3)", "PILHA", &local3);
}
static void nivel2(void) {
    int local2 = 2;
    registra("local2 (dentro de nivel2)", "PILHA", &local2);
    nivel3();
}

/* Cria um bloco no heap e devolve o endereço: CORRETO, o bloco continua
 * existindo depois do return. Quem recebe passa a ser dono e deve dar free. */
static int *cria_no_heap(int valor) {
    int *p = malloc(sizeof *p);
    if (p != NULL) *p = valor;
    return p;
}

/* ERRADO (não chamado): devolver o endereço de uma local.
 *     int *cria_na_pilha(void) { int x = 5; return &x; }
 * x morre no return; o ponteiro devolvido fica "pendurado" (dangling) e usá-lo
 * é comportamento indefinido. O compilador avisa (-Wreturn-stack-address /
 * -Wreturn-local-addr). */

/* Duas funções "irmãs": chamadas uma após a outra a partir do mesmo ponto,
 * seus quadros ocupam o MESMO lugar da pilha (o espaço é reaproveitado).
 * Cada uma anota, como NÚMERO, onde ficou sua local (devolver o endereço da
 * local seria justamente o erro descrito acima; o compilador avisaria). */
static uintptr_t endereco_a, endereco_b;
static void irma_a(void) { volatile int a = 111; endereco_a = (uintptr_t)&a; }
static void irma_b(void) { volatile int b = 222; endereco_b = (uintptr_t)&b; }

int main(void) {
    modo_interativo = isatty(STDIN_FILENO);
    static int static_local = 7;            /* dados estáticos, mesmo sendo "local" */
    int local_main = 1;                     /* pilha */
    const char *literal = "texto literal";  /* o texto fica em área somente leitura */

    int *heap_pequeno = malloc(sizeof *heap_pequeno);
    double *heap_vetor = calloc(100, sizeof *heap_vetor);
    int *heap_de_funcao = cria_no_heap(99);
    if (heap_pequeno == NULL || heap_vetor == NULL || heap_de_funcao == NULL) {
        free(heap_pequeno);
        free(heap_vetor);
        free(heap_de_funcao);
        printf("sem memoria\n");
        return 0;
    }

    registra("global_inicializada", "ESTATICA", &global_inicializada);
    registra("global_zerada", "ESTATICA", &global_zerada);
    registra("static_local (em main)", "ESTATICA", &static_local);
    registra("\"texto literal\"", "SO LEITURA", literal);
    registra("local_main", "PILHA", &local_main);
    nivel2();
    registra("heap_pequeno (1 int)", "HEAP", heap_pequeno);
    registra("heap_vetor (100 double)", "HEAP", heap_vetor);
    registra("heap_de_funcao (criado em funcao)", "HEAP", heap_de_funcao);

    printf("=== 1) Mapa da memoria, do MAIOR endereco para o menor ===\n");
    qsort(mapa, itens, sizeof mapa[0], compara);
    for (size_t i = itens; i-- > 0;)
        printf("  0x%014llx  %-10s  %s\n", (unsigned long long)mapa[i].endereco,
               mapa[i].regiao, mapa[i].nome);
    printf("\n  Repare: as locais ficam juntas (pilha) e, quanto mais funda a chamada,\n");
    printf("  menor o endereco (a pilha cresce para baixo na maioria das plataformas).\n");
    printf("  Os blocos do heap ficam em outra regiao. A ordem exata depende do sistema.\n");
    pausar();

    printf("\n=== 2) Quadros de pilha sao reaproveitados ===\n");
    irma_a();
    irma_b();
    uintptr_t ea = endereco_a, eb = endereco_b;
    printf("  local de irma_a: 0x%llx\n", (unsigned long long)ea);
    printf("  local de irma_b: 0x%llx  -> %s\n", (unsigned long long)eb,
           ea == eb ? "MESMO endereco: o quadro de irma_a ja tinha morrido"
                    : "endereco diferente (o compilador organizou de outro jeito)");
    printf("  Por isso um ponteiro para local de funcao que ja retornou e perigoso:\n");
    printf("  a proxima chamada sobrescreve aquele espaco.\n");
    pausar();

    printf("\n=== 3) O bloco do heap sobrevive a funcao que o criou ===\n");
    printf("  cria_no_heap ja retornou, mas *heap_de_funcao = %d (ainda valido)\n", *heap_de_funcao);
    printf("  heap_vetor veio do calloc: heap_vetor[50] = %.1f (zerado)\n", heap_vetor[50]);
    pausar();

    printf("\n=== 4) Tamanho: pilha e pequena, heap e grande ===\n");
    size_t grande = 64u * 1024u * 1024u; /* 64 MB */
    char *bloco = malloc(grande);
    printf("  malloc(64 MB) %s\n", bloco ? "funcionou (heap)" : "falhou (tratado: ponteiro NULL)");
    printf("  'char v[64 * 1024 * 1024];' como local estouraria a pilha de ~8 MB\n");
    printf("  (nao executado: o programa morreria com Segmentation fault).\n");
    free(bloco);

    printf("\n=== 5) Liberando o heap: cada malloc/calloc tem exatamente um free ===\n");
    free(heap_pequeno);
    free(heap_vetor);
    free(heap_de_funcao);
    heap_pequeno = NULL; /* depois do free, zerar evita uso acidental */
    heap_vetor = NULL;
    heap_de_funcao = NULL;
    printf("  3 blocos liberados. Pilha e estaticos sao liberados automaticamente.\n");

    printf("\nResumo:\n");
    printf("  %-10s | %-22s | %-26s | %s\n", "regiao", "quem cria/libera", "vida util", "tamanho");
    printf("  %-10s | %-22s | %-26s | %s\n", "pilha", "automatico (funcao)", "ate a funcao retornar", "pequeno (KB-MB)");
    printf("  %-10s | %-22s | %-26s | %s\n", "heap", "voce (malloc/free)", "ate o free", "grande (ate a RAM)");
    printf("  %-10s | %-22s | %-26s | %s\n", "estatica", "compilador", "o programa inteiro", "fixo na compilacao");
    return 0;
}
