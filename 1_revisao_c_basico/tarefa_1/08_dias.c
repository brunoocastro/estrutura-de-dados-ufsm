#include <stdio.h>
#include <time.h>

static int bissexto(int ano) {
    return ano % 400 == 0 || (ano % 4 == 0 && ano % 100 != 0);
}

static int dias_no_mes(int mes, int ano) {
    static const int dias[] = {0,31,28,31,30,31,30,31,31,30,31,30,31};
    return dias[mes] + (mes == 2 && bissexto(ano));
}

int main(void) {
    int dia, mes, ano;
    if (scanf("%d %d %d", &dia, &mes, &ano) != 3 || mes < 1 || mes > 12 ||
        dia < 1 || dia > dias_no_mes(mes, ano)) return 1;

    struct tm data = {0};
    data.tm_mday = dia; data.tm_mon = mes - 1; data.tm_year = ano - 1900;
    data.tm_hour = 12;
    time_t fornecida = mktime(&data);
    time_t agora = time(NULL);
    struct tm hoje = *localtime(&agora);
    hoje.tm_hour = 12; hoje.tm_min = 0; hoje.tm_sec = 0;
    time_t inicio_hoje = mktime(&hoje);
    if (fornecida == (time_t)-1 || fornecida > inicio_hoje) return 1;
    printf("%lld\n", (long long)((inicio_hoje - fornecida) / (24 * 60 * 60)));
    return 0;
}
