/*
 * ============================================================================
 * Arquivo : 05_tad_temporizador_ton.c
 * Módulo  : 04 — Tipos Abstratos de Dados       (Ementa: Unidade 1.2)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Implementar como TAD o temporizador com atraso na energização (TON) da
 *   norma IEC 61131-3, o mesmo bloco usado em Ladder/ST nos CLPs. O programa
 *   simula o ciclo de varredura (scan) do CLP e mostra, ciclo a ciclo, a
 *   entrada IN, o tempo decorrido ET e a saída Q; no fim desenha o diagrama de
 *   tempos. Depois usa VÁRIAS instâncias do mesmo TAD ao mesmo tempo, como um
 *   programa de CLP usa T1, T2, T3...
 *
 * EXEMPLO REAL
 *   - Filtro de ruído (debounce) de um pressostato: só considera "pressão OK"
 *     se o contato ficar fechado por 300 ms seguidos.
 *   - Partida de motor: só liga a bomba depois de 2 s de nível alto estável.
 *   - Alarme de porta aberta: avisa se a porta ficar aberta mais de 5 s.
 *
 * COMPLEXIDADE (k = número de temporizadores, c = ciclos de scan)
 *   Operação         Tempo   Espaço
 *   ton_iniciar      O(1)    O(1) por instância
 *   ton_atualizar    O(1)    O(1)
 *   simular c ciclos com k timers: O(c × k) tempo, O(k) espaço
 *   Por quê: o TON não guarda histórico, só ET e o estado; cada scan faz uma
 *   soma e uma comparação por temporizador.
 *
 * COMO EXECUTAR
 *   ./run 05_tad_temporizador_ton               (ENTER avança cada passo)
 *   ./run 05_tad_temporizador_ton < /dev/null   (executa direto, sem pausas)
 * ============================================================================
 */
#define _POSIX_C_SOURCE 200809L   /* antes de qualquer #include: libera isatty() em -std=c11 */
#include <stdint.h>
#include <stdio.h>
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
 * INTERFACE  (ton.h)
 *   Contrato do TON (IEC 61131-3):
 *     - IN = 0           -> ET = 0 e Q = 0 (reinicia na hora);
 *     - IN = 1           -> ET cresce com o tempo, saturando em PT;
 *     - Q = 1 quando ET >= PT (e IN continua 1).
 *   O usuário declara a variável (alocação estática, como num CLP, sem malloc)
 *   e só a manipula pelas funções abaixo.
 * ########################################################################## */

typedef struct {
    uint32_t pt_ms;   /* preset: tempo de atraso (privado: use ton_iniciar) */
    uint32_t et_ms;   /* elapsed: tempo decorrido (privado: use ton_decorrido) */
    int      q;       /* saída (privado: use ton_saida) */
} Ton;

void     ton_iniciar(Ton *t, uint32_t preset_ms);
int      ton_atualizar(Ton *t, int entrada, uint32_t dt_ms);   /* devolve Q */
int      ton_saida(const Ton *t);
uint32_t ton_decorrido(const Ton *t);

/* ##########################################################################
 * IMPLEMENTAÇÃO  (ton.c)
 * ########################################################################## */

void ton_iniciar(Ton *t, uint32_t preset_ms) {   /* O(1) */
    t->pt_ms = preset_ms;
    t->et_ms = 0;
    t->q = 0;
}

/* Um ciclo de scan: dt_ms é o tempo desde o scan anterior. O(1)
 * Por que saturar ET em PT? Para ET nunca estourar o uint32_t, mesmo que a
 * entrada fique ligada por meses (2^32 ms ≈ 49,7 dias). */
int ton_atualizar(Ton *t, int entrada, uint32_t dt_ms) {
    if (!entrada) {
        t->et_ms = 0;
        t->q = 0;
        return 0;
    }
    if (t->et_ms < t->pt_ms) {
        uint32_t falta = t->pt_ms - t->et_ms;
        t->et_ms += dt_ms < falta ? dt_ms : falta;   /* soma sem passar de PT */
    }
    t->q = t->et_ms >= t->pt_ms;
    return t->q;
}

int      ton_saida(const Ton *t)     { return t->q; }
uint32_t ton_decorrido(const Ton *t) { return t->et_ms; }

/* ##########################################################################
 * USUÁRIO DO TAD (o "programa do CLP")
 * ########################################################################## */

#define SCAN_MS 50u   /* tempo de ciclo do CLP simulado */

/* Barra de progresso ET/PT com 20 colunas. O(20) */
static void barra_progresso(uint32_t et, uint32_t pt) {
    int cheios = pt ? (int)(et * 20u / pt) : 20;
    putchar('[');
    for (int i = 0; i < 20; i++) putchar(i < cheios ? '=' : ' ');
    putchar(']');
}

static void passo_debounce(void) {
    printf("\n== PASSO 1 — Debounce de pressostato: TON com PT = 300 ms, scan = %u ms ==\n",
           SCAN_MS);
    /* Contato que "trepida" no começo (ruído) e depois fica estável. */
    const char *sinal = "0011010011111111111000";
    enum { MAX = 64 };
    char linha_in[MAX], linha_q[MAX];
    int n = 0;
    Ton filtro;
    ton_iniciar(&filtro, 300);

    printf("  ciclo  t(ms)  IN  ET(ms)  progresso               Q   comentário\n");
    for (; sinal[n] != '\0' && n < MAX - 1; n++) {
        int in = sinal[n] == '1';
        int q_antes = ton_saida(&filtro);
        int q = ton_atualizar(&filtro, in, SCAN_MS);
        const char *nota = "";
        if (in && ton_decorrido(&filtro) == SCAN_MS) nota = "IN subiu: começa a contar";
        if (!in && n > 0 && sinal[n - 1] == '1' && !q_antes) nota = "IN caiu antes de PT: zera (ruído filtrado)";
        if (q && !q_antes) nota = "ET chegou a PT: Q liga";
        if (!q && q_antes) nota = "IN caiu: Q desliga na hora";
        printf("  %5d  %5u   %d  %6u  ", n, (unsigned)(n * SCAN_MS), in,
               (unsigned)ton_decorrido(&filtro));
        barra_progresso(ton_decorrido(&filtro), 300);
        printf("  %d   %s\n", q, nota);
        linha_in[n] = in ? '#' : '_';
        linha_q[n] = q ? '#' : '_';
        pausar();
    }
    linha_in[n] = linha_q[n] = '\0';
    printf("\n  Diagrama de tempos (1 coluna = %u ms):\n", SCAN_MS);
    printf("    IN : %s\n", linha_in);
    printf("    Q  : %s\n", linha_q);
    printf("  Os pulsos curtos de IN não chegam a Q: só passa o que dura >= 300 ms.\n");
    pausar();
}

/* Vários TONs independentes: cada variável Ton é uma instância. */
static void passo_varias_instancias(void) {
    printf("\n== PASSO 2 — Três instâncias do TAD rodando no mesmo scan ==\n");
    printf("  T1: bomba liga após 2 s de nível alto      (PT = 2000 ms)\n");
    printf("  T2: alarme se porta aberta por 1 s         (PT = 1000 ms)\n");
    printf("  T3: ventilador liga após 500 ms de calor   (PT =  500 ms)\n\n");

    Ton t[3];
    const uint32_t presets[3] = { 2000, 1000, 500 };
    const char *nomes[3] = { "T1", "T2", "T3" };
    for (int i = 0; i < 3; i++) ton_iniciar(&t[i], presets[i]);

    /* Entradas a cada 250 ms (um quadro por linha para caber na tela). */
    const char *entradas[3] = {
        "0111111111111110",   /* nível alto */
        "0011110000111111",   /* porta */
        "1111000111111100",   /* temperatura alta */
    };
    const uint32_t passo = 250;
    printf("  t(ms) | ");
    for (int i = 0; i < 3; i++) printf("%s IN ET(ms)   Q | ", nomes[i]);
    printf("\n");
    for (int k = 0; entradas[0][k] != '\0'; k++) {
        printf("  %5u | ", (unsigned)(k * passo));
        for (int i = 0; i < 3; i++) {
            int in = entradas[i][k] == '1';
            int q = ton_atualizar(&t[i], in, passo);
            printf("    %d  %6u   %d | ", in, (unsigned)ton_decorrido(&t[i]), q);
        }
        printf("\n");
    }
    pausar();
    printf("\n  Cada Ton guarda o próprio ET: nenhuma variável global compartilhada.\n");
    printf("  Sem o TAD, seriam 3 conjuntos de variáveis soltas (et1, q1, et2, q2...) e a\n");
    printf("  lógica de saturação copiada 3 vezes: um erro corrigido em uma cópia continua\n");
    printf("  nas outras. Com o TAD, corrigir ton_atualizar corrige todos os timers.\n");
    pausar();
}

/* Versão "ingênua", sem TAD: soma ET sem saturar. Existe só para comparação. */
typedef struct { uint32_t et_ms; } ContadorIngenuo;

static int ingenuo_atualizar(ContadorIngenuo *c, uint32_t pt_ms, uint32_t dt_ms) {
    c->et_ms += dt_ms;            /* uint32_t: dá a volta em 2^32 ms ≈ 49,7 dias */
    return c->et_ms >= pt_ms;
}

static void passo_saturacao(void) {
    printf("\n== PASSO 3 — Por que a implementação satura ET: entrada ligada por 52 dias ==\n");
    printf("  Aviso de inspeção: Q = 1 quando o motor está ligado há mais de 8 h (PT = 8 h).\n");
    printf("  IN fica sempre ligado; para acelerar, cada scan simulado vale 1 hora.\n\n");
    const uint32_t hora = 3600000u, pt = 8u * hora;
    Ton tad;
    ContadorIngenuo ing = { 0 };
    ton_iniciar(&tad, pt);
    printf("  %6s  %14s  %3s  %14s  %3s\n", "dia", "ET TAD (ms)", "Q", "ET ingênuo", "Q");
    int q_ing_ant = 0;
    for (int h = 1; h <= 52 * 24; h++) {
        int q_tad = ton_atualizar(&tad, 1, hora);
        int q_ing = ingenuo_atualizar(&ing, pt, hora);
        if (h % (24 * 8) == 0 || (h >= 1192 && h <= 1203)) {
            const char *nota = "";
            if (!q_ing) nota = q_ing_ant ? "   <- ET deu a volta: o aviso SOME sozinho!"
                                         : "   <- aviso continua desligado";
            printf("  %6.2f  %14u  %3d  %14u  %3d%s\n", h / 24.0, (unsigned)ton_decorrido(&tad),
                   q_tad, (unsigned)ing.et_ms, q_ing, nota);
        }
        q_ing_ant = q_ing;
    }
    printf("\n  Bug real e famoso: sistemas que travam após 49,7 dias ligados (contador de ms\n");
    printf("  de 32 bits). No TAD, o cuidado fica num único lugar (ton_atualizar) e todo\n");
    printf("  usuário fica protegido sem nem saber que o problema existe.\n");
    printf("  Custo: O(1) por scan e sizeof(Ton) = %zu bytes por instância.\n", sizeof(Ton));
    pausar();
}

int main(void) {
    modo_interativo = isatty(STDIN_FILENO);
    printf("TAD TEMPORIZADOR TON (IEC 61131-3)\n");
    passo_debounce();
    passo_varias_instancias();
    passo_saturacao();
    printf("\nFim da demonstração.\n");
    return 0;
}
