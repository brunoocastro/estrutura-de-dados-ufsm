# 04 — Recursão

**Ementa:** 0.6 (recursividade). **Classroom:** "Revisão sobre funções, recursividade e
ponteiros" e "Recursividade" (exercícios).

Uma função recursiva resolve um problema chamando a si mesma para uma versão **menor** do mesmo
problema. Toda recursão precisa de:

1. **caso base**, que responde sem chamar de novo;
2. **passo recursivo**, que aproxima a entrada do caso base.

Cada chamada ocupa um **quadro** na pilha de chamadas. Os quadros se empilham na descida e se
desempilham na volta, e é na volta que o trabalho pendente acontece:

```text
 fatorial(3)                         pilha no ponto mais fundo
   ENTRA fatorial(3)                 +--------------------------------+
     ENTRA fatorial(2)               | fatorial(0): caso base -> 1    |  <- topo
       ENTRA fatorial(1)             | fatorial(1): aguarda 1 * f(0)  |
         ENTRA fatorial(0) -> 1      | fatorial(2): aguarda 2 * f(1)  |
       SAI 1 * 1 = 1                 | fatorial(3): aguarda 3 * f(2)  |
     SAI 2 * 1 = 2                   | main                           |
   SAI 3 * 2 = 6                     +--------------------------------+
```

## Ordem de estudo

| Arquivo | O que mostra | Complexidade-chave |
| --- | --- | --- |
| `01_fatorial_passos.c` | **pilha desenhada** a cada empilha/desempilha | O(n) tempo, O(n) pilha |
| `02_fibonacci_passos.c` | árvore de chamadas indentada, chamadas **repetidas** marcadas | O(2^n) |
| `03_soma_array.c` | `soma(v+1, n-1)`: a fatia que cada quadro enxerga | O(n) |
| `04_busca_binaria.c` | intervalo caindo pela metade, desenhado | O(log n) |
| `05_palindromo.c` | pontas comparadas a cada nível | O(n) |
| `06_mdc.c` | Euclides × força bruta | O(log min(a,b)) × O(min(a,b)) |
| `07_fibonacci_memoizacao.c` | ingênuo × memoização × laço; chamadas para n = 10, 20, 30, 40 | O(2^n) × O(n) × O(n) |
| `08_torres_de_hanoi.c` | **pinos desenhados** a cada movimento; 2^n − 1 movimentos | O(2^n) |
| `09_potencia_rapida.c` | b^e com O(log e) multiplicações; Diffie-Hellman | O(e) × O(log e) |
| `10_recursao_de_cauda.c` | recursão comum × de cauda × laço; trabalho pendente na pilha | pilha O(n) × O(1) |
| `11_estouro_de_pilha.c` | mede bytes por quadro e prevê o *stack overflow* sem provocá-lo | pilha O(n) |

O arquivo `../03_funcoes/06_recursao.c` é a introdução (fatorial simples); esta pasta o aprofunda.

## Comparação de complexidade

| Algoritmo | Chamadas | Profundidade da pilha | Alternativa |
| --- | --- | --- | --- |
| fatorial, soma de vetor | n + 1 | n + 1 | laço: O(1) de pilha |
| busca binária | ~log2 n | ~log2 n | laço `while` |
| Fibonacci ingênuo | 2·F(n+1) − 1 (n = 40: 331 milhões) | n | memoização: 2n − 1 chamadas |
| Fibonacci com memoização | 2n − 1 (n = 40: 79) | n | laço: n − 1 iterações, O(1) |
| Torres de Hanói | 2^n − 1 movimentos | n | não há solução melhor |
| potência rápida | ~log2 e | ~log2 e | versão iterativa por bits |
| Euclides (MDC) | O(log min(a,b)) | idem | laço `while (b)` |

**Quando usar recursão:** quando o problema é naturalmente recursivo (árvores, divisão e
conquista, backtracking) **e** a profundidade é pequena e conhecida. **Quando evitar:** recursão
linear com n grande (estoura a pilha) e subproblemas repetidos sem memoização (explosão
exponencial).

## Exemplos reais

- Árvores de tags de um SCADA, pastas de arquivos, expressões aritméticas (estrutura recursiva).
- Programação dinâmica com memoização no planejamento de trajetórias de robôs e AGVs.
- Potência modular na troca de chaves (TLS/VPN) entre gateway e supervisório.
- Firmware de CLP/MCU: normas como MISRA C proíbem recursão por causa do risco de estouro de pilha.

## Como executar

```sh
./run 01_fatorial_passos                     # pede n; ENTER avança cada passo
./run 08_torres_de_hanoi                     # pede o número de discos (1 a 6)
./run 07_fibonacci_memoizacao < /dev/null    # roteiro automático, sem pausas
./run list 04_recursao
```

## Exercícios propostos

1. Escreva `int conta_digitos(unsigned n)` recursiva. Qual a profundidade da pilha para n = 10^9?
2. Escreva `void imprime_binario(unsigned n)` recursiva (imprime os bits na ordem certa usando a
   volta da recursão). Por que imprimir antes da chamada inverte a ordem?
3. Escreva `int maior(const int v[], size_t n)` recursiva e depois a versão de cauda com
   acumulador. Converta a versão de cauda em laço.
4. Conte as chamadas de `C(n, k) = C(n-1, k-1) + C(n-1, k)` (combinações) para n = 20, 25, 30 e
   depois aplique memoização com uma matriz. Compare as contagens.
5. Modifique `08_torres_de_hanoi.c` para usar 4 pinos (problema de Reve) e compare os movimentos
   com a versão de 3 pinos.
6. Escreva `flood_fill` recursivo numa matriz 10×10 de `char` (pinta uma região delimitada) e
   desenhe a matriz a cada passo. Qual a profundidade máxima no pior caso?

Próximo: [`../05_alocacao_dinamica/`](../05_alocacao_dinamica/).
