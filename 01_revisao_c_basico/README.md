# 01 — Revisão de algoritmos e linguagem C (básico)

Ementa, **Unidade 0 — Revisão de algoritmos e linguagem C**, itens 0.1 a 0.5, e a introdução à
análise de complexidade (Big O) usada no resto da disciplina.

## O que a ementa pede e onde está

| Item da ementa | Onde estudar |
| --- | --- |
| 0.1 Apresentação da disciplina | [README da raiz](../README.md) e [EMENTA.md](../EMENTA.md) |
| 0.2 Introdução aos algoritmos e à linguagem C | [`complexidade_big_o.md`](complexidade_big_o.md), `01_contador_big_o.c`, `02_teste_de_mesa_lacos.c`, L01 ex. 4b (bisseção) |
| 0.3 Variáveis e estruturas de controle | [`variaveis.md`](variaveis.md), [`estruturas_de_controle.md`](estruturas_de_controle.md), `02_teste_de_mesa_lacos.c`, [Lista 01](lista_01/README.md) |
| 0.4 Vetores e matrizes | `03_vetores_leituras_sensor.c`, `04_matriz_mapa_forno.c`, L01 ex. 5 e 6 |
| 0.5 Strings | `05_strings_comando_serial.c`, L01 ex. 4a e 7 |
| Avaliação | [Tarefa 1 (T01)](tarefa_1/README.md): só o enunciado |

Continuação: funções e recursividade (0.6) em [`../02_revisao_funcoes_recursao/`](../02_revisao_funcoes_recursao/);
revisão sistemática de tipos, ponteiros e alocação dinâmica (0.7) em
[`../03_revisao_sistematica_c/`](../03_revisao_sistematica_c/).

## Conceito em uma página

```text
 algoritmo = sequência finita de passos     programa C = algoritmo + tipos + memória

 vetor  int v[6]          matriz  int m[3][4]               string  char s[] = "SET"
 idx:  0  1  2  3  4  5          col0 col1 col2 col3        idx:  0   1   2   3
     [48|52|55|61|58|66]   lin0 [ .  |  . |  . |  . ]           ['S'|'E'|'T'|'\0']
      ^ v[0]       v[5] ^  lin1 [ .  |  . |  . |  . ]           o '\0' marca o fim
      acesso v[k]: O(1)    lin2 [ .  |  . |  . |  . ]           strlen: O(n)
      varredura:   O(n)    varredura linha × coluna: O(n·m)
```

Big O mede **como o custo cresce** quando os dados crescem. A teoria completa, com tabela de
crescimento e um exemplo de CLP, está em [`complexidade_big_o.md`](complexidade_big_o.md).

## Ordem de estudo recomendada

| # | Arquivo | O que mostra | Complexidade-chave |
| --- | --- | --- | --- |
| 1 | [`variaveis.md`](variaveis.md) | tipos, tamanhos, escopo, armadilhas | — |
| 2 | [`estruturas_de_controle.md`](estruturas_de_controle.md) | `if`, `switch`, `for`, `while`, `do-while` | — |
| 3 | [`complexidade_big_o.md`](complexidade_big_o.md) | o que é Big O, regras, melhor/médio/pior caso, espaço | todas |
| 4 | `01_contador_big_o.c` | contadores de operações para O(1) a O(2ⁿ), tabela n = 10..10000 | O(1) … O(2ⁿ) |
| 5 | `02_teste_de_mesa_lacos.c` | teste de mesa: média móvel (for), varredura de CLP (while), autoteste (do-while) | O(n·k) vs O(n) |
| 6 | [Lista 01](lista_01/README.md) | 11 exercícios de controle, com soluções | O(1), O(n) |
| 7 | `03_vetores_leituras_sensor.c` | máximo, mínimo e média em uma passada; índice vs busca; armadilha do máximo = 0 | O(n), O(1) |
| 8 | `04_matriz_mapa_forno.c` | varredura linha × coluna com cursor, imagem térmica ASCII, memória row-major | O(n·m) |
| 9 | `05_strings_comando_serial.c` | parser de `SET VEL 120`, conversão texto → int, `strlen` no laço | O(n) vs O(n²) |
| 10 | [Tarefa 1](tarefa_1/README.md) | tarefa avaliada | — |

## Complexidade das operações deste módulo

| Operação | Vetor (n) | Matriz (n × m) | String (n caracteres) |
| --- | --- | --- | --- |
| Acessar elemento por índice | O(1) | O(1) | O(1) |
| Percorrer tudo (máx, mín, soma) | O(n) | O(n·m) | O(n) |
| Procurar um valor (sem ordem) | O(n) | O(n·m) | O(n) |
| Descobrir o tamanho | O(1) (constante conhecida) | O(1) | O(n) (`strlen` procura o `'\0'`) |
| Laço com `strlen` na condição | — | — | O(n²) |
| Média móvel de janela k, n amostras | O(n·k) ingênua / O(n) incremental | — | — |

## Exemplos reais

- **CLP**: ciclo de varredura (ler entradas → lógica → escrever saídas) é um `while`; a lógica com
  histerese é um `if / else if` que mantém o estado.
- **Sensores**: média móvel e máximo/mínimo de um buffer de leituras (vetor); filtro de ruído.
- **Visão e termografia**: imagem de câmera ou mapa de temperatura de forno é uma matriz;
  dobrar a resolução quadruplica o custo.
- **Comunicação serial**: inversores, balanças, CNCs e modems recebem comandos de texto
  (`SET VEL 120`, `AT+CSQ`) que o firmware precisa interpretar caractere a caractere.
- **Dia a dia**: calendário (dias decorridos, dia da semana), IMC, contatos ordenados no celular.

## Como executar

Na raiz do repositório (o script compila para `build/`; não use `gcc ... -o` dentro da pasta):

```bash
./run list 01_revisao_c_basico          # lista os programas desta pasta
./run 01_contador_big_o                 # ENTER avança cada passo
./run 02_teste_de_mesa_lacos
./run 03_vetores_leituras_sensor
./run 04_matriz_mapa_forno
./run 05_strings_comando_serial         # no fim, digite comandos (SAIR encerra)
./run 04_matriz_mapa_forno < /dev/null  # executa direto, sem pausas
```

Os exercícios da lista têm exemplos de entrada no [README da Lista 01](lista_01/README.md).

## Exercícios propostos (sem solução)

1. Em `03_vetores_leituras_sensor.c`, conte quantas leituras ficaram acima da média e diga qual é
   a complexidade da sua solução. Dá para fazer em uma passada só? Por quê?
2. Altere `02_teste_de_mesa_lacos.c` para uma janela de média móvel k = 8 e compare as contagens
   das versões ingênua e incremental para n = 1000.
3. No mapa do forno, calcule a média de cada **coluna** (posição ao longo do forno). Escreva os
   dois laços nas duas ordens possíveis e explique qual percorre a memória em sequência.
4. Acrescente ao interpretador serial o comando `SET TEMP <valor>` com faixa de 0 a 300 e a
   resposta `ERR` para valores fora da faixa.
5. Escreva uma função que diga se uma string é um número inteiro válido com sinal opcional
   (`-12`, `+7`, `42`). Qual é o custo em função do número de caracteres?
6. Um programa compara todos os pares de 2 000 leituras para achar duplicatas. Estime o número de
   comparações e diga se cabe em um ciclo de 10 ms de um CLP que faz 10⁷ operações por segundo.
7. Classifique cada trecho em O(1), O(log n), O(n), O(n log n) ou O(n²):
   `for (i = 1; i < n; i *= 2)`; `for (i = 0; i < n; i++) for (j = 0; j < 10; j++)`;
   `for (i = 0; i < n; i++) for (j = 0; j < i; j++)`.

## Material da disciplina (Google Classroom)

Tópico "Linguagem C (básico)": apresentações das aulas 01 a 03, exemplos de aula, Lista L01 (não
avaliada, soluções nesta pasta) e Tarefa T01 (avaliada, só o enunciado nesta pasta).
