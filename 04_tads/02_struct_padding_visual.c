/*
 * ============================================================================
 * Arquivo : 02_struct_padding_visual.c
 * Módulo  : 04 — Tipos Abstratos de Dados       (Ementa: Unidade 1.1)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Executar, passo a passo, o "algoritmo" que o compilador usa para dispor os
 *   campos de uma struct na memória: cada campo começa num endereço múltiplo do
 *   seu alinhamento; o que sobra vira padding (bytes desperdiçados). O
 *   programa simula a regra, confere com offsetof() e desenha o mapa de bytes.
 *   Depois mostra o custo em um vetor de registros e por que uma struct sozinha
 *   ainda NÃO é um TAD (não protege as regras dos dados).
 *
 * EXEMPLO REAL
 *   Registro de histórico (data logger) de um supervisório SCADA: cada amostra
 *   guarda tag, valor, qualidade e alarme. Com milhões de amostras, a ordem
 *   dos campos decide quantos megabytes vão para o cartão SD ou para a rede.
 *
 * COMPLEXIDADE (c = número de campos, n = número de registros)
 *   Operação                      Tempo     Espaço
 *   calcular o layout             O(c)      O(1)
 *   acessar r.campo               O(1)      —   (endereço base + offset fixo)
 *   copiar uma struct (a = b)     O(sizeof) —   (copia todos os bytes)
 *   vetor de n registros          —         O(n × sizeof(struct))
 *   Por quê: o offset de cada campo é decidido na compilação, então o acesso é
 *   uma soma. O padding multiplica-se por n quando há muitos registros.
 *
 * COMO EXECUTAR
 *   ./run 02_struct_padding_visual               (ENTER avança cada passo)
 *   ./run 02_struct_padding_visual < /dev/null   (executa direto, sem pausas)
 * ============================================================================
 */
#define _POSIX_C_SOURCE 200809L   /* antes de qualquer #include: libera isatty() em -std=c11 */
#include <stddef.h>
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

/* Registro na ordem "natural" em que alguém escreveria os campos. */
typedef struct {
    char     tag;        /* T: identificador curto do ponto (ex.: 'P' de pressão) */
    double   valor;      /* V: valor em unidade de engenharia */
    char     qualidade;  /* Q: 'G' bom, 'B' ruim (como no OPC) */
    int32_t  instante;   /* I: segundos desde o início do turno */
    uint16_t alarme;     /* A: palavra de bits de alarme */
} RegistroDesordenado;

/* Mesmos campos, do maior alinhamento para o menor. */
typedef struct {
    double   valor;
    int32_t  instante;
    uint16_t alarme;
    char     tag;
    char     qualidade;
} RegistroOrdenado;

/* Descrição de um campo: o que precisamos para desenhar o layout. */
typedef struct {
    const char *nome;
    char        letra;        /* letra usada no mapa de bytes */
    size_t      offset;       /* posição real, informada pelo compilador */
    size_t      tamanho;
    size_t      alinhamento;  /* _Alignof(tipo): o endereço deve ser múltiplo disto */
} Campo;

/* _Alignof só aceita NOME DE TIPO em C11, por isso o tipo do campo vai explícito. */
#define CAMPO(STRUCT, campo, TIPO, letra) \
    { #campo, letra, offsetof(STRUCT, campo), sizeof(TIPO), _Alignof(TIPO) }

/*
 * Simula a regra de layout e compara com o que o compilador fez.
 * Regra: offset do campo = próximo múltiplo do seu alinhamento;
 *        tamanho total   = múltiplo do maior alinhamento (para vetores).  O(c)
 * Devolve o número de bytes de padding.
 */
static size_t simular_layout(const char *nome, const Campo *campos, size_t c, size_t tam_real) {
    char mapa[64];
    size_t pos = 0, maior_alin = 1, padding = 0;

    printf("\n  Layout de %s (sizeof real = %zu bytes)\n", nome, tam_real);
    printf("  %-10s %4s %4s  %-36s %s\n", "campo", "tam", "alin", "posição livre -> decisão",
           "início (offsetof)");
    for (size_t i = 0; i < tam_real && i < sizeof mapa; i++) mapa[i] = '.';

    for (size_t i = 0; i < c; i++) {
        size_t a = campos[i].alinhamento;
        size_t falta = (a - pos % a) % a;     /* bytes até o próximo múltiplo de a */
        if (a > maior_alin) maior_alin = a;
        printf("  %-10s %4zu %4zu  ", campos[i].nome, campos[i].tamanho, a);
        if (falta > 0)
            printf("%2zu não é múlt. de %zu: +%zu padding    ", pos, a, falta);
        else
            printf("%2zu é múltiplo de %zu: sem padding    ", pos, a);
        pos += falta;
        padding += falta;
        printf("%2zu (%2zu) %s\n", pos, campos[i].offset,
               pos == campos[i].offset ? "OK" : "DIFERENTE!");
        for (size_t b = 0; b < campos[i].tamanho && campos[i].offset + b < sizeof mapa; b++)
            mapa[campos[i].offset + b] = campos[i].letra;
        pos += campos[i].tamanho;
    }
    size_t final = (maior_alin - pos % maior_alin) % maior_alin;
    printf("  %-10s termina em %zu; sizeof deve ser múltiplo de %zu -> +%zu padding no final\n",
           "(fim)", pos, maior_alin, final);
    padding += final;
    printf("  total calculado = %zu, sizeof = %zu %s\n", pos + final, tam_real,
           pos + final == tam_real ? "OK" : "DIFERENTE!");

    printf("\n  Mapa de bytes ('.' = padding desperdiçado):");
    for (size_t i = 0; i < tam_real && i < sizeof mapa; i++) {
        if (i % 8 == 0) printf("\n  %3zu: ", i);
        printf("[%c]", mapa[i]);
    }
    printf("\n  -> %zu de %zu bytes são padding (%.0f%%)\n", padding, tam_real,
           100.0 * (double)padding / (double)tam_real);
    return padding;
}

/* Struct transparente usada no último passo: qualquer código pode quebrá-la. */
typedef struct {
    double ganho;     /* regra do projeto: ganho > 0 */
    double minimo;    /* regra do projeto: minimo < maximo */
    double maximo;
} FaixaAtuador;

/* Satura u em [minimo, maximo]. Supõe que as regras acima valem. O(1) */
static double saturar(const FaixaAtuador *f, double u) {
    double y = f->ganho * u;
    if (y < f->minimo) return f->minimo;
    if (y > f->maximo) return f->maximo;
    return y;
}

int main(void) {
    modo_interativo = isatty(STDIN_FILENO);
    printf("STRUCTS NA MEMÓRIA: ALINHAMENTO E PADDING\n");
    printf("Legenda: T=tag  V=valor  Q=qualidade  I=instante  A=alarme\n");

    /* PASSO 1 ------------------------------------------------------------ */
    printf("\n== PASSO 1 — Alinhamento de cada tipo nesta máquina ==\n");
    printf("  char:%zu  uint16_t:%zu  int32_t:%zu  double:%zu\n", _Alignof(char),
           _Alignof(uint16_t), _Alignof(int32_t), _Alignof(double));
    printf("  A CPU lê um double de 8 bytes de uma vez só se o endereço for múltiplo de\n");
    printf("  %zu; senão precisaria de 2 acessos (ou geraria falha em alguns processadores).\n",
           _Alignof(double));
    pausar();

    /* PASSO 2 ------------------------------------------------------------ */
    printf("\n== PASSO 2 — Campos na ordem 'natural' ==\n");
    const Campo desord[] = {
        CAMPO(RegistroDesordenado, tag, char, 'T'),
        CAMPO(RegistroDesordenado, valor, double, 'V'),
        CAMPO(RegistroDesordenado, qualidade, char, 'Q'),
        CAMPO(RegistroDesordenado, instante, int32_t, 'I'),
        CAMPO(RegistroDesordenado, alarme, uint16_t, 'A'),
    };
    simular_layout("RegistroDesordenado", desord, sizeof desord / sizeof desord[0],
                   sizeof(RegistroDesordenado));
    pausar();

    /* PASSO 3 ------------------------------------------------------------ */
    printf("\n== PASSO 3 — Mesmos campos, do maior alinhamento para o menor ==\n");
    const Campo ord[] = {
        CAMPO(RegistroOrdenado, valor, double, 'V'),
        CAMPO(RegistroOrdenado, instante, int32_t, 'I'),
        CAMPO(RegistroOrdenado, alarme, uint16_t, 'A'),
        CAMPO(RegistroOrdenado, tag, char, 'T'),
        CAMPO(RegistroOrdenado, qualidade, char, 'Q'),
    };
    simular_layout("RegistroOrdenado", ord, sizeof ord / sizeof ord[0], sizeof(RegistroOrdenado));
    pausar();

    /* PASSO 4 ------------------------------------------------------------ */
    printf("\n== PASSO 4 — O padding multiplicado por n registros ==\n");
    printf("  %10s  %16s  %16s  %10s\n", "n", "desordenado (B)", "ordenado (B)", "economia");
    for (unsigned long n = 10; n <= 10000000UL; n *= 10) {
        unsigned long a = n * sizeof(RegistroDesordenado), b = n * sizeof(RegistroOrdenado);
        printf("  %10lu  %16lu  %16lu  %9.0f%%\n", n, a, b, 100.0 * (double)(a - b) / (double)a);
    }
    printf("  O padding é O(1) por registro, mas vira O(n) bytes no vetor inteiro.\n");
    printf("  Dica: '__attribute__((packed))' remove padding, mas não é C padrão e deixa o\n");
    printf("  acesso mais lento; para gravar/transmitir, prefira serializar campo a campo.\n");
    pausar();

    /* PASSO 5 ------------------------------------------------------------ */
    printf("\n== PASSO 5 — Acesso e cópia: endereço base + offset ==\n");
    RegistroOrdenado r = { 3.75, 120, 0x0004, 'P', 'G' };
    RegistroOrdenado copia = r;   /* '=' copia TODOS os bytes: O(sizeof) */
    copia.valor = 9.99;
    printf("  &r = %p; &r.alarme = %p (base + %zu)\n", (void *)&r, (void *)&r.alarme,
           offsetof(RegistroOrdenado, alarme));
    printf("  copia.valor alterado para %.2f; r.valor continua %.2f (cópias independentes)\n",
           copia.valor, r.valor);
    printf("  Passar a struct por valor para uma função copia %zu bytes; por ponteiro, %zu.\n",
           sizeof r, sizeof &r);
    pausar();

    /* PASSO 6 ------------------------------------------------------------ */
    printf("\n== PASSO 6 — Struct agrupa dados, mas não protege regras ==\n");
    FaixaAtuador valvula = { 2.0, 0.0, 100.0 };   /* abertura de válvula em % */
    printf("  Faixa correta : ganho=%.1f [%.0f, %.0f] -> saturar(30) = %.1f %%\n",
           valvula.ganho, valvula.minimo, valvula.maximo, saturar(&valvula, 30.0));
    valvula.minimo = 120.0;   /* qualquer trecho do programa pode escrever isto... */
    printf("  Alguém fez 'valvula.minimo = 120' em outro arquivo...\n");
    printf("  Faixa quebrada: ganho=%.1f [%.0f, %.0f] -> saturar(10) = %.1f %%  (absurdo!)\n",
           valvula.ganho, valvula.minimo, valvula.maximo, saturar(&valvula, 10.0));
    printf("\n  A struct não sabe que 'minimo < maximo' é uma regra (invariante).\n");
    printf("  Um TAD resolve isso: os dados só mudam por funções que verificam as regras.\n");
    printf("  Veja 03_tad_sensor_interface.c.\n");
    pausar();

    printf("\nFim da demonstração.\n");
    return 0;
}
