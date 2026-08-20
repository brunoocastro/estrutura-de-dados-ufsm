# Lista de Exercícios 02

Soluções independentes em C para os dez exercícios da lista.

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

## Compilação

Na raiz do repositório, compile um arquivo com:

```bash
gcc -std=c11 -Wall -Wextra -pedantic "2_revisao_funcoes_recursao/lista_2/01_potencia_int.c" -o /tmp/01_potencia
```

O exercício 10 usa funções matemáticas da biblioteca padrão e pode ser compilado
com `-lm`:

```bash
gcc -std=c11 -Wall -Wextra -pedantic "2_revisao_funcoes_recursao/lista_2/10_mediana.c" -lm -o /tmp/10_mediana
```

Os programas leem os dados pela entrada padrão (`stdin`).

- No exercício 2, informe `capital taxa meses`, como `1000 0.02 12`.
- No exercício 4, informe os inteiros e finalize com `0`.
- Nos exercícios 5 e 6, são lidos exatamente 10 valores.
- Nos exercícios 3 e 4, o vetor possui espaço para até 100 valores.
