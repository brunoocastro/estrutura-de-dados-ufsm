# Lista de Exercícios 01 (L01) — Estruturas de controle

Lista **não avaliada**: as soluções ficam publicadas para estudo. Tente resolver antes de abrir o
arquivo. Cada exercício é um programa independente, com um cabeçalho que explica o objetivo, um
exemplo real e a complexidade.

## Como executar

Na raiz do repositório (o script `run` compila para `build/` com `-lm` incluído):

```bash
./run list lista_01                                # lista os exercícios
echo "12 3" | ./run 01_divisibilidade
echo "2 3 4" | ./run 03_operacoes_tres_inteiros
echo "65" | ./run 04a_ascii
echo "1 2 0.000001" | ./run 04b_bissecao
echo "1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 100" | ./run 05_vetor_intervalo
echo "2024 1 1 10 1" | ./run 06_dias_decorridos      # ano dia1 mes1 dia2 mes2
echo "2" | ./run 07_dia_semana
echo "1 -3 2" | ./run 08_equacao_segundo_grau
echo "70 1.75" | ./run 09_imc
echo "3 4 5" | ./run 10_verifica_triangulo
echo "3 3 4" | ./run 11_classifica_triangulo
./run 02_soma_1_a_100                              # sem entrada; confira "Soma: 5050"
```

Sem `echo ... |`, o programa espera você digitar os valores. Com a entrada vazia
(`< /dev/null`), todos terminam normalmente.

| Exercício | Arquivo | Conceito praticado | Complexidade |
| --- | --- | --- | --- |
| 1 | `01_divisibilidade.c` | operador `%`, `if/else` | O(1) |
| 2 | `02_soma_1_a_100.c` | laço `for`, acumulador | O(n) |
| 3 | `03_operacoes_tres_inteiros.c` | relacionais, ternário, divisão inteira | O(1) |
| 4a | `04a_ascii.c` | `char` como número, laço | O(k) |
| 4b | `04b_bissecao.c` | `while` com critério de parada | O(log(L/tol)) |
| 5 | `05_vetor_intervalo.c` | vetor, validação | O(n) |
| 6 | `06_dias_decorridos.c` | vetor como tabela, funções | O(1) |
| 7 | `07_dia_semana.c` | vetor de strings como tabela | O(1) |
| 8 | `08_equacao_segundo_grau.c` | `if / else if / else`, `<math.h>` | O(1) |
| 9 | `09_imc.c` | cadeia de faixas | O(1) |
| 10 | `10_verifica_triangulo.c` | `&&` com curto-circuito | O(1) |
| 11 | `11_classifica_triangulo.c` | ordem dos testes | O(1) |

Os enunciados estão abaixo.

## 1. Divisibilidade — [`01_divisibilidade.c`](01_divisibilidade.c)

Escreva um programa em C para determinar se um número `A` é divisível por outro número `B`. Esses valores devem ser fornecidos pelo usuário ou pela usuária.

## 2. Números de 1 a 100 — [`02_soma_1_a_100.c`](02_soma_1_a_100.c)

Crie um programa em C que imprima todos os números de 1 até 100, inclusive, e a soma de todos eles.

## 3. Operações com três números inteiros — [`03_operacoes_tres_inteiros.c`](03_operacoes_tres_inteiros.c)

Dados três números inteiros com sinal, construa programas em C para:

### a) Números positivos

Verificar se os números são positivos.

### b) Maior número

Verificar qual dos números é o maior.

### c) Menor número

Verificar qual dos números é o menor.

### d) Operações

Realizar as seguintes operações:

1. `A + B + C`
2. `A * B > C`
3. `A / C < B`

## 4. Estruturas de controle

- Item a: [`04a_ascii.c`](04a_ascii.c)
- Item b: [`04b_bissecao.c`](04b_bissecao.c)

Utilizando estruturas de controle, resolva os problemas a seguir:

### a) Caracteres ASCII

Quais caracteres ASCII representam o valor inteiro de `A`, para `A < 123` e `A > 64`?

### b) Método da bisseção

Implemente o método da bisseção, conforme apresentado em aula anterior.

## 5. Vetor de valores inteiros — [`05_vetor_intervalo.c`](05_vetor_intervalo.c)

A partir de um vetor com 20 valores inteiros, fornecidos como entrada, verifique se todos os valores pertencem ao intervalo fechado `[0, 100]`.

## 6. Dias decorridos — [`06_dias_decorridos.c`](06_dias_decorridos.c)

Faça um programa que calcule o número de dias decorridos desde uma data fornecida pelo usuário ou pela usuária, considerando que ambas as datas estão dentro do mesmo ano.

## 7. Dia da semana — [`07_dia_semana.c`](07_dia_semana.c)

Crie um programa que leia um número inteiro entre 1 e 7 e escreva o dia da semana correspondente.

Caso o usuário ou a usuária digite um número fora desse intervalo, deverá aparecer uma mensagem informando que não existe dia da semana correspondente a esse número.

## 8. Equação do segundo grau — [`08_equacao_segundo_grau.c`](08_equacao_segundo_grau.c)

Construa um programa para calcular as raízes de uma equação do segundo grau, sendo que os valores dos coeficientes `A`, `B` e `C` devem ser fornecidos pelo usuário ou pela usuária através do teclado.

## 9. Índice de Massa Corpórea (IMC) — [`09_imc.c`](09_imc.c)

Construa um programa para determinar se o indivíduo está com um peso favorável.

Essa situação é determinada através do IMC (Índice de Massa Corpórea), definido como a relação entre o peso (`PESO`) e o quadrado da altura (`ALTURA`) do indivíduo:

$$
IMC = \frac{PESO}{ALTURA^2}
$$

A situação do peso é determinada pela tabela abaixo:

| Condição do IMC | Situação |
| --- | --- |
| IMC abaixo de 20 | Abaixo do peso |
| IMC de 20 a 25 | Peso normal |
| IMC de 25 até 30 | Sobrepeso |
| IMC de 30 a 40 | Obeso |
| IMC de 40 ou acima | Obeso mórbido |

> **Observação:** a tabela é ambígua nos limites `20`, `25`, `30` e `40`. A solução adota a regra
> "cada faixa inclui o limite inferior e exclui o superior" (IMC = 25,0 é sobrepeso).

## 10. Verificação de triângulo — [`10_verifica_triangulo.c`](10_verifica_triangulo.c)

Dados três valores `A`, `B` e `C`, construa um programa para verificar se esses valores podem ser os lados de um triângulo.

## 11. Classificação do triângulo — [`11_classifica_triangulo.c`](11_classifica_triangulo.c)

Dados três valores `A`, `B` e `C`, construa um programa para verificar se esses valores podem ser os lados de um triângulo e, em caso afirmativo, classificá-lo como:

- triângulo escaleno;
- triângulo equilátero; ou
- triângulo isósceles.
