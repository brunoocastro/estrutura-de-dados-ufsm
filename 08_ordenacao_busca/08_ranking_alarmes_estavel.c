/*
 * ============================================================================
 * Arquivo : 08_ranking_alarmes_estavel.c
 * Módulo  : 08 — Ordenação e Busca            (Ementa: Unidade 4.3 — aplicação)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Mostrar, com um caso de supervisório, por que ESTABILIDADE importa:
 *   ordenar a lista de alarmes por PRIORIDADE mantendo, entre alarmes de mesma
 *   prioridade, a ORDEM CRONOLÓGICA em que chegaram. Um método estável
 *   (inserção) acerta de graça; um instável (seleção) embaralha os horários, e
 *   o passo a passo mostra exatamente qual troca "pulou" um alarme por cima do
 *   outro. Por fim, a correção com CHAVE COMPOSTA (prioridade, horário) no
 *   qsort, que funciona com qualquer algoritmo.
 *
 * EXEMPLO REAL
 *   Norma ISA-18.2 / EEMUA 191: a tela de alarmes do SCADA lista primeiro os
 *   críticos e, dentro de cada prioridade, o mais antigo primeiro — o operador
 *   precisa saber QUAL falha veio antes (causa raiz) e qual é consequência.
 *   Se a ordenação embaralha os horários, o operador ataca o sintoma.
 *   Cotidiano: planilha ordenada por data e depois por cliente; e-mails por
 *   remetente mantendo a ordem de chegada.
 *
 * COMPLEXIDADE (n = número de alarmes)
 *   Operação                     Melhor      Médio       Pior        Espaço  Estável
 *   inserção por prioridade      O(n)        O(n²)       O(n²)       O(1)    sim
 *   seleção por prioridade       O(n²)       O(n²)       O(n²)       O(1)    NÃO
 *   qsort, chave composta        O(n log n)  O(n log n)  O(n log n)* O(log n) indiferente
 *   Por quê: estabilidade é uma propriedade do ALGORITMO, não da complexidade:
 *   a seleção troca elementos distantes e pode levar um item para trás de outro
 *   com a mesma chave. Com chave composta não existem "empates", então a
 *   estabilidade deixa de importar. *Depende da implementação da biblioteca.
 *
 * COMO EXECUTAR
 *   ./run 08_ranking_alarmes_estavel               (ENTER avança cada passo)
 *   ./run 08_ranking_alarmes_estavel < /dev/null   (executa direto, sem pausas)
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

typedef struct {
    int prioridade;        /* 1 = crítico, 2 = alto, 3 = médio (menor = mais urgente) */
    int hora;              /* segundos desde 00:00 (timestamp)                         */
    const char *tag;
    const char *mensagem;
} Alarme;

/* Log na ordem em que os alarmes CHEGARAM (já cronológico). A sequência
 * conta uma história: a bomba falhou -> pressão caiu -> temperatura subiu. */
static const Alarme log_chegada[] = {
    {2, 8 * 3600 + 0 * 60 + 12, "M-501",  "bomba P-501 desligou por sobrecorrente"},
    {3, 8 * 3600 + 0 * 60 + 40, "FT-305", "vazão de resfriamento baixa"},
    {1, 8 * 3600 + 1 * 60 + 5,  "PT-201", "pressão linha de vapor muito baixa"},
    {2, 8 * 3600 + 1 * 60 + 30, "LT-110", "nível tanque pulmão baixo"},
    {3, 8 * 3600 + 2 * 60 + 0,  "AT-220", "pH fora da faixa"},
    {1, 8 * 3600 + 2 * 60 + 45, "TT-101", "temperatura reator 1 muito alta"},
    {2, 8 * 3600 + 3 * 60 + 10, "TT-102", "temperatura reator 2 alta"},
    {1, 8 * 3600 + 3 * 60 + 50, "XV-402", "válvula de bloqueio fechou (intertravamento)"},
    {3, 8 * 3600 + 4 * 60 + 20, "LT-115", "nível tanque de condensado alto"},
};
#define N ((int)(sizeof log_chegada / sizeof log_chegada[0]))

/* Já com espaços até 8 colunas visíveis: "%-8s" contaria os bytes extras do
 * UTF-8 (Í, É) e desalinharia a tabela. */
static const char *nome_prio[] = {"", "CRÍTICO ", "ALTO    ", "MÉDIO   "};

static long comparacoes, trocas;

/* Imprime a lista; marca com "<-- fora de ordem" o alarme que tem a mesma
 * prioridade do anterior mas horário MAIS ANTIGO (estabilidade violada).
 * Devolve quantas violações encontrou. O(n). */
static int mostra(const Alarme a[], int n) {
    int violacoes = 0;
    printf("     #  prioridade  horário   tag     mensagem\n");
    for (int i = 0; i < n; i++) {
        int fora = i > 0 && a[i].prioridade == a[i - 1].prioridade && a[i].hora < a[i - 1].hora;
        violacoes += fora;
        printf("    %2d  %d %s  %02d:%02d:%02d  %-6s  %s%s\n", i, a[i].prioridade,
               nome_prio[a[i].prioridade], a[i].hora / 3600, a[i].hora / 60 % 60, a[i].hora % 60,
               a[i].tag, a[i].mensagem, fora ? "   <-- fora de ordem cronológica!" : "");
    }
    return violacoes;
}

/* Linha compacta "tag:prioridade", com o par que vai trocar entre colchetes. */
static void compacto(const Alarme a[], int n, int x, int y) {
    printf("      ");
    for (int i = 0; i < n; i++) {
        if (i == x || i == y) printf("[%s:%d]", a[i].tag, a[i].prioridade);
        else                  printf(" %s:%d ", a[i].tag, a[i].prioridade);
    }
    printf("\n");
}

/* INSERÇÃO por prioridade (estável): só passa um alarme para frente de outro
 * se a prioridade dele for ESTRITAMENTE menor. O(n²) pior, O(n) se já
 * estiver ordenado. */
static void insercao_prio(Alarme a[], int n) {
    for (int i = 1; i < n; i++) {
        Alarme x = a[i];
        int j = i - 1;
        while (j >= 0 && (comparacoes++, a[j].prioridade > x.prioridade)) {
            a[j + 1] = a[j];
            trocas++;
            j--;
        }
        a[j + 1] = x;
    }
}

/* SELEÇÃO por prioridade (instável). Com `mostrar`, narra cada troca e
 * aponta quando ela pula um alarme de mesma prioridade (quebra a ordem). O(n²). */
static void selecao_prio(Alarme a[], int n, int mostrar) {
    for (int i = 0; i < n - 1; i++) {
        int m = i;
        for (int j = i + 1; j < n; j++) {
            comparacoes++;
            if (a[j].prioridade < a[m].prioridade) m = j;
        }
        if (m == i) continue;
        if (mostrar) {
            compacto(a, n, i, m);
            printf("      passo %d: menor prioridade em v[%d..] é %s (P%d) -> troca com %s (P%d)\n", i + 1, i,
                   a[m].tag, a[m].prioridade, a[i].tag, a[i].prioridade);
            /* a[i] vai para a posição m: se existir entre i+1 e m-1 alguém de
             * mesma prioridade que a[i], a[i] (mais antigo) vai parar DEPOIS dele. */
            for (int k = i + 1; k < m; k++)
                if (a[k].prioridade == a[i].prioridade) {
                    printf("      !! %s salta por cima de %s (mesma prioridade P%d, mais novo): ordem quebrada\n",
                           a[i].tag, a[k].tag, a[i].prioridade);
                    break;
                }
        }
        Alarme t = a[i]; a[i] = a[m]; a[m] = t;
        trocas++;
        if (mostrar) pausar();
    }
}

/* Comparador SÓ por prioridade (empates ficam por conta do algoritmo). */
static int cmp_prio(const void *p, const void *q) {
    const Alarme *a = p, *b = q;
    return (a->prioridade > b->prioridade) - (a->prioridade < b->prioridade);
}

/* Comparador por CHAVE COMPOSTA: prioridade e, no empate, horário.
 * Assim não existe empate real e qualquer algoritmo dá o mesmo resultado. */
static int cmp_prio_hora(const void *p, const void *q) {
    const Alarme *a = p, *b = q;
    if (a->prioridade != b->prioridade) return (a->prioridade > b->prioridade) - (a->prioridade < b->prioridade);
    return (a->hora > b->hora) - (a->hora < b->hora);
}

int main(void) {
    modo_interativo = isatty(STDIN_FILENO);
    Alarme a[N];

    printf("RANKING DE ALARMES: POR QUE A ESTABILIDADE IMPORTA\n\n");
    printf("=== 0) Log na ordem de chegada (cronológica) ===\n");
    memcpy(a, log_chegada, sizeof a);
    mostra(a, N);
    printf("\n   Objetivo da tela: críticos primeiro e, em cada prioridade, o MAIS ANTIGO primeiro.\n\n");
    pausar();

    printf("=== 1) Inserção por prioridade (ESTÁVEL) ===\n");
    memcpy(a, log_chegada, sizeof a);
    comparacoes = trocas = 0;
    insercao_prio(a, N);
    int v = mostra(a, N);
    printf("   -> %d violação(ões); %ld comparações, %ld deslocamentos. Em cada prioridade a ordem\n", v,
           comparacoes, trocas);
    printf("      de chegada foi preservada: PT-201 (causa) aparece antes de TT-101 (consequência).\n\n");
    pausar();

    printf("=== 2) Seleção por prioridade (INSTÁVEL), passo a passo ===\n");
    memcpy(a, log_chegada, sizeof a);
    comparacoes = trocas = 0;
    selecao_prio(a, N, 1);
    printf("\n   Resultado:\n");
    v = mostra(a, N);
    printf("   -> %d violação(ões); %ld comparações, %ld trocas. As prioridades estão certas, mas o\n", v,
           comparacoes, trocas);
    printf("      operador veria a consequência antes da causa.\n\n");
    pausar();

    printf("=== 3) qsort SÓ por prioridade ===\n");
    memcpy(a, log_chegada, sizeof a);
    qsort(a, N, sizeof a[0], cmp_prio);
    v = mostra(a, N);
    printf("   -> %d violação(ões) NESTA biblioteca. O padrão C não promete estabilidade para qsort:\n", v);
    printf("      o resultado pode mudar entre sistemas, versões ou tamanhos de entrada.\n\n");
    pausar();

    printf("=== 4) Correção: qsort com CHAVE COMPOSTA (prioridade, horário) ===\n");
    memcpy(a, log_chegada, sizeof a);
    qsort(a, N, sizeof a[0], cmp_prio_hora);
    v = mostra(a, N);
    printf("   -> %d violação(ões), em qualquer biblioteca.\n\n", v);

    printf("Resumo: para ordenar por mais de um critério, ou (a) use chave composta no comparador,\n");
    printf("ou (b) ordene pelo critério secundário e depois por um método ESTÁVEL pelo primário\n");
    printf("(aqui o log já chega em ordem de horário, então só o passo (b) com inserção bastou).\n");
    return 0;
}
