/*
 * ENUMS EM C — formas de declaração e valores
 *
 * Um enum define constantes inteiras nomeadas. Ele melhora a legibilidade,
 * mas não cria uma string nem garante, por si só, que uma variável contenha
 * apenas os enumeradores declarados.
 *
 * Compile e execute nesta pasta:
 *   gcc -std=c11 -Wall -Wextra -pedantic 06_enums.c -o 06_enums
 *   ./06_enums
 */
#include <stdio.h>

/* FORMA 1 — enum com tag. Depois usamos "enum Estado" como tipo. */
enum Estado {
    DESLIGADO,
    LIGADO = 10,
    PAUSADO
};

/* FORMA 2 — enum anônimo com typedef. Tamanho é diretamente o nome do tipo. */
typedef enum {
    PEQUENO = 1,
    MEDIO,
    GRANDE
} Tamanho;

/* FORMA 3 — enum com tag e typedef. Podemos usar "enum Direcao" ou "Direcao". */
typedef enum Direcao {
    NORTE,
    SUL,
    LESTE,
    OESTE
} Direcao;

/* FORMA 4 — enum anônimo usado somente para criar constantes. */
enum {
    BUFFER_MINIMO = 16,
    BUFFER_MAXIMO = 128
};

int main(void) {
    /* Uso da tag: a palavra enum é necessária. */
    enum Estado estado = LIGADO;

    /* Uso do typedef anônimo: não escrevemos enum antes de Tamanho. */
    Tamanho tamanho = GRANDE;

    /* Uso do tipo com tag + typedef nas duas formas equivalentes. */
    Direcao direcao_1 = NORTE;
    enum Direcao direcao_2 = OESTE;

    printf("=== Valores dos enumeradores ===\n");
    printf("DESLIGADO = %d, LIGADO = %d, PAUSADO = %d\n",
           DESLIGADO, LIGADO, PAUSADO);
    printf("PEQUENO = %d, MEDIO = %d, GRANDE = %d\n",
           PEQUENO, MEDIO, GRANDE);
    printf("BUFFER_MINIMO = %d, BUFFER_MAXIMO = %d\n",
           BUFFER_MINIMO, BUFFER_MAXIMO);
    printf("sizeof(enum Estado) = %zu\n", sizeof estado);
    printf("endereco de estado = %p\n", (void *)&estado);

    printf("\n=== Tipos com tag e typedef ===\n");
    printf("direcao_1 (typedef Direcao) = %d\n", direcao_1);
    printf("direcao_2 (enum Direcao) = %d\n", direcao_2);
    printf("tamanho (typedef anonimo) = %d\n", tamanho);

    if (estado == LIGADO)
        printf("O dispositivo esta ligado.\n");

    /* Sem valor explícito, o primeiro enumerador vale 0 e os seguintes
       incrementam 1; PAUSADO vale 11 porque LIGADO vale 10. */
    printf("\nEnums sao constantes inteiras nomeadas, nao textos.\n");
    printf("Eles tornam comparacoes mais legiveis que numeros magicos.\n");
    return 0;
}
