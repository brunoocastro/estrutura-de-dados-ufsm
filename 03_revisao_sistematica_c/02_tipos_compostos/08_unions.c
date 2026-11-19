/*
 * ============================================================================
 * Arquivo : 08_unions.c
 * Módulo  : 03 — Revisão sistemática de C     (Ementa: 1.1 Estruturas)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Uma union agrupa alternativas que compartilham o MESMO espaço de memória:
 *   todos os membros começam no mesmo endereço e escrever um membro sobrescreve
 *   o anterior. Em geral, leia o membro que foi escrito por último.
 *
 * EXEMPLO REAL
 *   Protocolos como Modbus transmitem um float em 4 bytes: uma union
 *   {float valor; unsigned char bytes[4];} permite ver/montar esses bytes.
 *   (A ordem dos bytes depende da arquitetura: little/big endian.)
 *
 * COMPLEXIDADE
 *   Acesso a membro: O(1). sizeof(union) = tamanho do MAIOR membro (+ alinhamento).
 *
 * COMO EXECUTAR
 *   ./run 08_unions
 * ============================================================================
 */
#include <stdio.h>

/* FORMA 1 — union com tag. Para declarar depois, escrevemos "union Valor". */
union Valor {
    int inteiro;
    float real;
    unsigned char bytes[sizeof(float)];
};

/* FORMA 2 — union anônima com typedef. ValorCompacto é diretamente o tipo. */
typedef union {
    int inteiro;
    double real;
} ValorCompacto;

/* FORMA 3 — union com tag e typedef ao mesmo tempo. */
typedef union Dado {
    long inteiro_longo;
    char caractere;
} Dado;

/* FORMA 4 — union anônima usada apenas na declaração desta variável. */
union {
    int codigo;
    float medida;
} item_anonimo;

void mostra_valor(const union Valor *valor) {
    printf("valor inteiro = %d\n", valor->inteiro);
}

int main(void) {
    union Valor valor;
    ValorCompacto compacto = {.real = 3.5};
    Dado dado = {.caractere = 'X'};
    item_anonimo.codigo = 42;

    printf("=== Formas de declarar unions ===\n");
    valor.inteiro = 42;
    mostra_valor(&valor);
    printf("typedef anonimo: real = %.1f\n", compacto.real);
    printf("tag + typedef: caractere = %c\n", dado.caractere);
    printf("union anonima: codigo = %d\n", item_anonimo.codigo);

    printf("\n=== Memoria compartilhada ===\n");
    printf("sizeof(union Valor) = %zu byte(s)\n", sizeof valor);
    printf("sizeof(int) = %zu, sizeof(float) = %zu, sizeof(bytes) = %zu\n",
           sizeof(int), sizeof(float), sizeof valor.bytes);
    printf("endereco da union = %p\n", (void *)&valor);
    printf("endereco do inteiro = %p\n", (void *)&valor.inteiro);
    printf("endereco do real = %p\n", (void *)&valor.real);
    printf("endereco dos bytes = %p\n", (void *)valor.bytes);

    valor.real = 3.14f;
    printf("\nDepois de escrever real: %.2f\n", valor.real);
    printf("O valor inteiro anterior foi sobrescrito pelos mesmos bytes.\n");

    printf("Bytes da representacao atual: ");
    for (size_t i = 0; i < sizeof valor.bytes; i++)
        printf("%02x%s", valor.bytes[i], i + 1 == sizeof valor.bytes ? "\n" : " ");

    printf("\nUma union e adequada quando apenas uma alternativa precisa existir por vez.\n");
    printf("Use uma struct quando todos os campos forem necessarios simultaneamente.\n");
    return 0;
}
