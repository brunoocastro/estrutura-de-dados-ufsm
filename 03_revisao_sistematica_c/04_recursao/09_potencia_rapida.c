/*
 * ============================================================================
 * Arquivo : 09_potencia_rapida.c
 * Módulo  : 03 — Revisão sistemática de C     (Ementa: 0.6 Recursividade)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Calcular b^e (mod m) de dois jeitos e contar as multiplicações:
 *     - laço simples:            e multiplicações              -> O(e)
 *     - potência rápida (recursiva, "dividir para conquistar"):
 *         b^e = (b^(e/2))²        se e é par
 *         b^e = (b^(e/2))² · b    se e é ímpar             -> O(log e)
 *   O rastro mostra a pilha de chamadas com o expoente caindo pela METADE a
 *   cada nível, e uma tabela compara os dois métodos para e até 10^18.
 *
 * EXEMPLO REAL
 *   Troca de chaves Diffie-Hellman (e o RSA) usa potências modulares com
 *   expoentes de centenas de dígitos: é assim que um gateway industrial e um
 *   servidor SCADA combinam uma chave secreta pela rede (TLS, VPN). Com O(e)
 *   seria impossível; com O(log e) leva microssegundos.
 *
 * COMPLEXIDADE (e = expoente)
 *   Método            Multiplicações        Pilha
 *   laço simples      e                     O(1)
 *   potência rápida   ≤ 2·log2(e) + 2       O(log e) (um quadro por nível)
 *   Por quê: cada chamada resolve um problema de tamanho e/2 com 1 ou 2
 *   multiplicações; dividir e por 2 até chegar a 0 leva log2(e) passos.
 *   (Trabalhamos "mod m" para os números não estourarem: m < 2^32, então
 *   todo produto cabe em 64 bits.)
 *
 * COMO EXECUTAR
 *   ./run 09_potencia_rapida               (ENTER avança cada passo)
 *   ./run 09_potencia_rapida < /dev/null   (executa direto, sem pausas)
 * ============================================================================
 */
#define _POSIX_C_SOURCE 200809L   /* antes de qualquer #include: libera isatty() em -std=c11 */
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

typedef uint64_t u64;

static int modo_interativo = 0;   /* 1 quando stdin é um terminal */
static u64 multiplicacoes = 0;
static int mostrar = 0;

/* Pausa até o aluno teclar ENTER; não faz nada se a entrada não for um terminal. */
static void pausar(void) {
    if (!modo_interativo) return;
    printf("    [ENTER] próximo passo...");
    fflush(stdout);
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
}

/* O(1): multiplicação modular contada. Pré-condição: a, b < m < 2^32. */
static u64 mul_mod(u64 a, u64 b, u64 m) {
    multiplicacoes++;
    return (a * b) % m;
}

/* O(e): multiplica b por si mesmo e vezes. */
static u64 potencia_laco(u64 b, u64 e, u64 m) {
    u64 r = 1 % m;
    for (u64 i = 0; i < e; i++)
        r = mul_mod(r, b, m);
    return r;
}

/* O(log e) tempo e pilha. Resolve UMA vez o subproblema b^(e/2) e reaproveita
 * o resultado (metade * metade). Chamar potencia(b, e/2) duas vezes estragaria
 * tudo: voltaria a O(e) chamadas! */
static u64 potencia_rapida(u64 b, u64 e, u64 m, int nivel) {
    if (mostrar)
        printf("%*sENTRA pot(%llu, %llu)\n", nivel * 4, "",
               (unsigned long long)b, (unsigned long long)e);
    if (e == 0) {
        if (mostrar) printf("%*sCASO BASE: b^0 = 1\n", nivel * 4, "");
        return 1 % m;
    }
    u64 metade = potencia_rapida(b, e / 2, m, nivel + 1);
    u64 r = mul_mod(metade, metade, m);
    if (e % 2 == 1)
        r = mul_mod(r, b, m);
    if (mostrar)
        printf("%*sSAI pot(%llu, %llu) = %llu^2%s = %llu\n", nivel * 4, "",
               (unsigned long long)b, (unsigned long long)e, (unsigned long long)metade,
               e % 2 ? " * b  (e impar)" : "      (e par)", (unsigned long long)r);
    return r;
}

int main(void) {
    modo_interativo = isatty(STDIN_FILENO);
    const u64 m = 1000000007ULL; /* primo grande usado como módulo */

    printf("=== 1) Rastro de 3^13 (mod %llu) ===\n", (unsigned long long)m);
    printf("13 em binario = 1101: cada nivel olha um bit do expoente.\n\n");
    mostrar = 1;
    multiplicacoes = 0;
    u64 r = potencia_rapida(3, 13, m, 0);
    printf("\n3^13 = %llu com %llu multiplicacoes (o laco faria 13).\n",
           (unsigned long long)r, (unsigned long long)multiplicacoes);
    mostrar = 0;
    pausar();

    printf("\n=== 2) Crescimento: multiplicacoes executadas ===\n");
    printf("%22s | %14s | %8s | %s\n", "expoente e", "laco O(e)", "rapida", "resultados iguais?");
    printf("-----------------------+----------------+----------+-------------------\n");
    u64 e = 10;
    for (int k = 1; k <= 6; k++, e *= 10) {
        multiplicacoes = 0;
        u64 r1 = potencia_laco(7, e, m);
        u64 c1 = multiplicacoes;
        multiplicacoes = 0;
        u64 r2 = potencia_rapida(7, e, m, 0);
        printf("%22llu | %14llu | %8llu | %s\n", (unsigned long long)e,
               (unsigned long long)c1, (unsigned long long)multiplicacoes,
               r1 == r2 ? "sim" : "NAO");
    }
    multiplicacoes = 0;
    potencia_rapida(7, 1000000000000000000ULL, m, 0);
    printf("%22llu | %14s | %8llu | (laco levaria ~30 anos)\n",
           1000000000000000000ULL, "10^18", (unsigned long long)multiplicacoes);
    pausar();

    printf("\n=== 3) Exemplo real: troca de chaves Diffie-Hellman (didatica) ===\n");
    /* Parâmetros públicos: primo p e gerador g. Os segredos a e b nunca
     * trafegam na rede; só A = g^a e B = g^b. Ambos chegam a g^(ab). */
    const u64 p = 2147483647ULL, g = 7;           /* 2^31 - 1, primo */
    const u64 segredo_scada = 1234567891ULL, segredo_rtu = 987654321ULL;
    multiplicacoes = 0;
    u64 pub_scada = potencia_rapida(g, segredo_scada, p, 0);
    u64 pub_rtu = potencia_rapida(g, segredo_rtu, p, 0);
    u64 chave_scada = potencia_rapida(pub_rtu, segredo_scada, p, 0);
    u64 chave_rtu = potencia_rapida(pub_scada, segredo_rtu, p, 0);
    printf("SCADA envia A = g^a mod p = %llu\n", (unsigned long long)pub_scada);
    printf("RTU   envia B = g^b mod p = %llu\n", (unsigned long long)pub_rtu);
    printf("SCADA calcula B^a = %llu | RTU calcula A^b = %llu -> %s\n",
           (unsigned long long)chave_scada, (unsigned long long)chave_rtu,
           chave_scada == chave_rtu ? "mesma chave!" : "ERRO");
    printf("Total: %llu multiplicacoes para 4 potencias com expoentes ~10^9.\n",
           (unsigned long long)multiplicacoes);
    printf("(Na pratica usam-se primos de 2048 bits; a ideia e a mesma.)\n");
    return 0;
}
