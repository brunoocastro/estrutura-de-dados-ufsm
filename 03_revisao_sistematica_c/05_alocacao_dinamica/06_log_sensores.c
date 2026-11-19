/*
 * ============================================================================
 * Arquivo : 06_log_sensores.c
 * Módulo  : 03 — Revisão sistemática de C     (Ementa: 0.7 Alocação dinâmica)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Exemplo real completo: um registrador de dados (data logger) guarda
 *   leituras de três sensores de uma planta durante um ensaio cuja duração e
 *   taxa de amostragem não são conhecidas de antemão (ao surgir um alarme, a
 *   taxa aumenta). O buffer começa com 4 posições e DOBRA com realloc quando
 *   enche; cada crescimento é narrado. No fim: estatísticas por canal,
 *   ajuste do tamanho ("shrink to fit") e comparação com um vetor fixo.
 *
 * EXEMPLO REAL
 *   Sensores: TT-101 (temperatura do reator, °C), PT-201 (pressão, bar) e
 *   FT-301 (vazão, m³/h). Situação típica de comissionamento: o engenheiro
 *   grava tudo durante um teste e exporta para análise depois.
 *
 * COMPLEXIDADE (n = leituras gravadas)
 *   Operação             Custo                          Por quê
 *   log_adiciona         O(1) amortizado (pior O(n))    realloc dobra a capacidade
 *   estatisticas         O(n)                           uma passada em tudo
 *   ajusta_capacidade    O(n) no pior caso              realloc pode copiar
 *   log_libera           O(1)                           um único free
 *   Espaço: n ≤ capacidade < 2n leituras (antes do ajuste final).
 *
 * COMO EXECUTAR
 *   ./run 06_log_sensores               (ENTER avança a cada crescimento)
 *   ./run 06_log_sensores < /dev/null   (executa direto, sem pausas)
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

enum { TT101, PT201, FT301, NUM_CANAIS };
static const char *NOME_CANAL[NUM_CANAIS] = {"TT-101", "PT-201", "FT-301"};
static const char *UNIDADE[NUM_CANAIS] = {"C", "bar", "m3/h"};

/* Campos maiores primeiro para reduzir padding (veja 07_structs.c). */
typedef struct {
    float valor;
    uint32_t t_ms;     /* instante da leitura em milissegundos */
    uint8_t canal;
} Leitura;

typedef struct {
    Leitura *itens;    /* bloco no heap, cresce com realloc */
    size_t n;          /* leituras gravadas */
    size_t cap;        /* capacidade atual */
    unsigned crescimentos;
    unsigned mudou_endereco;
} LogSensores;

/* O(1): log vazio com capacidade inicial pequena. */
static int log_inicia(LogSensores *log, size_t cap_inicial) {
    log->itens = malloc(cap_inicial * sizeof *log->itens);
    log->n = 0;
    log->cap = log->itens ? cap_inicial : 0;
    log->crescimentos = log->mudou_endereco = 0;
    return log->itens != NULL;
}

/* O(1) amortizado. Retorna 0 se faltar memória: o log antigo continua
 * íntegro (padrão do ponteiro temporário no realloc). */
static int log_adiciona(LogSensores *log, Leitura l) {
    if (log->n == log->cap) {
        size_t nova = log->cap * 2;
        uintptr_t antes = (uintptr_t)log->itens;
        Leitura *tmp = realloc(log->itens, nova * sizeof *tmp);
        if (tmp == NULL) return 0;
        int moveu = (uintptr_t)tmp != antes;
        log->itens = tmp;
        log->crescimentos++;
        if (moveu) log->mudou_endereco++;
        printf("  t=%5u ms | buffer cheio (%zu leituras) -> realloc para %zu (%zu bytes)%s\n",
               (unsigned)l.t_ms, log->cap, nova, nova * sizeof *tmp,
               moveu ? ", MUDOU de endereco: copiou tudo" : ", cresceu no lugar");
        log->cap = nova;
        pausar();
    }
    log->itens[log->n++] = l;
    return 1;
}

/* O(n): no fim do ensaio devolve ao sistema a sobra da capacidade. */
static void log_ajusta_capacidade(LogSensores *log) {
    if (log->n == 0 || log->n == log->cap) return;
    Leitura *tmp = realloc(log->itens, log->n * sizeof *tmp);
    if (tmp != NULL) {           /* se falhar, apenas mantemos o bloco maior */
        log->itens = tmp;
        log->cap = log->n;
    }
}

static void log_libera(LogSensores *log) {
    free(log->itens);
    log->itens = NULL;
    log->n = log->cap = 0;
}

/* Gerador pseudoaleatório simples (LCG) com semente fixa: a saída é sempre a
 * mesma, o que permite conferir os números da aula. */
static uint32_t semente = 12345u;
static float ruido(float amplitude) {
    semente = semente * 1103515245u + 12345u;
    float u = (float)((semente >> 8) & 0xFFFFu) / 65535.0f;  /* 0..1 */
    return (u - 0.5f) * 2.0f * amplitude;
}

/* Simula a leitura física de um canal no instante t. Entre 1200 e 1600 ms
 * acontece uma sobretemperatura no reator. */
static float le_sensor(int canal, uint32_t t) {
    int alarme = t >= 1200 && t < 1600;
    switch (canal) {
    case TT101: return (alarme ? 92.0f : 75.0f) + ruido(1.5f);
    case PT201: return (alarme ? 6.8f : 5.0f) + ruido(0.1f);
    default:    return 12.0f + ruido(0.4f);
    }
}

int main(void) {
    modo_interativo = isatty(STDIN_FILENO);
    LogSensores log;
    if (!log_inicia(&log, 4)) { printf("sem memoria\n"); return 0; }

    printf("=== Ensaio de 3 s: amostragem a cada 100 ms; durante o alarme, a cada 10 ms ===\n");
    printf("    (a quantidade de leituras so e conhecida no fim -> memoria dinamica)\n\n");
    for (uint32_t t = 0; t < 3000;) {
        int alarme = t >= 1200 && t < 1600;
        for (int c = 0; c < NUM_CANAIS; c++) {
            Leitura l = {le_sensor(c, t), t, (uint8_t)c};
            if (!log_adiciona(&log, l)) {
                printf("  memoria esgotada em t=%u ms: ensaio interrompido, dados preservados\n",
                       (unsigned)t);
                t = 3000;
                break;
            }
        }
        if (t == 1200) printf("  t= 1200 ms | ALARME: TT-101 acima de 90 C, amostragem 10x mais rapida\n");
        if (t == 1600) printf("  t= 1600 ms | alarme normalizado, volta a 100 ms\n");
        if (t < 3000) t += alarme ? 10 : 100;
    }

    printf("\n=== Resumo do buffer ===\n");
    printf("  leituras: %zu | capacidade: %zu | ocupacao: %.0f%%\n",
           log.n, log.cap, 100.0 * (double)log.n / (double)log.cap);
    printf("  crescimentos: %u (log2 da capacidade final / inicial) | mudou de endereco: %u\n",
           log.crescimentos, log.mudou_endereco);
    pausar();

    printf("\n=== Estatisticas por canal: uma passada, O(n) ===\n");
    float minimo[NUM_CANAIS], maximo[NUM_CANAIS];
    double soma[NUM_CANAIS] = {0};
    size_t quantos[NUM_CANAIS] = {0};
    for (int c = 0; c < NUM_CANAIS; c++) { minimo[c] = 1e30f; maximo[c] = -1e30f; }
    for (size_t i = 0; i < log.n; i++) {
        const Leitura *l = &log.itens[i];
        if (l->valor < minimo[l->canal]) minimo[l->canal] = l->valor;
        if (l->valor > maximo[l->canal]) maximo[l->canal] = l->valor;
        soma[l->canal] += l->valor;
        quantos[l->canal]++;
    }
    printf("  %-7s | %7s | %8s | %8s | %8s | %s\n", "canal", "leit.", "minimo", "media", "maximo", "unid.");
    for (int c = 0; c < NUM_CANAIS; c++)
        printf("  %-7s | %7zu | %8.2f | %8.2f | %8.2f | %s\n", NOME_CANAL[c], quantos[c],
               (double)minimo[c], quantos[c] ? soma[c] / (double)quantos[c] : 0.0,
               (double)maximo[c], UNIDADE[c]);

    printf("\n  Ultimas 4 leituras gravadas:\n");
    for (size_t i = log.n >= 4 ? log.n - 4 : 0; i < log.n; i++)
        printf("    t=%4u ms  %-7s = %6.2f %s\n", (unsigned)log.itens[i].t_ms,
               NOME_CANAL[log.itens[i].canal], (double)log.itens[i].valor,
               UNIDADE[log.itens[i].canal]);
    pausar();

    printf("\n=== Ajuste final (shrink to fit) ===\n");
    size_t antes = log.cap;
    log_ajusta_capacidade(&log);
    printf("  capacidade %zu -> %zu: devolveu %zu bytes ao sistema\n",
           antes, log.cap, (antes - log.cap) * sizeof(Leitura));

    printf("\n=== E se fosse um vetor fixo? ===\n");
    printf("  Leitura fixa[64]:    guardaria so 64 de %zu leituras: perderia o alarme!\n", log.n);
    printf("  Leitura fixa[100000]: caberia, mas ocuparia %zu bytes para usar %zu.\n",
           (size_t)100000 * sizeof(Leitura), log.n * sizeof(Leitura));
    printf("  O vetor dinamico usa so o necessario e cresce com custo amortizado O(1).\n");

    log_libera(&log);
    printf("\n  log_libera: 1 free para o buffer inteiro (os itens estao num bloco unico).\n");
    return 0;
}
