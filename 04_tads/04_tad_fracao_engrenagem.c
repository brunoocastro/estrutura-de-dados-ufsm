/*
 * ============================================================================
 * Arquivo : 04_tad_fracao_engrenagem.c
 * Módulo  : 04 — Tipos Abstratos de Dados       (Ementa: Unidade 1.2)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Construir o TAD Fração passo a passo e ver a INVARIANTE em ação: toda
 *   fração criada pelas operações do TAD está sempre na forma canônica
 *   (denominador > 0 e mdc(num, den) = 1). O programa mostra o algoritmo de
 *   Euclides executando a cada simplificação e compara o resultado EXATO da
 *   fração com o acúmulo de erro de float/double.
 *
 *   Diferente de 03_tad_sensor_interface.c (TAD OPACO, sempre usado por
 *   ponteiro), aqui o TAD é passado POR VALOR: a struct é pequena (2 inteiros)
 *   e copiar é barato. A disciplina continua: o usuário só cria e altera
 *   frações pelas funções fracao_*.
 *
 * EXEMPLO REAL
 *   "Engrenagem eletrônica" de servoacionamentos (parâmetros numerador/
 *   denominador em inversores e servodrives): converte pulsos do encoder em
 *   milímetros do eixo. Encoder 4096 pulsos/volta, redutor 15:1, fuso de
 *   5 mm/volta -> 5/(4096 × 15) = 1/12288 mm por pulso. Em float, somar
 *   0,0000813802... milhões de vezes acumula erro de posicionamento; em fração,
 *   não.
 *
 * COMPLEXIDADE (a, b = módulos dos inteiros envolvidos)
 *   Operação              Tempo            Espaço
 *   mdc (Euclides)        O(log min(a,b))  O(1)
 *   fracao_criar          O(log min(a,b))  O(1)   (simplifica)
 *   somar/multiplicar     O(log min(a,b))  O(1)
 *   comparar              O(1)             O(1)   (produto cruzado)
 *   Por quê: cada passo de Euclides troca (a, b) por (b, a mod b) e o resto
 *   cai pelo menos pela metade a cada dois passos (Lamé).
 *
 * COMO EXECUTAR
 *   ./run 04_tad_fracao_engrenagem               (ENTER avança cada passo)
 *   ./run 04_tad_fracao_engrenagem < /dev/null   (executa direto, sem pausas)
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

/* ##########################################################################
 * INTERFACE  (fracao.h)
 * ########################################################################## */

/* Os campos são visíveis (TAD por valor), mas o CONTRATO diz: não os altere
 * diretamente. Invariante: den > 0 e mdc(|num|, den) == 1. */
typedef struct {
    long long num;
    long long den;
} Fracao;

/* Cria num/den simplificada. *ok = 0 se den == 0 (fração inválida). */
Fracao fracao_criar(long long num, long long den, int *ok);
Fracao fracao_somar(Fracao a, Fracao b);
Fracao fracao_multiplicar(Fracao a, Fracao b);
Fracao fracao_inteiro(long long k);                 /* k/1 */
int    fracao_comparar(Fracao a, Fracao b);         /* <0, 0, >0 como strcmp */
double fracao_para_double(Fracao a);
void   fracao_imprimir(Fracao a);
void   fracao_para_texto(Fracao a, char *buf, size_t cap);   /* "num/den" ou "num" */
void   fracao_mostrar_euclides(int ligado);          /* liga a narração do mdc */

/* ##########################################################################
 * IMPLEMENTAÇÃO  (fracao.c)
 * ########################################################################## */

static int narrar_euclides = 0;   /* 'static': variável privada do "módulo" */

void fracao_mostrar_euclides(int ligado) { narrar_euclides = ligado; }

static long long absoluto(long long x) { return x < 0 ? -x : x; }

/* Máximo divisor comum por Euclides: mdc(a, b) = mdc(b, a mod b); mdc(a, 0) = a.
 * O(log min(a, b)). Iterativo para não depender da pilha de chamadas. */
static long long mdc(long long a, long long b) {
    a = absoluto(a);
    b = absoluto(b);
    if (narrar_euclides) printf("      Euclides: mdc(%lld, %lld)", a, b);
    while (b != 0) {
        long long r = a % b;
        a = b;
        b = r;
        if (narrar_euclides) printf(" = mdc(%lld, %lld)", a, b);
    }
    if (narrar_euclides) printf(" = %lld\n", a);
    return a;
}

/* Restaura a invariante: sinal no numerador e termos primos entre si. O(log) */
static Fracao normalizar(long long num, long long den) {
    Fracao f;
    if (den < 0) { num = -num; den = -den; }
    long long g = mdc(num, den);
    if (g == 0) g = 1;            /* só ocorre com 0/0, que fracao_criar recusa */
    f.num = num / g;
    f.den = den / g;
    return f;
}

Fracao fracao_criar(long long num, long long den, int *ok) {
    Fracao zero = { 0, 1 };
    if (den == 0) { if (ok) *ok = 0; return zero; }
    if (ok) *ok = 1;
    return normalizar(num, den);
}

Fracao fracao_inteiro(long long k) { Fracao f = { k, 1 }; return f; }

/* a/b + c/d = (a·(m/b) + c·(m/d)) / m, com m = mmc(b, d).
 * Usar o mmc em vez de b·d mantém os números pequenos (evita overflow). */
Fracao fracao_somar(Fracao a, Fracao b) {
    long long g = mdc(a.den, b.den);
    long long mmc = a.den / g * b.den;
    return normalizar(a.num * (mmc / a.den) + b.num * (mmc / b.den), mmc);
}

/* Simplificação cruzada ANTES de multiplicar: (a/b)·(c/d) com mdc(a,d) e mdc(c,b). */
Fracao fracao_multiplicar(Fracao a, Fracao b) {
    long long g1 = mdc(a.num, b.den), g2 = mdc(b.num, a.den);
    if (g1 == 0) g1 = 1;
    if (g2 == 0) g2 = 1;
    return normalizar((a.num / g1) * (b.num / g2), (a.den / g2) * (b.den / g1));
}

/* Como den > 0 (invariante), comparar a/b com c/d é comparar a·d com c·b. O(1) */
int fracao_comparar(Fracao a, Fracao b) {
    long long esq = a.num * b.den, dir = b.num * a.den;
    return (esq > dir) - (esq < dir);
}

double fracao_para_double(Fracao a) { return (double)a.num / (double)a.den; }

void fracao_para_texto(Fracao a, char *buf, size_t cap) {
    if (a.den == 1) snprintf(buf, cap, "%lld", a.num);
    else            snprintf(buf, cap, "%lld/%lld", a.num, a.den);
}

void fracao_imprimir(Fracao a) {
    char buf[48];
    fracao_para_texto(a, buf, sizeof buf);
    fputs(buf, stdout);
}

/* ##########################################################################
 * USUÁRIO DO TAD
 * ########################################################################## */

int main(void) {
    modo_interativo = isatty(STDIN_FILENO);
    printf("TAD FRAÇÃO: INVARIANTE E ENGRENAGEM ELETRÔNICA\n");

    /* PASSO 1 ------------------------------------------------------------ */
    printf("\n== PASSO 1 — O construtor sempre devolve a forma canônica ==\n");
    fracao_mostrar_euclides(1);
    const long long entradas[][2] = { { 6, 8 }, { 84, -36 }, { 0, 5 }, { 1071, 462 }, { 3, 0 } };
    for (size_t i = 0; i < sizeof entradas / sizeof entradas[0]; i++) {
        int ok;
        printf("  fracao_criar(%lld, %lld)\n", entradas[i][0], entradas[i][1]);
        Fracao f = fracao_criar(entradas[i][0], entradas[i][1], &ok);
        if (!ok) {
            printf("      -> recusada: denominador zero (a invariante proíbe)\n");
            continue;
        }
        printf("      -> ");
        fracao_imprimir(f);
        printf("   (sinal no numerador, termos primos entre si)\n");
    }
    printf("  Consequência: duas frações iguais têm SEMPRE os mesmos campos (6/8 == 3/4).\n");
    pausar();

    /* PASSO 2 ------------------------------------------------------------ */
    printf("\n== PASSO 2 — Montando a engrenagem eletrônica ==\n");
    int ok;
    Fracao fuso = fracao_criar(5, 1, &ok);              /* 5 mm por volta do fuso */
    Fracao redutor = fracao_criar(1, 15, &ok);          /* 1 volta do fuso a cada 15 do motor */
    Fracao encoder = fracao_criar(1, 4096, &ok);        /* 1 volta do motor = 4096 pulsos */
    printf("  mm/pulso = fuso × redutor × encoder = 5 × 1/15 × 1/4096\n");
    Fracao parcial = fracao_multiplicar(fuso, redutor);
    printf("  5 × 1/15 = ");
    fracao_imprimir(parcial);
    printf("\n");
    Fracao mm_por_pulso = fracao_multiplicar(parcial, encoder);
    printf("  1/3 × 1/4096 = ");
    fracao_imprimir(mm_por_pulso);
    printf(" mm por pulso (= %.10f mm)\n", fracao_para_double(mm_por_pulso));
    pausar();

    /* PASSO 3 ------------------------------------------------------------ */
    printf("\n== PASSO 3 — Somar a fração 1 pulso por vez: exato x float x double ==\n");
    fracao_mostrar_euclides(0);   /* milhões de mdc: narração desligada */
    Fracao pos_exata = fracao_inteiro(0);
    float pos_f = 0.0f;
    double pos_d = 0.0;
    const float passo_f = (float)fracao_para_double(mm_por_pulso);
    const double passo_d = fracao_para_double(mm_por_pulso);
    long long pulsos = 0;
    printf("  %10s  %14s  %14s  %16s  %12s\n", "pulsos", "exato (mm)", "float (mm)",
           "double (mm)", "erro float");
    for (long long marco = 12288; marco <= 12288LL * 1000; marco *= 10) {
        while (pulsos < marco) {      /* O(log) por soma, por causa do mdc */
            pos_exata = fracao_somar(pos_exata, mm_por_pulso);
            pos_f += passo_f;
            pos_d += passo_d;
            pulsos++;
        }
        char txt[48];
        fracao_para_texto(pos_exata, txt, sizeof txt);
        printf("  %10lld  %14s  %14.6f  %16.10f  %+12.6f\n", pulsos, txt, (double)pos_f, pos_d,
               (double)pos_f - fracao_para_double(pos_exata));
    }
    printf("  Com 1 m de curso o float já erra mais de 10 cm; a fração continua exata.\n");
    printf("  (Na prática o drive guarda num/den inteiros e um acumulador do resto.)\n");
    pausar();

    /* PASSO 4 ------------------------------------------------------------ */
    printf("\n== PASSO 4 — Comparar sem converter para double ==\n");
    Fracao a = fracao_criar(1, 3, &ok), b = fracao_criar(333333, 1000000, &ok);
    printf("  1/3 ");
    int c = fracao_comparar(a, b);
    printf("%s 333333/1000000  (1·1000000 = 1000000 vs 333333·3 = 999999)\n",
           c > 0 ? ">" : c < 0 ? "<" : "==");
    printf("  Comparação O(1) por produto cruzado; vale porque o denominador é sempre > 0\n");
    printf("  (é a invariante que permite essa implementação simples).\n");
    pausar();

    printf("\nFim da demonstração.\n");
    return 0;
}
