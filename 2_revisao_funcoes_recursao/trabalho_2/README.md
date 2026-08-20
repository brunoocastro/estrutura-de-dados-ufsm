# Trabalho 2 — Funções e recursão

**DPEE1038 — Estrutura de Dados para Automação**

Cada problema possui um programa independente em C. As funções pedidas ficam no
próprio arquivo e também há um `main` simples para testar a solução pela entrada
padrão.

| Problema | Arquivo | Entrada do programa de teste |
| --- | --- | --- |
| 1 | `01_potencia_int.c` | `a b` |
| 2 | `02_existe_vec.c` | `tamanho`, elementos, valor procurado |
| 3 | `03_insere_em_ordem.c` | `n`, elementos ordenados, `x` |
| 4 | `04_mediana.c` | `n`, elementos |

## Compilação

Na raiz do repositório:

```bash
gcc -std=c11 -Wall -Wextra -pedantic 2_revisao_funcoes_recursao/trabalho_2/01_potencia_int.c -lm -o /tmp/potencia
```

Substitua o nome do arquivo e do executável para testar os demais problemas.

## Observações

- `potencia_int` aceita expoentes negativos; nesse caso calcula o inverso da potência.
- `existe_vec` é recursiva e retorna `1` quando encontra o valor e `0` caso contrário.
- `insere_em_ordem` recebe um vetor com espaço para `n+1` elementos e desloca os elementos maiores que `x`.
- `mediana` retorna a mediana quando o vetor está ordenado. Para vetor vazio ou não ordenado, retorna `NAN`; o `main` informa essa situação.
