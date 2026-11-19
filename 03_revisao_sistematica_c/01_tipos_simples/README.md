# 01 — Tipos simples (primitivos)

**Ementa:** Unidade 1.1 (tipos primitivos de dados), revisão da Unidade 0.3 (variáveis).

Todo dado em C tem um **tipo**, que define quantos bytes ele ocupa, que valores aceita e como os
bits são interpretados. Antes de montar estruturas de dados é preciso saber quanto cada peça custa
em memória e onde ela pode "estourar".

```text
 char      [ 1 byte ]                       'A' == 65
 short     [ 2 bytes ]                      -32768 .. 32767 (típico)
 int       [ 4 bytes ]                      depende da plataforma!
 long      [ 4 ou 8 bytes ]                 Windows 64: 4; Linux/macOS 64: 8
 float     [ 4 bytes ]   ~7 dígitos         0.1 + 0.2 != 0.3
 double    [ 8 bytes ]   ~15 dígitos
 uint16_t  [ 2 bytes ]   exatamente 16 bits (<stdint.h>)
```

## Ordem de estudo

| Arquivo | O que mostra | Complexidade-chave |
| --- | --- | --- |
| `00_tipos_simples_visao_geral.c` | todos os tipos com valor, endereço e `sizeof` | O(1) |
| `01_char.c` | `char` é um inteiro; `'7' - '0' == 7`; sinal de `char` depende da plataforma | O(1) |
| `02_inteiros.c` | limites (`<limits.h>`), `unsigned` dando a volta, `uint8_t/int16_t/uint32_t` | O(1) |
| `03_reais.c` | precisão de `float/double`; por que não comparar reais com `==` | O(1) |
| `04_conversoes.c` | divisão inteira, cast, ADC → tensão, armadilha `signed` × `unsigned` | O(1) |
| `05_constantes.c` | `#define` × `const` × `enum` (e por que `const int` não dimensiona vetor em C) | O(1) |

## Resumo comparativo

| Tipo | Tamanho típico (64 bits) | Faixa / precisão | Quando usar |
| --- | --- | --- | --- |
| `char` / `uint8_t` | 1 | -128..127 / 0..255 | texto, bytes de protocolo |
| `int16_t` / `uint16_t` | 2 | ±32 767 / 0..65 535 | registradores Modbus, ADC |
| `int` | 4 | ±2,1 bilhões | contadores e índices comuns |
| `uint32_t` | 4 | 0..4,29 bilhões | tempo em ms (dá a volta em ~49,7 dias) |
| `long long` | 8 | ±9,2·10^18 | contagens grandes |
| `float` | 4 | ~7 dígitos | sensores em MCU sem FPU dupla |
| `double` | 8 | ~15 dígitos | cálculos de engenharia em geral |

## Exemplos reais

- Registrador Modbus de 16 bits, ADC de 12 bits (0..4095) convertido em tensão.
- Encoder incremental que dá a volta (`uint16_t`), contador de milissegundos (`uint32_t`).
- Comparação de temperatura com tolerância (banda morta) em vez de `==`.

## Como executar

```sh
./run 00_tipos_simples_visao_geral
./run 02_inteiros
./run list tipos_simples
```

## Exercícios propostos

1. Escreva um programa que imprima `sizeof` de `int8_t` a `int64_t` e os limites de cada um
   usando as macros de `<stdint.h>`.
2. Um ADC de 10 bits mede de 0 a 5 V. Escreva a conversão leitura → tensão e teste-a com
   0, 512 e 1023. Mostre o resultado errado que surge se a divisão for feita primeiro.
3. Um contador `uint8_t` é incrementado 300 vezes a partir de 0. Qual o valor final? Confirme
   executando e explique.
4. Escreva `iguais(a, b, tol)` para `double` e use-a para comparar `0.1 * 3` com `0.3`.
5. Explique, com um exemplo, por que `-1 < 1u` é falso em C.

Próximo: [`../02_tipos_compostos/`](../02_tipos_compostos/).
