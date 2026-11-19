/*
 * ============================================================================
 * Arquivo : 02_teste_de_mesa_lacos.c
 * Módulo  : 01 — Revisão de C básico          (Ementa: Unidade 0.2 e 0.3)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Fazer o "teste de mesa" de laços na tela: a cada volta, o valor de cada
 *   variável e o resultado da condição de parada.
 *     Parte A (for):   média móvel de 4 amostras de um sensor ruidoso,
 *                      versão ingênua contra versão com soma acumulada.
 *     Parte B (while): ciclo de varredura de um CLP que controla a bomba de
 *                      um tanque com histerese, até o botão de parada.
 *     Parte C (do-while): leitura que precisa acontecer ao menos uma vez.
 *
 * EXEMPLO REAL
 *   A média móvel filtra ruído de sensores (nível, vazão, temperatura) em
 *   CLPs e microcontroladores. O ciclo de varredura (ler entradas -> executar
 *   lógica -> escrever saídas) é o laço que todo CLP executa sem parar.
 *
 * COMPLEXIDADE (n = amostras, k = tamanho da janela, c = ciclos)
 *   Operação                    Por amostra   Total      Espaço
 *   média móvel ingênua         O(k)          O(n·k)     O(k)
 *   média móvel incremental     O(1)          O(n)       O(k)
 *   varredura do CLP            O(1)/ciclo    O(c)       O(1)
 *   Por quê: a versão ingênua soma a janela inteira a cada amostra; a
 *   incremental só soma a amostra que entra e subtrai a que sai.
 *
 * COMO EXECUTAR
 *   ./run 02_teste_de_mesa_lacos               (ENTER avança cada passo)
 *   ./run 02_teste_de_mesa_lacos < /dev/null   (executa direto, sem pausas)
 * ============================================================================
 */
#define _POSIX_C_SOURCE 200809L   /* antes de qualquer #include: libera isatty() em -std=c11 */
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

#define N_AMOSTRAS 10
#define JANELA     4

/* ---------------------------------------------------------------------------
 * PARTE A — laço for: média móvel
 * ------------------------------------------------------------------------- */

/* Leituras de um sensor de nível (%), com um pico de ruído em i = 5. */
static const double leituras[N_AMOSTRAS] = {50.0, 52.0, 51.0, 53.0, 52.0,
                                             80.0, 54.0, 53.0, 55.0, 54.0};

/* Imprime a janela atual (as últimas 'qtd' amostras até a posição i). */
static void mostra_janela(int i, int qtd) {
    printf("[");
    for (int j = i - qtd + 1; j <= i; j++)
        printf("%5.1f", leituras[j]);
    for (int j = qtd; j < JANELA; j++)   /* janela ainda incompleta */
        printf("   - ");
    printf(" ]");
}

static void parte_a_for(void) {
    printf("PARTE A — laço for: média móvel de %d amostras\n", JANELA);
    printf("================================================================\n");
    printf("for (i = 0; i < %d; i++) { soma += v[i]; if (i >= %d) soma -= v[i-%d]; ... }\n\n",
           N_AMOSTRAS, JANELA, JANELA);
    printf(" i | leitura | janela                  |  soma  | média | i<%d?\n",
           N_AMOSTRAS);
    printf("---+---------+-------------------------+--------+-------+------\n");

    double soma = 0.0;                 /* soma da janela, mantida entre voltas */
    long ops_incremental = 0, ops_ingenua = 0;

    for (int i = 0; i < N_AMOSTRAS; i++) {
        soma += leituras[i];           /* amostra que entra */
        ops_incremental++;
        if (i >= JANELA) {             /* janela cheia: a mais antiga sai */
            soma -= leituras[i - JANELA];
            ops_incremental++;
        }
        int qtd = i + 1 < JANELA ? i + 1 : JANELA;

        /* Versão ingênua, só para contar: somaria a janela inteira de novo. */
        ops_ingenua += qtd;

        printf("%2d | %7.1f | ", i, leituras[i]);
        mostra_janela(i, qtd);
        printf(" | %6.1f | %5.1f | sim\n", soma, soma / qtd);
        if (i == 5)
            printf("   |  -> pico de ruído (80): a média sobe só para %.1f, não para 80\n",
                   soma / qtd);
        pausar();
    }
    /* Armadilha: a condição é testada mais uma vez e dá falso, encerrando. */
    printf("%2d |   (condição i < %d é falsa: o laço termina sem executar o corpo)\n\n",
           N_AMOSTRAS, N_AMOSTRAS);

    printf("Operações de soma/subtração: incremental = %ld, ingênua = %ld.\n",
           ops_incremental, ops_ingenua);
    printf("Com n = 1 000 000 amostras e janela k = 100: incremental ~2 milhões,\n");
    printf("ingênua ~100 milhões (O(n) contra O(n·k)).\n");
    printf("Armadilha: usar i <= %d leria v[%d], fora do vetor (off-by-one).\n\n",
           N_AMOSTRAS, N_AMOSTRAS);
    pausar();
}

/* ---------------------------------------------------------------------------
 * PARTE B — laço while: ciclo de varredura do CLP
 * ------------------------------------------------------------------------- */

#define NIVEL_LIGA     30.0    /* abaixo disso a bomba liga  */
#define NIVEL_DESLIGA  80.0    /* acima disso a bomba desliga */
#define MAX_CICLOS     40      /* watchdog didático: nunca deixe um laço sem saída garantida */

static void parte_b_while(void) {
    printf("PARTE B — laço while: ciclo de varredura de um CLP\n");
    printf("================================================================\n");
    printf("while (!parada && ciclo < %d) { ler entradas; lógica; escrever saídas; }\n",
           MAX_CICLOS);
    printf("Lógica: bomba liga se nível < %.0f%%, desliga se nível > %.0f%% (histerese).\n",
           NIVEL_LIGA, NIVEL_DESLIGA);
    printf("Entre os limites, a bomba MANTÉM o estado anterior (memória do laço).\n\n");
    printf("ciclo | nível | bomba | parada? | decisão\n");
    printf("------+-------+-------+---------+--------------------------------\n");

    double nivel = 45.0;      /* estado do processo (simulado) */
    int bomba = 0;            /* saída: 0 = desligada, 1 = ligada */
    int parada = 0;           /* entrada: botão de parada */
    int ciclo = 0;

    while (!parada && ciclo < MAX_CICLOS) {
        /* 1) Leitura das entradas: o operador aperta "parada" no ciclo 18. */
        parada = (ciclo == 18);

        /* 2) Lógica: if/else if sem else final = histerese. */
        const char *decisao;
        if (parada) {
            bomba = 0;
            decisao = "PARADA: desliga tudo";
        } else if (nivel < NIVEL_LIGA) {
            bomba = 1;
            decisao = "nível baixo -> liga bomba";
        } else if (nivel > NIVEL_DESLIGA) {
            bomba = 0;
            decisao = "nível alto -> desliga bomba";
        } else {
            decisao = "entre limites -> mantém";
        }

        /* 3) Escrita das saídas e evolução do processo simulado:
              bomba ligada enche 9%/ciclo, consumo constante tira 4%/ciclo. */
        printf("%5d | %5.1f | %-5s | %-7s | %s\n",
               ciclo, nivel, bomba ? "ON" : "off", parada ? "sim" : "nao", decisao);
        nivel += (bomba ? 9.0 : 0.0) - 4.0;
        ciclo++;
        pausar();
    }
    printf("\nO laço saiu porque a condição (!parada && ciclo < %d) ficou falsa\n",
           MAX_CICLOS);
    printf("no teste seguinte ao ciclo %d (parada = %d).\n", ciclo - 1, parada);
    printf("Armadilha: sem 'ciclo++' ou sem ler 'parada' DENTRO do laço, a condição\n");
    printf("nunca muda e o programa trava (laço infinito; num CLP, o watchdog dispara).\n\n");
    pausar();
}

/* ---------------------------------------------------------------------------
 * PARTE C — laço do-while: executa ao menos uma vez
 * ------------------------------------------------------------------------- */

static void parte_c_do_while(void) {
    printf("PARTE C — laço do-while: autoteste na energização\n");
    printf("================================================================\n");
    printf("O equipamento testa o sensor ANTES de decidir se repete o teste;\n");
    printf("por isso o corpo roda pelo menos uma vez.\n\n");

    /* Respostas simuladas do sensor: as duas primeiras falham. */
    const int resposta_ok[] = {0, 0, 1};
    int tentativa = 0, ok;
    do {
        ok = resposta_ok[tentativa];
        printf("  tentativa %d: sensor %s\n", tentativa + 1, ok ? "OK" : "sem resposta");
        tentativa++;
        pausar();
    } while (!ok && tentativa < 3);
    printf("  -> %s após %d tentativa(s).\n\n",
           ok ? "sensor liberado" : "falha de sensor", tentativa);

    printf("Resumo: for quando se sabe quantas voltas (n amostras); while quando a\n");
    printf("saída depende de um evento (botão, nível); do-while quando o corpo precisa\n");
    printf("rodar antes do primeiro teste.\n");
}

int main(void) {
    modo_interativo = isatty(STDIN_FILENO);
    printf("Teste de mesa de laços (for, while, do-while)\n\n");
    parte_a_for();
    parte_b_while();
    parte_c_do_while();
    return 0;
}
