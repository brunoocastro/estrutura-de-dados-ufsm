/*
 * ============================================================================
 * Arquivo : 11_estouro_de_pilha.c
 * Módulo  : 03 — Revisão sistemática de C     (Ementa: 0.6 Recursividade, 0.7 Memória)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Entender o risco de STACK OVERFLOW (estouro da pilha) sem provocá-lo:
 *     1. lê o tamanho da pilha deste processo (getrlimit);
 *     2. MEDE quantos bytes cada quadro de uma função recursiva ocupa;
 *     3. calcula até que profundidade a recursão é segura e, para cada n,
 *        diz se soma_recursiva(n) caberia ou estouraria — executando só os
 *        casos seguros;
 *     4. resolve o mesmo problema com laço e com uma "pilha explícita" no heap.
 *
 * EXEMPLO REAL
 *   Num microcontrolador a pilha pode ter 1 a 8 KB. Uma recursão que funciona
 *   no PC do laboratório trava o CLP em campo (ou, pior, corrompe variáveis
 *   vizinhas em silêncio, pois muitos MCUs não detectam o estouro). Threads de
 *   sistemas operacionais também têm pilhas menores (ex.: 512 KB).
 *
 * COMPLEXIDADE (n = profundidade da recursão)
 *   Memória de pilha: O(n) * (bytes por quadro). Com pilha de 8 MB e quadros
 *   de ~100 bytes, n ≈ 80 000 já é o limite.
 *   Laço: O(1) de pilha. Pilha explícita: O(n) no HEAP (limitado pela RAM,
 *   não pelos poucos MB da pilha) e falha de forma controlada (malloc == NULL).
 *
 * COMO EXECUTAR
 *   ./run 11_estouro_de_pilha               (ENTER avança cada passo)
 *   ./run 11_estouro_de_pilha < /dev/null   (executa direto, sem pausas)
 * ============================================================================
 */
#define _POSIX_C_SOURCE 200809L   /* antes de qualquer #include: libera isatty() em -std=c11 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/resource.h>
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

static uintptr_t endereco_topo = 0;   /* local do quadro mais profundo medido */

/* Recursão comum usada nas medições: O(n) tempo e pilha. Ao chegar à
 * profundidade "marcar", registra o endereço de sua variável local. */
static unsigned long long soma_recursiva(unsigned long n, unsigned long prof,
                                         unsigned long marcar) {
    volatile unsigned long local = n; /* volatile: o compilador não a elimina */
    if (prof == marcar) endereco_topo = (uintptr_t)&local;
    if (n == 0) return 0;
    return local + soma_recursiva(n - 1, prof + 1, marcar);
}

/* Mede bytes por quadro: compara o endereço da local no quadro 0 com o do
 * quadro "prof". Pilhas costumam crescer para endereços MENORES. */
static double bytes_por_quadro(unsigned long prof) {
    soma_recursiva(prof, 0, 0);
    uintptr_t a0 = endereco_topo;
    soma_recursiva(prof, 0, prof);
    uintptr_t ap = endereco_topo;
    uintptr_t dif = a0 > ap ? a0 - ap : ap - a0;
    printf("  local no quadro 0      : 0x%llx\n", (unsigned long long)a0);
    printf("  local no quadro %-6lu : 0x%llx  (%s)\n", prof, (unsigned long long)ap,
           ap < a0 ? "endereco MENOR: a pilha cresce para baixo" : "endereco maior");
    return (double)dif / (double)prof;
}

/* O(n) tempo, O(1) pilha. */
static unsigned long long soma_laco(unsigned long n) {
    unsigned long long s = 0;
    for (unsigned long i = 1; i <= n; i++) s += i;
    return s;
}

/* O(n) tempo, O(n) memória no HEAP: simula a pilha de chamadas num vetor.
 * Fase 1 ("descida"): empilha n, n-1, ..., 1 (o que cada quadro guardaria).
 * Fase 2 ("subida"): desempilha somando, como os retornos da recursão. */
static int soma_pilha_explicita(unsigned long n, unsigned long long *resultado) {
    unsigned long *pilha = malloc((n > 0 ? n : 1) * sizeof *pilha); /* evita malloc(0) */
    if (pilha == NULL) return 0;          /* falha controlada, sem travar */
    size_t topo = 0;
    for (unsigned long k = n; k > 0; k--) pilha[topo++] = k;
    unsigned long long s = 0;
    while (topo > 0) s += pilha[--topo];
    free(pilha);
    *resultado = s;
    return 1;
}

int main(void) {
    modo_interativo = isatty(STDIN_FILENO);

    printf("=== 1) Tamanho da pilha deste processo ===\n");
    struct rlimit lim;
    double pilha_bytes = 8.0 * 1024 * 1024; /* valor típico, caso a consulta falhe */
    if (getrlimit(RLIMIT_STACK, &lim) == 0 && lim.rlim_cur != RLIM_INFINITY) {
        pilha_bytes = (double)lim.rlim_cur;
        printf("  getrlimit(RLIMIT_STACK) = %.0f bytes = %.1f MB\n", pilha_bytes,
               pilha_bytes / (1024.0 * 1024.0));
    } else {
        printf("  limite ilimitado ou indisponivel: assumindo 8 MB\n");
    }
    printf("  (no Linux/macOS, 'ulimit -s' mostra o mesmo valor em KB)\n");
    pausar();

    printf("\n=== 2) Medindo o tamanho de cada quadro de soma_recursiva ===\n");
    double por_quadro = bytes_por_quadro(1000);
    if (por_quadro < 1.0) por_quadro = 1.0;
    unsigned long max_prof = (unsigned long)(pilha_bytes / por_quadro);
    printf("  => cerca de %.0f bytes por quadro (parametros, endereco de retorno,\n", por_quadro);
    printf("     locais e alinhamento; com -fsanitize ou sem otimizacao fica maior)\n");
    printf("  => profundidade maxima estimada: %lu chamadas\n", max_prof);
    pausar();

    printf("\n=== 3) Quais n cabem na pilha? (so os seguros sao executados) ===\n");
    printf("%10s | %14s | %10s | %s\n", "n", "pilha (bytes)", "% do total", "situacao");
    printf("-----------+----------------+------------+------------------------------\n");
    unsigned long ns[] = {1000, 10000, 100000, 1000000, 10000000};
    for (size_t k = 0; k < sizeof ns / sizeof ns[0]; k++) {
        unsigned long n = ns[k];
        double precisa = (double)n * por_quadro;
        double pct = 100.0 * precisa / pilha_bytes;
        printf("%10lu | %14.0f | %9.1f%% | ", n, precisa, pct);
        if (pct < 25.0) {
            /* margem folgada: executa de verdade e confere com a fórmula */
            unsigned long long r = soma_recursiva(n, 0, (unsigned long)-1);
            printf("ok: executado, soma = %llu\n", r);
        } else if (pct < 100.0) {
            printf("arriscado: NAO executado\n");
        } else {
            printf("ESTOURARIA (Segmentation fault)\n");
        }
    }
    printf("\nO que acontece num estouro: o programa morre com 'Segmentation fault'\n");
    printf("(SIGSEGV), sem mensagem util; num microcontrolador sem protecao, a pilha\n");
    printf("invade outras variaveis e o defeito aparece longe da causa.\n");
    pausar();

    printf("\n=== 4) As alternativas seguras, com n = 10 000 000 ===\n");
    unsigned long n = 10000000UL;
    printf("  soma_laco             = %llu  (pilha O(1))\n", soma_laco(n));
    unsigned long long r;
    if (soma_pilha_explicita(n, &r))
        printf("  soma_pilha_explicita  = %llu  (%lu itens no HEAP: %.1f MB)\n",
               r, n, (double)n * sizeof(unsigned long) / (1024.0 * 1024.0));
    else
        printf("  soma_pilha_explicita: malloc falhou, erro tratado sem travar\n");
    printf("  formula n(n+1)/2      = %llu\n", (unsigned long long)n * (n + 1) / 2);
    printf("\nRegra pratica: recursao so quando a profundidade e pequena e conhecida\n");
    printf("(ex.: O(log n) da busca binaria ou de uma arvore balanceada).\n");
    return 0;
}
