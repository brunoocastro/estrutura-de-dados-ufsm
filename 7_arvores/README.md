# Árvores em C

Pilhas, filas e listas organizam elementos em **sequência**: cada elemento tem no máximo um sucessor. Uma **árvore** organiza elementos em **hierarquia**: cada elemento pode ter vários sucessores (filhos), mas apenas um antecessor (pai). Exemplos do dia a dia: diretórios de um sistema de arquivos, organograma de uma empresa, sumário de um livro.

## Conceitos fundamentais

```text
                 A            <- raiz (nível 0)
              /  |  \
             B   C   D        <- nível 1
            / \      |
           E   F     G        <- nível 2
                     |
                     H        <- nível 3 (folha)
```

| Termo | Definição | No exemplo |
| --- | --- | --- |
| **Nó** | Elemento da árvore; guarda um valor e as ligações para os filhos. | A, B, …, H |
| **Aresta** | Ligação entre um pai e um filho. Uma árvore com `n` nós tem `n - 1` arestas. | A–B, B–E, … |
| **Raiz** | Único nó sem pai; ponto de entrada da árvore. | A |
| **Pai / filho** | Se existe aresta de X para Y, X é pai de Y e Y é filho de X. | B é pai de E e F |
| **Irmãos** | Nós com o mesmo pai. | B, C e D |
| **Folha** (nó externo) | Nó sem filhos. | E, F, C, H |
| **Nó interno** | Nó com pelo menos um filho. | A, B, D, G |
| **Ancestral / descendente** | Qualquer nó no caminho da raiz até X é ancestral de X; X é descendente dele. | A e D são ancestrais de H |
| **Subárvore** | Um nó junto com todos os seus descendentes. Toda subárvore também é uma árvore. | D, G, H |
| **Grau de um nó** | Número de filhos do nó. | grau(A) = 3, grau(B) = 2 |
| **Grau da árvore** | Maior grau entre todos os nós. | 3 |
| **Caminho** | Sequência de nós ligados por arestas. | A → D → G → H |
| **Nível / profundidade** | Número de arestas da raiz até o nó. A raiz está no nível 0. | nível(G) = 2 |
| **Altura de um nó** | Número de arestas no caminho mais longo desse nó até uma folha. | altura(D) = 2 |
| **Altura da árvore** | Altura da raiz. Por convenção, árvore vazia tem altura -1 e árvore com um só nó tem altura 0. | 3 |

Propriedades importantes:

- Existe **um único caminho** entre a raiz e qualquer nó; por isso não há ciclos.
- A árvore é uma estrutura **recursiva**: ela é vazia, ou é uma raiz cujos filhos são raízes de subárvores. Por isso quase todas as operações (contar, buscar, medir altura, liberar) são escritas naturalmente com recursão. Ver [`2_revisao_funcoes_recursao`](../2_revisao_funcoes_recursao).

### Percursos

Percorrer uma árvore significa visitar todos os nós uma vez. Como não há uma ordem linear única, definem-se ordens:

- **Pré-ordem**: visita o nó, depois os filhos. Útil para copiar ou imprimir a hierarquia.
- **Pós-ordem**: visita os filhos, depois o nó. Útil para liberar memória (os filhos precisam ser liberados antes do pai) e calcular valores que dependem das subárvores, como a altura.
- **Em ordem** (só em árvores binárias): esquerda, nó, direita. Em uma árvore binária de busca, produz os valores em ordem crescente.
- **Em nível** (largura): visita nível por nível, da esquerda para a direita; implementa-se com uma fila ([`5_filas`](../5_filas)).

## Árvore genérica (n-ária)

Em uma árvore genérica, cada nó pode ter **qualquer número** de filhos. Guardar um array de ponteiros por nó exigiria escolher um máximo ou redimensionar. O exemplo [`01_arvore.c`](01_arvore.c) usa a representação **primeiro filho / próximo irmão**:

```c
typedef struct No {
    int valor;
    struct No *primeiro_filho;
    struct No *proximo_irmao;
} No;
```

Os filhos de um nó formam uma lista encadeada ([`6_listas`](../6_listas)) que começa em `primeiro_filho` e segue por `proximo_irmao`. Assim, todo nó tem exatamente dois ponteiros, não importa quantos filhos tenha:

```text
A
|
B ---> C ---> D        (| = primeiro_filho, ---> = proximo_irmao)
|             |
E ---> F      G
              |
              H
```

O programa monta a árvore do diagrama (com números 1 a 8 no lugar das letras), imprime em pré-ordem com indentação, conta nós e folhas, calcula altura e grau e busca valores. A busca é `O(n)`: sem regra de ordenação, pode ser preciso visitar todos os nós.

## Árvore binária

Uma **árvore binária** é uma árvore em que cada nó tem **no máximo dois filhos**, identificados como **esquerdo** e **direito**:

```c
typedef struct No {
    int valor;
    struct No *esquerda;
    struct No *direita;
} No;
```

O exemplo [`02_arvore_binaria.c`](02_arvore_binaria.c) implementa uma **árvore binária de busca** (ABB), uma árvore binária com uma regra extra: para todo nó, os valores na subárvore esquerda são menores que ele e os da subárvore direita são maiores.

```text
            50
          /    \
        30      70
       /  \    /  \
      20  40  60  80
```

A regra permite buscar descartando uma subárvore a cada passo, como na busca binária em um vetor ordenado. O programa insere, busca, remove (folha, nó com um filho e nó com dois filhos, usando o sucessor) e imprime os três percursos em profundidade.

| Operação na ABB | Árvore balanceada | Pior caso (degenerada) |
| --- | --- | --- |
| Buscar | `O(log n)` | `O(n)` |
| Inserir | `O(log n)` | `O(n)` |
| Remover | `O(log n)` | `O(n)` |

O custo é proporcional à altura. Inserir valores já ordenados (10, 20, 30, …) gera uma árvore degenerada, em que cada nó só tem filho à direita, que se comporta como uma lista. Árvores autobalanceadas (AVL, rubro-negra) fazem rotações para manter a altura em `O(log n)`.

Tipos de árvore binária:

- **Estritamente binária** (ou cheia): todo nó tem 0 ou 2 filhos.
- **Completa**: todos os níveis estão cheios, exceto possivelmente o último, que é preenchido da esquerda para a direita. É a forma usada pelo *heap*, que pode ser guardado em um vetor.
- **Perfeita**: todos os nós internos têm 2 filhos e todas as folhas estão no mesmo nível. Com altura `h`, tem `2^(h+1) - 1` nós.

## Diferença entre árvore e árvore binária

| Aspecto | Árvore (genérica) | Árvore binária |
| --- | --- | --- |
| Número de filhos | Qualquer (0, 1, 2, …, n) | No máximo 2 |
| Posição dos filhos | Os filhos formam uma sequência; não há "lado". | Cada filho é **esquerdo** ou **direito**, e a posição tem significado. |
| Árvore vazia | Em muitas definições, uma árvore tem ao menos a raiz. | A árvore binária vazia é válida, e cada filho pode ser vazio independentemente. |
| Representação típica | Lista de filhos, ou primeiro filho / próximo irmão | Dois ponteiros fixos: `esquerda` e `direita` |
| Percursos | Pré-ordem, pós-ordem, em nível | Os mesmos, mais **em ordem** (esquerda, nó, direita) |
| Uso típico | Hierarquias naturais: diretórios, menus, árvore DOM, organogramas | Busca ordenada (ABB, AVL), heaps, árvores de expressão, codificação de Huffman |

O ponto mais sutil é a **posição**. Em uma árvore binária, um nó com um único filho à esquerda é **diferente** de um nó com um único filho à direita:

```text
   A          A
  /            \
 B              B
```

Como árvores genéricas, as duas são iguais (A com um filho B). Como árvores binárias, são distintas. Por isso uma árvore binária **não** é apenas "uma árvore de grau 2": é uma estrutura em que cada nó tem duas posições, e cada uma pode estar ocupada ou vazia.

Qualquer árvore genérica pode ser convertida em binária: a representação primeiro filho / próximo irmão de `01_arvore.c` já é uma árvore binária, em que `primeiro_filho` faz o papel de `esquerda` e `proximo_irmao` faz o papel de `direita`.

## Compilar e executar

A partir da raiz do repositório:

```bash
gcc -std=c11 -Wall -Wextra -pedantic 7_arvores/01_arvore.c -o /tmp/01_arvore
/tmp/01_arvore

gcc -std=c11 -Wall -Wextra -pedantic 7_arvores/02_arvore_binaria.c -o /tmp/02_arvore_binaria
/tmp/02_arvore_binaria
```

## Tarefa Avaliada 06

### Questão 1: [`03_t06_delete.c`](03_t06_delete.c)

`int delete(No **raiz, int valor)` remove o **ramo** cuja raiz contém `valor`: o nó e todos os seus descendentes são liberados em pós-ordem, e o ponteiro do pai passa a ser `NULL`. A função recebe o endereço do ponteiro (`No **`) para conseguir anular o campo `esquerda`/`direita` do pai, ou a própria raiz da árvore quando o valor está nela. A busca percorre a árvore inteira, então funciona para qualquer árvore binária, não só para ABB. Retorna 1 se removeu e 0 se o valor não existe. O `main` remove uma subárvore interna, uma folha, um valor inexistente e a raiz.

### Questão 2: [`04_t06_genealogia.c`](04_t06_genealogia.c)

Cada `Pessoa` tem uma **lista encadeada de filhos** de tamanho arbitrário e uma lista de progenitores. Não há campo de gênero: "progenitor" vale para pai ou mãe e "filho" vale para filho ou filha.

- **Entrada de dados**: uma relação `progenitor;filho` por linha, até uma linha vazia ou `fim`. Pessoas são criadas na primeira vez em que aparecem. Relações repetidas, autorrelação e relações que criariam ciclo (alguém ser ascendente de si mesmo) são rejeitadas.
- **Consulta**: lista os ascendentes de **cada** pessoa cadastrada (progenitores, avós, bisavós…) subindo recursivamente pelos progenitores, e depois aceita nomes para consulta individual.

Como cada pessoa pode ter dois progenitores, um mesmo ancestral pode ser alcançado por dois caminhos; uma marca por consulta evita imprimi-lo duas vezes. Por isso a "árvore" genealógica é, a rigor, um grafo dirigido acíclico ([`8_grafos`](../8_grafos/README.md)).

```bash
gcc -std=c11 -Wall -Wextra -pedantic 7_arvores/03_t06_delete.c -o /tmp/03_t06_delete
/tmp/03_t06_delete

gcc -std=c11 -Wall -Wextra -pedantic 7_arvores/04_t06_genealogia.c -o /tmp/04_t06_genealogia
printf 'Ana;Bia\nCarlos;Bia\nBia;Duda\nEdu;Duda\nDuda;Fabi\n\nFabi\n\n' | /tmp/04_t06_genealogia
```

A terceira parte da tarefa (mapa rodoviário) é um problema de grafos e está em [`8_grafos/03_t06_mapa_rodoviario.c`](../8_grafos/03_t06_mapa_rodoviario.c).

## Aplicação em Engenharia de Controle e Automação: [`05_automacao_scada.c`](05_automacao_scada.c)

Modela a hierarquia de ativos de uma planta segundo a norma ISA-95 / endereçamento OPC-UA (`Planta -> Área -> CLP -> Sensores/Atuadores`) usando árvore genérica (`primeiro_filho` / `proximo_irmao`):

- **Pré-ordem**: exibe a árvore de ativos do supervisório SCADA com o estado (`ON`/`OFF`) e consumo.
- **Pós-ordem**: calcula a potência elétrica total (W) somando os equipamentos filhos antes da área/CLP pai e executa o desligamento seguro em cascata quando o disjuntor da `Area_Envase` desarma.

```bash
gcc -std=c11 -Wall -Wextra -pedantic 7_arvores/05_automacao_scada.c -o /tmp/05_automacao_scada
/tmp/05_automacao_scada
```
