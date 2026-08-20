# Lista de Exercícios 02

Soluções independentes em C para os dez exercícios da lista. Os arquivos contêm
comentários explicativos e exibem mensagens (`printf`) antes de solicitar as
entradas.

| Exercício | Arquivo |
| --- | --- |
| 1. Potência por multiplicações | `01_potencia_int.c` |
| 2. Juros compostos | `02_juros_compostos.c` |
| 3. Função de inversão | `03_inverte_vetor.c` |
| 4. Teste da inversão | `04_testa_inverte_vetor.c` |
| 5. Função `em_ordem` | `05_em_ordem.c` |
| 6. Teste de `em_ordem` | `06_testa_em_ordem.c` |
| 7. Inserção em vetor ordenado | `07_insere_em_ordem.c` |
| 8. Verificação de triângulo | `08_forma_triangulo.c` |
| 9. Classificação de triângulo | `09_classifica_triangulo.c` |
| 10. Mediana | `10_mediana.c` |

## Build e execução

Na raiz do repositório, compile o arquivo desejado com:

```bash
gcc -std=c11 -Wall -Wextra -pedantic \
  "2_revisao_funcoes_recursao/lista_2/01_potencia_int.c" \
  -o /tmp/01_potencia_int
```

Execute depois o binário criado:

```bash
/tmp/01_potencia_int
```

Para o exercício 10, inclua `-lm` na compilação:

```bash
gcc -std=c11 -Wall -Wextra -pedantic \
  "2_revisao_funcoes_recursao/lista_2/10_mediana.c" \
  -lm -o /tmp/10_mediana
/tmp/10_mediana
```

Para outro exercício, troque o nome do arquivo e do executável. Também é possível
fornecer as entradas diretamente pelo terminal, usando `printf`:

```bash
printf "2 3\\n" | /tmp/01_potencia_int
printf "1000 0.02 12\\n" | /tmp/02_juros_compostos
printf "1 2 3 0\\n" | /tmp/03_inverte_vetor
```

## Formato das entradas

- Exercício 1: `a b`, com base real e expoente inteiro não negativo.
- Exercício 2: `capital taxa meses`, por exemplo `1000 0.02 12`.
- Exercícios 3 e 4: inteiros até `0`, que encerra a leitura; o vetor suporta até 100 valores.
- Exercícios 5 e 6: exatamente 10 valores reais.
- Exercício 7: `n`, os `n` valores ordenados e o valor a inserir.
- Exercícios 8 e 9: os três lados `A B C`.
- Exercício 10: o tamanho do vetor seguido dos valores em ordem crescente.
