/*
 * DPEE1038 - Estrutura de Dados para Automacao
 * Lista de Exercicios 04 - Filas
 *
 * Duas filas de atendimento: normal e prioritaria.
 * A fila e implementada com um array alocado por malloc. Quando o array
 * fica cheio, insert aumenta sua capacidade automaticamente com realloc.
 * A fila e circular, portanto remover um elemento nao exige deslocar todos
 * os demais elementos.
 *
 * Compilacao:
 *   gcc -std=c11 -Wall -Wextra -pedantic 01_fila_atendimento.c -o 01_fila_atendimento
 * Execucao:
 *   ./01_fila_atendimento
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    unsigned long senha;
} Cliente;

typedef struct {
    Cliente *dados;
    size_t capacidade;
    size_t inicio; /* indice do primeiro elemento */
    size_t tamanho;
} Fila;

/* Cria a fila com N posicoes, conforme solicitado no enunciado. */
int fila_cria(Fila *fila, size_t capacidade_inicial) {
    if (capacidade_inicial == 0)
        capacidade_inicial = 1;

    fila->dados = malloc(capacidade_inicial * sizeof *fila->dados);
    if (fila->dados == NULL)
        return 0;

    fila->capacidade = capacidade_inicial;
    fila->inicio = 0;
    fila->tamanho = 0;
    return 1;
}

int fila_vazia(const Fila *fila) {
    return fila->tamanho == 0;
}

/*
 * Aumenta a fila. A copia para um array temporario preserva a ordem logica
 * mesmo quando os elementos estao divididos no fim e no inicio do array
 * circular. A memoria principal da fila e redimensionada com realloc.
 */
int fila_aumenta(Fila *fila) {
    size_t nova_capacidade = fila->capacidade <= SIZE_MAX / 2
                           ? fila->capacidade * 2
                           : fila->capacidade + 1;
    Cliente *temporario = malloc(fila->tamanho * sizeof *temporario);
    if (temporario == NULL && fila->tamanho != 0)
        return 0;

    for (size_t i = 0; i < fila->tamanho; i++) {
        size_t indice = (fila->inicio + i) % fila->capacidade;
        temporario[i] = fila->dados[indice];
    }

    Cliente *redimensionado = realloc(
        fila->dados, nova_capacidade * sizeof *redimensionado);
    if (redimensionado == NULL) {
        free(temporario);
        return 0;
    }

    fila->dados = redimensionado;
    if (fila->tamanho != 0)
        memcpy(fila->dados, temporario, fila->tamanho * sizeof *fila->dados);
    free(temporario);
    fila->capacidade = nova_capacidade;
    fila->inicio = 0;
    return 1;
}

/* insert insere no fim e aumenta a capacidade automaticamente se necessario. */
int insert(Fila *fila, Cliente cliente) {
    if (fila->tamanho == fila->capacidade && !fila_aumenta(fila))
        return 0;

    size_t fim = (fila->inicio + fila->tamanho) % fila->capacidade;
    fila->dados[fim] = cliente;
    fila->tamanho++;
    return 1;
}

int remove_cliente(Fila *fila, Cliente *cliente) {
    if (fila_vazia(fila))
        return 0;

    *cliente = fila->dados[fila->inicio];
    fila->inicio = (fila->inicio + 1) % fila->capacidade;
    fila->tamanho--;
    if (fila->tamanho == 0)
        fila->inicio = 0;
    return 1;
}

void fila_mostra(const Fila *fila, const char *nome) {
    printf("%s: %zu cliente(s), capacidade %zu\n", nome,
           fila->tamanho, fila->capacidade);
    for (size_t i = 0; i < fila->tamanho; i++) {
        size_t indice = (fila->inicio + i) % fila->capacidade;
        printf("  [%zu] senha %lu\n", i + 1, fila->dados[indice].senha);
    }
}

void fila_libera(Fila *fila) {
    free(fila->dados);
    fila->dados = NULL;
    fila->capacidade = 0;
    fila->inicio = 0;
    fila->tamanho = 0;
}

/*
 * Regra de atendimento: no maximo cinco clientes prioritarios consecutivos;
 * depois, se houver cliente normal, atende-se um cliente normal. Se uma fila
 * estiver vazia, a outra e atendida para que o guiche nao fique parado.
 */
int atende_proximo(Fila *normal, Fila *prioritaria, unsigned *prioritarios_seguidos) {
    Cliente cliente;
    int atende_prioridade;

    if (fila_vazia(prioritaria))
        atende_prioridade = 0;
    else if (fila_vazia(normal))
        atende_prioridade = 1;
    else
        atende_prioridade = *prioritarios_seguidos < 5;

    if (atende_prioridade) {
        remove_cliente(prioritaria, &cliente);
        (*prioritarios_seguidos)++;
        printf("Atendendo senha %lu (fila prioritaria).\n", cliente.senha);
    } else {
        remove_cliente(normal, &cliente);
        *prioritarios_seguidos = 0;
        printf("Atendendo senha %lu (fila normal).\n", cliente.senha);
    }
    return 1;
}

int le_tamanho_inicial(void) {
    int n;
    do {
        printf("Informe N (capacidade inicial de cada fila): ");
        if (scanf("%d", &n) != 1) {
            int caractere;
            while ((caractere = getchar()) != '\n' && caractere != EOF)
                ;
            n = 0;
        }
        if (n <= 0)
            printf("N deve ser positivo.\n");
    } while (n <= 0);
    return n;
}

int main(void) {
    Fila normal, prioritaria;
    unsigned long proxima_normal = 1;
    unsigned long proxima_prioritaria = 1;
    unsigned prioritarios_seguidos = 0;
    int n = le_tamanho_inicial();
    int opcao;

    if (!fila_cria(&normal, (size_t)n) ||
        !fila_cria(&prioritaria, (size_t)n)) {
        fprintf(stderr, "Erro: memoria insuficiente para criar as filas.\n");
        fila_libera(&normal);
        fila_libera(&prioritaria);
        return EXIT_FAILURE;
    }

    do {
        printf("\n=== Sistema de atendimento ===\n"
               "1 - Chegada normal\n"
               "2 - Chegada prioritaria\n"
               "3 - Atender proximo\n"
               "4 - Mostrar filas\n"
               "0 - Sair\n"
               "Opcao: ");
        if (scanf("%d", &opcao) != 1) {
            int caractere;
            while ((caractere = getchar()) != '\n' && caractere != EOF)
                ;
            opcao = -1;
        }

        if (opcao == 1) {
            Cliente cliente = {proxima_normal++};
            if (insert(&normal, cliente))
                printf("Senha normal %lu inserida.\n", cliente.senha);
            else
                printf("Nao foi possivel inserir: memoria insuficiente.\n");
        } else if (opcao == 2) {
            Cliente cliente = {proxima_prioritaria++};
            if (insert(&prioritaria, cliente))
                printf("Senha prioritaria %lu inserida.\n", cliente.senha);
            else
                printf("Nao foi possivel inserir: memoria insuficiente.\n");
        } else if (opcao == 3) {
            if (!atende_proximo(&normal, &prioritaria, &prioritarios_seguidos))
                printf("Nao ha clientes aguardando.\n");
        } else if (opcao == 4) {
            fila_mostra(&normal, "Fila normal");
            fila_mostra(&prioritaria, "Fila prioritaria");
        } else if (opcao != 0) {
            printf("Opcao invalida.\n");
        }
    } while (opcao != 0);

    fila_libera(&normal);
    fila_libera(&prioritaria);
    return EXIT_SUCCESS;
}
