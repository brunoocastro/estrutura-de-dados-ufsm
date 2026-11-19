# Complexidade de algoritmos e notação Big O

Este texto é a referência de complexidade usada por todos os módulos do repositório. Os outros
módulos linkam para cá e trazem só a análise da estrutura que estudam.

> Para ver os números crescendo na tela, rode `./run 01_contador_big_o`.

## 1. A pergunta que o Big O responde

Um engenheiro não quer saber se o programa leva 3,2 ms no notebook dele. Quer saber **o que
acontece quando os dados crescem**: o CLP passa de 100 para 10 000 tags, o supervisório passa a
guardar um ano de histórico em vez de um dia, a câmera troca 320×240 por 1920×1080.

Para responder, contamos as **operações básicas** (comparações, atribuições, acessos à memória) em
função do tamanho da entrada `n` e olhamos só para o **ritmo de crescimento**:

```text
 código                                  operações        Big O
 ---------------------------------       --------------   ------
 x = v[0];                               1                O(1)
 for (i = 0; i < n; i++) soma += v[i];   n                O(n)
 for i ... for j ... (n × n)             n²               O(n²)
```

Definição (versão de engenharia): `f(n) = O(g(n))` quando, a partir de um certo `n₀`, `f(n)` fica
sempre abaixo de `c · g(n)` para alguma constante `c`. Em palavras: **`g` é um teto para o
crescimento de `f`, ignorando constantes e valores pequenos de `n`**.

Por que ignorar constantes? Porque elas dependem do processador, do compilador e da linguagem.
O formato da curva não: um algoritmo O(n²) sempre perde de um O(n log n) quando `n` fica grande o
bastante, não importa o hardware.

## 2. Regras de simplificação

| Regra | Exemplo | Resultado |
| --- | --- | --- |
| Constantes multiplicativas somem | `3n + 3n` = `6n` | O(n) |
| Fica só o termo dominante | `n² + 100n + 5000` | O(n²) |
| Blocos em **sequência** somam (vence o maior) | laço O(n) seguido de laço O(n²) | O(n²) |
| Blocos **aninhados** multiplicam | laço de `n` dentro de laço de `m` | O(n·m) |
| Laço que **divide** (ou multiplica) o contador por 2 | `while (n > 1) n /= 2;` | O(log n) |
| Chamar uma função custa o que a função custa | `strlen(s)` dentro de um laço de `n` | O(n) × O(n) = O(n²) |
| Base do logaritmo não importa | `log₂ n` e `log₁₀ n` diferem por constante | O(log n) |

A penúltima regra é a armadilha mais comum de quem começa em C: `for (i = 0; i < strlen(s); i++)`
parece O(n), mas `strlen` percorre a string inteira a cada teste da condição. Veja
`05_strings_comando_serial.c`.

## 3. Melhor caso, caso médio e pior caso

O custo pode depender de **quais** dados chegam, não só de quantos. Exemplo: procurar o código de
um alarme em uma lista de `n` alarmes ativos, de um em um.

| Caso | Situação | Comparações | Big O |
| --- | --- | --- | --- |
| Melhor | o alarme é o primeiro da lista | 1 | O(1) |
| Médio | o alarme está numa posição qualquer | ≈ n/2 | O(n) |
| Pior | o alarme é o último ou não está na lista | n | O(n) |

Em sistemas de controle o **pior caso** costuma ser o que importa: o ciclo de varredura de um CLP
ou a interrupção de um microcontrolador têm prazo fixo, e o prazo precisa ser cumprido também no
dia ruim.

## 4. Complexidade de espaço

A mesma ideia vale para a **memória extra** que o algoritmo usa, além da entrada:

| Situação | Espaço extra |
| --- | --- |
| Algumas variáveis (`soma`, `i`, `max`) | O(1) |
| Uma cópia do vetor de entrada | O(n) |
| Uma matriz `n × n` auxiliar | O(n²) |
| Recursão com profundidade `n` (cada chamada ocupa um quadro na pilha) | O(n) |

Em microcontroladores com 2 KB a 64 KB de RAM, O(n) de espaço extra pode ser o que inviabiliza uma
solução, mesmo que ela seja rápida.

## 5. Tabela de crescimento

Número aproximado de operações para cada classe (log na base 2):

| n | O(1) | O(log n) | O(n) | O(n log n) | O(n²) | O(2ⁿ) |
| ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| 10 | 1 | 3 | 10 | 33 | 100 | 1 024 |
| 100 | 1 | 7 | 100 | 664 | 10 000 | 1,3 × 10³⁰ |
| 1 000 | 1 | 10 | 1 000 | 9 966 | 1 000 000 | 10³⁰¹ |
| 10 000 | 1 | 13 | 10 000 | 132 877 | 10⁸ | — |
| 1 000 000 | 1 | 20 | 10⁶ | 2 × 10⁷ | 10¹² | — |

Traduzindo em tempo, supondo um PC que faz cerca de 10⁸ operações simples por segundo:

| n = 1 000 000 | Tempo aproximado |
| --- | --- |
| O(log n) | 0,2 µs |
| O(n) | 10 ms |
| O(n log n) | 0,2 s |
| O(n²) | 10 000 s ≈ 2,8 horas |
| O(2ⁿ) com n = 60 | ≈ 365 anos |

Mesmo com `n = 16` a diferença já aparece (cada `#` vale 16 operações):

```text
 O(1)        1      #
 O(log n)    4      #
 O(n)        16     #
 O(n log n)  64     ####
 O(n²)       120    ########            (pares i < j: n(n-1)/2)
 O(2ⁿ)       65536  ############################################################... (4096 #)
```

## 6. Exemplos de código por classe

```c
/* O(1): acesso direto pelo índice, não depende de n. */
float leitura_atual = leituras[k];

/* O(log n): a cada passo a faixa de busca cai pela metade
   (busca binária em vetor ordenado, Unidade 4). */
while (inicio <= fim) {
    int meio = inicio + (fim - inicio) / 2;
    if (v[meio] == alvo) break;
    if (v[meio] < alvo) inicio = meio + 1; else fim = meio - 1;
}

/* O(n): visita cada leitura uma vez (máximo, mínimo, média). */
for (int i = 0; i < n; i++)
    if (v[i] > maior) maior = v[i];

/* O(n log n): ordenações eficientes (merge sort, quicksort no caso médio).
   Divide em log n níveis e faz O(n) de trabalho em cada nível. */

/* O(n²): compara cada par de elementos (procura leituras repetidas). */
for (int i = 0; i < n; i++)
    for (int j = i + 1; j < n; j++)
        if (v[i] == v[j]) repetidos++;

/* O(n·m): percorre uma matriz de n linhas e m colunas (imagem, mapa de calor). */
for (int lin = 0; lin < n; lin++)
    for (int col = 0; col < m; col++)
        soma += mapa[lin][col];

/* O(2ⁿ): testa todas as combinações liga/desliga de n válvulas. */
for (unsigned long estado = 0; estado < (1UL << n); estado++)
    avalia(estado);
```

## 7. Exemplo real: o ciclo de varredura do CLP

Um CLP executa em laço: lê as entradas, executa o programa, escreve as saídas. Suponha um ciclo
configurado em **10 ms** em uma CPU que faz cerca de 10⁷ operações simples por segundo, ou seja,
um orçamento de **100 000 operações por ciclo**. O programa precisa verificar se há tags
duplicadas em uma lista de `n = 1 000` tags de alarme:

| Estratégia | Operações | Cabe em 10 ms? |
| --- | ---: | --- |
| Comparar cada par de tags, O(n²) | ≈ 500 000 | Não: ≈ 50 ms, o watchdog dispara |
| Ordenar e comparar vizinhos, O(n log n) | ≈ 10 000 | Sim |
| Lista mantida ordenada a cada inclusão, O(n) por verificação | ≈ 1 000 | Sim, com folga |

O código "certo" na bancada, com 20 tags, passa nos testes. Com 1 000 tags no campo, a máquina para.
Esse é o motivo de estudar estruturas de dados e complexidade em um curso de Controle e Automação.

Outros exemplos do dia a dia:

- Procurar um nome na lista de contatos do celular: a lista é mantida ordenada, então a busca é
  O(log n) e não O(n).
- Navegador: voltar para a página anterior é O(1) porque o histórico é uma pilha.
- GPS: calcular rotas em um grafo de milhões de cruzamentos só é viável com algoritmos
  O((V + E) log V).

## 8. Como estimar a complexidade de um trecho de código

1. Defina o que é `n` (número de leituras, de tags, de linhas da matriz, de caracteres...).
2. Escolha a operação básica (a comparação dentro do laço, o acesso à matriz).
3. Conte quantas vezes cada laço roda; multiplique os aninhados e some os sequenciais.
4. Não esqueça o custo das funções chamadas (`strlen`, `strcmp`, funções suas).
5. Descarte constantes e termos menores.
6. Confira empiricamente com um contador, como fazem os programas desta pasta.

## 9. Onde cada classe aparece na disciplina

| Estrutura / algoritmo | Operação | Complexidade | Módulo |
| --- | --- | --- | --- |
| Vetor | acesso por índice | O(1) | [01](README.md) |
| Vetor | busca sequencial | O(n) | [08](../08_ordenacao_busca/) |
| Vetor ordenado | busca binária | O(log n) | [08](../08_ordenacao_busca/) |
| Pilha | empilhar / desempilhar | O(1) | [05](../05_pilhas/) |
| Fila circular | enfileirar / desenfileirar | O(1) | [06](../06_filas/) |
| Lista encadeada | inserir no início | O(1) | [07](../07_listas/) |
| Lista encadeada | acessar o k-ésimo elemento | O(n) | [07](../07_listas/) |
| Ordenação simples (bolha, inserção) | ordenar | O(n²) | [08](../08_ordenacao_busca/) |
| Merge sort / quicksort médio | ordenar | O(n log n) | [08](../08_ordenacao_busca/) |
| Árvore AVL | buscar / inserir | O(log n) | [09](../09_arvores/) |
| BFS / DFS em grafo | percorrer | O(V + E) | [10](../10_grafos/) |

## 10. Armadilhas comuns

- **Confundir O(n) com "rápido"**: O(n) com `n = 10⁹` em um microcontrolador não é rápido.
  Big O compara crescimento, não tempo absoluto.
- **Esquecer o custo escondido**: `strlen`, `strcmp`, `memcpy` e funções suas também custam.
- **Medir só com `n` pequeno**: com 10 elementos quase tudo parece instantâneo. Teste com 10, 100,
  1 000 e 10 000 e veja se o tempo cresce 10×, 100× ou "explode".
- **Ignorar o pior caso**: em tempo real, o pior caso é o caso de projeto.
- **Ignorar o espaço**: recursão profunda estoura a pilha antes de ficar lenta.
