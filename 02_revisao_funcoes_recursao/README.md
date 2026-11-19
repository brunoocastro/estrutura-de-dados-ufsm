# 02 — Revisão de funções e recursividade

Ementa, **Unidade 0.6 — Funções e recursividade**. Esta pasta cobre o uso de funções para
organizar programas de controle (passagem por valor, retorno, reuso, testes) e o primeiro contato
com recursão. A revisão aprofundada de recursão (fatorial, Fibonacci, busca binária recursiva,
profundidade da pilha), ponteiros e alocação dinâmica fica em
[`../03_revisao_sistematica_c/`](../03_revisao_sistematica_c/) (pastas `03_funcoes/` e
`04_recursao/`).

## O que a ementa pede e onde está

| Tópico | Onde estudar |
| --- | --- |
| Funções: parâmetros, retorno, passagem por valor | `01_funcoes_passagem_valor.c`, [Lista 02](lista_2/README.md) ex. 1, 2, 8, 9 |
| Funções com vetores | `01_funcoes_passagem_valor.c` (Parte 3), L02 ex. 3 a 7 e 10 |
| Modularização e reuso | `02_modularizacao_controle_nivel.c`, L02 ex. 2, 9, 10 |
| Recursividade (introdução) | `03_recursao_partida_esteiras.c`, [`../03_revisao_sistematica_c/04_recursao/`](../03_revisao_sistematica_c/04_recursao/) |
| Avaliação | [Trabalho 2 (T02)](trabalho_2/README.md): só o enunciado |

Pré-requisito: [`../01_revisao_c_basico/`](../01_revisao_c_basico/) (controle, vetores, strings e
[Big O](../01_revisao_c_basico/complexidade_big_o.md)).

## Conceito em uma página

```text
 Passagem por valor                       Recursão: ida e volta
 main            setpoint = 75            partir(1) verifica E1 ─┐ IDA (empilha)
   │ chama f(setpoint)                     partir(2) verifica E2 ─┤
   ▼                                        partir(3) caso base   ─┘
 f               setpoint = 75 (cópia)      partir(2) liga E2     ─┐ VOLTA (desempilha)
                 setpoint = 0  (só aqui)   partir(1) liga E1     ─┘
   │ return
   ▼                                       pilha de chamadas: um quadro por chamada
 main            setpoint = 75 (intacto)   espaço extra O(profundidade)
```

- Argumentos são **copiados**; o resultado volta por `return`. Vetores são a exceção: a função
  recebe onde o vetor começa e pode alterá-lo.
- Uma função recursiva precisa de **caso base** (para quando parar) e de um **passo** que se
  aproxima do caso base. Sem caso base: estouro de pilha.

## Ordem de estudo recomendada

| # | Arquivo | O que mostra | Complexidade-chave |
| --- | --- | --- | --- |
| 1 | `01_funcoes_passagem_valor.c` | caixas de memória da cópia, `return`, funções puras (ADC, 4–20 mA) testadas em tabela, vetores não copiados | O(1) por chamada |
| 2 | `02_modularizacao_controle_nivel.c` | laço de controle de nível em 6 funções; troca P ↔ liga/desliga mudando uma linha | O(1) por ciclo, O(K) total |
| 3 | [Lista 02](lista_2/README.md) | 10 exercícios de funções com vetores, com soluções | O(1) a O(n) |
| 4 | `03_recursao_partida_esteiras.c` | pilha de chamadas desenhada; trabalho na ida e na volta; versão iterativa | O(n) tempo, O(n) pilha |
| 5 | [`../03_revisao_sistematica_c/04_recursao/`](../03_revisao_sistematica_c/04_recursao/) | fatorial, Fibonacci, busca binária, MDC | O(n), O(2ⁿ), O(log n) |
| 6 | [Trabalho 2](trabalho_2/README.md) | trabalho avaliado | — |

## Comparação: iterativo × recursivo

| Critério | Iterativo (laço) | Recursivo |
| --- | --- | --- |
| Tempo (mesmo algoritmo) | O(n) | O(n) |
| Espaço extra | O(1) | O(n): um quadro de pilha por chamada |
| Custo por passo | incremento e teste | chamada + retorno (mais caro) |
| Risco | laço infinito | estouro de pilha (sem caso base ou profundidade grande) |
| Quando brilha | percursos lineares simples | problemas definidos por partes menores: árvores, divisão e conquista |

Em microcontroladores com pouca RAM a pilha é pequena (às vezes 1 a 4 KB): recursão profunda é
evitada e muitas normas de software embarcado (MISRA C) a proíbem.

## Exemplos reais

- **Conversões de sinal** (ADC → volts, 4–20 mA → bar): funções pequenas, puras e testáveis.
- **Blocos de função de CLP** (FB/FC da IEC 61131-3): a mesma ideia de modularização: entradas,
  saídas e uma responsabilidade.
- **Intertravamento de esteiras**: verificar na ordem do fluxo, ligar na ordem inversa (pilha).
- **Dia a dia**: o botão "desfazer" de um editor e a navegação "voltar" do navegador usam a mesma
  ideia de pilha que a recursão usa por baixo dos panos (veja [`../05_pilhas/`](../05_pilhas/)).

## Como executar

Na raiz do repositório:

```bash
./run list 02_revisao_funcoes_recursao
./run 01_funcoes_passagem_valor           # ENTER avança cada passo
./run 02_modularizacao_controle_nivel
./run 03_recursao_partida_esteiras
./run 03_recursao_partida_esteiras < /dev/null   # sem pausas
```

Os exercícios da lista têm exemplos de entrada no [README da Lista 02](lista_2/README.md).

## Exercícios propostos (sem solução)

1. Escreva `double celsius_para_fahrenheit(double c)` e um programa de teste em tabela com pelo
   menos cinco casos (incluindo -40, em que as duas escalas coincidem).
2. Em `02_modularizacao_controle_nivel.c`, acrescente um controlador PI
   (`u = Kp·e + Ki·Σe`). Que informação extra ele precisa guardar entre ciclos? Como passá-la sem
   variáveis globais?
3. Mude `03_recursao_partida_esteiras.c` para que a **parada** da linha também seja recursiva:
   desligar E1 primeiro e En por último. O trabalho deve ficar na ida ou na volta?
4. Escreva uma função recursiva que imprima os dígitos de um número inteiro positivo na ordem
   normal e outra que imprima na ordem inversa, mudando apenas a posição do `printf` em relação à
   chamada recursiva.
5. Escreva uma função recursiva `soma_digitos(n)` (ex.: 1234 → 10). Qual é o caso base? Qual é a
   profundidade máxima da recursão para um `int` de 32 bits?
6. Para cada função da Lista 02, diga o melhor e o pior caso em número de comparações.

## Material da disciplina (Google Classroom)

Tópico "Revisão sobre funções, recursividade e ponteiros": Lista L02 (não avaliada, soluções nesta
pasta), Trabalho T02 (avaliado, só o enunciado nesta pasta) e o exemplo de recursividade em
Python mostrado em aula. Ponteiros: [`../03_revisao_sistematica_c/`](../03_revisao_sistematica_c/).
