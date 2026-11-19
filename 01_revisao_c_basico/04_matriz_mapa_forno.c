/*
 * ============================================================================
 * Arquivo : 04_matriz_mapa_forno.c
 * Módulo  : 01 — Revisão de C básico          (Ementa: Unidade 0.4)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Ver uma matriz sendo percorrida linha × coluna, com um cursor [ ] na
 *   célula visitada, enquanto o programa procura o ponto mais quente e
 *   calcula a média de cada linha. Depois a matriz vira uma "imagem térmica"
 *   em ASCII e o programa mostra como ela fica guardada na memória
 *   (linha após linha) e quanto custa percorrê-la: O(n·m).
 *
 * EXEMPLO REAL
 *   Uma câmera termográfica (ou uma grade de termopares) mede a temperatura
 *   da esteira de um forno de túnel. Cada célula da matriz é um ponto da
 *   esteira; o CLP precisa achar pontos quentes e zonas frias. Uma imagem de
 *   câmera industrial é exatamente isso: uma matriz de pixels.
 *
 * COMPLEXIDADE (n = linhas, m = colunas)
 *   Operação                    Tempo     Espaço
 *   acessar mapa[i][j]          O(1)      O(1)
 *   achar máximo / médias       O(n·m)    O(1) (O(n) para guardar as médias)
 *   limiarizar (imagem ASCII)   O(n·m)    O(1)
 *   Por quê: cada uma das n·m células é visitada exatamente uma vez; para uma
 *   matriz quadrada n×n isso é O(n²): dobrar a resolução quadruplica o custo.
 *
 * COMO EXECUTAR
 *   ./run 04_matriz_mapa_forno               (ENTER avança cada passo)
 *   ./run 04_matriz_mapa_forno < /dev/null   (executa direto, sem pausas)
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

#define LINHAS  5       /* posições ao longo da largura da esteira */
#define COLUNAS 8       /* posições ao longo do comprimento do forno */

/* Mapa de temperatura (°C). Há um ponto quente em (1,5) e uma borda fria
   na linha 4 (resistência queimada perto da parede?). */
static const int mapa[LINHAS][COLUNAS] = {
    {182, 190, 196, 201, 205, 207, 203, 195},
    {185, 194, 201, 208, 215, 238, 210, 199},
    {186, 195, 203, 210, 214, 216, 211, 200},
    {184, 192, 199, 205, 209, 211, 206, 197},
    {161, 166, 170, 174, 176, 177, 175, 169},
};

/* Desenha a matriz; a célula (cur_i, cur_j) aparece entre colchetes.
   Use cur_i = -1 para desenhar sem cursor. O(n·m). */
static void desenha_mapa(int cur_i, int cur_j) {
    printf("        ");
    for (int j = 0; j < COLUNAS; j++) printf(" col%d ", j);
    printf("\n");
    for (int i = 0; i < LINHAS; i++) {
        printf("  lin%d ", i);
        for (int j = 0; j < COLUNAS; j++) {
            if (i == cur_i && j == cur_j) printf(" [%3d]", mapa[i][j]);
            else                          printf("  %3d ", mapa[i][j]);
        }
        printf("\n");
    }
}

/* Parte 1: varredura linha × coluna com cursor. A primeira linha é mostrada
   célula a célula; as demais, uma linha por passo (senão seriam 40 pausas). */
static void parte1_varredura(void) {
    printf("PARTE 1 — Varredura linha × coluna procurando o ponto mais quente\n");
    printf("================================================================\n");
    printf("for (i = 0; i < %d; i++)          <- laço externo: linhas\n", LINHAS);
    printf("    for (j = 0; j < %d; j++)      <- laço interno: colunas\n\n", COLUNAS);

    int max = mapa[0][0], max_i = 0, max_j = 0;
    long acessos = 0;
    double media_linha[LINHAS];

    for (int i = 0; i < LINHAS; i++) {
        long soma = 0;
        for (int j = 0; j < COLUNAS; j++) {
            acessos++;
            soma += mapa[i][j];
            int novo = mapa[i][j] > max;
            if (novo) { max = mapa[i][j]; max_i = i; max_j = j; }
            if (i == 0) {   /* primeira linha: passo a passo */
                desenha_mapa(i, j);
                printf("  (%d,%d) = %d °C -> %s | máx = %d em (%d,%d) | acessos = %ld\n\n",
                       i, j, mapa[i][j], novo ? "novo máximo" : "sem mudança",
                       max, max_i, max_j, acessos);
                pausar();
            }
        }
        media_linha[i] = (double)soma / COLUNAS;
        if (i > 0) {        /* demais linhas: resumo ao fim da linha */
            desenha_mapa(i, COLUNAS - 1);
            printf("  fim da linha %d: média = %.1f °C | máx até aqui = %d em (%d,%d) | "
                   "acessos = %ld\n\n", i, media_linha[i], max, max_i, max_j, acessos);
            pausar();
        }
    }

    printf("  Ponto mais quente: %d °C na linha %d, coluna %d.\n", max, max_i, max_j);
    printf("  Média por linha (cada linha é uma faixa da esteira):\n");
    for (int i = 0; i < LINHAS; i++) {
        printf("    lin%d %6.1f °C %s\n", i, media_linha[i],
               media_linha[i] < 180.0 ? "<- zona fria: verificar resistência" : "");
    }
    printf("  Acessos totais = %ld = %d linhas × %d colunas  -> O(n·m)\n\n",
           acessos, LINHAS, COLUNAS);
    pausar();
}

/* Converte temperatura em um "pixel" ASCII (limiarização). O(1). */
static char pixel(int t) {
    if (t < 180) return '.';     /* frio */
    if (t < 200) return '-';
    if (t < 212) return '+';
    if (t < 225) return '#';
    return '@';                  /* ponto quente */
}

/* Parte 2: a mesma matriz vista como imagem: cada célula vira um caractere. */
static void parte2_imagem(void) {
    printf("PARTE 2 — A matriz como imagem térmica (limiarização)\n");
    printf("================================================================\n");
    printf("  Legenda: '.' < 180  '-' < 200  '+' < 212  '#' < 225  '@' >= 225 °C\n\n");
    for (int i = 0; i < LINHAS; i++) {
        printf("   ");
        for (int j = 0; j < COLUNAS; j++)
            printf(" %c%c", pixel(mapa[i][j]), pixel(mapa[i][j]));
        printf("\n");
    }
    printf("\n  Uma câmera de 640×480 faz a mesma conta em 307 200 pixels por quadro;\n");
    printf("  a 30 quadros/s são ~9,2 milhões de visitas por segundo: O(n·m) importa.\n\n");
    pausar();
}

/* Parte 3: como a matriz fica na memória (row-major) e por que a ordem dos
   laços importa na prática, mesmo com o mesmo Big O. */
static void parte3_memoria(void) {
    printf("PARTE 3 — Matriz na memória: linha após linha (row-major)\n");
    printf("================================================================\n");
    printf("  Em C, mapa[i][j] fica na posição linear i × COLUNAS + j:\n\n");
    printf("  posição: ");
    for (int i = 0; i < 2; i++)
        for (int j = 0; j < COLUNAS; j++)
            printf("%3d", i * COLUNAS + j);   /* 3 colunas por célula: 24 por linha */
    printf(" ...\n");
    printf("           <------ linha 0 -------><------ linha 1 ------->\n\n");

    printf("  Ordem das visitas nos dois jeitos de escrever os laços (3 primeiras):\n");
    printf("    i fora, j dentro (por linha):  ");
    for (int k = 0; k < 3; k++) printf("mapa[0][%d]=pos %d  ", k, k);
    printf("\n    j fora, i dentro (por coluna): ");
    for (int k = 0; k < 3; k++) printf("mapa[%d][0]=pos %d  ", k, k * COLUNAS);
    printf("\n\n  Os dois são O(n·m), mas por linha anda em posições vizinhas (aproveita a\n");
    printf("  cache do processador); por coluna salta %d posições a cada acesso.\n\n",
           COLUNAS);
    pausar();
}

/* Parte 4: crescimento do custo com a resolução. */
static void parte4_crescimento(void) {
    const int lado[] = {10, 100, 1000, 10000};
    printf("PARTE 4 — Crescimento: acessos para varrer uma matriz lado × lado\n");
    printf("================================================================\n");
    printf("%8s | %16s | %s\n", "lado", "acessos (n·m)", "vs. linha anterior");
    printf("---------+------------------+-------------------\n");
    for (int k = 0; k < 4; k++) {
        long long acessos = (long long)lado[k] * lado[k];
        printf("%8d | %16lld | %s\n", lado[k], acessos, k == 0 ? "-" : "100× mais");
    }
    printf("\n  Lado 10× maior -> 100× mais trabalho: matriz quadrada é O(n²).\n");
}

int main(void) {
    modo_interativo = isatty(STDIN_FILENO);
    printf("Matrizes: mapa de temperatura de um forno de túnel (%d × %d)\n\n",
           LINHAS, COLUNAS);
    parte1_varredura();
    parte2_imagem();
    parte3_memoria();
    parte4_crescimento();
    return 0;
}
