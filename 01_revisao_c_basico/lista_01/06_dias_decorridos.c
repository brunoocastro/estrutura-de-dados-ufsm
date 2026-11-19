/*
 * ============================================================================
 * Arquivo : 06_dias_decorridos.c
 * Módulo  : 01 — Revisão de C básico, Lista L01    (Ementa: Unidade 0.3 e 0.4)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Exercício 6: dias decorridos entre duas datas do mesmo ano, usando um
 *   vetor com a quantidade de dias de cada mês e a regra do ano bissexto.
 *
 * EXEMPLO REAL
 *   Plano de manutenção preventiva: quantos dias faltam (ou passaram) até a
 *   próxima troca de óleo de um redutor; registro de "dia juliano" em
 *   dataloggers.
 *
 * COMPLEXIDADE
 *   O(12) = O(1): o laço soma no máximo 12 meses. O vetor de meses funciona
 *   como tabela de consulta (lookup table): dias[mes] em O(1) em vez de uma
 *   cadeia de if/else.
 *
 * COMO EXECUTAR
 *   echo "2024 1 1 10 1" | ./run 06_dias_decorridos    (ano dia1 mes1 dia2 mes2)
 * ============================================================================
 */
#include <stdio.h>

static int bissexto(int ano) {
    return ano % 400 == 0 || (ano % 4 == 0 && ano % 100 != 0);
}

/* Pré-condição: 1 <= mes <= 12. */
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
    int lidos = scanf("%d %d %d %d %d", &ano, &d1, &m1, &d2, &m2);
    if (lidos == EOF) {
        printf("\nSem entrada.\n");
        return 0;
    }
    /* Os meses são testados antes de dias_no_mes ser chamada: o || para no
       primeiro teste verdadeiro e evita acessar dias[13]. */
    if (lidos != 5 ||
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
