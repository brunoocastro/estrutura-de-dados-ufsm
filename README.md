# Estrutura de Dados para Engenharia

Este repositório possui os conceitos e exemplos de Estrutura de Dados para Engenharia, servindo como material de apoio a disciplina UFSM00064 - ESTRUTURA DE DADOS para o Curso Engenharia de Controle e Automação da UFSM, ministrada em 2026/2.

Links úteis:

- [Ementa](EMENTA.md)
- [Introdução à notação Big O](01_revisao_c_basico/complexidade_big_o.md) — leia antes dos módulos de estruturas
- [Slides: Árvores, Ordenação, Buscas e Grafos](AULA_ARVORES_E_GRAFOS.md) ([versão em PDF](AULA_ARVORES_E_GRAFOS.pdf))
## Módulos (na ordem da ementa)

Cada pasta tem um `README.md` com o que a ementa pede, a ordem de estudo recomendada, a tabela de complexidade, exemplos reais e exercícios propostos. Os arquivos `.c` trazem cabeçalho com objetivo, exemplo real e análise Big O. Os arquivos de demonstração mostram o algoritmo **passo a passo** no terminal: tecle ENTER para avançar cada passo.

| Pasta | Ementa | Destaques |
| --- | --- | --- |
| [`01_revisao_c_basico`](01_revisao_c_basico/) | Unidade 0 (0.1–0.5) | Big O, teste de mesa de laços, vetores de sensores, matriz de forno, parser serial, L01 e enunciado de T01 |
| [`02_revisao_funcoes_recursao`](02_revisao_funcoes_recursao/) | Unidade 0 (0.6) | passagem por valor, modularização de controle, primeira recursão, L02 e enunciado de T02 |
| [`03_revisao_sistematica_c`](03_revisao_sistematica_c/) | Unidade 0 (0.4–0.7) | tipos, vetores, strings, structs, ponteiros, funções, recursão (Hanói, memoização), alocação dinâmica |
| [`04_tads`](04_tads/) | Unidade 1 | tipos primitivos e padding, TAD opaco (sensor 4–20 mA, TON), duas implementações da mesma interface, definição recorrente |
| [`05_pilhas`](05_pilhas/) | Unidade 2 | vetor × encadeada, validadores de delimitadores, RPN, shunting-yard, desfazer/refazer de IHM, backtracking de AGV |
| [`06_filas`](06_filas/) | Unidade 3 | linear × circular, fila encadeada, ring buffer de UART, deque, janela deslizante, round-robin de CLP, enunciado de L04 |
| [`07_listas`](07_listas/) | Unidade 1.3 (TAD recorrente) | religamento passo a passo, vetor × lista, lista dupla (alarmes), circular (carrossel de IHM), sentinela, enunciado de T05 |
| [`08_ordenacao_busca`](08_ordenacao_busca/) | Unidade 4 | sequencial × binária, bolha/seleção/inserção, quicksort, merge sort, benchmark, filtro de mediana, estabilidade |
| [`09_arvores`](09_arvores/) | Unidade 5 | árvore genérica, SCADA, BST (inserção, busca, remoção), percursos, degenerada × balanceada, AVL, heap de alarmes, enunciado de T06 |
| [`10_grafos`](10_grafos/) | Unidade 6 | matriz × lista, BFS, DFS e ciclos, ordenação topológica, Dijkstra, AGV/PROFINET, árvore geradora mínima |

## Como compilar e executar

Todos os exemplos são arquivos `.c` independentes. O script [`run`](run) na raiz compila (com `cc -Wall -g ... -lm`) e executa qualquer um deles. Os executáveis ficam em `build/`, numa estrutura de pastas igual à do repositório.

```sh
./run 04_quicksort_visual             # compila (se necessário) e executa 08_ordenacao_busca/04_quicksort_visual.c
./run 10_grafos/01_grafo.c            # caminho completo também funciona
./run 04_quicksort_visual < /dev/null # sem pausas: executa direto até o fim
./run 08_args a b c                   # argumentos extras são repassados ao programa
echo "SET VEL 120" | ./run 05_strings_comando_serial   # entrada via stdin
./run build                           # compila todos os arquivos e mostra um resumo
./run build 01_arvore                 # compila apenas um arquivo
./run list                            # lista todos os .c
./run list pilhas                     # lista filtrando por trecho do caminho
./run clean                           # remove build/
```

O nome pode ser o caminho, o nome do arquivo (com ou sem `.c`) ou um trecho único do caminho. Se o trecho corresponder a mais de um arquivo (ex.: `valida`), o script lista as opções. Só recompila quando o `.c` é mais recente que o executável. `CC`, `CFLAGS` e `LDLIBS` podem ser sobrescritos: `CC=gcc CFLAGS="-O2" ./run 01_grafo`.

Para digitar apenas `run <arquivo>`, crie um alias apontando para a pasta onde você clonou o repositório (ex.: no `~/.zshrc` ou `~/.bashrc`):

```sh
alias run="$HOME/caminho/para/estrutura-de-dados-ufsm/run"
```

No Windows, use o WSL ou o Git Bash. O script precisa de `bash` e de um compilador C (`cc`, `gcc` ou `clang`).

## Cronograma
| Data  | Assunto                                               |     |
| ----- | ----------------------------------------------------- | --- |
| 6/8   | Apresentação da disciplina e configuração de ambiente |     |
| 13/8  | Variáveis e estruturas de controle                    |     |
| 20/8  | Strings                                               |     |
| 27/8  | Ponteiros                                             |     |
| 3/9   | Pilhas                                                |     |
| 10/9  | Fila simples                                          |     |
| 17/9  | Atendimento -  avaliação dos trabalhos                |     |
| 24/9  | Listas                                                |     |
| 01/10 | Árvores                                               |     |
| 8/10  | Grafos                                                |     |
| 15/10 | Ordenação                                             |     |
| 22/10 | Aplicações                                            |     |
| 5/11  | Projetos finais                                       |     |
| 19/11 | Projetos finais                                       |     |
| 26/11 | Avaliação                                             |     |

---

## O que é uma Estrutura de Dados?

De forma abstrata, a computação se baseia em dois pilares fundamentais:
- o **estado** (os dados que o programa conhece);
- o **controle de fluxo** (as regras que ditam como e quando esse estado é alterado).

### Por que aprender em C?

C é uma linguagem de baixo nível, então suas estruturas de dados são mais "puras" e, portanto, mais fáceis de compreender em seu conceito.

Python, JavaScript e outras linguagens de mais alto nível também implementam essas estruturas, porém já "tomando algumas decisões" e "fazendo coisas por nós", como alocação e limpeza automática de memória.

Isso não tira a importância e utilidade dessas linguagens, mas o uso de C nos permite compreender a origem das estruturas e quando utilizá-las, aplicando o mesmo raciocínio em qualquer outra linguagem.

### Por que entender Estruturas de Dados é tão importante?

Desde o programador iniciante até vagas em grandes empresas de tecnologia (Google, Amazon, Meta), estruturas de dados e algoritmos são cobrados porque permitem modelar problemas reais de diferentes maneiras:

> Uma rede elétrica pode ser modelada como uma **Árvore** (para focar na distribuição hierárquica) ou como um **Grafo** (para calcular rotas alternativas de energia em caso de falha).

> O estoque de peças na manufatura pode usar uma **Fila** (para processamento sequencial e contínuo) ou uma **Árvore de Busca / Tabela Hash** (para busca rápida de um componente específico pelo código).

Normalmente buscamos uma solução "ótima", mas o conceito de ótimo depende das restrições do projeto:
- Foco em velocidade (para pequenas ou grandes quantidades de dados);
- Foco em uso previsível de memória (sistemas embarcados e CLPs);
- Foco em garantir uma hierarquia (supervisórios SCADA / ISA-95);
- Foco em construir uma sequência ou malha redundante de dados.

Só quem entende de estruturas de dados é capaz de projetar uma solução ótima, pois compreende como os dados são organizados na memória do computador e sabe avaliar qual estrutura resolve melhor cada problema.

## Por onde começar

1. [Variáveis](01_revisao_c_basico/variaveis.md) e [Estruturas de Controle](01_revisao_c_basico/estruturas_de_controle.md)
2. [Notação Big O](01_revisao_c_basico/complexidade_big_o.md)
3. Siga a tabela de [módulos](#módulos-na-ordem-da-ementa) em ordem; cada `README.md` indica a sequência de arquivos.
