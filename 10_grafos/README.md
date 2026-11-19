# Grafos em C (Unidade 6)

Um **grafo** `G = (V, E)` é um conjunto de **vértices** `V` (os objetos) e um conjunto de **arestas** `E` (as relações entre pares de objetos). É a estrutura mais geral vista na disciplina: listas, pilhas, filas e árvores podem ser vistas como grafos com restrições.

## O que a ementa pede e onde está

| Item da ementa | Onde estudar |
| --- | --- |
| **6.1 Conceitos e definições**: vértice, aresta, grau, caminho, ciclo, conexidade, dirigido/ponderado, DAG | Seção [Conceitos](#conceitos-fundamentais) abaixo e [`01_grafo.c`](01_grafo.c) |
| **6.2 Representação física**: matriz de adjacência, lista de adjacência, lista de arestas | [`02_matriz_vs_lista_adjacencia.c`](02_matriz_vs_lista_adjacencia.c) (passo a passo e medições) e [`03_formas_grafo.c`](03_formas_grafo.c) |
| **6.3 Implementação**: inserir/remover, percursos (BFS, DFS), ciclos, componentes, ordenação topológica, caminho mínimo, árvore geradora mínima | [`01_grafo.c`](01_grafo.c) e de [`04`](04_bfs_saltos_rede.c) a [`10`](10_mst_cabeamento_sensores.c) |

A teoria de Big O usada nas tabelas está em [`../01_revisao_c_basico/complexidade_big_o.md`](../01_revisao_c_basico/complexidade_big_o.md).

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
| **Árvore geradora** | Subgrafo conexo e sem ciclos que contém todos os vértices (`V - 1` arestas). | só existe se o grafo for conexo |

### Variações

- **Não dirigido**: a aresta `u - v` vale nos dois sentidos (cabo de rede, amizade).
- **Dirigido** (dígrafo): a aresta `u → v` tem sentido (intertravamento "u libera v", link entre páginas, dependência entre tarefas).
- **Ponderado**: cada aresta tem um peso (distância, latência, metros de cabo, tempo de deslocamento).
- **Acíclico**: sem ciclos. Um grafo dirigido acíclico (**DAG**) modela dependências e admite ordenação topológica.
- **Denso / esparso**: muitas ou poucas arestas em relação ao máximo `V(V-1)/2`. Redes industriais, mapas e redes sociais são esparsos.

### Representação física (6.2)

O mesmo grafo `CLP—SW, IHM—SW, SCADA—SW, INV—SW, RIO—SW, CLP—RIO` nas três formas:

```text
 Matriz de adjacência (V x V)          Lista de adjacência (V cabeças + 2E nós)
          CLP IHM  SW INV RIO SCADA       CLP   -> [RIO|*]-->[SW|*]-->NULL
   CLP     .   .   1   .   1   .          IHM   -> [SW|*]-->NULL
   IHM     .   .   1   .   .   .          SW    -> [RIO|*]-->[INV|*]-->[SCADA|*]-->[IHM|*]-->[CLP|*]-->NULL
   SW      1   1   .   1   1   1          INV   -> [SW|*]-->NULL
   INV     .   .   1   .   .   .          RIO   -> [CLP|*]-->[SW|*]-->NULL
   RIO     1   .   1   .   .   .          SCADA -> [SW|*]-->NULL
   SCADA   .   .   1   .   .   .
                                       Lista de arestas (E registros)
                                          (CLP,SW) (IHM,SW) (SCADA,SW) (INV,SW) (RIO,SW) (CLP,RIO)
```

| Operação | Matriz de adjacência | Lista de adjacência | Lista de arestas |
| --- | --- | --- | --- |
| Memória | `O(V²)` | `O(V + E)` | `O(E)` |
| Inserir aresta | `O(1)` | `O(1)` (no início da lista) | `O(1)` (no fim do vetor) |
| Existe a aresta `u-v`? | `O(1)` | `O(grau(u))` | `O(E)` |
| Vizinhos de `u` | `O(V)` | `O(grau(u))` | `O(E)` |
| Remover aresta `u-v` | `O(1)` | `O(grau(u) + grau(v))` | `O(E)` |
| Remover vértice | `O(V)` (zera linha e coluna) | `O(V + E)` (varre todas as listas) | `O(E)` |
| Indicada para | grafos pequenos ou densos; muitas consultas "existe?" | grafos esparsos; percursos (BFS, DFS, Dijkstra) | algoritmos que ordenam arestas (Kruskal) |

Esparso × denso, com as contas do [`02`](02_matriz_vs_lista_adjacencia.c) (matriz de 1 byte por célula; nó de lista com 16 bytes):

| V | Esparso (E = 2V): matriz | Esparso: lista | Denso (E = V²/4): matriz | Denso: lista |
| --- | --- | --- | --- | --- |
| 10 | 100 B | 720 B | 100 B | 800 B |
| 1 000 | 977 KB | 70 KB | 977 KB | 7,6 MB |
| 10 000 | 95 MB | 703 KB | 95 MB | 763 MB |

### Percursos

- **Busca em largura (BFS)**: visita a origem, depois todos os vizinhos, depois os vizinhos dos vizinhos, usando uma **fila** ([`../06_filas/`](../06_filas/)). Em grafo sem pesos, encontra o **caminho com menos arestas**.
- **Busca em profundidade (DFS)**: segue um caminho até não poder mais e então volta, usando recursão ou uma **pilha** ([`../05_pilhas/`](../05_pilhas/)). Usada para detectar ciclos, encontrar componentes e fazer ordenação topológica.

Como um grafo pode ter ciclos, os dois precisam **marcar os vértices visitados**; senão o percurso entraria em laço infinito. Com lista de adjacência os dois custam `O(V + E)`. As listas encadeadas usadas aqui são as de [`../07_listas/`](../07_listas/).

```text
 BFS a partir de SCADA (camadas)          DFS a partir de A (mergulha e volta)
   nível 0: SCADA                           A -> B -> D -> G   (beco: volta a D, a B)
   nível 1: SW-CORE                              B -> E -> F -> C   (C vê A: ciclo!)
   nível 2: SW-A SW-B SW-C                                F -> H
   fila: frente -> [ SW-B | SW-C | CLP-A ] <- fim     pilha: | C | H | C | <- topo
```

## Diferença principal entre grafo e árvore

Toda **árvore é um grafo**: um grafo **conexo e acíclico**. Escolhendo um vértice como raiz, as arestas passam a ter a orientação pai → filho. O grafo é o caso geral, sem essas restrições (árvores em [`../09_arvores/`](../09_arvores/)).

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

A árvore genealógica, estudada em [`../09_arvores/`](../09_arvores/), mostra o limite da árvore: cada pessoa tem dois progenitores, e primos que tenham filhos juntos fazem dois caminhos chegarem ao mesmo ancestral. Na prática, a "árvore" genealógica é um grafo dirigido acíclico: ao percorrer os ascendentes de alguém, o mesmo ancestral pode ser alcançado por mais de um caminho.

## O que o grafo nos traz de benefícios

Abandonar as restrições da árvore permite:

1. **Modelar relações muitos-para-muitos.** Um CLP conversa com vários dispositivos e cada dispositivo com vários outros; uma cidade liga-se a várias cidades. Numa árvore, cada nó só teria um "pai".
2. **Representar ciclos e caminhos alternativos.** Redes industriais têm rotas redundantes (anel PROFINET/MRP) justamente para tolerar falhas: se um enlace cai, outro caminho é usado. Uma árvore não tem alternativa: remover uma aresta a divide em duas.
3. **Responder perguntas sobre conectividade e distância.** Com BFS, DFS, Dijkstra e MST: qual o menor caminho entre dois pontos, se dois elementos estão conectados, quais grupos estão isolados, qual o cabeamento mínimo.
4. **Representar dependências.** Um DAG mostra em que ordem partir máquinas, compilar módulos ou cursar disciplinas com pré-requisitos.
5. **Uma estrutura, muitos domínios.** O mesmo código de BFS calcula graus de separação numa rede social, saltos numa rede industrial ou a propagação de uma falha num circuito.

O custo da generalidade: os algoritmos precisam tratar ciclos (marcar visitados), não há uma raiz natural para começar e algumas buscas que em árvores balanceadas custam `O(log n)` passam a custar `O(V + E)`. Quando os dados são de fato hierárquicos, a árvore continua sendo a escolha mais simples.

## Ordem de estudo recomendada

| Arquivo | O que mostra | Complexidade-chave |
| --- | --- | --- |
| [`01_grafo.c`](01_grafo.c) | TAD grafo com lista de adjacência, dirigido ou não: inserir (rejeita laço e repetida), remover, consultar, grau, BFS, DFS, caminho com menos arestas, ciclo em grafo não dirigido | buscas `O(V + E)` |
| [`02_matriz_vs_lista_adjacencia.c`](02_matriz_vs_lista_adjacencia.c) | **Passo a passo**: o mesmo grafo montado aresta por aresta na matriz e na lista; "existe aresta?" e "vizinhos de v" narrados; memória com `sizeof`; tabelas esparso × denso (fórmula e contagem real) | `O(V²)` × `O(V + E)` |
| [`03_formas_grafo.c`](03_formas_grafo.c) | Três formas de armazenar (matriz, lista, lista de arestas), remoção de vértice inteiro, DFS com pilha, ordenação de arestas por peso, Kahn compacto | remover vértice `O(V + E)` |
| [`04_bfs_saltos_rede.c`](04_bfs_saltos_rede.c) | **Passo a passo**: BFS numa rede industrial com fila, visitados, saltos e pai desenhados; camadas; rota com menos switches do SCADA até cada equipamento; tabela de crescimento | `O(V + E)` (lista) × `O(V²)` (matriz) |
| [`05_dfs_ciclos.c`](05_dfs_ciclos.c) | **Passo a passo**: DFS recursiva (pilha de chamadas, descoberta/término) e com pilha explícita; ciclo em grafo não dirigido e em dirigido (branco/cinza/preto) num esquema de intertravamentos; componentes conexos; profundidade da recursão | `O(V + E)`, pilha `O(V)` |
| [`06_rede_social.c`](06_rede_social.c) | Aplicação: amigos, grau, amigos em comum, "pessoas que você talvez conheça", grau de separação (BFS), grupos isolados | BFS `O(V + E)` |
| [`07_topologica_partida_maquinas.c`](07_topologica_partida_maquinas.c) | **Passo a passo**: Kahn na sequência de partida de uma linha (QGBT, compressor, prensa, esteiras…); etapas paralelas; intertravamento circular detectado; Kahn × varredura ingênua | `O(V + E)` × `O(V·(V + E))` |
| [`08_dijkstra_latencia_rede.c`](08_dijkstra_latencia_rede.c) | **Passo a passo**: Dijkstra na latência de uma rede industrial, com tabela dist/pred e vértice fixado a cada iteração; BFS (saltos) × Dijkstra (µs); versão com heap; vetor × heap em grafos esparsos, densos e no pior caso | `O(V²)` × `O((V + E) log V)` |
| [`09_automacao_agv_profinet.c`](09_automacao_agv_profinet.c) | Aplicação integrada com matriz: anel PROFINET (BFS acha a rota redundante quando um cabo rompe) e AGV (Dijkstra recalcula a rota com um corredor interditado) | `O(V²)` |
| [`10_mst_cabeamento_sensores.c`](10_mst_cabeamento_sensores.c) | **Passo a passo**: árvore geradora mínima para o cabeamento de sensores: Kruskal com Union-Find (componentes desenhadas a cada aresta) e Prim com tabela; floresta numa rede desconexa | `O(E log E)` × `O(V²)` |

`04` e `08` usam uma rede industrial parecida de propósito: a rota com menos saltos (BFS) não é a de menor latência (Dijkstra). `09` mantém a versão compacta dos mesmos algoritmos sobre matriz, sem narração.

## Complexidade dos algoritmos em cada representação

`V` = vértices, `E` = arestas. "Matriz" e "lista" referem-se a como os vizinhos são obtidos.

| Algoritmo | Matriz de adjacência | Lista de adjacência | Espaço extra | Arquivos |
| --- | --- | --- | --- | --- |
| BFS | `O(V²)` | `O(V + E)` | `O(V)` (fila, visitados) | 01, 04, 06, 09 |
| DFS (recursiva ou com pilha) | `O(V²)` | `O(V + E)` | `O(V)` (pilha) | 01, 03, 05 |
| Ciclo (não dirigido: "visitado e não é o pai"; dirigido: cores) | `O(V²)` | `O(V + E)` | `O(V)` | 01, 05 |
| Componentes conexos (DFS/BFS) | `O(V²)` | `O(V + E)` | `O(V)` | 05, 06 |
| Ordenação topológica (Kahn) | `O(V²)` | `O(V + E)` | `O(V)` (graus, fila) | 03, 07 |
| Dijkstra com vetor | `O(V²)` | `O(V² + E) = O(V²)` | `O(V)` | 08, 09 |
| Dijkstra com heap binário | `O(V² + E log V)` | `O((V + E) log V)` | `O(V + E)` (heap) | 08 |
| Prim com vetor | `O(V²)` | `O(V²)` | `O(V)` | 10 |
| Prim com heap binário | `O(V² + E log V)` | `O(E log V)` | `O(V + E)` | (exercício) |
| Kruskal + Union-Find | `O(V² + E log E)` (extrair as arestas da matriz custa `V²`) | `O(E log E)` | `O(V + E)` | 10 |

Leitura rápida:

- Com **matriz**, achar os vizinhos de um vértice custa sempre `V`; todo algoritmo que visita os vizinhos de todos os vértices paga pelo menos `V²`.
- Com **lista**, a soma dos tamanhos das listas é `2E` (não dirigido) ou `E` (dirigido): daí o `O(V + E)`.
- **Dijkstra/Prim com vetor** são `O(V²)` em qualquer representação (a busca do mínimo domina) e vencem em grafos densos no pior caso; com **heap** vencem em grafos esparsos. O `08` mede os dois casos.
- **Dijkstra exige pesos não negativos.** Com pesos negativos use Bellman-Ford, `O(V·E)`.

## Exemplos reais

**Engenharia de Controle e Automação**

- Rede Ethernet industrial: saltos entre switches (BFS) e latência mínima (Dijkstra), como fazem os protocolos de roteamento OSPF/IS-IS ([`04`](04_bfs_saltos_rede.c), [`08`](08_dijkstra_latencia_rede.c)).
- Anel PROFINET com MRP: a redundância é um ciclo no grafo; quando um cabo rompe, a rota alternativa é outro caminho ([`09`](09_automacao_agv_profinet.c)).
- AGVs e robôs móveis: o mapa de corredores é um grafo ponderado; desvios são recalculados quando um trecho é bloqueado ([`09`](09_automacao_agv_profinet.c)).
- Intertravamentos e sequência de partida de máquinas: DAG + ordenação topológica; um ciclo de permissivos é um deadlock de partida ([`05`](05_dfs_ciclos.c), [`07`](07_topologica_partida_maquinas.c)).
- Cabeamento mínimo de sensores, redes elétricas e tubulações: árvore geradora mínima ([`10`](10_mst_cabeamento_sensores.c)).
- GRAFCET/SFC: etapas e transições formam um grafo dirigido; verificar alcançabilidade de etapas é uma busca.

**Cotidiano**

- Rotas no GPS (Dijkstra/A*), "pessoas que você talvez conheça" ([`06`](06_rede_social.c)), dependências de pacotes e do `make`, pré-requisitos de disciplinas.

## Como executar

A partir da raiz do repositório (o script compila para `build/`):

```bash
./run list grafos                          # lista os arquivos deste módulo
./run 01_grafo
./run 02_matriz_vs_lista_adjacencia        # ENTER avança cada passo
./run 04_bfs_saltos_rede
./run 05_dfs_ciclos
./run 07_topologica_partida_maquinas
./run 08_dijkstra_latencia_rede
./run 10_mst_cabeamento_sensores
./run 08_dijkstra_latencia_rede < /dev/null   # roda direto, sem pausas
```

Os arquivos passo a passo pausam a cada etapa quando a entrada é um terminal; com a entrada redirecionada, executam do início ao fim.

## Exercícios propostos

Para praticar (sem solução neste repositório). Os exercícios da lista L07 (não avaliativa) do Classroom seguem a mesma linha.

1. **Conversões.** Escreva funções que convertam matriz → lista de adjacência e lista → matriz. Qual o custo de cada conversão em função de `V` e `E`?
2. **Graus em dígrafo.** Para um grafo dirigido em lista de adjacência, calcule grau de entrada e de saída de todos os vértices em `O(V + E)` e liste as fontes (entrada 0) e os sumidouros (saída 0).
3. **Switch crítico.** Em [`04_bfs_saltos_rede.c`](04_bfs_saltos_rede.c), remova um switch por vez e informe quais equipamentos perdem a comunicação com o SCADA. Qual o custo total? Que switches são pontos únicos de falha?
4. **Bipartido.** Use BFS com duas cores para decidir se um grafo é bipartido (por exemplo, se dá para separar tarefas em dois turnos sem que tarefas conflitantes fiquem no mesmo turno).
5. **DFS sem recursão para dígrafos.** Reescreva a detecção de ciclo com cores de [`05_dfs_ciclos.c`](05_dfs_ciclos.c) usando pilha explícita e imprima o ciclo encontrado.
6. **Todas as ordens.** Em [`07_topologica_partida_maquinas.c`](07_topologica_partida_maquinas.c), troque a fila de Kahn por uma pilha e compare as ordens. Depois, liste por backtracking **todas** as ordens topológicas de um DAG pequeno.
7. **Pesos negativos.** Acrescente um enlace de peso negativo em [`08_dijkstra_latencia_rede.c`](08_dijkstra_latencia_rede.c) e mostre um caso em que Dijkstra erra. Implemente Bellman-Ford e compare.
8. **Prim com heap.** Implemente Prim com heap em [`10_mst_cabeamento_sensores.c`](10_mst_cabeamento_sensores.c) e compare as contagens com Kruskal e com Prim com vetor em grafos esparsos e densos gerados.
9. **Pontes.** Uma ponte é uma aresta cuja remoção desconecta o grafo. Encontre as pontes de uma rede por força bruta (remove a aresta e testa a conexidade). Qual o custo? Como isso se relaciona com a redundância de uma rede industrial?

## Tarefa Avaliada 06 — Questão 3: mapa rodoviário (enunciado e orientações)

**Enunciado.** Usando o mapa rodoviário da região central do RS disponibilizado no Classroom, que inclui Santa Maria, Silveira Martins, Ivorá, Faxinal do Soturno, Nova Palma, Dona Francisca, Agudo, São João do Polêsine, Recanto Maestro, São Miguel, Mosquito e Restinga Seca, modele a malha como um **grafo não dirigido ponderado**, com as distâncias em km como pesos, e implemente o **algoritmo de Dijkstra** para calcular a menor distância e a rota entre as cidades.

**Orientações** (sem solução):

- As distâncias de cada trecho devem ser lidas do mapa do Classroom. Entroncamentos sem nome, de onde saem estradas, podem ser modelados como vértices auxiliares.
- Escolha a representação (matriz ou lista de adjacência) e justifique pelo tamanho e pela densidade do grafo; veja [`02_matriz_vs_lista_adjacencia.c`](02_matriz_vs_lista_adjacencia.c).
- A BFS não resolve o problema: ela minimiza o número de trechos, não os km. A Parte 1 de [`08_dijkstra_latencia_rede.c`](08_dijkstra_latencia_rede.c) mostra a diferença em outro cenário.
- Guarde o predecessor de cada vértice para reconstruir a rota, não só a distância.
- Teste casos-limite: origem igual ao destino, nome de cidade inexistente e uma rota com mais trechos que seja mais curta que outra com menos trechos.
- Os exemplos deste módulo usam cenários diferentes (rede industrial, AGV) para estudo; a tarefa deve ser resolvida por você.

## Solução

A solução de referência da Questão 3 é [`t06/03_t06_mapa_rodoviario.c`](t06/03_t06_mapa_rodoviario.c). Tente resolver antes de consultar. Para executar: `./run t06/03_t06_mapa_rodoviario.c "Santa Maria" "Agudo"`.
