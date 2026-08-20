#include <stdio.h>

static int bissexto(int ano) {
    return ano % 400 == 0 || (ano % 4 == 0 && ano % 100 != 0);
}

static int dias_no_mes(int mes, int ano) {
    const int dias[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    return dias[mes] + (mes == 2 && bissexto(ano));
}

static int dia_do_ano(int dia, int mes, int ano) {
    int total = dia;
    for (int m = 1; m < mes; m++)
        total += dias_no_mes(m, ano);
    return total;
}

int main(void) {
    int d1, m1, d2, m2, ano;

    printf("Digite o ano e as duas datas (dia mes dia mes): ");
    if (scanf("%d %d %d %d %d", &ano, &d1, &m1, &d2, &m2) != 5 ||
        ano < 1 || m1 < 1 || m1 > 12 || m2 < 1 || m2 > 12 ||
        d1 < 1 || d1 > dias_no_mes(m1, ano) ||
        d2 < 1 || d2 > dias_no_mes(m2, ano)) {
        fprintf(stderr, "Datas invalidas.\n");
        return 1;
    }

    printf("Dias decorridos entre as datas: %d\n",
           dia_do_ano(d2, m2, ano) - dia_do_ano(d1, m1, ano));
    return 0;
}
