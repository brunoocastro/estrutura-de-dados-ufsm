# Tipos Abstratos de Dados (TADs) em C

Esta pasta liga a revisão de C (pastas `01` a `03`) às estruturas de dados (pilhas, filas, listas,
árvores e grafos). A ideia central é que **toda estrutura de dados do curso é um TAD**: um conjunto
de dados que só pode ser usado por meio de operações bem definidas. Uma pilha, por exemplo, é
qualquer coisa que ofereça `push`, `pop` e `topo`. Por dentro, ela pode guardar os elementos num
vetor ou numa lista encadeada.

## 1. O que a ementa pede e onde está

| Ementa (Unidade 1) | Onde | O que o aluno vê |
| --- | --- | --- |
| 1.1 Tipos primitivos de dados | `01_tipos_primitivos_sensores.c` | tamanhos e faixas, ADC → bar, complemento de dois, endianness, estouro de `uint16_t`, precisão de `float` |
| 1.1 Estruturas (structs) | `02_struct_padding_visual.c` | o compilador decide o layout (alinhamento e padding) passo a passo, mapa de bytes, custo em vetores; mostra que uma struct não protege regras |
| 1.2 Tipos abstratos de dados | `03_tad_sensor_interface.c` | TAD **opaco**: interface, usuário e implementação no mesmo arquivo, com encapsulamento garantido pelo compilador |
| 1.2 | `04_tad_fracao_engrenagem.c` | TAD **por valor** com invariante (forma canônica) e Euclides em execução; fração exata comparada com `float` |
| 1.2 | `05_tad_temporizador_ton.c` | TAD TON (IEC 61131-3) com várias instâncias e diagrama de tempos; a implementação protege contra o estouro de 49,7 dias |
| 1.2 | `06_tad_conjunto_duas_impl.c` | **mesma interface, duas implementações** (vetor não ordenado × ordenado), com contadores e uma tabela de crescimento |
| 1.3 Definição recorrente de TADs | `07_lista_definicao_recorrente.c` | "lista = vazia OU elemento + lista": struct auto-referenciada e recursão desenhada (tamanho, soma, busca, impressão) |
| 1.3 | `08_arvore_expressao_recorrente.c` | "expressão = número OU variável OU (expr op expr)": árvore binária, avaliação recursiva, infixa/pós-fixa, malha de controle |

Revisão de base: os detalhes da linguagem (tipos, `struct`, `union`, `enum`, ponteiros) estão em
[`../03_revisao_sistematica_c/01_tipos_simples`](../03_revisao_sistematica_c/01_tipos_simples/) e
[`../03_revisao_sistematica_c/02_tipos_compostos`](../03_revisao_sistematica_c/02_tipos_compostos/).
Esta pasta não repete esse conteúdo: ela o usa para *projetar* tipos.

## 2. Conceito

### Do primitivo ao TAD

```text
 tipo primitivo           struct                       TAD
 (bytes + regra)          (agrupa campos)              (dados + operações + regras)

  int16_t  [FF][7F]       Sensor { tag, valor, ... }   sensor_criar()   sensor_ler()
  float    [..][..][..]   qualquer código pode         sensor_valor()   sensor_destruir()
                          escrever s.valor = -999      os campos só mudam por estas funções,
                                                       que mantêm as invariantes
```

Uma **invariante** é uma regra que vale para todo objeto do TAD, antes e depois de cada operação.
Por exemplo: "denominador > 0 e fração simplificada", "alarme_baixo < alarme_alto", "vetor sempre
ordenado". O construtor estabelece a invariante e cada operação a preserva. É por isso que o usuário
não deve mexer nos campos diretamente.

### Interface × implementação

```text
            +-------------------------------+
 usuário -> |  INTERFACE (o QUE faz)        |   sensor.h: tipos + protótipos + contrato
            +-------------------------------+
            |  IMPLEMENTAÇÃO (COMO faz)     |   sensor.c: struct real + código + static
            +-------------------------------+
 Trocar a implementação (outra struct, outro algoritmo) não muda o código do usuário.
```

Há dois estilos nesta pasta:

| Estilo | Exemplo | Como o usuário declara | Proteção |
| --- | --- | --- | --- |
| **Opaco** | `Sensor` (03) | `Sensor *s = sensor_criar(...)` | o compilador **impede** o acesso aos campos (tipo incompleto) |
| **Por valor / transparente** | `Fracao` (04), `Ton` (05) | `Fracao f = fracao_criar(...)` / `Ton t; ton_iniciar(&t, ...)` | só por **contrato** (convenção); permite alocação estática, sem `malloc`, como num CLP |

### Definição recorrente

```text
 Lista   ::=  vazia  |  elemento  Lista
 Árvore  ::=  vazia  |  raiz  Árvore(esq)  Árvore(dir)

 rota:  (10:12m) -> (20:8m) -> (30:15m) -> []        expressão:      [+]
         └ elemento ┘└──────── Lista ─────────┘                      /    \
                                                                  [*]    bias
 typedef struct No {                                              /  \
     int estacao, metros;                                        2   [-]
     struct No *resto;   /* outra Lista */                           /  \
 } No;                                                              sp   pv
```

Cada caso da definição vira um caso da função recursiva: lista vazia → **caso base**; elemento +
resto → **caso recursivo**. O mesmo molde aparece em [`../07_listas`](../07_listas/), em versão
iterativa, e em [`../09_arvores`](../09_arvores/).

## 3. Ordem de estudo recomendada

| Arquivo | O que mostra | Complexidade-chave |
| --- | --- | --- |
| `01_tipos_primitivos_sensores.c` | bytes, faixas, ADC, complemento de dois, estouro, `float` | O(1) por valor; memória = n × `sizeof` |
| `02_struct_padding_visual.c` | algoritmo de layout, padding, cópia, struct sem invariante | layout O(c campos); acesso O(1) |
| `03_tad_sensor_interface.c` | TAD opaco, construtor que valida, histerese, instâncias | todas as operações O(1) |
| `04_tad_fracao_engrenagem.c` | invariante, Euclides passo a passo, exato × `float` | O(log min(a,b)) por operação |
| `05_tad_temporizador_ton.c` | TON, scan do CLP, diagrama de tempos, saturação | O(1) por scan; O(k) com k timers |
| `06_tad_conjunto_duas_impl.c` | mesma interface, duas implementações, contadores | O(n) × O(log n) na busca |
| `07_lista_definicao_recorrente.c` | lista recorrente, rastreio da recursão | O(n) tempo, O(n) pilha |
| `08_arvore_expressao_recorrente.c` | árvore de expressão, avaliação recursiva | O(n) tempo, O(h) pilha |

## 4. Complexidade comparada

**TAD Conjunto (`06`)**: a mesma interface com duas implementações.

| Operação | A: vetor não ordenado | B: vetor ordenado |
| --- | --- | --- |
| `contem` | O(n) | **O(log n)** (busca binária) |
| `inserir` (sem repetidos) | O(n) comparações + O(1) movimento | O(log n) comparações + O(n) movimentos |
| `remover` | O(n) comparações + O(1) (troca com o último) | O(log n) comparações + O(n) movimentos |
| `minimo` | O(n) | **O(1)** |
| listar em ordem | O(n log n) (precisa ordenar) | O(n) |
| espaço | O(n) | O(n) |

Medido pelo programa: com n = 10000 crachás, 10000 consultas custam cerca de 50 milhões de
comparações em A e cerca de 124 mil em B.

**Definição recorrente (`07`, `08`)**: recursiva × iterativa.

| Operação | Tempo | Espaço recursivo | Espaço iterativo |
| --- | --- | --- | --- |
| tamanho/soma/busca em lista | O(n) | O(n) quadros na pilha | O(1) |
| avaliar/contar/altura em árvore | O(n) | O(h), com h = altura | exigiria uma pilha explícita, O(h) |

Para a teoria de Big O, veja [`../01_revisao_c_basico/complexidade_big_o.md`](../01_revisao_c_basico/complexidade_big_o.md).

## 5. Exemplos reais

- **Sensores e CLP**: transmissores 4–20 mA (escala, cabo rompido abaixo de 3,8 mA, alarmes com
  histerese); temporizadores TON/TOF; contadores de encoder de 16 bits.
- **Acionamentos**: a "engrenagem eletrônica" (numerador/denominador) dos servodrives, guardada como
  fração para não acumular erro de posição.
- **Redes industriais**: registradores Modbus de 16 bits em big-endian; listas de escravos ativos.
- **Supervisório/IHM**: registros de histórico (o padding pesa em milhões de amostras); blocos de
  cálculo configuráveis, avaliados como árvores de expressão.
- **Dia a dia**: catraca com crachás autorizados (conjunto ordenado), rota de entregas (lista),
  calculadora (árvore de expressão), o bug de "travar após 49,7 dias ligado".

## 6. Como um projeto real divide um TAD em `.h` e `.c`

Nesta pasta, cada exemplo fica num único `.c`, porque o script `run` compila cada arquivo
isoladamente. As seções `INTERFACE`, `USUÁRIO` e `IMPLEMENTAÇÃO` marcam onde cada arquivo começaria.
Num projeto real, o TAD Sensor de `03` ficaria assim:

**`sensor.h`**: a interface, que é o único arquivo que o usuário inclui.

```c
#ifndef SENSOR_H          /* include guard: evita definir tudo duas vezes */
#define SENSOR_H

typedef struct Sensor Sensor;   /* tipo opaco: os campos NÃO aparecem aqui */

typedef enum { SENSOR_NORMAL, SENSOR_ALARME_BAIXO, SENSOR_ALARME_ALTO, SENSOR_FALHA } EstadoSensor;

Sensor      *sensor_criar(const char *tag, const char *unidade, double ma_min, double ma_max,
                          double eng_min, double eng_max, double alarme_baixo,
                          double alarme_alto, double histerese);
EstadoSensor sensor_ler(Sensor *s, double corrente_ma);
double       sensor_valor(const Sensor *s);
void         sensor_destruir(Sensor *s);

#endif
```

**`sensor.c`**: a implementação. Só ela conhece a struct.

```c
#include <stdlib.h>
#include "sensor.h"

struct Sensor {                 /* definição completa, invisível para quem só inclui sensor.h */
    char tag[16];
    double eng_min, eng_max, valor;
    /* ... */
};

static double escalar(const Sensor *s, double ma) { /* 'static': privada deste arquivo */ }

Sensor *sensor_criar(/* ... */) { /* valida, malloc, preenche */ }
void sensor_destruir(Sensor *s) { free(s); }
```

**`main.c`**: o usuário.

```c
#include "sensor.h"

int main(void) {
    Sensor *p = sensor_criar("PT-101", "bar", 4, 20, 0, 10, 2, 8, 0.5);
    sensor_ler(p, 12.0);
    /* p->valor = 5;   ERRO de compilação: 'struct Sensor' é incompleta aqui */
    sensor_destruir(p);
    return 0;
}
```

Compilação em vários arquivos (comando de referência; **não** é usado nesta pasta):

```text
cc -std=c11 -Wall -Wextra -pedantic -c sensor.c          # gera sensor.o
cc -std=c11 -Wall -Wextra -pedantic -c main.c            # gera main.o
cc sensor.o main.o -o supervisorio -lm                   # liga os dois
```

Pontos importantes:

- `#include "sensor.h"` (com aspas) procura o arquivo na pasta do projeto, e `<stdio.h>` procura
  nas pastas do sistema.
- Funções `static` e a definição da struct ficam **só** no `.c`. Se a implementação mudar, apenas
  `sensor.o` é recompilado, e o usuário só precisa religar.
- A troca de implementação de `06` acontece aqui na hora de ligar: você liga `conjunto_vetor.o` ou
  `conjunto_ordenado.o`, os dois com o mesmo `conjunto.h`. A tabela de ponteiros para função do
  exemplo `06` é a versão em um arquivo só dessa ideia.
- Armadilha comum: definir variáveis ou funções não `static` dentro do `.h`. Quando o `.h` é
  incluído em dois `.c`, a ligação falha com "multiple definition".

## 7. Como executar

Na raiz do repositório (o script compila para `build/`):

```sh
./run 01_tipos_primitivos_sensores          # ENTER avança cada passo
./run 03_tad_sensor_interface
./run 06_tad_conjunto_duas_impl < /dev/null # roda direto, sem pausas
./run list 04_tads                          # lista os exemplos desta pasta
```

As pausas só acontecem quando a entrada é um terminal. Com `< /dev/null` ou um pipe, o programa
roda até o fim sem esperar.

## 8. Exercícios propostos

1. **Tipos primitivos**: um ADC de 16 bits com sinal mede de −50 °C a +150 °C. Escreva a conversão
   `int16_t → double`, calcule a resolução em °C e mostre o que acontece se o cálculo for feito
   todo em inteiros.
2. **Padding**: preveja à mão `sizeof` e os offsets de
   `struct { char a; int32_t b; char c; double d; uint16_t e; }`, desenhe o mapa de bytes e
   confira com `offsetof`. Depois reordene os campos para o menor tamanho possível.
3. **TAD Sensor**: troque a média simples por uma média móvel das últimas 8 leituras, sem alterar
   nenhuma linha do `main`. Qual passa a ser o custo de espaço por sensor?
4. **TAD Ponto2D**: crie um TAD opaco com `ponto_criar`, `ponto_distancia`, `ponto_transladar` e
   `ponto_rotacionar`, para descrever a posição de um AGV. Explique por que o tipo opaco permite
   trocar coordenadas cartesianas por polares sem mudar o usuário.
5. **TAD Fração**: acrescente `fracao_subtrair` e `fracao_dividir`, mantendo a invariante. O que
   deve acontecer ao dividir por zero?
6. **Temporizadores**: implemente, com o mesmo estilo de interface, o **TOF** (atraso no
   desligamento) e o **TP** (pulso de duração fixa) da IEC 61131-3. Desenhe os diagramas de tempos.
7. **Conjunto**: acrescente `uniao` e `intersecao` às duas implementações. Quantas comparações cada
   uma faz para dois conjuntos de tamanho n? Meça com os contadores.
8. **Lista recorrente**: escreva versões recursivas de `maximo`, `contar_ocorrencias`, `concatenar`
   e `copiar`. Indique o caso base e o caso recursivo de cada uma.
9. **Árvore de expressão**: acrescente o operador unário de negação e a função `folhas`. Monte a
   árvore de `u = kp·e + ki·soma_e` (controlador PI) e simule a mesma malha de nível. O erro de
   regime desaparece?

## Ligações com os outros módulos

- Antes: [`../03_revisao_sistematica_c`](../03_revisao_sistematica_c/) (tipos, structs,
  ponteiros, recursão).
- Depois: [`../05_pilhas`](../05_pilhas/) (TAD pilha em vetor e encadeado), [`../06_filas`](../06_filas/),
  [`../07_listas`](../07_listas/) (a lista recorrente, iterativa), [`../08_ordenacao_busca`](../08_ordenacao_busca/)
  (busca binária do conjunto ordenado), [`../09_arvores`](../09_arvores/) e [`../10_grafos`](../10_grafos/).
