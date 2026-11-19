/*
 * ============================================================================
 * Arquivo : 07_filtro_mediana_sensor.c
 * Módulo  : 08 — Ordenação e Busca            (Ementa: Unidade 4.3 — aplicação)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Usar ORDENAÇÃO para limpar o sinal de um sensor: a cada nova amostra, a
 *   janela das últimas k leituras é copiada, ordenada por INSERÇÃO e o valor
 *   do meio (a MEDIANA) vira a saída do filtro. O aluno vê a janela antes e
 *   depois de ordenar e compara com a MÉDIA MÓVEL, que é arrastada pelos picos.
 *
 * EXEMPLO REAL
 *   Um PT100 ligado a um cartão analógico de CLP sofre picos de interferência
 *   eletromagnética quando um inversor de frequência chaveia ao lado: leituras
 *   isoladas de 99,9 °C ou -40 °C (fio solto). Se o controlador PID usar a
 *   leitura crua, a válvula dá "trancos". O filtro de mediana remove picos
 *   isolados SEM borrar degraus verdadeiros (mudança real de setpoint),
 *   enquanto a média móvel espalha o pico pela janela e atrasa o degrau.
 *   Também é usado em processamento de imagem (ruído "sal e pimenta") e em
 *   sensores ultrassônicos de distância (ecos falsos).
 *
 * COMPLEXIDADE (N = amostras, k = tamanho da janela, ímpar)
 *   Operação                 Melhor   Médio    Pior     Espaço
 *   ordenar janela (inserção) O(k)    O(k²)    O(k²)    O(1)
 *   filtrar N amostras       O(N·k)   O(N·k²)  O(N·k²)  O(k)
 *   média móvel (soma)       O(N)     O(N)     O(N)     O(k)
 *   Por quê: para k = 5 ou 7 a inserção é imbatível (poucos elementos, quase
 *   sem overhead); como a janela muda só 1 elemento por amostra, ela costuma
 *   estar quase ordenada. Para janelas grandes há métodos O(log k) por
 *   amostra (duas heaps ou árvore balanceada) — ligação com 09_arvores.
 *
 * COMO EXECUTAR
 *   ./run 07_filtro_mediana_sensor               (ENTER avança cada passo)
 *   ./run 07_filtro_mediana_sensor < /dev/null   (executa direto, sem pausas)
 * ============================================================================
 */
#define _POSIX_C_SOURCE 200809L   /* antes de qualquer #include: libera isatty() em -std=c11 */
#include <stdio.h>
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

#define K 5                /* tamanho da janela: ímpar, para existir um "meio" */
#define N_AMOSTRAS 32

static long comparacoes;   /* soma das comparações de todas as ordenações */

/* Ordena w[0..m-1] por inserção. Escolhida porque m é pequeno e a janela
 * deslizante está quase ordenada (melhor caso O(m)). O(m²) no pior caso. */
static void insercao(double w[], int m) {
    for (int i = 1; i < m; i++) {
        double x = w[i];
        int j = i - 1;
        while (j >= 0 && (comparacoes++, w[j] > x)) { w[j + 1] = w[j]; j--; }
        w[j + 1] = x;
    }
}

static void mostra(const char *rot, const double w[], int m) {
    printf("      %-9s [", rot);
    for (int i = 0; i < m; i++) printf("%s%5.1f", i ? " " : "", w[i]);
    printf("]");
}

/* Filtro causal (só usa o passado, como num CLP em tempo real): saída da
 * amostra t = mediana de x[t-K+1..t]. No início, com menos de K amostras, usa
 * as que existem (janela parcial). Mantém a janela ORIGINAL intocada: ordena
 * uma CÓPIA — armadilha comum é ordenar o histórico e perder a ordem
 * temporal. Também calcula a média móvel para comparação. O(N·K²). */
static void filtra(const double x[], int n, double med[], double media[], int mostrar_ate) {
    for (int t = 0; t < n; t++) {
        int ini = t - K + 1 < 0 ? 0 : t - K + 1;
        int m = t - ini + 1;                       /* tamanho efetivo da janela */
        double w[K];
        memcpy(w, &x[ini], (size_t)m * sizeof w[0]);

        double soma = 0;
        for (int i = 0; i < m; i++) soma += w[i];
        media[t] = soma / m;

        if (t < mostrar_ate) {
            printf("   t = %2d, chegou %.1f\n", t, x[t]);
            mostra("janela:", w, m);
            printf("\n");
        }
        insercao(w, m);
        /* com m par (só no início), a mediana é a média dos dois centrais */
        med[t] = m % 2 ? w[m / 2] : (w[m / 2 - 1] + w[m / 2]) / 2;
        if (t < mostrar_ate) {
            mostra("ordenada:", w, m);
            printf("  -> mediana = %.1f   (média móvel = %.1f)\n", med[t], media[t]);
            pausar();
        }
    }
}

/* Gráfico ASCII de 20 °C a 36 °C em 48 colunas. '.' = leitura crua,
 * 'a' = média móvel, 'M' = mediana (desenhada por último: vence empates).
 * Valores fora da escala aparecem como '<' ou '>' na borda. */
static void plota_ponto(char linha[], double v, char c) {
    const double lo = 20.0, hi = 36.0;
    const int largura = 48;
    int col;
    if (v < lo)       { col = 0; c = '<'; }
    else if (v > hi)  { col = largura - 1; c = '>'; }
    else              col = (int)((v - lo) / (hi - lo) * (largura - 1) + 0.5);
    if (linha[col] != 'M') linha[col] = c;
}

int main(void) {
    modo_interativo = isatty(STDIN_FILENO);
    printf("FILTRO DE MEDIANA: ORDENAR A JANELA PARA REMOVER RUÍDO IMPULSIVO\n\n");

    /* Temperatura de um tanque (°C): ruído pequeno + picos de EMI +
     * fio solto (-40) + um DEGRAU real em t = 20 (aquecedor ligado). */
    const double ruido[] = {0.2, -0.1, 0.3, 0.0, -0.2, 0.1, -0.3, 0.2};
    double x[N_AMOSTRAS];
    for (int t = 0; t < N_AMOSTRAS; t++) x[t] = (t < 20 ? 25.0 : 31.0) + ruido[t % 8];
    x[3] = 99.9;                    /* pico isolado de EMI                    */
    x[9] = -40.0;                   /* leitura de fio aberto                  */
    x[13] = 88.0; x[14] = 91.0;     /* rajada de 2 picos (ainda filtrada)     */
    x[26] = 95.0; x[27] = 97.0; x[28] = 99.0;   /* 3 seguidos: K=5 NÃO segura */

    double med[N_AMOSTRAS], media[N_AMOSTRAS];

    printf("=== 1) Passo a passo das primeiras amostras (janela K = %d) ===\n\n", K);
    filtra(x, N_AMOSTRAS, med, media, 7);
    printf("\n");

    printf("=== 2) Sinal completo ===\n");
    printf("   escala: 20 °C à esquerda ... 36 °C à direita   . bruto   a média móvel   M mediana\n\n");
    printf("    t   bruto  média  mediana  |\n");
    for (int t = 0; t < N_AMOSTRAS; t++) {
        char linha[49];
        memset(linha, ' ', 48);
        linha[48] = '\0';
        plota_ponto(linha, x[t], '.');
        plota_ponto(linha, media[t], 'a');
        plota_ponto(linha, med[t], 'M');
        printf("   %2d  %6.1f %6.1f  %6.1f   |%s|\n", t, x[t], media[t], med[t], linha);
    }
    printf("\n");
    pausar();

    /* Erro em relação ao valor "verdadeiro" (sem picos). */
    double erro_media = 0, erro_med = 0;
    for (int t = K; t < N_AMOSTRAS; t++) {
        double real = (t < 20 ? 25.0 : 31.0) + ruido[t % 8];
        double em = media[t] - real, ed = med[t] - real;
        erro_media += em < 0 ? -em : em;
        erro_med += ed < 0 ? -ed : ed;
    }
    printf("=== 3) Resultado ===\n");
    printf("   erro absoluto médio (t >= %d): média móvel = %.2f °C   mediana = %.2f °C\n", K,
           erro_media / (N_AMOSTRAS - K), erro_med / (N_AMOSTRAS - K));
    printf("   comparações totais nas %d ordenações: %ld (%.1f por amostra; pior caso K(K-1)/2 = %d)\n\n",
           N_AMOSTRAS, comparacoes, (double)comparacoes / N_AMOSTRAS, K * (K - 1) / 2);
    printf("   Observe:\n");
    printf("   - Picos isolados (t = 3, 9) e a rajada de 2 (t = 13-14) somem na mediana, mas\n");
    printf("     contaminam a média móvel por %d amostras.\n", K);
    printf("   - O degrau real em t = 20 aparece LIMPO na mediana após (K+1)/2 = %d amostras; a média\n", (K + 1) / 2);
    printf("     sobe em rampa.\n");
    printf("   - 3 picos seguidos (t = 26-28) = mais da metade da janela: a mediana também falha.\n");
    printf("     Regra: K = 5 remove até (K-1)/2 = 2 picos consecutivos; aumentar K atrasa a resposta.\n");
    return 0;
}
