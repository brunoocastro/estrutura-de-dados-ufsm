# Trabalho 2 — Funções e recursão

**DPEE1038 — Estrutura de Dados para Automação**

Cada problema possui um programa independente em C. Os comentários no código
explicam a lógica das funções e os `printf` indicam ao usuário cada entrada
solicitada.

| Problema | Arquivo | Entrada solicitada |
| --- | --- | --- |
| 1 | `01_potencia_int.c` | base `a` e expoente `b` |
| 2 | `02_existe_vec.c` | tamanho, elementos e valor procurado |
| 3 | `03_insere_em_ordem.c` | tamanho, vetor ordenado e valor `x` |
| 4 | `04_mediana.c` | tamanho e elementos do vetor |

## Build e execução

Na raiz do repositório, compile um arquivo com `gcc`:

```bash
gcc -std=c11 -Wall -Wextra -pedantic \
  2_revisao_funcoes_recursao/trabalho_2/01_potencia_int.c \
  -lm -o /tmp/01_potencia_int
```

Depois execute o programa:

```bash
/tmp/01_potencia_int
```

Para compilar e executar qualquer outro exercício, substitua o nome do arquivo
e do executável. Os exercícios 1 e 4 usam a biblioteca matemática e devem ser
compilados com `-lm`.

Exemplo com entrada redirecionada:

```bash
printf "2 8\\n" | /tmp/01_potencia_int
```

Os programas leem os dados pela entrada padrão (`stdin`).

## Observações

- `potencia_int` calcula potências recursivamente, inclusive expoentes negativos.
- `existe_vec` percorre o vetor recursivamente e retorna `1` ou `0`.
- `insere_em_ordem` recebe espaço para `n+1` elementos e desloca os maiores que `x`.
- `mediana` retorna a mediana se o vetor estiver ordenado e `NAN` caso contrário ou se estiver vazio.
