/*
 * ============================================================================
 * Arquivo : 03_ring_buffer_uart.c
 * Módulo  : 06 — Filas                        (Ementa: Unidade 3.1)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Simular, em tempo discreto (ticks), o buffer circular (ring buffer) que
 *   liga a interrupção de recepção da UART (produtor) ao laço principal do
 *   firmware (consumidor). O aluno vê:
 *     - os índices `esc` (escrita, só a ISR mexe) e `lei` (leitura, só o laço
 *       principal mexe) dando a volta no vetor;
 *     - o truque de capacidade potência de 2: `indice & MASCARA` == `% CAP`;
 *     - contadores livres de 8 bits que transbordam (255 -> 0) sem quebrar a
 *       conta de ocupação `esc - lei`;
 *     - o OVERFLOW quando o laço principal fica ocupado (gravando EEPROM) e as
 *       duas políticas de descarte: descartar o NOVO ou sobrescrever o ANTIGO;
 *     - como dimensionar o buffer para não perder bytes.
 *
 * EXEMPLO REAL
 *   Um sensor envia linhas "T=25.1\n" pela serial a 115200 bit/s para um
 *   microcontrolador (ou um escravo Modbus RTU recebe quadros). Cada byte gera
 *   uma interrupção curta que só guarda o byte no ring buffer; o laço principal
 *   retira os bytes quando pode e monta as mensagens. O mesmo padrão aparece em
 *   drivers de CAN, USB-CDC, ADC com DMA e filas de mensagens de RTOS.
 *
 * COMPLEXIDADE (CAP = capacidade do buffer)
 *   Operação              Tempo   Espaço
 *   guardar byte (ISR)    O(1)    O(CAP), alocação estática (sem malloc)
 *   retirar byte (laço)   O(1)
 *   ocupação              O(1)    (esc - lei, aritmética sem sinal)
 *   Por quê: só um índice anda por operação e não há cópias nem laços. Isso é
 *   obrigatório numa ISR, que precisa terminar em tempo curto e previsível.
 *
 * COMO EXECUTAR
 *   ./run 03_ring_buffer_uart               (ENTER avança cada tick)
 *   ./run 03_ring_buffer_uart < /dev/null   (executa direto, sem pausas)
 * ============================================================================
 */
#define _POSIX_C_SOURCE 200809L /* antes de qualquer #include: libera isatty() em -std=c11 */
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define CAP_MAX 64 /* maior capacidade usada no programa; precisa dividir 256 */

static int modo_interativo = 0; /* 1 quando stdin é um terminal */

/* Pausa até o aluno teclar ENTER; não faz nada se a entrada não for um terminal. */
static void pausar(void) {
    if (!modo_interativo) return;
    printf("    [ENTER] próximo passo...");
    fflush(stdout);
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
}

typedef enum { DESCARTA_NOVO, SOBRESCREVE_ANTIGO } Politica;

/*
 * Os índices são contadores LIVRES de 8 bits: nunca são reduzidos com módulo,
 * só incrementados, e transbordam naturalmente de 255 para 0. A posição real
 * no vetor é `indice & mascara`. Vantagens:
 *   - ocupação = (uint8_t)(esc - lei) funciona mesmo depois do transbordo;
 *   - cheio (ocupação == cap) e vazio (ocupação == 0) não se confundem, sem
 *     precisar de um campo `tam` separado (que seria escrito pelos DOIS lados).
 * Requisito: cap é potência de 2 e divide 256 (2, 4, ..., 128).
 * Num microcontrolador real, `esc` e `lei` seriam `volatile`, pois a ISR os
 * altera "por fora" do fluxo normal do laço principal.
 */
typedef struct {
    char buf[CAP_MAX];
    uint8_t cap, mascara;
    uint8_t esc;      /* só a ISR escreve (produtor)  */
    uint8_t lei;      /* só o laço principal escreve (consumidor) */
    Politica politica;
    int recebidos, consumidos, descartados, ocupacao_max;
} RingBuffer;

/* O(1). Devolve 0 se cap não for potência de 2 entre 2 e CAP_MAX. */
static int rb_iniciar(RingBuffer *rb, int cap, Politica politica, uint8_t valor_inicial) {
    if (cap < 2 || cap > CAP_MAX || (cap & (cap - 1)) != 0) return 0;
    memset(rb, 0, sizeof *rb);
    rb->cap = (uint8_t)cap;
    rb->mascara = (uint8_t)(cap - 1); /* ex.: cap 8 -> 0b0111 */
    rb->esc = rb->lei = valor_inicial; /* começar perto de 255 mostra o transbordo */
    rb->politica = politica;
    return 1;
}

/* O(1): quantos bytes aguardam. A subtração sem sinal "dá a volta" corretamente. */
static uint8_t rb_ocupacao(const RingBuffer *rb) {
    return (uint8_t)(rb->esc - rb->lei);
}

/*
 * O(1). Chamada pela "ISR" para cada byte recebido. Devolve 1 se guardou o
 * byte novo e 0 se ele foi descartado. Em `*perdido` devolve o byte que se
 * perdeu (o novo ou o antigo), ou '\0' se nada se perdeu.
 */
static int rb_isr_guardar(RingBuffer *rb, char c, char *perdido) {
    *perdido = '\0';
    rb->recebidos++;
    if (rb_ocupacao(rb) == rb->cap) {
        rb->descartados++;
        if (rb->politica == DESCARTA_NOVO) {
            /* Como o "overrun" de uma UART: o byte que chegou é perdido e
               o erro é contado. Preserva o início da mensagem. */
            *perdido = c;
            return 0;
        }
        /* SOBRESCREVE_ANTIGO: abre espaço jogando fora o byte mais antigo.
           ATENÇÃO: aqui a ISR altera `lei`, que pertence ao consumidor. Num
           sistema real isso exige proteção (desabilitar a interrupção enquanto
           o laço lê) - o modelo "um escritor por índice" deixa de valer. */
        *perdido = rb->buf[rb->lei & rb->mascara];
        rb->lei++;
    }
    rb->buf[rb->esc & rb->mascara] = c;
    rb->esc++; /* só depois de escrever o dado: o consumidor nunca vê lixo */
    if (rb_ocupacao(rb) > rb->ocupacao_max) rb->ocupacao_max = rb_ocupacao(rb);
    return 1;
}

/* O(1). Chamada pelo laço principal. Devolve 0 se o buffer estiver vazio. */
static int rb_retirar(RingBuffer *rb, char *c) {
    if (rb_ocupacao(rb) == 0) return 0;
    *c = rb->buf[rb->lei & rb->mascara];
    rb->lei++;
    rb->consumidos++;
    return 1;
}

/* Escreve um caractere de forma visível: '\n' vira "\n". */
static void mostrar_char(char c) {
    if (c == '\n') printf("\\n");
    else printf("%c", c);
}

/* O(cap): desenha o vetor físico, quais casas estão ocupadas e os índices. */
static void rb_desenhar(const RingBuffer *rb) {
    uint8_t p_esc = rb->esc & rb->mascara, p_lei = rb->lei & rb->mascara;
    uint8_t ocup = rb_ocupacao(rb);
    printf("      casa : ");
    for (int i = 0; i < rb->cap; i++) printf(" %2d  ", i);
    printf("\n      buf  : ");
    for (int i = 0; i < rb->cap; i++) {
        /* Ocupada se estiver a menos de `ocup` passos de `lei`, com volta. */
        int ocupada = ((i - p_lei + rb->cap) & rb->mascara) < ocup;
        if (!ocupada) printf("[   ]");
        else if (rb->buf[i] == '\n') printf("[\\n ]");
        else printf("[ %c ]", rb->buf[i]);
    }
    printf("\n             ");
    for (int i = 0; i < rb->cap; i++) {
        if (i == p_esc && i == p_lei) printf(" e=l ");
        else if (i == p_esc) printf(" esc ");
        else if (i == p_lei) printf(" lei ");
        else printf("     ");
    }
    printf("\n      contadores livres: esc=%3u lei=%3u  ocupação=(uint8_t)(%u-%u)=%u/%u"
           "  casa=contador&%u\n",
           (unsigned)rb->esc, (unsigned)rb->lei, (unsigned)rb->esc, (unsigned)rb->lei,
           (unsigned)ocup, (unsigned)rb->cap, (unsigned)rb->mascara);
}

/* ------------------------------------------------------------------------- */
/* Simulação em tempo discreto                                               */
/* ------------------------------------------------------------------------- */

static const char MENSAGEM[] = "T=25.1\nT=25.3\nT=25.4\n"; /* 21 bytes do sensor */
#define BYTES_POR_TICK 3                                    /* taxa de chegada */

/*
 * Quantos bytes o laço principal consegue processar em cada tick. Nos ticks
 * 2 a 4 ele está ocupado gravando a EEPROM e não lê nada.
 */
static const int consumo_por_tick[] = {2, 2, 0, 0, 0, 4, 4, 4, 4, 4, 4, 4};
#define N_TICKS ((int)(sizeof consumo_por_tick / sizeof consumo_por_tick[0]))

/*
 * Roda o cenário inteiro com um buffer de capacidade `cap` e uma política.
 * `detalhar` != 0 desenha cada tick. O texto que o laço principal conseguiu
 * montar vai para `recebido` (tamanho >= sizeof MENSAGEM).
 */
static void simular(int cap, Politica politica, int detalhar, char *recebido) {
    RingBuffer rb;
    /* Começa em 250 para que os contadores transbordem durante a simulação. */
    if (!rb_iniciar(&rb, cap, politica, 250)) { printf("capacidade inválida\n"); return; }
    int enviados = 0, n_rec = 0;
    const int total = (int)strlen(MENSAGEM);

    if (detalhar) {
        printf("\n  estado inicial (buffer vazio, contadores começam em 250):\n");
        rb_desenhar(&rb);
        pausar();
    }
    /* Para quando a mensagem acabou de chegar e o buffer esvaziou. */
    for (int t = 0; t < N_TICKS && (enviados < total || rb_ocupacao(&rb) > 0); t++) {
        uint8_t esc_antes = rb.esc, lei_antes = rb.lei;
        /* 1) interrupções: chegam até BYTES_POR_TICK bytes da serial */
        if (detalhar) printf("\n  t=%2d  ISR  :", t);
        for (int k = 0; k < BYTES_POR_TICK && enviados < total; k++) {
            char c = MENSAGEM[enviados++], perdido;
            int ok = rb_isr_guardar(&rb, c, &perdido);
            if (!detalhar) continue;
            printf(" '");
            mostrar_char(c);
            if (ok && perdido == '\0') printf("'");
            else if (ok) { printf("'(cheio! sobrescreveu '"); mostrar_char(perdido); printf("')"); }
            else printf("'(cheio! DESCARTADO)");
        }
        /* 2) laço principal: retira o que conseguir neste tick */
        if (detalhar) {
            printf("\n        laço : ");
            if (consumo_por_tick[t] == 0) printf("ocupado gravando EEPROM, não lê nada");
            else printf("pode ler até %d byte(s):", consumo_por_tick[t]);
        }
        for (int k = 0; k < consumo_por_tick[t]; k++) {
            char c;
            if (!rb_retirar(&rb, &c)) break;
            recebido[n_rec++] = c;
            if (detalhar) { printf(" '"); mostrar_char(c); printf("'"); }
        }
        if (detalhar) {
            printf("\n");
            rb_desenhar(&rb);
            if (rb.esc < esc_antes || rb.lei < lei_antes)
                printf("      (um contador passou de 255 para 0: a subtração sem sinal continua "
                       "dando a ocupação certa)\n");
            pausar();
        }
    }
    recebido[n_rec] = '\0';
    printf("  cap=%2d  política=%-18s recebidos=%2d consumidos=%2d descartados=%2d "
           "ocupação máx=%2d\n", cap,
           politica == DESCARTA_NOVO ? "descarta novo" : "sobrescreve antigo",
           rb.recebidos, rb.consumidos, rb.descartados, rb.ocupacao_max);
}

/* Mostra o texto enviado e o texto montado pelo laço principal, linha a linha. */
static void comparar_texto(const char *recebido) {
    printf("    enviado : ");
    for (const char *p = MENSAGEM; *p; p++) mostrar_char(*p);
    printf("\n    montado : ");
    for (const char *p = recebido; *p; p++) mostrar_char(*p);
    printf("\n");
}

int main(void) {
    modo_interativo = isatty(STDIN_FILENO);
    char recebido[sizeof MENSAGEM];

    printf("RING BUFFER DA UART: ISR (produtor) -> buffer circular -> laço principal (consumidor)\n");
    printf("Chegam %d bytes por tick; o laço lê 2/tick, para nos ticks 2-4 (EEPROM) e depois lê 4/tick.\n",
           BYTES_POR_TICK);
    printf("Truque da máscara: com cap = 8, 13 & 7 = %d e 13 %% 8 = %d (iguais, mas & é 1 instrução).\n",
           13 & 7, 13 % 8);
    pausar();

    printf("\n=== CENÁRIO 1: cap = 8, política DESCARTA NOVO (como o overrun de uma UART) ===\n");
    simular(8, DESCARTA_NOVO, 1, recebido);
    comparar_texto(recebido);
    printf("    -> perderam-se bytes do MEIO da mensagem; a linha corrompida só é detectada\n"
           "       se o protocolo tiver verificação (o CRC do Modbus RTU existe para isso).\n");
    pausar();

    printf("\n=== CENÁRIO 2: cap = 8, política SOBRESCREVE ANTIGO (data logger: guarda o mais recente) ===\n");
    simular(8, SOBRESCREVE_ANTIGO, 1, recebido);
    comparar_texto(recebido);
    printf("    -> agora os bytes perdidos são os mais ANTIGOS ainda não lidos.\n");
    pausar();

    printf("\n=== DIMENSIONAMENTO: mesma carga, capacidades diferentes ===\n");
    const int caps[] = {4, 8, 16, 32};
    for (int i = 0; i < 4; i++) simular(caps[i], DESCARTA_NOVO, 0, recebido);
    printf("\n  Regra prática: cap >= bytes pendentes + taxa de chegada x tempo sem leitura,\n"
           "  arredondado para a potência de 2 seguinte. Aqui: 2 bytes pendentes no fim do\n"
           "  tick 1 + 4 rajadas de 3 bytes (ticks 2 a 5) até o laço voltar a ler = 14 -> 16.\n"
           "  Na serial real: a 115200 bit/s chegam ~11520 bytes/s; se o laço pode ficar 5 ms\n"
           "  sem ler, são ~58 bytes -> buffer de 64.\n");
    return 0;
}
