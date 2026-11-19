/*
 * ============================================================================
 * Arquivo : 03_vetores_leituras_sensor.c
 * Módulo  : 01 — Revisão de C básico          (Ementa: Unidade 0.4)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Ver um vetor sendo percorrido índice a índice (marcador ^i), enquanto o
 *   programa atualiza máximo, mínimo e soma de leituras de temperatura.
 *   Mostra também: acesso por índice O(1) contra busca O(n), a armadilha de
 *   iniciar o máximo com 0 e o crescimento linear das comparações.
 *
 * EXEMPLO REAL
 *   Um inversor guarda as últimas leituras de temperatura do motor em um
 *   vetor (buffer de amostras). A IHM mostra máximo, mínimo e média do
 *   turno, e quantas amostras passaram do limite de alarme.
 *
 * COMPLEXIDADE (n = número de leituras)
 *   Operação                    Melhor   Médio   Pior    Espaço
 *   acessar v[k]                O(1)     O(1)    O(1)    O(1)
 *   máx/mín/soma (uma passada)  O(n)     O(n)    O(n)    O(1)
 *   primeira leitura > limite   O(1)     O(n)    O(n)    O(1)
 *   Por quê: v[k] é calculado direto (início + k × tamanho do elemento);
 *   máximo e mínimo exigem olhar TODAS as leituras, pois qualquer uma pode
 *   ser a maior; a busca pode parar cedo se o evento ocorrer no início.
 *
 * COMO EXECUTAR
 *   ./run 03_vetores_leituras_sensor               (ENTER avança cada passo)
 *   ./run 03_vetores_leituras_sensor < /dev/null   (executa direto, sem pausas)
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

#define N_LEITURAS  12
#define LIMITE_ALARME 70
#define N_MAX_TESTE 10000

/* Desenha o vetor e um marcador ^ embaixo da posição i (i < 0: sem marcador). */
static void desenha_vetor(const int v[], int n, int i) {
    printf("   idx:");
    for (int k = 0; k < n; k++) printf("%5d", k);
    printf("\n     v:");
    for (int k = 0; k < n; k++) printf("%5d", v[k]);
    printf("\n       ");
    for (int k = 0; k < n; k++) printf("%5s", k == i ? "^i" : "");
    printf("\n");
}

/* Resultados da passada. A struct só agrupa os valores para a função poder
   devolver todos de uma vez com 'return' (structs: ../04_tads/; devolver
   resultados por ponteiro: ../03_revisao_sistematica_c/). */
typedef struct {
    int maior;
    int menor;
    long soma;
    long comparacoes;
} Estatisticas;

/* Uma única passada calcula máximo, mínimo e soma.
   Pré-condição: n >= 1 (com vetor vazio não existe máximo).
   O(n) tempo, O(1) espaço; também conta as comparações feitas. */
static Estatisticas estatisticas(const int v[], int n, int mostrar) {
    Estatisticas e;
    /* Por que começar com v[0] e não com 0? Ver a armadilha na Parte 3. */
    e.maior = v[0];
    e.menor = v[0];
    e.soma = v[0];
    e.comparacoes = 0;
    if (mostrar) {
        desenha_vetor(v, n, 0);
        printf("   i=0: começa com maior = menor = soma = v[0] = %d\n", v[0]);
        pausar();
    }
    for (int i = 1; i < n; i++) {
        e.soma += v[i];
        e.comparacoes++;
        int novo_maior = v[i] > e.maior;
        if (novo_maior) e.maior = v[i];
        int novo_menor = 0;
        if (!novo_maior) {         /* se é o novo maior, não pode ser o novo menor */
            e.comparacoes++;
            novo_menor = v[i] < e.menor;
            if (novo_menor) e.menor = v[i];
        }
        if (mostrar) {
            desenha_vetor(v, n, i);
            printf("   i=%d: v[%d]=%d -> %s | maior=%d menor=%d soma=%ld | comparações=%ld\n",
                   i, i, v[i],
                   novo_maior ? "NOVO MÁXIMO" : novo_menor ? "NOVO MÍNIMO" : "sem mudança",
                   e.maior, e.menor, e.soma, e.comparacoes);
            pausar();
        }
    }
    return e;
}

/* Busca sequencial do primeiro índice com valor > limite; -1 se não houver.
   O(1) no melhor caso (logo a primeira), O(n) no pior (nenhuma).
   Número de testes feitos: pos + 1 se achou, n se não achou. */
static int primeira_acima(const int v[], int n, int limite) {
    for (int i = 0; i < n; i++)
        if (v[i] > limite) return i;
    return -1;
}

static void parte1_uma_passada(const int temp[]) {
    printf("PARTE 1 — Máximo, mínimo e média em uma passada\n");
    printf("================================================================\n");
    printf("Temperatura do motor (°C), uma leitura por minuto:\n\n");
    Estatisticas e = estatisticas(temp, N_LEITURAS, 1);
    printf("\n   Resultado: máx = %d °C, mín = %d °C, média = %.1f °C\n",
           e.maior, e.menor, (double)e.soma / N_LEITURAS);
    printf("   Comparações: %ld (no máximo 2(n-1) = %d) -> O(n)\n\n",
           e.comparacoes, 2 * (N_LEITURAS - 1));
    pausar();
}

static void parte2_acesso_vs_busca(const int temp[]) {
    printf("PARTE 2 — Acesso por índice O(1) contra busca O(n)\n");
    printf("================================================================\n");
    printf("   \"Qual era a temperatura no minuto 7?\" -> temp[7] = %d °C (1 acesso,\n",
           temp[7]);
    printf("   o endereço é calculado: início + 7 × sizeof(int) = início + %zu bytes)\n",
           7 * sizeof(int));
    int pos = primeira_acima(temp, N_LEITURAS, LIMITE_ALARME);
    printf("   \"Quando passou de %d °C pela primeira vez?\" -> ", LIMITE_ALARME);
    if (pos >= 0)
        printf("minuto %d, após %d testes\n", pos, pos + 1);
    else
        printf("nunca, após %d testes (pior caso)\n", N_LEITURAS);
    printf("   Saber o ÍNDICE é barato; descobrir ONDE está um valor custa uma varredura.\n");
    printf("   (Busca binária, O(log n), exige vetor ordenado: ver ../08_ordenacao_busca/)\n\n");
    pausar();
}

static void parte3_armadilha(void) {
    printf("PARTE 3 — Armadilha: iniciar o máximo com 0\n");
    printf("================================================================\n");
    const int freezer[5] = {-18, -21, -19, -17, -20};   /* câmara fria, °C */
    int maior_errado = 0;          /* ERRADO: 0 é maior que todas as leituras */
    for (int i = 0; i < 5; i++)
        if (freezer[i] > maior_errado) maior_errado = freezer[i];
    int maior_certo = freezer[0];  /* CERTO: começa com um elemento do vetor */
    for (int i = 1; i < 5; i++)
        if (freezer[i] > maior_certo) maior_certo = freezer[i];
    desenha_vetor(freezer, 5, -1);
    printf("   maior iniciado com 0    -> %d °C (nenhuma leitura vale 0!)\n", maior_errado);
    printf("   maior iniciado com v[0] -> %d °C (correto)\n\n", maior_certo);
    pausar();
}

/* Gera n leituras determinísticas entre 20 e 89 (gerador congruente linear),
   para a tabela dar sempre os mesmos números. */
static void gera_leituras(int v[], int n) {
    unsigned int semente = 12345u;
    for (int i = 0; i < n; i++) {
        semente = semente * 1103515245u + 12345u;
        v[i] = 20 + (int)((semente >> 16) % 70u);
    }
}

static void parte4_crescimento(void) {
    /* static: 10000 ints (40 KB) ficam fora da pilha. */
    static int v[N_MAX_TESTE];
    const int tamanhos[] = {10, 100, 1000, 10000};
    printf("PARTE 4 — Crescimento: comparações para máx/mín\n");
    printf("================================================================\n");
    /* Cabeçalho escrito à mão: letras acentuadas ocupam 2 bytes e
       desalinhariam um "%12s". */
    printf("       n |  comparações |      comp. / n | acesso v[k]\n");
    printf("---------+--------------+----------------+------------\n");
    for (int t = 0; t < 4; t++) {
        int n = tamanhos[t];
        gera_leituras(v, n);
        Estatisticas e = estatisticas(v, n, 0);
        printf("%8d | %12ld | %14.2f | 1\n", n, e.comparacoes, (double)e.comparacoes / n);
    }
    printf("\nA razão comparações/n fica quase constante (no máximo 2): crescimento\n");
    printf("linear, O(n).\n");
    printf("O acesso por índice continua 1, seja qual for n: O(1).\n");
}

int main(void) {
    modo_interativo = isatty(STDIN_FILENO);
    const int temp[N_LEITURAS] = {48, 52, 55, 61, 58, 66, 72, 69, 45, 63, 60, 57};
    printf("Vetores: leituras de sensor de temperatura\n\n");
    parte1_uma_passada(temp);
    parte2_acesso_vs_busca(temp);
    parte3_armadilha();
    parte4_crescimento();
    return 0;
}
