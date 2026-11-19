/*
 * ============================================================================
 * Arquivo : 01_tipos_primitivos_sensores.c
 * Módulo  : 04 — Tipos Abstratos de Dados       (Ementa: Unidade 1.1)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Ver, passo a passo, o que um tipo primitivo realmente é: um número fixo de
 *   bytes com uma regra de interpretação. O programa mostra tamanhos e faixas,
 *   converte a leitura crua de um ADC em grandeza de engenharia, desenha os
 *   bits de um int16_t negativo (complemento de dois), mostra a ordem dos
 *   bytes na memória (endianness), o "dar a volta" de um contador uint16_t e
 *   os limites de precisão do float.
 *
 * EXEMPLO REAL
 *   - ADC de 12 bits de um microcontrolador lendo um transmissor 0–10 bar.
 *   - Célula de carga com conversor de 16 bits COM sinal (tração/compressão).
 *   - Registradores Modbus de 16 bits (ordem de bytes importa na rede).
 *   - Contador de pulsos de encoder de 16 bits que "estoura" a cada 65536 pulsos.
 *   - Totalizador de produção guardado em float que para de contar.
 *
 * COMPLEXIDADE (n = número de amostras, b = número de bits do tipo)
 *   Operação                      Tempo     Espaço
 *   converter 1 leitura ADC       O(1)      O(1)
 *   converter n leituras          O(n)      O(1) extra
 *   desenhar os bits de um valor  O(b)      O(1)
 *   Por quê: tipos primitivos têm tamanho fixo; a CPU opera neles em uma
 *   instrução. O custo só cresce quando repetimos para muitas amostras.
 *   Mas a ESCOLHA do tipo muda a memória total: n amostras × sizeof(tipo).
 *
 * COMO EXECUTAR
 *   ./run 01_tipos_primitivos_sensores               (ENTER avança cada passo)
 *   ./run 01_tipos_primitivos_sensores < /dev/null   (executa direto, sem pausas)
 * ============================================================================
 */
#define _POSIX_C_SOURCE 200809L   /* antes de qualquer #include: libera isatty() em -std=c11 */
#include <float.h>
#include <inttypes.h>
#include <limits.h>
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

static void titulo(int passo, const char *texto) {
    printf("\n==========================================================================\n");
    printf(" PASSO %d — %s\n", passo, texto);
    printf("==========================================================================\n");
}

/*
 * Imprime os 'nbits' menos significativos de 'v', agrupados de 4 em 4.
 * Recebe uint32_t (sem sinal) de propósito: deslocar bits de valores
 * negativos com sinal tem comportamento definido pela implementação.  O(b)
 */
static void imprimir_bits(uint32_t v, int nbits) {
    for (int i = nbits - 1; i >= 0; i--) {
        putchar((v >> i) & 1u ? '1' : '0');
        if (i % 4 == 0 && i != 0) putchar(' ');
    }
}

/*
 * Mostra os bytes de um objeto na ordem em que estão na memória
 * (do menor endereço para o maior). Qualquer objeto pode ser lido como
 * vetor de unsigned char: é a única "espiada" garantida pelo padrão C.  O(tam)
 */
static void imprimir_bytes(const void *obj, size_t tam) {
    const unsigned char *p = obj;
    for (size_t i = 0; i < tam; i++) printf("[%02X]", p[i]);
}

/* Barra horizontal proporcional a 'valor' em [0, max], com 'largura' colunas. O(largura) */
static void barra(double valor, double max, int largura) {
    int cheios = (int)(valor / max * largura + 0.5);
    if (cheios < 0) cheios = 0;
    if (cheios > largura) cheios = largura;
    putchar('|');
    for (int i = 0; i < largura; i++) putchar(i < cheios ? '#' : ' ');
    putchar('|');
}

/* --------------------------------------------------------------------------
 * PASSO 1: tamanhos e faixas. sizeof dá o tamanho NESTA máquina; os tipos de
 * <stdint.h> (int16_t, uint32_t...) têm tamanho exato em qualquer máquina,
 * por isso são os preferidos em firmware e protocolos industriais.
 * -------------------------------------------------------------------------- */
/* Uma linha da tabela; a faixa chega pronta (montada com snprintf) para alinhar. */
static void linha_tipo(const char *tipo, size_t bytes, const char *faixa, const char *uso) {
    printf("  %-10s %5zu   %-44s %s\n", tipo, bytes, faixa, uso);
}

static void passo_tamanhos(void) {
    char f[64];
    titulo(1, "Tamanho e faixa dos tipos primitivos nesta máquina");
    printf("  %-10s %5s   %-44s %s\n", "tipo", "bytes", "faixa", "uso típico");
    snprintf(f, sizeof f, "%d .. %d", CHAR_MIN, CHAR_MAX);
    linha_tipo("char", sizeof(char), f, "caractere, byte de protocolo");
    snprintf(f, sizeof f, "%d .. %d", INT_MIN, INT_MAX);
    linha_tipo("int", sizeof(int), f, "contador, índice (tamanho varia)");
    snprintf(f, sizeof f, "%lld .. %lld", LLONG_MIN, LLONG_MAX);
    linha_tipo("long long", sizeof(long long), f, "totalizador grande");
    snprintf(f, sizeof f, "0 .. %u", (unsigned)UINT8_MAX);
    linha_tipo("uint8_t", sizeof(uint8_t), f, "byte de status, 8 bits de I/O");
    snprintf(f, sizeof f, "%d .. %d", INT16_MIN, INT16_MAX);
    linha_tipo("int16_t", sizeof(int16_t), f, "ADC com sinal, registrador Modbus");
    snprintf(f, sizeof f, "0 .. %u", (unsigned)UINT16_MAX);
    linha_tipo("uint16_t", sizeof(uint16_t), f, "ADC 12/16 bits, contador de encoder");
    snprintf(f, sizeof f, "0 .. %" PRIu32, UINT32_MAX);
    linha_tipo("uint32_t", sizeof(uint32_t), f, "tempo em ms desde o boot");
    snprintf(f, sizeof f, "~%d algarismos; |x| <= %g", FLT_DIG, (double)FLT_MAX);
    linha_tipo("float", sizeof(float), f, "grandeza de engenharia");
    snprintf(f, sizeof f, "~%d algarismos; |x| <= %g", DBL_DIG, DBL_MAX);
    linha_tipo("double", sizeof(double), f, "cálculo, acumulador");
    printf("\n  Um tipo = quantidade de bytes + regra de interpretação desses bits.\n");
    printf("  Os mesmos 2 bytes 0xFFFF valem 65535 como uint16_t e -1 como int16_t.\n");
    pausar();
}

/* --------------------------------------------------------------------------
 * PASSO 2: do valor cru do ADC (inteiro) para a grandeza física (real).
 * A conversão linear é a mesma de um bloco SCALE de CLP:
 *     eng = eng_min + (cru - cru_min) * (eng_max - eng_min) / (cru_max - cru_min)
 * -------------------------------------------------------------------------- */
static double adc_para_bar(uint16_t cru) {   /* O(1) */
    const double CRU_MAX = 4095.0;           /* 12 bits: 2^12 - 1 */
    const double BAR_MAX = 10.0;
    return cru * BAR_MAX / CRU_MAX;
}

static void passo_adc(void) {
    titulo(2, "ADC de 12 bits (uint16_t) -> pressão em bar (double)");
    printf("  Transmissor 0–10 bar ligado a um ADC de 12 bits: cru = 0..4095.\n");
    printf("  Resolução = 10 bar / 4095 = %.5f bar por contagem (menor variação visível).\n\n",
           10.0 / 4095.0);

    const uint16_t leituras[] = { 0, 1, 1024, 2048, 3071, 4095 };
    const size_t n = sizeof leituras / sizeof leituras[0];
    for (size_t i = 0; i < n; i++) {
        double bar = adc_para_bar(leituras[i]);
        printf("  cru = %4u  bits = ", (unsigned)leituras[i]);
        imprimir_bits(leituras[i], 12);
        printf("  -> %6.3f bar ", bar);
        barra(bar, 10.0, 20);
        putchar('\n');
    }
    printf("\n  Armadilha: 'cru * 10 / 4095' com tudo inteiro trunca: 1024*10/4095 = %d bar!\n",
           1024 * 10 / 4095);
    printf("  Por isso a conversão usa constantes double: 1024*10.0/4095 = %.3f bar.\n",
           1024 * 10.0 / 4095);
    pausar();
}

/* --------------------------------------------------------------------------
 * PASSO 3: complemento de dois e ordem dos bytes.
 * -------------------------------------------------------------------------- */
static void passo_complemento_e_bytes(void) {
    titulo(3, "int16_t com sinal: complemento de dois e bytes na memória");
    printf("  Célula de carga: -32768 = compressão máxima, +32767 = tração máxima.\n");
    printf("  Regra do complemento de dois: -x = (inverte todos os bits de x) + 1.\n\n");

    const int16_t valores[] = { 0, 1, -1, 300, -300, INT16_MAX, INT16_MIN };
    const size_t n = sizeof valores / sizeof valores[0];
    for (size_t i = 0; i < n; i++) {
        int16_t v = valores[i];
        /* Converter negativo para unsigned é bem definido (soma 2^16). */
        uint16_t padrao = (uint16_t)v;
        printf("  %6d  bits = ", v);
        imprimir_bits(padrao, 16);
        printf("  hex = 0x%04X  memória: ", (unsigned)padrao);
        imprimir_bytes(&v, sizeof v);
        putchar('\n');
    }

    uint16_t teste = 0x0102;
    const unsigned char *primeiro = (const unsigned char *)&teste;
    printf("\n  0x0102 na memória: ");
    imprimir_bytes(&teste, sizeof teste);
    printf(" -> esta máquina é %s.\n",
           *primeiro == 0x02 ? "LITTLE-ENDIAN (byte menos significativo primeiro)"
                             : "BIG-ENDIAN (byte mais significativo primeiro)");
    printf("  Modbus transmite cada registrador em big-endian: o código que monta o valor\n");
    printf("  a partir dos bytes recebidos precisa fazer (alto << 8) | baixo, e não um memcpy.\n");
    pausar();
}

/* --------------------------------------------------------------------------
 * PASSO 4: estouro. Em tipos SEM sinal o estouro é definido (aritmética
 * módulo 2^bits). Em tipos COM sinal é comportamento indefinido: nunca conte
 * com ele.
 * -------------------------------------------------------------------------- */
static void passo_estouro_encoder(void) {
    titulo(4, "Contador de encoder uint16_t: o estouro (wrap-around)");
    uint16_t contador = 65530;
    uint16_t anterior = contador;
    printf("  Contador de hardware de 16 bits começa em %u e recebe 3 pulsos por ciclo.\n\n",
           (unsigned)contador);
    printf("  ciclo  contador  delta = (uint16_t)(atual - anterior)\n");
    for (int ciclo = 1; ciclo <= 5; ciclo++) {
        anterior = contador;
        contador = (uint16_t)(contador + 3u);          /* 65535 + 1 vira 0 */
        uint16_t delta = (uint16_t)(contador - anterior);
        printf("  %5d  %8u  %u pulsos%s\n", ciclo, (unsigned)contador, (unsigned)delta,
               contador < anterior ? "   <- deu a volta, mas o delta continua certo!" : "");
    }
    printf("\n  Moral: com unsigned, a diferença módulo 2^16 ainda dá a velocidade correta.\n");
    printf("  Com int16_t, passar de 32767 é comportamento INDEFINIDO: verifique antes\n");
    printf("  (if (x > INT16_MAX - inc) ...) ou use um tipo maior.\n");
    pausar();
}

/* --------------------------------------------------------------------------
 * PASSO 5: precisão do float. float tem 24 bits de mantissa: inteiros acima
 * de 2^24 = 16777216 não são todos representáveis, e 0,1 não é exato em
 * binário (assim como 1/3 não é exato em decimal).
 * -------------------------------------------------------------------------- */
static void passo_precisao_float(void) {
    titulo(5, "Precisão do float: acumuladores e totalizadores");
    float  soma_f = 0.0f;
    double soma_d = 0.0;
    for (int i = 0; i < 100000; i++) {   /* 100000 amostras de 0,1 L = 10000 L */
        soma_f += 0.1f;
        soma_d += 0.1;
    }
    printf("  Totalizador de vazão: soma de 100000 amostras de 0,1 L (esperado: 10000 L)\n");
    printf("    float : %.4f L  (erro de %.4f L)\n", (double)soma_f, (double)soma_f - 10000.0);
    printf("    double: %.4f L  (erro de %.10f L)\n\n", soma_d, soma_d - 10000.0);

    float pecas = 16777214.0f;
    printf("  Contador de peças em float, somando 1 por peça:\n");
    for (int i = 0; i < 4; i++) {
        printf("    pecas = %.1f\n", (double)pecas);
        pecas += 1.0f;
    }
    printf("  A partir de 16777216 (2^24) somar 1 não muda mais o float: a peça se perde!\n");
    printf("  Contagem é inteira: use uint32_t/uint64_t. Nunca compare floats com '=='.\n");
    pausar();
}

/* --------------------------------------------------------------------------
 * PASSO 6: escolha do tipo = memória. Registro de 1 hora a 1 kHz.
 * -------------------------------------------------------------------------- */
static void passo_memoria(void) {
    titulo(6, "Escolher o tipo é escolher a memória: 1 h de registro a 1 kHz");
    const unsigned long amostras = 3600UL * 1000UL;
    printf("  n = %lu amostras. Memória = n × sizeof(tipo):\n\n", amostras);
    printf("    int16_t (cru do ADC) : %8.1f MB ", amostras * sizeof(int16_t) / 1e6);
    barra(2, 8, 32); putchar('\n');
    printf("    float   (engenharia) : %8.1f MB ", amostras * sizeof(float) / 1e6);
    barra(4, 8, 32); putchar('\n');
    printf("    double  (engenharia) : %8.1f MB ", amostras * sizeof(double) / 1e6);
    barra(8, 8, 32); putchar('\n');
    printf("\n  Prática comum: guardar o valor CRU (int16_t) e converter só ao exibir.\n");
    printf("  Próximo passo: agrupar vários primitivos numa struct (02_struct_padding_visual).\n");
    pausar();
}

int main(void) {
    modo_interativo = isatty(STDIN_FILENO);
    printf("TIPOS PRIMITIVOS NA LEITURA DE SENSORES\n");
    passo_tamanhos();
    passo_adc();
    passo_complemento_e_bytes();
    passo_estouro_encoder();
    passo_precisao_float();
    passo_memoria();
    printf("\nFim da demonstração.\n");
    return 0;
}
