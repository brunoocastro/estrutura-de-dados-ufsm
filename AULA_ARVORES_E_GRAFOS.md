# Slides: Árvores, Ordenação, Buscas e Grafos

> **UFSM00064 — Estrutura de Dados** | Engenharia de Controle e Automação — UFSM  
> **Versão em PDF ilustrada:** [`AULA_ARVORES_E_GRAFOS.pdf`](AULA_ARVORES_E_GRAFOS.pdf) | Os códigos estão em [`09_arvores/`](09_arvores/) e [`10_grafos/`](10_grafos/) (execute com `./run`).

---

## Slide 1 — Capa: De Estruturas Lineares a Árvores e Grafos

```text
  LINEAR (Lista/Pilha/Fila)          HIERARQUIA (Árvore)              REDE / MALHA (Grafo)
                                             [A]                          (0)-------(1)
   [A] ---> [B] ---> [C]                   /  |  \                         | \     / |
                                         [B] [C] [D]                       |   \ /   |
   1 antecessor, 1 sucessor              / \      |                       (2)-------(3)
                                       [E] [F]   [G]
                                    1 pai, vários filhos             Vários caminhos e ciclos
```

- **Árvores** • **Ordenação e Busca** • **Grafos** • **Aplicações na Automação**

---

## Slide 2 — Parte 1: Os 4 Problemas Reais (Sem revelar a solução!)

> **Regra desta etapa:** Usar apenas o que já sabemos (`struct`, ponteiros, vetores, listas, pilhas e filas).

| # | Desafio Real | Pergunta Central |
| :-: | :--- | :--- |
| **1** | **Pastas na IHM / Sistema de Arquivos** | Como guardar $0..N$ subpastas e apagar um ramo sem perder ponteiros? |
| **2** | **Banco de Dados em Tempo Real** | Como buscar rápido, inserir rápido **e** listar em ordem crescente? |
| **3** | **Estradas da Região Central do RS** | Como achar a menor rota sem entrar em *loop* infinito nas voltas? |
| **4** | **Planta Industrial (SCADA + AGVs)** |A mesma estrutura serve para a hierarquia de sensores e para o robô AGV? |

---

## Slide 3 — Problema 1: Pastas e Remoção Segura (`free`)

```text
          [ /painel_ihm ]                 ❌ SE DER free(/alarmes) PRIMEIRO:
         /       |       \
   [config]  [receitas]  [alarmes]  <--- X (ponteiro perdido!)
                         /       \
                   [criticos]  [historico]  (MEMORY LEAK: órfãos na memória!)
                       |
                  [log_01.txt]
```

- Cada pasta pode ter **0, 1 ou 50 itens** (quantos ponteiros colocar na `struct`?).
- Para apagar `/alarmes`, **quem precisa ser liberado primeiro?**

---

## Slide 4 — Problema 2: Busca Rápida + Inserção + Ordem

```text
  VETOR ORDENADO: Busca Binária Rápida O(log n), mas Inserir empurra tudo O(n)!
  +----+----+----+----+----+----+
  | 20 | 30 | 40 | 60 | 70 | 80 |   Inserir 50? Precisa deslocar 60, 70, 80 -->
  +----+----+----+----+----+----+

  LISTA ENCADEADA: Inserir Rápido O(1), mas Busca varre um por um O(n)!
  [20] -> [30] -> [40] -> [50] -> [60] -> [70] -> [80]   (Sem acesso direto ao meio!)
```

- **Desafio:** Como usar ponteiros para começar pelo elemento do **meio** e descartar metade dos dados a cada passo?

---

## Slide 5 — Problema 3: Rotas na Região Central do RS

```text
  [Santa Maria] --30km-- [Trevo SM] --13km-- [Silveira Martins] --21km-- [Ivorá]
                             |                                              |
                            6km                                           18km
                             |                                              |
                     [Recanto Maestro] --14km-- [São João] ----5km---- [Faxinal] --14km-- [Nova Palma]
                             |                                              |
                           19km                                           10km
                             |                                              |
  [Restinga Seca] -13km- [São Miguel] --5km-- [Mosquito] --8km-- [Agudo] -10km- [Dona Francisca]
```

- **Ciclo real:** *Trevo SM → Silveira Martins → Ivorá → Faxinal → São João → Recanto Maestro → Trevo SM*.
- **Dilema:** Menos cidades no caminho $=$ menor distância em km?

---

## Slide 6 — Problema 4: Automação Industrial (Dois Desafios)

```text
  DESAFIO 4A: Supervisório SCADA (ISA-95)         DESAFIO 4B: AGVs e Rede PROFINET em Anel
             [Planta Fabril]                         [CLP 1] ======= [CLP 2]
                /       \                               ||              ||
         [Usinagem]   [Envase] <--- Disjuntor caiu!     ||    X (Cabo   ||  <-- Rota alternativa
                       /    \                           ||    rompido)  ||      pelo anel!
                  [CLP-1]  [CLP-2]                   [CLP 4] ======= [CLP 3]
                   /   \
              [Motor] [Sensor]                    + Robô AGV desviando de corredor bloqueado!
```

- **4A:** Desligar toda uma área e somar a potência elétrica dos equipamentos.
- **4B:** Recalcular rota quando um corredor ou cabo é interrompido.

---

## Slide 7 — ⏸️ INTERVALO PARA PENSAR NA SOLUÇÃO (10–15 min)

```text
       +---------------------------------------+
       |  typedef struct No {                  |
       |      int dado;                        |
       |      struct No * ??? ;                |
       |      struct No * ??? ;                |
       |  } No;                                |
       +---------------------------------------+
```

1. **Desenhem no papel:** Bolinhas (nós) e setas (ponteiros) para os Problemas **1, 2, 3 e 4**.
2. **Pensem:** Quantos ponteiros saem de cada nó? E quantas setas **chegam** em cada nó?
3. **Cuidado:** O que acontece se uma função recursiva seguir setas que formam um anel?

---

## Slide 8 — Parte 2: Árvores (A Estrutura da Hierarquia)

```text
  Nível 0:               [ A ]               <-- RAIZ (único nó sem pai)
                        /  |  \
  Nível 1:           [B]  [C]  [D]           <-- B, C, D são IRMÃOS (mesmo pai A)
                     / \        |
  Nível 2:         [E] [F]     [G]           <-- E, F, C são FOLHAS (sem filhos)
                                |
  Nível 3:                     [H]           <-- FOLHA (Altura da árvore = 3)
```

- **Regra de Ouro:** Cada nó tem **1 único pai** (exceto a raiz) e **0 ou mais filhos**.
- **Sem ciclos** • **Caminho único** da raiz até qualquer nó • **$n$ nós $\Rightarrow n - 1$ arestas**.

---

## Slide 9 — Árvore Genérica vs. Árvore Binária em C

```text
  ÁRVORE GENÉRICA (Primeiro Filho / Próximo Irmão)       ÁRVORE BINÁRIA (Máx. 2 filhos: Esq ≠ Dir)
        [A]                                                         [50]
         |  (primeiro_filho)                                       /    \
        [B] ----> [C] ----> [D] (proximo_irmao)               (esq)      (dir)
         |                   |                                [30]        [70]
        [E] ----> [F]       [G]
```

```c
/* Genérica: qualquer nº de filhos */          /* Binária: posição esquerda/direita importa */
typedef struct No {                            typedef struct No {
    int valor;                                     int valor;
    struct No *primeiro_filho;                     struct No *esquerda;
    struct No *proximo_irmao;                      struct No *direita;
} No;                                          } No;
```

---

## Slide 10 — Ordenação em Árvores: Árvore Binária de Busca (ABB)

```text
         MENORES (<)        MAIORES (>)                 PIOR CASO (Inserir 10, 20, 30, 40):
                 \            /                         Árvore Degenerada = Vira Lista O(n)!
                    [ 50 ]                                  [10]
                   /      \                                    \
               [ 30 ]    [ 70 ]                                [20]
               /    \    /    \                                   \
             [20]  [40][60]  [80]                                 [30] -> Solução: AVL / Rubro-Negra
```

- **Regra da ABB:** `Subárvore Esquerda < Nó Pai < Subárvore Direita`.
- **Balanceada:** Altura $\approx \log_2(n)$ $\Rightarrow$ Busca e Inserção em **$O(\log n)$**.

---

## Slide 11 — Busca Binária na ABB: Cortando Metade a Cada Passo

```text
  Procurando o valor [60]:
                                  [ 50 ]        Passo 1: 60 > 50 (Vai p/ Direita)
                                 //    \\
         CORTADO! ❌ <--------- [30]    [ 70 ]  Passo 2: 60 < 70 (Vai p/ Esquerda)
        /          \                   /     \\
      [20]        [40]              [60]     [80] ---> ❌ CORTADO!
                                   ACHOU! ✅ (em apenas 3 passos)
```

| Quantidade de Nós ($n$) | Busca Sequencial em Lista ($O(n)$) | Busca na ABB Balanceada ($O(\log_2 n)$) |
| :--- | :---: | :---: |
| $1.000$ sensores | até $1.000$ passos | **$\le 10$ passos** |
| $1.000.000$ sensores | até $1.000.000$ passos | **$\le 20$ passos** |

---

## Slide 12 — Os 4 Métodos de Percurso (Visitar Todos os Nós)

```text
            [ 50 ]
           /      \
        [ 30 ]    [ 70 ]
        /    \    /    \
      [20]  [40][60]  [80]
```

| Percurso | Ordem de Visita | Saída no Exemplo | Aplicação Principal |
| :--- | :--- | :--- | :--- |
| **1. Pré-Ordem** | `Raiz → Esq → Dir` | `50, 30, 20, 40, 70, 60, 80` | Imprimir pastas / Clonar hierarquia |
| **2. Em-Ordem** | `Esq → Raiz → Dir` | **`20, 30, 40, 50, 60, 70, 80`** | **ORDENAÇÃO CRESCENTE ($O(n)$)!** |
| **3. Pós-Ordem** | `Esq → Dir → Raiz` | `20, 40, 30, 60, 80, 70, 50` | **Apagar (`free`) e somar filhos antes do pai!** |
| **4. Em Nível (BFS)** | Camada 0, 1, 2 (Fila) | `50, 30, 70, 20, 40, 60, 80` | Achar item mais próximo da raiz |

---

## Slide 13 — Parte 3: Grafos — Quando a Hierarquia Não Basta

```text
       Componente Conexa 1                Componente 2 (Isolada)
       (0) -------- (1) -------- (4)
        |  \         |                        (5) -------- (6)
        |   CICLO    |
       (2) -------- (3)
```

- **Grafo $G = (V, E)$:** Vértices ($V$) + Arestas ($E$) sem restrição de hierarquia.
- **Variações:**
  - **Não Dirigido:** `A — B` (mão dupla) • **Dirigido:** `A → B` (mão única) • **Ponderado:** `A —(15km)— B`.

---

## Slide 14 — Representação de Grafos na Memória em C

```text
  GRAFO:                  MATRIZ DE ADJACÊNCIA O(V²)        LISTA DE ADJACÊNCIA O(V + E)
  (0)---(1)                  0   1   2   3                  [0] -> [1] -> [2] -> NULL
   |   / |                0 [0] [1] [1] [0]                 [1] -> [0] -> [2] -> [3] -> NULL
   |  /  |                1 [1] [0] [1] [1]                 [2] -> [0] -> [1] -> NULL
  (2)   (3)               2 [1] [1] [0] [0]                 [3] -> [1] -> NULL
                          3 [0] [1] [0] [0]
```

- **Matriz:** Rápida para consultar se aresta existe ($O(1)$); ótima para grafos pequenos/densos.
- **Lista Encadeada:** Economiza memória em redes gigantes com poucas conexões por nó.

---

## Slide 15 — Buscas em Grafos: BFS, DFS e Dijkstra

> ⚠️ **Regra Vital:** Como existem **ciclos**, é obrigatório marcar `visitado[v] = true`!

```text
  BFS (Busca em Largura)           DFS (Busca em Profundidade)       DIJKSTRA (Menor Peso)
  Usa FILA (Ondas: 1, 2, 3...)     Usa PILHA / Recursão (Mergulha)   Relaxa arestas acumulando km
       (Orig)                          (Orig)->(A)->(B)               (A)---5km---(B)--2km--(C)
      /   |   \                        (volta se bater no fim)         \________15km________/
    (N1) (N1) (N1)                                                     Escolhe 5+2 = 7km (< 15km)!
```

- **BFS:** Menor número de paradas/saltos • **DFS:** Detecta ciclos e ilhas • **Dijkstra:** Menor distância/tempo.

---

## Slide 16 — Parte 4: Árvore vs. Grafo (A Distinção Visual)

```text
              ÁRVORE (Hierarquia)                          GRAFO (Rede / Malha)
                    [Raiz]                                   (A) ========= (B)
                    /    \                                    ||            ||
                  [A]    [B] <-- Se cortar aqui,              ||            || <-- Se cortar aqui,
                  /        \     [D] fica isolado!            ||            ||     desvia por (C)!
                [C]        [D]                               (C) ========= (D)
```

| Critério | Árvore | Grafo |
| :--- | :--- | :--- |
| **Ciclos / Voltas** | ❌ Proibidos | ✅ Permitidos |
| **Caminhos entre 2 nós** | **Exatamente 1** | **0, 1 ou vários (redundância)** |
| **Pais por nó** | **1 único pai** | Múltiplas conexões |
| **Vetor `visitado[]`** | Não precisa | **Obrigatório** |

---

## Slide 17 — Exemplos Reais no Cotidiano e na Computação

```text
  ÁRVORES NO DIA A DIA                             GRAFOS NO DIA A DIA
  +--------------------+  +--------------------+   +--------------------+  +--------------------+
  | 📁 Pastas do SO    |  | 🧮 Compilador (AST)|   | 🗺️ GPS / Waze      |  | 👥 Redes Sociais   |
  | /home/bruno/ufsm   |  |     [+]            |   | Cruzamentos + km   |  | Amigos (BFS nvl 2) |
  |                    |  |    /   \           |   +--------------------+  +--------------------+
  | 🌐 HTML DOM        |  |  [a]   [*]         |   | 🌐 Internet (BGP)  |  | 🧬 Genealogia Real |
  | html > body > div  |  |       /   \        |   | Rotas redundantes  |  | 2 pais = Grafo DAG |
  +--------------------+  |     [b]   [c]      |   +--------------------+  +--------------------+
                          +--------------------+
```

---

## Slide 18 — Aplicações na Engenharia de Controle e Automação

```text
  1. SUPERVISÓRIO / OPC-UA (ÁRVORE)        2. REDE PROFINET EM ANEL (GRAFO)     3. ROBÔ AGV & GRAFCET (GRAFO)
         [Planta_Industrial]                   [CLP-1] ---- [CLP-2]              (Etapa 1: Encher)
             /         \                          |            |                        |
      [Area_Envase]  [Area_Pintura]               |   ANEL     |                        v
          /     \                              [CLP-4] ---- [CLP-3]              (Etapa 2: Aquecer)
     [CLP_01]  [Inversor_WEG]                 Se 1 cabo rompe, mantém                   |
       /   \                                  comunicação pelo outro!            (Volta à Etapa 1)
   [PT100] [Valvula]
```

- **Árvores:** Hierarquia ISA-95, endereçamento OPC-UA, Árvore de Falhas (FTA).
- **Grafos:** Rotas de robôs AGVs (Dijkstra/A*), Anel de redundância PROFINET (MRP), Sequenciamento GRAFCET/SFC.

---

## Slide 19 — Parte 5: Perguntas-guia para os Problemas 1 e 2

```text
  PROBLEMA 1: Remover um ramo com segurança          PROBLEMA 2: Buscar, inserir e listar em ordem
              [ ? ]  <-- quando liberar o pai?                       [50]
               /   \                                                /    \
          [ ? ]   [ ? ]  <-- e os filhos?                        [30]    [70]
```

- **P1:** Em que ordem os nós precisam ser visitados para que nenhum ponteiro se perca antes do `free`?
  Qual dos percursos (pré, em, pós-ordem) garante isso? Quem precisa receber `NULL` no final?
- **P2:** Por que a ABB descarta metade dos dados a cada comparação *só quando está balanceada*?
  Qual percurso imprime os valores em ordem crescente?
- Estude: [`09_arvores/06_percursos_arvore.c`](09_arvores/06_percursos_arvore.c),
  [`09_arvores/04_bst_insercao_busca.c`](09_arvores/04_bst_insercao_busca.c) e
  [`09_arvores/07_bst_degenerada_vs_balanceada.c`](09_arvores/07_bst_degenerada_vs_balanceada.c).

---

## Slide 20 — Perguntas-guia para os Problemas 3 e 4

- **P3 (mapa do RS):** BFS encontra o caminho com menos cidades; ele é também o de menor km?
  Monte a tabela distância/predecessor de Dijkstra à mão a partir de Santa Maria antes de programar.
- **P4A (SCADA ISA-95):** Para desligar uma área e somar a potência, os filhos precisam ser
  processados antes ou depois do pai?
- **P4B (AGV / PROFINET):** O que muda na busca quando um corredor/cabo é removido do grafo?
  Por que `visitado[]` é obrigatório aqui e não era na árvore?

```bash
# Demonstrações usadas em aula (compilam em build/ e executam passo a passo):
./run 09_arvores/02_automacao_scada.c
./run 09_arvores/03_arvore_binaria.c
./run 10_grafos/04_bfs_saltos_rede.c
./run 10_grafos/08_dijkstra_latencia_rede.c
./run 10_grafos/09_automacao_agv_profinet.c
```
