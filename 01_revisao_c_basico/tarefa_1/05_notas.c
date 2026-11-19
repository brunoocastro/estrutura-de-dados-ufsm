#include <stdio.h>

int main(void) {
    double nota1, nota2;
    if (scanf("%lf %lf", &nota1, &nota2) != 2) return 1;

    double media = (nota1 + nota2) / 2.0;
    if (media >= 7.0) {
        printf("aprovado/aprovada\n");
    } else {
        double exame;
        if (scanf("%lf", &exame) != 1) return 1;
        double media_final = (media + exame) / 2.0;
        printf("%s\n", media_final >= 5.0
            ? "aprovado/aprovada" : "reprovado/reprovada");
    }
    return 0;
}
