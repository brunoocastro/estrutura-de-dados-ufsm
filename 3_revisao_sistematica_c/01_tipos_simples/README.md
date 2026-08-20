# Tipos simples de C

Esta pasta detalha os tipos escalares e operações comuns em C. Cada arquivo é
um programa independente, com comentários no código e saída demonstrando valores,
tamanhos e, quando relevante, endereços de memória.

## Arquivos

| Arquivo | Conteúdo |
| --- | --- |
| `00_visao_geral.c` | visão geral de tipos, modificadores, `sizeof` e endereços |
| `01_char.c` | caracteres, valores numéricos e sequências de escape |
| `02_inteiros.c` | `int`, `short`, `long`, `long long`, `signed`, `unsigned` e limites |
| `03_reais.c` | `float`, `double`, `long double`, precisão e limites |
| `04_conversoes.c` | divisão inteira, *casts*, promoções e truncamento |
| `05_constantes.c` | `const`, `#define` e constantes inteiras nomeadas |

## Como usar o GCC

`gcc` é o compilador da GNU Compiler Collection. A forma geral de compilar um
arquivo C é:

```bash
gcc [opcoes] arquivo.c -o nome_do_executavel
```

Nos exemplos desta pasta usamos:

- `-std=c11`: seleciona o padrão C11 da linguagem.
- `-Wall`: ativa a maioria dos avisos importantes.
- `-Wextra`: ativa avisos adicionais para encontrar código suspeito.
- `-pedantic`: solicita conformidade mais rigorosa com o padrão C11.
- `-o nome`: escolhe o nome do executável; sem essa opção, o GCC usa `a.out`.

Os avisos ajudam a detectar erros, conversões inesperadas e problemas de
portabilidade antes da execução. Por isso, mantemos essas três opções de aviso
em todos os comandos do material.

Exemplo completo:

```bash
gcc -std=c11 -Wall -Wextra -pedantic 01_char.c -o 01_char
./01_char
```

O primeiro comando compila e o segundo executa. O prefixo `./` significa que o
programa será procurado no diretório atual.

## Compilar e executar um exemplo

Na raiz do repositório, copie e cole o bloco correspondente:

```bash
cd 3_revisao_sistematica_c/01_tipos_simples
gcc -std=c11 -Wall -Wextra -pedantic 01_char.c -o 01_char
./01_char
```

```bash
cd 3_revisao_sistematica_c/01_tipos_simples
gcc -std=c11 -Wall -Wextra -pedantic 02_inteiros.c -o 02_inteiros
./02_inteiros
```

```bash
cd 3_revisao_sistematica_c/01_tipos_simples
gcc -std=c11 -Wall -Wextra -pedantic 03_reais.c -o 03_reais
./03_reais
```

```bash
cd 3_revisao_sistematica_c/01_tipos_simples
gcc -std=c11 -Wall -Wextra -pedantic 04_conversoes.c -o 04_conversoes
./04_conversoes
```

```bash
cd 3_revisao_sistematica_c/01_tipos_simples
gcc -std=c11 -Wall -Wextra -pedantic 05_constantes.c -o 05_constantes
./05_constantes
```

## Compilar todos os exemplos

```bash
cd 3_revisao_sistematica_c/01_tipos_simples
for arquivo in *.c; do
    executavel="${arquivo%.c}"
    gcc -std=c11 -Wall -Wextra -pedantic "$arquivo" -o "$executavel" || exit 1
done
```

Executar todos os programas compilados:

```bash
cd 3_revisao_sistematica_c/01_tipos_simples
for executavel in 00_visao_geral 01_char 02_inteiros 03_reais 04_conversoes 05_constantes; do
    echo "=== $executavel ==="
    ./$executavel
done
```

## Ideias fundamentais

- `char` armazena um caractere, mas também pode participar de operações inteiras.
- Tipos inteiros diferem em tamanho e faixa; `unsigned` remove valores negativos e
  aumenta a faixa positiva, mas não torna o tipo necessariamente maior.
- `float`, `double` e `long double` representam números reais aproximados.
- Literais podem ter sufixos: `f` para `float`, `L` para `long` e `U` para
  `unsigned`. Combinações comuns são `UL`, `LL` e `ULL`.
- `sizeof` informa o tamanho de um tipo ou objeto na implementação atual.
- Conversões podem provocar truncamento, perda de precisão ou resultados inesperados.
- `const` protege uma variável contra alterações depois da inicialização.
