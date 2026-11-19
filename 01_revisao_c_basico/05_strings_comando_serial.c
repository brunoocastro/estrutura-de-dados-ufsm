/*
 * ============================================================================
 * Arquivo : 05_strings_comando_serial.c
 * Módulo  : 01 — Revisão de C básico          (Ementa: Unidade 0.5)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Ver uma string como vetor de char terminado em '\0' e acompanhar, letra
 *   a letra, um interpretador de comandos de texto do tipo "SET VEL 120":
 *   separação em palavras (tokens), conversão de "120" para o inteiro 120 e
 *   validação. No fim, comparar strlen() dentro da condição do laço (O(n²))
 *   com o laço que para no '\0' (O(n)).
 *
 * EXEMPLO REAL
 *   Inversores, balanças, CNCs e placas Arduino/ESP32 recebem comandos pela
 *   serial (RS-232/RS-485/USB) em protocolos de texto como "SET VEL 120\n"
 *   ou comandos AT de modems ("AT+CSQ"). O firmware precisa interpretar cada
 *   linha rapidamente e rejeitar comandos malformados sem travar.
 *
 * COMPLEXIDADE (n = número de caracteres da linha)
 *   Operação                         Tempo     Espaço
 *   separar tokens (uma passada)     O(n)      O(n) para os tokens
 *   converter "120" -> 120           O(d)      O(1)   (d = dígitos)
 *   comparar comando (strcmp)        O(t)      O(1)   (t = tamanho do token)
 *   laço com strlen() na condição    O(n²)     O(1)
 *   laço até '\0'                    O(n)      O(1)
 *   Por quê: strlen percorre a string toda até achar '\0'; chamada a cada
 *   teste da condição, ela repete n vezes um trabalho de n passos.
 *
 * COMO EXECUTAR
 *   ./run 05_strings_comando_serial               (roteiro com pausas, depois
 *                                                   digite comandos; SAIR encerra)
 *   ./run 05_strings_comando_serial < /dev/null   (só o roteiro, sem pausas)
 *   echo "SET VEL 300" | ./run 05_strings_comando_serial   (roteiro + seu comando)
 * ============================================================================
 */
#define _POSIX_C_SOURCE 200809L   /* antes de qualquer #include: libera isatty() em -std=c11 */
#include <ctype.h>
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

#define MAX_TOKENS  4
#define TAM_TOKEN   12          /* inclui o '\0' final: 11 caracteres úteis */
#define TAM_LINHA   128
#define VEL_MAXIMA  3000        /* rpm */

/* Estado do equipamento simulado. */
static int velocidade_rpm = 0;
static int temperatura_c = 41;

/* ---------------------------------------------------------------------------
 * Parte 1: anatomia de uma string
 * ------------------------------------------------------------------------- */

/* Mostra índice, caractere e código ASCII de cada posição, incluindo o '\0'.
   O(n). */
static void mostra_anatomia(const char s[]) {
    printf("   idx: ");
    for (int i = 0; s[i] != '\0'; i++) printf("%4d", i);
    printf("%4d\n  char: ", (int)strlen(s));
    for (int i = 0; s[i] != '\0'; i++) printf(" '%c'", s[i]);
    printf("  \\0\n ASCII: ");
    for (int i = 0; s[i] != '\0'; i++) printf("%4d", s[i]);
    printf("%4d\n", 0);
}

static void parte1_anatomia(void) {
    const char comando[] = "SET VEL 120";
    printf("PARTE 1 — Uma string é um vetor de char terminado em '\\0'\n");
    printf("================================================================\n");
    printf("  const char comando[] = \"SET VEL 120\";\n\n");
    mostra_anatomia(comando);
    printf("\n  strlen = %zu caracteres visíveis; sizeof = %zu bytes (o '\\0' ocupa 1).\n",
           strlen(comando), sizeof comando);
    printf("  Armadilha: um vetor char[11] não cabe \"SET VEL 120\": falta lugar\n");
    printf("  para o '\\0' e as funções de string passam do fim do vetor.\n\n");
    pausar();
}

/* ---------------------------------------------------------------------------
 * Parte 2: interpretador de comandos
 * ------------------------------------------------------------------------- */

/* Desenha a linha com um marcador ^ na posição i. O(n). */
static void desenha_cursor(const char linha[], int i) {
    printf("     ");
    for (int k = 0; linha[k] != '\0'; k++) printf("%c ", linha[k]);
    printf("\\0\n     ");
    for (int k = 0; k < i; k++) printf("  ");
    printf("^\n");
}

/* Separa a linha em palavras separadas por espaços, em UMA passada.
   Converte letras para maiúsculas (aceita "set vel 120").
   Devolve a quantidade de tokens, ou -1 se houver tokens demais ou longos
   demais (proteção contra estouro do vetor: nunca confie no que chega pela
   serial). O(n). */
static int separa_tokens(const char linha[], char tokens[][TAM_TOKEN], int mostrar) {
    int qtd = 0;        /* tokens completos ou em andamento */
    int tam = 0;        /* caracteres no token atual */
    int dentro = 0;     /* 1 enquanto estamos no meio de uma palavra */

    for (int i = 0; linha[i] != '\0' && linha[i] != '\n'; i++) {
        char c = (char)toupper((unsigned char)linha[i]);
        const char *acao;
        if (c == ' ' || c == '\t' || c == '\r') {
            if (dentro) {                       /* fim de palavra */
                tokens[qtd - 1][tam] = '\0';
                dentro = 0;
                acao = "espaço -> fecha o token";
            } else {
                acao = "espaço repetido -> ignora";
            }
        } else {
            if (!dentro) {                      /* começo de palavra */
                if (qtd == MAX_TOKENS) return -1;
                qtd++;
                tam = 0;
                dentro = 1;
            }
            if (tam == TAM_TOKEN - 1) return -1;
            tokens[qtd - 1][tam++] = c;
            acao = "letra/dígito -> acrescenta ao token";
        }
        if (mostrar) {
            desenha_cursor(linha, i);
            printf("     '%c': %-38s tokens: ", linha[i], acao);   /* 38: o acento ocupa 2 bytes */
            for (int t = 0; t < qtd; t++)
                printf("[%.*s] ", (t == qtd - 1 && dentro) ? tam : TAM_TOKEN, tokens[t]);
            printf("\n");
            pausar();
        }
    }
    if (dentro) tokens[qtd - 1][tam] = '\0';    /* última palavra termina no '\0' */
    if (mostrar) printf("     '\\0': fim da string -> %d token(s)\n", qtd);
    return qtd;
}

/* Converte um token só de dígitos para int, acumulando valor = valor·10 + dígito.
   Devolve -1 se houver caractere que não é dígito ou se passar de VEL_MAXIMA
   (testar o limite DENTRO do laço evita estouro de int com "99999999999").
   O(d), d = número de dígitos. */
static int texto_para_int(const char s[], int mostrar) {
    int valor = 0;
    if (s[0] == '\0') return -1;
    for (int i = 0; s[i] != '\0'; i++) {
        if (!isdigit((unsigned char)s[i])) {
            if (mostrar) printf("     '%c' não é dígito -> valor inválido\n", s[i]);
            return -1;
        }
        int digito = s[i] - '0';      /* '0' = 48 em ASCII; '7' - '0' = 7 */
        if (mostrar)
            printf("     '%c' -> dígito %d: valor = %d × 10 + %d = %d\n",
                   s[i], digito, valor, digito, valor * 10 + digito);
        valor = valor * 10 + digito;
        if (valor > VEL_MAXIMA) {
            if (mostrar) printf("     %d passou do limite %d -> para a conversão\n",
                                valor, VEL_MAXIMA);
            return -1;
        }
    }
    return valor;
}

/* Interpreta uma linha e escreve a resposta do equipamento.
   Protocolo:  SET VEL <0..3000> | GET VEL | GET TEMP | STOP
   O(n) para separar + O(1) comparações de tokens curtos. */
static void interpreta(const char linha[], int mostrar) {
    char tokens[MAX_TOKENS][TAM_TOKEN];
    int qtd = separa_tokens(linha, tokens, mostrar);
    int set_vel = qtd == 3 && strcmp(tokens[0], "SET") == 0 && strcmp(tokens[1], "VEL") == 0;
    int valor = -1;

    if (set_vel) {   /* converte antes de responder, para a narração sair em ordem */
        if (mostrar) printf("\n     Convertendo \"%s\" para inteiro:\n", tokens[2]);
        valor = texto_para_int(tokens[2], mostrar);
    }

    printf("  >> ");
    if (qtd < 0) {
        printf("ERR comando longo demais\n");
    } else if (qtd == 0) {
        printf("ERR linha vazia\n");
    } else if (strcmp(tokens[0], "SET") == 0) {
        if (!set_vel)
            printf("ERR uso: SET VEL <0..%d>\n", VEL_MAXIMA);
        else if (valor < 0)
            printf("ERR valor inválido ou fora de 0..%d\n", VEL_MAXIMA);
        else {
            velocidade_rpm = valor;
            printf("OK VEL=%d\n", velocidade_rpm);
        }
    } else if (strcmp(tokens[0], "GET") == 0 && qtd == 2) {
        if (strcmp(tokens[1], "VEL") == 0)       printf("VEL=%d\n", velocidade_rpm);
        else if (strcmp(tokens[1], "TEMP") == 0) printf("TEMP=%d\n", temperatura_c);
        else                                     printf("ERR parâmetro desconhecido\n");
    } else if (strcmp(tokens[0], "STOP") == 0 && qtd == 1) {
        velocidade_rpm = 0;
        printf("OK VEL=0 (parada)\n");
    } else {
        printf("ERR comando desconhecido\n");
    }
}

static void parte2_interpretador(void) {
    printf("PARTE 2 — Interpretando \"SET VEL 120\" caractere a caractere\n");
    printf("================================================================\n");
    printf("  Protocolo: SET VEL <0..%d> | GET VEL | GET TEMP | STOP\n\n", VEL_MAXIMA);
    printf("  << SET VEL 120\n");
    interpreta("SET VEL 120", 1);
    printf("\n");
    pausar();

    /* Vetor de strings = matriz de char: cada linha guarda um comando. */
    const char roteiro[][20] = {
        "get vel",            /* minúsculas: normalizado com toupper */
        "SET   VEL    250",   /* espaços repetidos são ignorados */
        "SET VEL 9999",       /* fora da faixa */
        "SET VEL 12a",        /* caractere inválido */
        "SET VEL",            /* falta argumento */
        "GET TEMP",
        "RESET",              /* desconhecido */
        "STOP",
    };
    printf("  Outros comandos recebidos pela serial:\n");
    for (int k = 0; k < (int)(sizeof roteiro / sizeof roteiro[0]); k++) {
        printf("  << %s\n", roteiro[k]);
        interpreta(roteiro[k], 0);
    }
    printf("\n");
    pausar();
}

/* ---------------------------------------------------------------------------
 * Parte 3: strlen() dentro do laço
 * ------------------------------------------------------------------------- */

/* Contador de caracteres examinados. É global só para instrumentar as duas
   versões abaixo sem mudar a assinatura delas. */
static long visitas = 0;

/* Igual à strlen da biblioteca, mas contando cada caractere examinado.
   (Usamos a nossa porque o compilador pode otimizar a strlen original e
   esconder o problema.) O(n). */
static size_t meu_strlen(const char s[]) {
    size_t n = 0;
    while (visitas++, s[n] != '\0') n++;
    return n;
}

/* Conta espaços chamando strlen a cada teste da condição: O(n²). */
static int conta_espacos_lento(const char s[]) {
    int espacos = 0;
    for (size_t i = 0; i < meu_strlen(s); i++) {
        visitas++;
        if (s[i] == ' ') espacos++;
    }
    return espacos;
}

/* Conta espaços parando no '\0': cada caractere é visto uma vez, O(n). */
static int conta_espacos_rapido(const char s[]) {
    int espacos = 0;
    for (size_t i = 0; visitas++, s[i] != '\0'; i++)
        if (s[i] == ' ') espacos++;
    return espacos;
}

static void parte3_strlen_no_laco(void) {
    static char texto[10001];   /* static: 10 KB fora da pilha */
    const char padrao[] = "SET VEL 120 ";
    const int tamanhos[] = {10, 100, 1000, 10000};

    printf("PARTE 3 — strlen() na condição do laço: O(n²) escondido\n");
    printf("================================================================\n");
    printf("  lento:  for (i = 0; i < strlen(s); i++)   <- strlen roda a cada volta\n");
    printf("  rápido: for (i = 0; s[i] != '\\0'; i++)    <- cada char visto uma vez\n\n");
    printf("       n |  visitas lento | visitas rápido | razão\n");   /* à mão: acentos = 2 bytes */
    printf("---------+----------------+----------------+-------\n");
    for (int t = 0; t < 4; t++) {
        int n = tamanhos[t];
        for (int i = 0; i < n; i++) texto[i] = padrao[i % (int)(sizeof padrao - 1)];
        texto[n] = '\0';

        visitas = 0;
        int e1 = conta_espacos_lento(texto);
        long lento = visitas;
        visitas = 0;
        int e2 = conta_espacos_rapido(texto);
        long rapido = visitas;
        printf("%8d | %14ld | %14ld | %5.0f×%s\n", n, lento, rapido,
               (double)lento / rapido, e1 == e2 ? "" : " (ERRO: resultados diferentes)");
    }
    printf("\n  Mesma resposta, custo muito diferente: com uma linha de 10 000 caracteres\n");
    printf("  a versão lenta examina ~100 milhões de caracteres. Num microcontrolador\n");
    printf("  recebendo dados pela serial, isso faz perder bytes que continuam chegando.\n\n");
    pausar();
}

/* ---------------------------------------------------------------------------
 * Parte 4: modo livre (lê comandos até EOF ou SAIR)
 * ------------------------------------------------------------------------- */

static void parte4_modo_livre(void) {
    char linha[TAM_LINHA];
    printf("PARTE 4 — Modo livre: digite comandos (SAIR ou Ctrl+D encerra)\n");
    printf("================================================================\n");
    while (1) {
        printf("  << ");
        fflush(stdout);
        if (fgets(linha, sizeof linha, stdin) == NULL) {   /* EOF: fim da entrada */
            printf("(fim da entrada)\n");
            break;
        }
        if (strchr(linha, '\n') == NULL) {   /* linha maior que o buffer: descarta o resto */
            int c;
            while ((c = getchar()) != '\n' && c != EOF) { }
        }
        if (!modo_interativo) printf("%s", linha);   /* ecoa o que veio por pipe */
        char primeiro[TAM_TOKEN];
        if (sscanf(linha, "%11s", primeiro) == 1) {
            for (int i = 0; primeiro[i] != '\0'; i++)
                primeiro[i] = (char)toupper((unsigned char)primeiro[i]);
            if (strcmp(primeiro, "SAIR") == 0) break;
        }
        interpreta(linha, 0);
    }
}

int main(void) {
    modo_interativo = isatty(STDIN_FILENO);
    printf("Strings: interpretador de comandos seriais\n\n");
    parte1_anatomia();
    parte2_interpretador();
    parte3_strlen_no_laco();
    parte4_modo_livre();
    return 0;
}
