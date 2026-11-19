# Lista de Exercícios 02 (L02) — Funções e vetores

Lista **não avaliada**: as soluções ficam publicadas para estudo. Tente resolver antes de abrir o
arquivo. Cada exercício é um programa independente, com um cabeçalho que explica o objetivo, um
exemplo real e a complexidade. Os programas mostram uma mensagem antes de cada leitura.

| Exercício | Arquivo | O que pede | Complexidade |
| --- | --- | --- | --- |
| 1 | `01_potencia_int.c` | função `potencia_int(a, b)`: `a^b` por multiplicações sucessivas (`b >= 0`) | O(b) |
| 2 | `02_juros_compostos.c` | montante `M = C(1 + i)^t` reaproveitando `potencia_int` | O(t) |
| 3 | `03_inverte_vetor.c` | função que inverte um vetor no próprio lugar | O(n) |
| 4 | `04_testa_inverte_vetor.c` | programa de teste da inversão (inclui casos automáticos) | O(n) |
| 5 | `05_em_ordem.c` | função `em_ordem(v, n)`: 1 se o vetor está em ordem crescente | O(n) |
| 6 | `06_testa_em_ordem.c` | programa de teste de `em_ordem` (inclui casos automáticos) | O(n) |
| 7 | `07_insere_em_ordem.c` | inserir `x` em vetor ordenado, deslocando os maiores | O(n) |
| 8 | `08_forma_triangulo.c` | função `forma_triangulo(a, b, c)` | O(1) |
| 9 | `09_classifica_triangulo.c` | função `classifica_triangulo(a, b, c)` reusando a anterior | O(1) |
| 10 | `10_mediana.c` | função `mediana(v, n)` para vetor ordenado; `NAN` se vazio ou desordenado | O(n) |

Os exercícios 3 e 4 (e 5 e 6) têm a mesma função: o primeiro arquivo é o exercício "escreva a
função"; o segundo é o "escreva um programa que teste a função".

## Como executar

Na raiz do repositório:

```bash
./run list lista_2
echo "2 3" | ./run lista_2/01_potencia_int        # caminho parcial: o nome sozinho é ambíguo
echo "1000 0.02 12" | ./run 02_juros_compostos
echo "1 2 3 0" | ./run 03_inverte_vetor
./run 04_testa_inverte_vetor < /dev/null         # só os testes automáticos
echo "1 2 3 4 5 6 7 8 9 10" | ./run 05_em_ordem
echo "4 1 3 5 7 4" | ./run 07_insere_em_ordem
echo "3 4 5" | ./run 08_forma_triangulo
echo "3 3 4" | ./run 09_classifica_triangulo
echo "5 1 2 3 4 5" | ./run 10_mediana
```

## Formato das entradas

- Exercício 1: `a b`, com base real e expoente inteiro não negativo.
- Exercício 2: `capital taxa meses`, por exemplo `1000 0.02` (2% ao mês) `12`.
- Exercícios 3 e 4: inteiros até `0`, que encerra a leitura; o vetor suporta até 100 valores.
- Exercícios 5 e 6: exatamente 10 valores reais.
- Exercício 7: `n` (0 a 100), os `n` valores ordenados e o valor a inserir.
- Exercícios 8 e 9: os três lados `A B C`.
- Exercício 10: o tamanho do vetor (0 a 100) seguido dos valores em ordem crescente.

Veja também [`../01_funcoes_passagem_valor.c`](../01_funcoes_passagem_valor.c): explica por que
`inverte_vetor` consegue alterar o vetor da `main` e como escrever testes em tabela.
