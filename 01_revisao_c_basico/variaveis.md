# 1. Variáveis (Gerenciamento de Estado)

Abstratamente, uma variável é um rótulo simbólico que aponta para um endereço na memória física do computador onde um valor está armazenado.

```text
 nome         tipo     endereço (exemplo)   memória
 idade        int      0x7ffd...a0          [ 33        ]  4 bytes
 moeda        float    0x7ffd...a4          [ 2.5       ]  4 bytes
 dinheiro     double   0x7ffd...a8          [ 2.44444   ]  8 bytes
 sexo         char     0x7ffd...b0          [ 'M' = 77  ]  1 byte
```

Para entender variáveis profundamente, precisamos dominar dois conceitos relacionados:

* **Tipagem (Estática vs. Dinâmica):** Em linguagens de tipagem estática (como C), o tamanho e o formato da memória (inteiro, decimal, caractere) são definidos na compilação e não mudam. Em linguagens de tipagem dinâmica (Python e JavaScript), o interpretador descobre o tipo em tempo de execução e a mesma variável pode apontar para um número agora e para um texto depois.

* **Escopo e Ciclo de Vida:** Define onde a variável "existe". Em C e JS (com `let`/`const`), o escopo é baseado em blocos `{}`. Em Python, o escopo é delimitado por funções ou módulos.

### O exemplo da aula, em C

A figura usada em aula ([`image.png`](image.png)) está escrita em **C++** (`iostream`, `cout`,
`string`, `bool`, `system("pause")`). Nesta disciplina usamos **C**. O equivalente em C é:

```c
#include <stdbool.h>   /* bool, true, false (C99 em diante) */
#include <stdio.h>

int main(void) {
    int idade = 33;
    float moeda = 2.5f;           /* sufixo f: literal float, não double */
    double dinheiro = 2.44444;
    char sexo = 'M';              /* aspas simples: um caractere */
    char nome[] = "Marco Antonio"; /* aspas duplas: string = vetor de char + '\0' */
    bool permissao = true;

    printf("%d\n%.1f\n%.5f\n%c\n%s\n%d\n",
           idade, moeda, dinheiro, sexo, nome, permissao);
    return 0;
}
```

Diferenças importantes: C não tem o tipo `string` (usa vetor de `char`, ver
[`05_strings_comando_serial.c`](05_strings_comando_serial.c)); a saída é feita com `printf` e
especificadores de formato; e `system("pause")` só funciona no Windows (prefira executar no
terminal).

### Tipos básicos de C (tamanhos típicos em PC de 64 bits)

| Tipo | Tamanho típico | Faixa típica | `printf` / `scanf` | Uso em automação |
| --- | --- | --- | --- | --- |
| `char` | 1 byte | -128..127 ou 0..255 | `%c` | caractere de protocolo serial |
| `int` | 4 bytes | ±2,1 × 10⁹ | `%d` | contadores, estados |
| `long long` | 8 bytes | ±9,2 × 10¹⁸ | `%lld` | totalizadores, tempo em ms |
| `float` | 4 bytes | ~7 dígitos | `%f` / `%f` | leituras de sensor em microcontrolador |
| `double` | 8 bytes | ~15 dígitos | `%f` / `%lf` | cálculos de engenharia |

Os tamanhos dependem da plataforma: em muitos microcontroladores de 8 bits `int` tem 2 bytes
(-32 768..32 767). Quando o tamanho importa (registradores, protocolos), use `<stdint.h>`:
`uint8_t`, `int16_t`, `uint32_t`. Detalhes em
[`../03_revisao_sistematica_c/01_tipos_simples/`](../03_revisao_sistematica_c/01_tipos_simples/).

### Escopo em C

```c
int total_pecas = 0;              /* global: visível no arquivo todo (evite abusar) */

void conta_peca(void) {
    int lote = 10;                /* local: existe só durante a chamada */
    total_pecas += lote;
    for (int i = 0; i < 3; i++) { /* i existe só dentro do for */
        /* ... */
    }
    /* aqui 'i' não existe mais */
}
```

### Armadilhas comuns

- **Variável não inicializada**: `int soma; soma += x;` usa lixo de memória. Inicialize sempre.
- **Divisão inteira**: `int media = 7 / 2;` dá `3`. Use `7 / 2.0` ou converta para `double`.
- **Estouro (overflow)**: `int` de 16 bits em um microcontrolador estoura em 32 767; um contador
  de peças que "fica negativo" costuma ser isso.
- **`scanf` sem `&`**: `scanf("%d", x)` em vez de `scanf("%d", &x)` faz o programa escrever em um
  endereço aleatório.
- **`float` com `==`**: `0.1 + 0.2 == 0.3` é falso. Compare com tolerância.

### Exemplo 1: Engenharia (Física/Elétrica) - Implementado em C

Na engenharia, variáveis frequentemente representam grandezas físicas lidas de sensores, onde o rigor com o tamanho da memória e a precisão do dado é crítico (sistemas embarcados).

```c
// C exige a declaração do tipo (float) e a quantidade de memória é fixa.
float tensao_sensor_v = 3.3f;
float limiar_operacao = 5.0f;

// O compilador avisa se tentarmos colocar um texto aqui por engano.
```

### Exemplo 2: Ciência da Computação - Implementado em JavaScript

Na ciência da computação, variáveis frequentemente guardam estruturas de dados complexas ou referências em memória, como caches ou sessões ativas, onde a flexibilidade é mais vantajosa que o controle rigoroso de hardware.

```javascript
// JS (dinâmico) usa 'let' ou 'const'. O tipo é inferido em tempo de execução.
const maxConexoes = 100; // Valor primitivo (imutável)
let sessaoAtiva = {
    usuarioId: 4021,
    token: "abc123xyz"
}; // Objeto dinâmico na memória heap
```
