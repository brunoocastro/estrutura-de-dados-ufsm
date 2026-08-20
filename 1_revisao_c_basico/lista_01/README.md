# Lista de Exercícios 01

As soluções abaixo foram implementadas em C. Cada exercício possui um arquivo-fonte independente.

## Compilação e execução

Requisitos: `gcc` ou outro compilador compatível com C11.

A partir da raiz do repositório, compile um exercício com:

```bash
gcc -std=c11 -Wall -Wextra -Wpedantic 0_revisao/lista_01/01_divisibilidade.c -o /tmp/exercicio01
/tmp/exercicio01
```

Para o exercício 08, que usa funções matemáticas, acrescente `-lm`:

```bash
gcc -std=c11 -Wall -Wextra -Wpedantic 0_revisao/lista_01/08_equacao_segundo_grau.c -lm -o /tmp/exercicio08
/tmp/exercicio08
```

Para compilar todos os exercícios de uma vez:

```bash
mkdir -p /tmp/lista-01-bin
for arquivo in 0_revisao/lista_01/*.c; do
  nome=$(basename "$arquivo" .c)
  if [ "$nome" = "08_equacao_segundo_grau" ]; then
    gcc -std=c11 -Wall -Wextra -Wpedantic "$arquivo" -lm -o "/tmp/lista-01-bin/$nome"
  else
    gcc -std=c11 -Wall -Wextra -Wpedantic "$arquivo" -o "/tmp/lista-01-bin/$nome"
  fi
done
```

Em sistemas Windows, substitua `/tmp/lista-01-bin` por uma pasta local e execute os arquivos `.exe` gerados.

### Testes rápidos

Os comandos a seguir executam casos representativos após a compilação de todos os arquivos:

```bash
echo "12 3" | /tmp/lista-01-bin/01_divisibilidade
echo "2 3 4" | /tmp/lista-01-bin/03_operacoes_tres_inteiros
echo "65" | /tmp/lista-01-bin/04a_ascii
echo "1 2 0.000001" | /tmp/lista-01-bin/04b_bissecao
echo "1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 100" | /tmp/lista-01-bin/05_vetor_intervalo
echo "2024 1 1 10 1" | /tmp/lista-01-bin/06_dias_decorridos
echo "2" | /tmp/lista-01-bin/07_dia_semana
echo "1 -3 2" | /tmp/lista-01-bin/08_equacao_segundo_grau
echo "70 1.75" | /tmp/lista-01-bin/09_imc
echo "3 4 5" | /tmp/lista-01-bin/10_verifica_triangulo
echo "3 3 4" | /tmp/lista-01-bin/11_classifica_triangulo
```

O exercício 02 não recebe entrada; basta executar o binário e conferir `Soma: 5050`.

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

> **Observação:** os limites `25`, `30` e `40` devem seguir exatamente a regra definida na implementação.

## 10. Verificação de triângulo — [`10_verifica_triangulo.c`](10_verifica_triangulo.c)

Dados três valores `A`, `B` e `C`, construa um programa para verificar se esses valores podem ser os lados de um triângulo.

## 11. Classificação do triângulo — [`11_classifica_triangulo.c`](11_classifica_triangulo.c)

Dados três valores `A`, `B` e `C`, construa um programa para verificar se esses valores podem ser os lados de um triângulo e, em caso afirmativo, classificá-lo como:

- triângulo escaleno;
- triângulo equilátero; ou
- triângulo isósceles.
