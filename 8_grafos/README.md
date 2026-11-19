# Grafos em C

Um **grafo** `G = (V, E)` é um conjunto de **vértices** `V` (os objetos) e um conjunto de **arestas** `E` (as relações entre pares de objetos). É a estrutura mais geral vista na disciplina: listas, pilhas, filas e árvores podem ser vistas como grafos com restrições.

## Conceitos fundamentais

```text
   0 --- 1 --- 4
   |     |
   2 --- 3     5 --- 6
```

| Termo | Definição | No exemplo |
| --- | --- | --- |
| **Vértice** (nó) | Elemento do grafo. | 0 a 6 |
| **Aresta** | Ligação entre dois vértices. | 0-1, 1-3, … |
| **Adjacentes** (vizinhos) | Vértices ligados por uma aresta. | 1 é vizinho de 0, 3 e 4 |
| **Grau** | Número de arestas incidentes no vértice. Em grafo dirigido, separa-se **grau de entrada** e **grau de saída**. | grau(1) = 3 |
| **Caminho** | Sequência de vértices em que cada par consecutivo é adjacente. | 2 - 0 - 1 - 4 |
| **Comprimento** | Número de arestas do caminho (ou soma dos pesos). | 3 |
| **Ciclo** | Caminho que começa e termina no mesmo vértice, sem repetir arestas. | 0 - 1 - 3 - 2 - 0 |
| **Conexo** | Grafo em que existe caminho entre qualquer par de vértices. | não é conexo |
| **Componente conexa** | Subconjunto máximo de vértices conectados entre si. | {0,1,2,3,4} e {5,6} |
| **Laço** | Aresta de um vértice para ele mesmo. | não há |
| **Grafo simples** | Sem laços e sem arestas repetidas entre o mesmo par. | sim |

### Variações

- **Não dirigido**: a aresta `u - v` vale nos dois sentidos (amizade).
- **Dirigido** (dígrafo): a aresta `u → v` tem sentido (seguir alguém, link entre páginas, dependência entre tarefas).
- **Ponderado**: cada aresta tem um peso (distância, custo, tempo de transmissão).
- **Acíclico**: sem ciclos. Um grafo dirigido acíclico (DAG) modela dependências e permite ordenação topológica.
- **Denso / esparso**: muitas ou poucas arestas em relação ao máximo `n(n-1)/2`.

### Representações

| | Matriz de adjacência | Lista de adjacência |
| --- | --- | --- |
| Estrutura | Matriz `n × n`; `m[u][v] = 1` se há aresta | Para cada vértice, lista encadeada de vizinhos |
| Memória | `O(n²)` | `O(n + m)` |
| Testar se `u-v` existe | `O(1)` | `O(grau(u))` |
| Percorrer vizinhos de `u` | `O(n)` | `O(grau(u))` |
| Indicada para | Grafos densos e pequenos | Grafos esparsos (a maioria dos reais) |

Os exemplos usam lista de adjacência, reaproveitando as listas encadeadas de [`6_listas`](../6_listas). Uma rede social tem milhões de pessoas e cada uma tem poucas centenas de amigos: a matriz seria quase toda de zeros.

### Percursos

- **Busca em largura (BFS)**: visita a origem, depois todos os vizinhos, depois os vizinhos dos vizinhos, usando uma **fila** ([`5_filas`](../5_filas)). Em grafo sem pesos, encontra o **caminho com menos arestas**.
- **Busca em profundidade (DFS)**: segue um caminho até não poder mais e então volta, usando recursão ou uma **pilha** ([`4_pilhas`](../4_pilhas)). Usada para detectar ciclos, encontrar componentes e fazer ordenação topológica.

Como um grafo pode ter ciclos, ambos precisam **marcar os vértices visitados**; senão o percurso entraria em laço infinito. Os dois custam `O(n + m)` com lista de adjacência.

## Diferença principal entre grafo e árvore

Toda **árvore é um grafo**: um grafo **conexo e acíclico**. Escolhendo um vértice como raiz, as arestas passam a ter a orientação pai → filho. O grafo é o caso geral, sem essas restrições.

| Aspecto | Árvore | Grafo |
| --- | --- | --- |
| Ciclos | Proibidos | Permitidos |
| Caminho entre dois nós | Exatamente um | Zero, um ou vários |
| Antecessores | Cada nó tem um único pai (exceto a raiz) | Um vértice pode ter qualquer número de vizinhos "de entrada" |
| Raiz | Existe uma raiz | Não há vértice especial |
| Conexidade | Sempre conexa | Pode ter partes isoladas |
| Número de arestas | Exatamente `n - 1` | De `0` até `n(n-1)/2` (não dirigido simples) |
| Relação modelada | Hierarquia (contém, é chefe de, é pai de) | Rede (é amigo de, liga-se a, depende de) |
| Percurso | Recursão simples; não precisa marcar visitados | Precisa marcar visitados por causa dos ciclos |

Em resumo: a árvore impõe **hierarquia** (um único pai, um único caminho); o grafo representa **relações arbitrárias**.

A árvore genealógica de [`7_arvores/04_t06_genealogia.c`](../7_arvores/04_t06_genealogia.c) mostra o limite da árvore: cada pessoa tem dois progenitores, e primos que tenham filhos juntos fazem dois caminhos chegarem ao mesmo ancestral. Na prática, a "árvore" genealógica é um grafo dirigido acíclico, e por isso aquele programa marca os ascendentes já impressos.

## O que o grafo nos traz de benefícios

Abandonar as restrições da árvore permite:

1. **Modelar relações muitos-para-muitos.** Uma pessoa tem muitos amigos e cada amigo tem muitos outros; uma cidade liga-se a várias cidades. Em uma árvore, cada nó só teria um "pai".
2. **Representar ciclos e caminhos alternativos.** Redes de computadores têm rotas redundantes justamente para tolerar falhas: se um enlace cai, outro caminho é usado. Uma árvore não tem alternativa: remover uma aresta a divide em duas.
3. **Responder perguntas sobre conectividade e distância.** Com BFS, DFS e algoritmos como Dijkstra: qual o menor caminho entre dois pontos, se dois elementos estão conectados, quais grupos estão isolados, qual nó é mais central.
4. **Representar dependências.** Um DAG mostra em que ordem executar tarefas, compilar módulos ou cursar disciplinas com pré-requisitos.
5. **Uma estrutura, muitos domínios.** O mesmo código de BFS calcula graus de separação em rede social, rotas em mapas, alcance em redes de automação industrial ou propagação de falhas em um circuito.

O custo da generalidade: os algoritmos precisam tratar ciclos (marcar visitados), não há uma raiz natural para começar e algumas buscas que em árvores balanceadas custam `O(log n)` passam a custar `O(n + m)`. Quando os dados são de fato hierárquicos, a árvore continua sendo a escolha mais simples.

## Exemplos

### [`01_grafo.c`](01_grafo.c): grafo simples

Grafo com lista de adjacência e vértices `0..n-1`, dirigido ou não dirigido. Implementa inserção (rejeita laços e arestas repetidas), remoção e consulta de arestas, grau, BFS, DFS, caminho mínimo em número de arestas (BFS com vetor de pais) e detecção de ciclo em grafo não dirigido. O `main` monta o grafo do diagrama acima, remove uma aresta para quebrar o ciclo e mostra que em um grafo dirigido `0 → 2` existe, mas `2 → 0` não.

### [`02_rede_social.c`](02_rede_social.c): rede social

```text
   Ana ---- Bruno ---- Diego ---- Fabio
    |  \      |          |
    |   \     |          |
  Carla --- Eva        Gabi

   Hugo ---- Iris
```

Pessoas são vértices e amizades são arestas não dirigidas. Cada pergunta da rede corresponde a uma operação no grafo:

| Pergunta | Operação no grafo |
| --- | --- |
| Quem são os amigos de X? | Vizinhos de X |
| Quantos amigos X tem? / Quem é mais popular? | Grau / maior grau |
| Amigos em comum entre X e Y | Interseção das vizinhanças |
| "Pessoas que você talvez conheça" | Vértices a distância 2, ordenados por vizinhos em comum |
| Grau de separação entre X e Y | Caminho mínimo por BFS |
| Grupos que não se conhecem | Componentes conexas |

## Compilar e executar

A partir da raiz do repositório:

```bash
gcc -std=c11 -Wall -Wextra -pedantic 8_grafos/01_grafo.c -o /tmp/01_grafo
/tmp/01_grafo

gcc -std=c11 -Wall -Wextra -pedantic 8_grafos/02_rede_social.c -o /tmp/02_rede_social
/tmp/02_rede_social
```

## Tarefa Avaliada 06: mapa rodoviário

[`03_t06_mapa_rodoviario.c`](03_t06_mapa_rodoviario.c) modela o mapa da região central do RS (Santa Maria, Silveira Martins, Ivorá, Faxinal do Soturno, Nova Palma, Dona Francisca, Agudo, São João do Polêsine, Recanto Maestro, São Miguel, Mosquito, Restinga Seca) como **grafo não dirigido ponderado**, com matriz de adjacência e distâncias em km como pesos. O trevo sem nome entre Santa Maria e Recanto Maestro, onde sai a estrada para Silveira Martins, vira o vértice `Trevo SM`.

A menor rota é calculada com **Dijkstra**: a cada passo, fixa o vértice ainda não fixado com menor distância e relaxa suas arestas. Com matriz de adjacência, o custo é `O(n²)`. A BFS não serve aqui, porque minimiza o número de trechos e não os km: Santa Maria → Agudo por Mosquito (68 km) tem mais trechos que outras rotas, mas é a mais curta.

```bash
gcc -std=c11 -Wall -Wextra -pedantic 8_grafos/03_t06_mapa_rodoviario.c -o /tmp/mapa
/tmp/mapa                                  # Santa Maria -> todas as cidades
/tmp/mapa "Restinga Seca" "Nova Palma"     # rota entre duas cidades (60 km)
```

## Aplicação em Engenharia de Controle e Automação: [`04_automacao_agv_profinet.c`](04_automacao_agv_profinet.c)

Modela a topologia de chão de fábrica como **grafo não dirigido ponderado** para resolver dois problemas de redundância em automação industrial:

- **Rede PROFINET em Anel (BFS)**: encontra a rota com menor número de saltos entre CLPs e redireciona automaticamente pelo outro lado do anel quando o cabo direto se rompe.
- **Navegação de Robô Móvel AGV (Dijkstra)**: calcula a trajetória de menor tempo (em segundos) entre a Doca e a Célula de Montagem e recalcula o desvio ótimo pelo Pátio Sul quando o Corredor Norte é interditado.

```bash
gcc -std=c11 -Wall -Wextra -pedantic 8_grafos/04_automacao_agv_profinet.c -o /tmp/04_automacao_agv_profinet
/tmp/04_automacao_agv_profinet
```
