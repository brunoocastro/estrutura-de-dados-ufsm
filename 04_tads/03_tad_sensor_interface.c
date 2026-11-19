/*
 * ============================================================================
 * Arquivo : 03_tad_sensor_interface.c
 * Módulo  : 04 — Tipos Abstratos de Dados       (Ementa: Unidade 1.2)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Mostrar um Tipo Abstrato de Dados (TAD) completo: um conjunto de dados +
 *   as ÚNICAS operações permitidas sobre eles. O arquivo é dividido em três
 *   seções, na ordem em que o compilador as lê:
 *     1) INTERFACE      : o que o usuário do TAD pode ver (o futuro sensor.h);
 *     2) USUÁRIO (main) : código que só conhece a interface;
 *     3) IMPLEMENTAÇÃO  : a struct de verdade e o código das operações
 *                         (o futuro sensor.c).
 *   Como a struct só é definida DEPOIS do main, o compilador impede o main de
 *   acessar os campos: tente escrever 's->valor = 5;' no main e veja o erro
 *   "incomplete type". Isso é encapsulamento garantido pelo compilador.
 *
 * EXEMPLO REAL
 *   Transmissor de pressão 4–20 mA (0–10 bar) e PT100 condicionado 4–20 mA
 *   (0–150 °C) lidos por um CLP: escala linear, detecção de cabo rompido
 *   (< 3,8 mA), alarmes alto/baixo com histerese (evita o alarme "piscar" quando
 *   o valor fica oscilando em cima do limite) e estatísticas da leitura.
 *
 * COMPLEXIDADE (por operação; n = leituras já feitas)
 *   Operação           Tempo   Espaço
 *   sensor_criar       O(1)    O(1)   (um malloc)
 *   sensor_ler         O(1)    O(1)   (média incremental, não guarda as n leituras)
 *   sensor_valor/...   O(1)    O(1)
 *   sensor_destruir    O(1)    —
 *   Por quê: o TAD guarda só o estado necessário (último valor, mín, máx, soma,
 *   contagem, estado do alarme). A média sai de soma/contagem, sem vetor.
 *
 * COMO EXECUTAR
 *   ./run 03_tad_sensor_interface               (ENTER avança cada passo)
 *   ./run 03_tad_sensor_interface < /dev/null   (executa direto, sem pausas)
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

/* ##########################################################################
 * 1) INTERFACE  (em um projeto real: sensor.h)
 *    Só tipos e protótipos. O usuário NÃO vê os campos de Sensor.
 * ########################################################################## */

/* Tipo OPACO: declarado, mas não definido aqui. Só dá para usar Sensor*. */
typedef struct Sensor Sensor;

typedef enum { SENSOR_NORMAL, SENSOR_ALARME_BAIXO, SENSOR_ALARME_ALTO, SENSOR_FALHA } EstadoSensor;

/* Cria um sensor com escala linear [ma_min, ma_max] mA -> [eng_min, eng_max].
 * Devolve NULL se os parâmetros violarem as regras (faixas invertidas,
 * limites de alarme fora da faixa, histerese negativa) ou faltar memória. */
Sensor *sensor_criar(const char *tag, const char *unidade, double ma_min, double ma_max,
                     double eng_min, double eng_max, double alarme_baixo, double alarme_alto,
                     double histerese);
/* Registra uma nova leitura em mA. Atualiza valor, alarmes e estatísticas. */
EstadoSensor sensor_ler(Sensor *s, double corrente_ma);
double       sensor_valor(const Sensor *s);          /* último valor válido */
double       sensor_media(const Sensor *s);          /* média das leituras válidas */
EstadoSensor sensor_estado(const Sensor *s);
const char  *sensor_nome_estado(EstadoSensor e);
void         sensor_imprimir(const Sensor *s);       /* linha com barra ASCII */
void         sensor_resumo(const Sensor *s);
void         sensor_destruir(Sensor *s);

/* ##########################################################################
 * 2) USUÁRIO DO TAD
 *    Daqui para baixo, até a seção 3, só se usa o que está na interface.
 *    s->valor, s->minimo... NÃO compilam aqui: a struct ainda é incompleta.
 * ########################################################################## */

int main(void) {
    modo_interativo = isatty(STDIN_FILENO);
    printf("TAD SENSOR: INTERFACE x IMPLEMENTAÇÃO\n");

    /* PASSO 1: o construtor valida as regras ANTES de existir um objeto. */
    printf("\n== PASSO 1 — Criar: o construtor recusa configurações inválidas ==\n");
    Sensor *ruim = sensor_criar("PT-99", "bar", 4.0, 20.0, 0.0, 10.0, 9.0, 3.0, 0.2);
    printf("  sensor_criar(alarme_baixo=9, alarme_alto=3) -> %s\n",
           ruim == NULL ? "NULL (recusado: baixo >= alto)" : "criado?!");
    sensor_destruir(ruim);   /* destruir(NULL) é seguro, como free(NULL) */

    Sensor *pressao = sensor_criar("PT-101", "bar", 4.0, 20.0, 0.0, 10.0, 2.0, 8.0, 0.5);
    Sensor *temperatura = sensor_criar("TT-205", "°C", 4.0, 20.0, 0.0, 150.0, 20.0, 120.0, 3.0);
    if (pressao == NULL || temperatura == NULL) {
        fprintf(stderr, "falha ao criar sensores\n");
        sensor_destruir(pressao);
        sensor_destruir(temperatura);
        return 1;
    }
    printf("  PT-101: 4–20 mA -> 0–10 bar,  alarmes 2,0 / 8,0 bar, histerese 0,5 bar\n");
    printf("  TT-205: 4–20 mA -> 0–150 °C, alarmes 20 / 120 °C,   histerese 3 °C\n");
    printf("  Duas INSTÂNCIAS independentes do mesmo TAD: cada uma com seu estado.\n");
    pausar();

    printf("\n== PASSO 2 — Leituras do PT-101 (histerese no alarme alto) ==\n");
    printf("  Alarme alto ARMA em >= 8,0 bar e só DESARMA abaixo de 8,0 - 0,5 = 7,5 bar.\n\n");
    /* corrente = 4 + bar × 1,6  (16 mA de faixa / 10 bar) */
    const double correntes[] = { 12.0, 15.2, 16.64, 16.96, 16.48, 16.16, 15.84, 12.0, 6.88, 7.36,
                                 3.1, 12.0 };
    const char *narracao[] = {
        "5,0 bar: valor no meio da faixa",
        "7,0 bar: subindo, ainda abaixo de 8,0",
        "7,9 bar: quase no limite",
        "8,1 bar >= 8,0 -> ARMA alarme alto",
        "caiu para 7,8: sem histerese o alarme piscaria; aqui continua ALTO",
        "7,6 ainda > 7,5: continua ALTO",
        "7,4 < 7,5: DESARMA",
        "volta ao normal",
        "1,8 bar < 2,0 -> ARMA alarme baixo",
        "2,1 bar < 2,5 (2,0 + 0,5): continua BAIXO",
        "3,1 mA < 3,8 mA: cabo rompido -> FALHA (valor anterior é mantido)",
        "cabo reparado: leitura válida de novo",
    };
    const size_t n = sizeof correntes / sizeof correntes[0];
    for (size_t i = 0; i < n; i++) {
        EstadoSensor e = sensor_ler(pressao, correntes[i]);
        printf("  %5.2f mA -> ", correntes[i]);
        sensor_imprimir(pressao);
        printf("           %s: %s\n", sensor_nome_estado(e), narracao[i]);
        pausar();
    }

    /* PASSO 3: a mesma interface servindo outro sensor. */
    printf("\n== PASSO 3 — Mesmo TAD, outra instância (TT-205) ==\n");
    const double temp_ma[] = { 4.5, 8.0, 14.0, 17.0, 17.4, 16.7, 18.9 };
    for (size_t i = 0; i < sizeof temp_ma / sizeof temp_ma[0]; i++) {
        sensor_ler(temperatura, temp_ma[i]);
        printf("  %5.1f mA -> ", temp_ma[i]);
        sensor_imprimir(temperatura);
    }
    printf("  O PT-101 não foi afetado: valor atual = %.2f bar, estado %s.\n",
           sensor_valor(pressao), sensor_nome_estado(sensor_estado(pressao)));
    pausar();

    /* PASSO 4: consultas só pela interface. */
    printf("\n== PASSO 4 — Consultas pela interface ==\n");
    sensor_resumo(pressao);
    sensor_resumo(temperatura);
    printf("\n  O main nunca leu um campo da struct. Se a implementação trocar a média\n");
    printf("  simples por uma média móvel, este código continua igual: só sensor.c muda.\n");
    pausar();

    sensor_destruir(pressao);
    sensor_destruir(temperatura);
    printf("\nFim da demonstração (memória dos dois sensores liberada).\n");
    return 0;
}

/* ##########################################################################
 * 3) IMPLEMENTAÇÃO  (em um projeto real: sensor.c)
 *    Só aqui a struct é conhecida. Funções auxiliares são 'static' para não
 *    vazarem para fora do arquivo (equivale a "privadas").
 * ########################################################################## */

#define CORRENTE_FALHA_MA 3.8   /* abaixo disto: cabo rompido (NAMUR NE43) */

struct Sensor {
    char tag[16];
    char unidade[8];
    double ma_min, ma_max;            /* invariante: ma_min < ma_max */
    double eng_min, eng_max;          /* invariante: eng_min < eng_max */
    double alarme_baixo, alarme_alto; /* invariante: eng_min <= baixo < alto <= eng_max */
    double histerese;                 /* invariante: histerese >= 0 */
    double valor;                     /* último valor válido (eng) */
    double minimo, maximo, soma;      /* estatísticas das leituras válidas */
    unsigned long validas, falhas;
    EstadoSensor estado;
};

/* Copia uma string limitando ao tamanho do destino (strncpy não garante '\0'). O(len) */
static void copiar_texto(char *dest, size_t cap, const char *orig) {
    size_t i = 0;
    for (; orig != NULL && orig[i] != '\0' && i + 1 < cap; i++) dest[i] = orig[i];
    dest[i] = '\0';
}

Sensor *sensor_criar(const char *tag, const char *unidade, double ma_min, double ma_max,
                     double eng_min, double eng_max, double alarme_baixo, double alarme_alto,
                     double histerese) {
    /* Verificar AQUI garante que nenhum Sensor inválido chegue a existir. O(1) */
    if (!(ma_min < ma_max) || !(eng_min < eng_max)) return NULL;
    if (!(eng_min <= alarme_baixo && alarme_baixo < alarme_alto && alarme_alto <= eng_max))
        return NULL;
    if (histerese < 0.0) return NULL;

    Sensor *s = malloc(sizeof *s);
    if (s == NULL) return NULL;
    copiar_texto(s->tag, sizeof s->tag, tag);
    copiar_texto(s->unidade, sizeof s->unidade, unidade);
    s->ma_min = ma_min;             s->ma_max = ma_max;
    s->eng_min = eng_min;           s->eng_max = eng_max;
    s->alarme_baixo = alarme_baixo; s->alarme_alto = alarme_alto;
    s->histerese = histerese;
    s->valor = eng_min;
    s->minimo = eng_max;            /* começam "invertidos" para a 1ª leitura ajustar */
    s->maximo = eng_min;
    s->soma = 0.0;
    s->validas = s->falhas = 0;
    s->estado = SENSOR_NORMAL;
    return s;
}

/* Escala linear (bloco SCALE de CLP). O(1) */
static double escalar(const Sensor *s, double ma) {
    return s->eng_min + (ma - s->ma_min) * (s->eng_max - s->eng_min) / (s->ma_max - s->ma_min);
}

/* Máquina de estados do alarme com histerese. O(1)
 * Para SAIR de um alarme, o valor precisa voltar 'histerese' para dentro da
 * faixa; para ENTRAR, basta tocar o limite. */
static EstadoSensor proximo_estado(const Sensor *s, double v) {
    switch (s->estado) {
    case SENSOR_ALARME_ALTO:
        if (v > s->alarme_alto - s->histerese) return SENSOR_ALARME_ALTO;
        break;
    case SENSOR_ALARME_BAIXO:
        if (v < s->alarme_baixo + s->histerese) return SENSOR_ALARME_BAIXO;
        break;
    default:
        break;
    }
    if (v >= s->alarme_alto) return SENSOR_ALARME_ALTO;
    if (v <= s->alarme_baixo) return SENSOR_ALARME_BAIXO;
    return SENSOR_NORMAL;
}

EstadoSensor sensor_ler(Sensor *s, double corrente_ma) {   /* O(1) */
    if (corrente_ma < CORRENTE_FALHA_MA) {
        s->falhas++;
        s->estado = SENSOR_FALHA;   /* mantém o último valor válido */
        return s->estado;
    }
    double v = escalar(s, corrente_ma);
    if (v < s->eng_min) v = s->eng_min;   /* satura pequenas sobras (3,8–4 mA, >20 mA) */
    if (v > s->eng_max) v = s->eng_max;
    if (s->estado == SENSOR_FALHA) s->estado = SENSOR_NORMAL;  /* recomeça a histerese */
    s->estado = proximo_estado(s, v);
    s->valor = v;
    if (v < s->minimo) s->minimo = v;
    if (v > s->maximo) s->maximo = v;
    s->soma += v;
    s->validas++;
    return s->estado;
}

double sensor_valor(const Sensor *s) { return s->valor; }

double sensor_media(const Sensor *s) {
    return s->validas > 0 ? s->soma / (double)s->validas : s->eng_min;
}

EstadoSensor sensor_estado(const Sensor *s) { return s->estado; }

const char *sensor_nome_estado(EstadoSensor e) {
    switch (e) {
    case SENSOR_NORMAL:       return "NORMAL";
    case SENSOR_ALARME_BAIXO: return "ALARME BAIXO";
    case SENSOR_ALARME_ALTO:  return "ALARME ALTO";
    case SENSOR_FALHA:        return "FALHA";
    }
    return "?";
}

/* Desenha a faixa [eng_min, eng_max] com 'L' e 'H' nos limites e '#' até o valor. O(L) */
void sensor_imprimir(const Sensor *s) {
    enum { L = 40 };
    double span = s->eng_max - s->eng_min;
    int pos = (int)((s->valor - s->eng_min) / span * (L - 1) + 0.5);
    int pl = (int)((s->alarme_baixo - s->eng_min) / span * (L - 1) + 0.5);
    int ph = (int)((s->alarme_alto - s->eng_min) / span * (L - 1) + 0.5);
    printf("%-6s %7.2f %-3s |", s->tag, s->valor, s->unidade);
    for (int i = 0; i < L; i++) {
        char c = i <= pos ? '#' : ' ';
        if (i == pl) c = 'L';
        if (i == ph) c = 'H';
        putchar(c);
    }
    printf("| %s\n", sensor_nome_estado(s->estado));
}

void sensor_resumo(const Sensor *s) {
    printf("  %-6s válidas=%lu falhas=%lu  mín=%.2f  máx=%.2f  média=%.2f %s  estado=%s\n",
           s->tag, s->validas, s->falhas, s->validas ? s->minimo : s->eng_min,
           s->validas ? s->maximo : s->eng_min, sensor_media(s), s->unidade,
           sensor_nome_estado(s->estado));
}

void sensor_destruir(Sensor *s) { free(s); }
