# Estrutura de Dados para Engenharia

Este repositório possui os conceitos e exemplos de Estrutura de Dados para Engenharia, servindo como material de apoio a disciplina UFSM00064 - ESTRUTURA DE DADOS para o Curso Engenharia de Controle e Automação da UFSM, ministrada em 2026/2.

Links úteis:

- [Ementa](EMENTA.md)

## Como compilar e executar

Todos os exemplos são arquivos `.c` independentes. O script [`run`](run) na raiz compila (com `cc -Wall -g ... -lm`) e executa qualquer um deles; os executáveis ficam em `build/`, espelhando a estrutura de pastas.

```sh
./run 01_grafo                    # compila (se necessário) e executa 8_grafos/01_grafo.c
./run 8_grafos/01_grafo.c         # caminho completo também funciona
./run 08_args a b c               # argumentos extras são repassados ao programa
echo 7 | ./run 03_primo           # entrada via stdin
./run build                       # compila todos os arquivos e mostra um resumo
./run build 01_arvore             # compila apenas um arquivo
./run list                        # lista todos os .c
./run list pilhas                 # lista filtrando por trecho do caminho
./run clean                       # remove build/
```

O nome pode ser o caminho, o nome do arquivo (com ou sem `.c`) ou um trecho único do caminho. Se o trecho corresponder a mais de um arquivo (ex.: `valida_colchetes`), o script lista as opções. Só recompila quando o `.c` é mais recente que o executável. `CC`, `CFLAGS` e `LDLIBS` podem ser sobrescritos: `CC=gcc CFLAGS="-O2" ./run 01_grafo`.

Para digitar apenas `run <arquivo>`, crie um alias (ex.: no `~/.zshrc`):

```sh
alias run="$HOME/www/personal/mestrado/estrutura_de_dados_para_engenharia/run"
```

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

 # Estrutura de Dados

 De forma abstrata, a computação se baseia em dois pilares fundamentais: 
 - o **estado** (os dados que o programa conhece) 
 - e o **controle de fluxo** (as regras que ditam como e quando esse estado é alterado)

 Hoje vamos explorar, de forma generalista, as variáveis e as estruturas de controle de fluxo dentro da programação.

 ## Por que aprender em C ?

C é uma linguagem de baixo nível, então suas estruturas de dados são mais "puras", e, portanto, mais fáceis de compreender em seu conceito.

Python, Javascript, e outras linguagens são de mais alto nível, também implementam as estruturas de dados porém já "tomando algumas decisões" e "fazendo coisas por nós", como desalocação/limpeza de memória.

Isso não tira a sua importância e utilidade, mas o uso do C nos permite compreender a origem das estruturas e quando utilizá-las.

Dito isto, minha ideia é apresentar em mais de uma linguagens as diferentes estruturas, de forma a permitir que vocês aprendam não somente a IMPLEMENTAÇÃO em C, mas sim o conceito e a utilidade delas, podendo aplicar nas mais diferentes linguagens.

## Por que entender Estruturas de Dados é tão importante ?

Quando eu falo importante, é importante mesmo, desde o mais básico programador até os mais bem pagos do mundo:
> Vagas em Big Techs (Google, Amazon, Meta) normalmente cobram Estrutura de Dados e Algoritmos como uma etapa. Pagando milhares de $$$ por ano. Por que ?


Entender as estruturas de dados nos permite modelar a vida real de diferentes formas, já que o mesmo problema pode ter diversas soluções.

> Uma rede elétrica pode ser modelada como uma *Árvore* (para focar na distribuição hierárquica) ou como um *Grafo* (para calcular rotas alternativas de energia em caso de falha).

> O estoque de peças na manufatura pode usar uma Fila (para processamento sequencial e contínuo) ou uma Tabela Hash (para busca instantânea de um componente específico pelo código).

Normalmente estamos buscando uma solução "ótima", mas o conceito de ótima é relativo:
- Foco em velocidade
  - Pequena quantidade de dados
  - Grande quantidade de dados
- Foco em 
- Foco em garantir uma hierarquia
- Foco em construir uma "sequência" de dados


Só quem entende de estruturas de dados é capaz de modelar uma solução ótima, pois entende como os dados são estruturados na raiz da linguagem, na memória do computador, e sabe avaliar quais são as melhores estruturas para cada problema.

# [Variáveis](0_revisao/variaveis.md)
# [Estruturas de Controle](0_revisao/estruturas_de_controle.md)

# [Listas encadeadas](6_listas/README.md)
# [Árvores](7_arvores/README.md)
# [Grafos](8_grafos/README.md)
