# Estruturas de Controle/Condicionais (Desvios Lógicos)

Abstratamente, condicionais são bifurcações (grafos direcionados) no fluxo de execução do programa. A máquina avalia uma expressão reduzindo-a a uma resposta binária (Verdadeiro ou Falso) e escolhe qual caminho de memória seguir.

Conceitos relacionados necessários:

* **Expressões Booleanas e Operadores Relacionais:** A base da decisão. Comparações como maior que (`>`), igualdade (`==` ou `===`).
* **Valores Truthy e Falsy:** Em C, o número `0` ou um ponteiro `NULL` são considerados Falsos; qualquer outro número é Verdadeiro. Em Python e JS, coleções vazias (`[]`, `{}`), textos vazios (`""`) ou valores indefinidos (`undefined`) também podem ser avaliados implicitamente como Falsos.

### Exemplo 1: Engenharia (Controle de Processos) - Implementado em Python

Na engenharia de controle, condicionais atuam como atuadores lógicos, ativando ou desativando sistemas (como válvulas ou alarmes) baseados em limites de segurança.

```python
# Python não usa chaves {}, mas sim indentação para definir o bloco.
temperatura_reator = 350.5
TEMP_CRITICA = 300.0

if temperatura_reator > TEMP_CRITICA:
    print("ALERTA: Iniciando resfriamento de emergência.")
    # Código para abrir válvula de alívio
elif temperatura_reator < 100.0:
    print("Aquecedores ativados.")
else:
    print("Operação nominal.")

```

### Exemplo 2: Ciência da Computação - Implementado em C

Em ciência da computação pura (especialmente no nível de sistemas), condicionais são vitais para verificar o sucesso de alocações de recursos do sistema operacional ou a existência de nós em estruturas de dados.

```c
// Em C, a sintaxe exige parênteses para a condição e chaves para o bloco.
int *memoria_alocada = malloc(1024 * sizeof(int));

if (memoria_alocada == NULL) {
    // "NULL" avalia para falso (0). Protege o sistema contra Segmentation Fault.
    printf("Erro crítico de CC: Sistema sem memória disponível.\n");
    return 1; 
}

```


# Estruturas de Repetição (Iteração e Ciclos)

Abstratamente, uma estrutura de repetição permite reexecutar um bloco de instruções de memória sem precisar reescrever o código, até que uma **condição de parada** seja atingida.

Conceitos relacionados necessários:

* **Condição de Saída (Exit Condition):** O estado lógico que quebra o ciclo. Sem isso, você cria um "loop infinito", travando o processador no mesmo bloco.
* **Iterabilidade:** A capacidade de percorrer uma coleção de dados (como matrizes ou listas) elemento por elemento. Python e JS abstraem muito isso, enquanto C exige manipulação manual de índices.
* **Complexidade / Big-O:** cada laço aninhado **multiplica** o custo: um laço de `n` dentro de outro de `n` faz `n²` repetições (crescimento polinomial, não exponencial). Veja [`complexidade_big_o.md`](complexidade_big_o.md). (Complexidade ciclomática é outra métrica: conta os caminhos independentes do código, ou seja, quantos testes são necessários para cobrir todos os desvios.)

### Exemplo 1: Engenharia (Simulação e Controle PID) - Implementado em JavaScript

Na engenharia de software aplicada a hardware/simulação, loops frequentemente representam "timesteps" (passos de tempo) ou ciclos de correção contínua (como um drone tentando estabilizar seu voo).

```javascript
let erro_inclinacao = 15; // Graus fora do eixo
const taxa_correcao = 2.5; // Potência do motor

// O loop while executa enquanto a condição lógica for verdadeira
while (Math.abs(erro_inclinacao) > 0.5) {
    console.log(`Corrigindo inclinação. Erro atual: ${erro_inclinacao}`);
    // Simula a atuação do motor diminuindo o erro
    erro_inclinacao -= taxa_correcao; 
}
console.log("Drone estabilizado.");

```

### Exemplo 2: Ciência da Computação - Implementado em Python

Em algoritmos clássicos, a repetição é usada para busca, ordenação ou travessia de estruturas (como grafos e árvores). A abstração do loop `for` difere entre as linguagens: enquanto C gerencia um contador (i), Python itera diretamente sobre os elementos (Iterators).

```python
# Busca Linear (Computer Science) - O Python abstrai o índice 
# e entrega o valor diretamente a cada repetição através do iterador.

lista_processos = ["PID:204", "PID:442", "PID:900", "PID:102"]
alvo = "PID:900"
encontrado = False

for processo in lista_processos:
    if processo == alvo:
        print("Processo localizado em memória!")
        encontrado = True
        break  # Quebra o loop prematuramente (otimização)

if not encontrado:
    print("Processo inexistente.")

```

# As estruturas de controle em C (resumo)

```c
/* if / else if / else: faixas de alarme (cada teste só olha o limite superior) */
if (temp < 80.0)        estado = NORMAL;
else if (temp < 95.0)   estado = ALERTA;
else                    estado = TRIP;

/* switch: escolha por valor inteiro (código de comando, estado da máquina) */
switch (comando) {
    case 'L': liga_motor();    break;
    case 'D': desliga_motor(); break;   /* sem break, a execução "cai" no próximo case */
    default:  sinaliza_erro(); break;
}

/* for: número de repetições conhecido (n amostras) -> O(n) */
for (int i = 0; i < n; i++) soma += leitura[i];

/* while: repete enquanto um evento não acontece (ciclo de varredura) */
while (!botao_parada) { ler_entradas(); executar_logica(); escrever_saidas(); }

/* do-while: o corpo roda pelo menos uma vez (autoteste na energização) */
do { ok = testa_sensor(); tentativas++; } while (!ok && tentativas < 3);
```

Armadilhas: `if (x = 5)` (atribuição) em vez de `if (x == 5)`; `;` logo depois do `if (...)` ou
do `for (...)`, que vira um corpo vazio; `<=` no lugar de `<` ao percorrer um vetor (off-by-one);
laço `while` cuja condição nunca muda (laço infinito).

Para ver laços executando passo a passo, com teste de mesa: `./run 02_teste_de_mesa_lacos`.
