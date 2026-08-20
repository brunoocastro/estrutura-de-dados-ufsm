#include <stdio.h>

int main(void) {
    int numero;
    const char *dias[] = {"domingo", "segunda-feira", "terca-feira",
                          "quarta-feira", "quinta-feira", "sexta-feira",
                          "sabado"};

    printf("Digite um numero de 1 a 7: ");
    if (scanf("%d", &numero) != 1) {
        fprintf(stderr, "Entrada invalida.\n");
        return 1;
    }
    if (numero < 1 || numero > 7)
        printf("Nao existe dia da semana com esse numero.\n");
    else
        printf("Dia da semana: %s.\n", dias[numero - 1]);

    return 0;
}
