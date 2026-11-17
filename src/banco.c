/**
 * @file banco.c
 * @author Paulo Valadares (paulovaladares@alunos.utfpr.edu.br)
 * @brief implementação do banco de perguntas (vetor dinâmico com cadastro completo)
 * @version 0.1
 * @date 2026-11-17
 *
 * @copyright Copyright (c) 2026
 *
 */
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "banco.h"

#define CAPACIDADE_INICIAL 8
#define FATOR_DE_CRESCIMENTO 2
#define FATOR_PARA_ENCOLHER 4

/**
 * @brief verifica se o texto contém o trecho, sem diferenciar maiúsculas de minúsculas
 *
 * @param texto texto onde vamos procurar
 * @param trecho texto procurado
 * @return 1 se o trecho aparece no texto, 0 caso contrário
 */
static int texto_contem_trecho(const char *texto, const char *trecho) {
    //pegando o tamanho dos dois textos
    size_t tamanho_do_texto = strlen(texto);
    size_t tamanho_do_trecho = strlen(trecho);

    //descartando quando o trecho é maior que o texto
    if (tamanho_do_trecho > tamanho_do_texto) {
        return 0;
    }

    //testando o trecho em cada posição possível do texto
    for (size_t posicao_inicial = 0; posicao_inicial <= tamanho_do_texto - tamanho_do_trecho; posicao_inicial++) {
        size_t quantidade_de_letras_iguais = 0;

        //comparando letra por letra ignorando maiúsculas
        while (quantidade_de_letras_iguais < tamanho_do_trecho &&
               tolower((unsigned char)texto[posicao_inicial + quantidade_de_letras_iguais]) ==
               tolower((unsigned char)trecho[quantidade_de_letras_iguais])) {
            quantidade_de_letras_iguais++;
        }

        //achando o trecho inteiro
        if (quantidade_de_letras_iguais == tamanho_do_trecho) {
            return 1;
        }
    }

    return 0;
}

void banco_iniciar(Banco *banco) {
    //verificando se o ponteiro é válido
    if (banco == NULL) {
        return;
    }

    //deixando o banco vazio
    banco->itens = NULL;
    banco->quantidade = 0;
    banco->capacidade = 0;
}

void banco_liberar(Banco *banco) {
    //verificando se o ponteiro é válido
    if (banco == NULL) {
        return;
    }

    //liberando o vetor (free(NULL) não faz nada) e zerando o banco
    free(banco->itens);
    banco->itens = NULL;
    banco->quantidade = 0;
    banco->capacidade = 0;
}

const Pergunta *banco_obter(const Banco *banco, size_t indice) {
    //verificando o ponteiro e se o índice existe
    if (banco == NULL || indice >= banco->quantidade) {
        return NULL;
    }

    return &banco->itens[indice];
}

Status banco_inserir(Banco *banco, const Pergunta *pergunta) {
    //verificando os parâmetros e se a pergunta é válida
    if (banco == NULL || pergunta == NULL || pergunta_validar(pergunta) != STATUS_OK) {
        return STATUS_ERRO_PARAMETRO;
    }

    //aumentando a capacidade quando o vetor está cheio
    if (banco->quantidade == banco->capacidade) {
        size_t nova_capacidade;
        if (banco->capacidade == 0) {
            nova_capacidade = CAPACIDADE_INICIAL;
        } else {
            nova_capacidade = banco->capacidade * FATOR_DE_CRESCIMENTO;
        }

        //guardando o resultado em outro ponteiro para não perder o vetor se o realloc falhar
        Pergunta *novo_vetor = realloc(banco->itens, nova_capacidade * sizeof(Pergunta));
        if (novo_vetor == NULL) {
            return STATUS_ERRO_MEMORIA;
        }

        banco->itens = novo_vetor;
        banco->capacidade = nova_capacidade;
    }

    //copiando a pergunta para o final do vetor
    banco->itens[banco->quantidade] = *pergunta;
    banco->quantidade++;

    return STATUS_OK;
}

Status banco_alterar(Banco *banco, size_t indice, const Pergunta *pergunta) {
    //verificando os parâmetros, o índice e se a pergunta é válida
    if (banco == NULL || pergunta == NULL || indice >= banco->quantidade ||
        pergunta_validar(pergunta) != STATUS_OK) {
        return STATUS_ERRO_PARAMETRO;
    }

    //substituindo a pergunta da posição
    banco->itens[indice] = *pergunta;

    return STATUS_OK;
}

Status banco_remover(Banco *banco, size_t indice) {
    //verificando o ponteiro e se o índice existe
    if (banco == NULL || indice >= banco->quantidade) {
        return STATUS_ERRO_PARAMETRO;
    }

    //deslocando as perguntas seguintes uma posição para trás
    size_t quantidade_de_perguntas_depois = banco->quantidade - indice - 1;
    memmove(&banco->itens[indice], &banco->itens[indice + 1],
            quantidade_de_perguntas_depois * sizeof(Pergunta));
    banco->quantidade--;

    //encolhendo o vetor pela metade quando sobra muito espaço
    if (banco->capacidade > CAPACIDADE_INICIAL &&
        banco->quantidade <= banco->capacidade / FATOR_PARA_ENCOLHER) {
        size_t nova_capacidade = banco->capacidade / FATOR_DE_CRESCIMENTO;
        Pergunta *novo_vetor = realloc(banco->itens, nova_capacidade * sizeof(Pergunta));

        //se o realloc falhar, continuamos com o vetor antigo (não é um erro)
        if (novo_vetor != NULL) {
            banco->itens = novo_vetor;
            banco->capacidade = nova_capacidade;
        }
    }

    return STATUS_OK;
}

size_t banco_buscar_texto(const Banco *banco, const char *trecho,
                          size_t *indices, size_t max_indices) {
    //verificando os parâmetros
    if (banco == NULL || trecho == NULL || indices == NULL) {
        return 0;
    }

    //percorrendo o banco e guardando as posições que contêm o trecho
    size_t quantidade_de_encontradas = 0;
    for (size_t i = 0; i < banco->quantidade && quantidade_de_encontradas < max_indices; i++) {
        if (texto_contem_trecho(banco->itens[i].enunciado, trecho)) {
            indices[quantidade_de_encontradas] = i;
            quantidade_de_encontradas++;
        }
    }

    return quantidade_de_encontradas;
}

size_t banco_filtrar_nivel(const Banco *banco, int nivel,
                           size_t *indices, size_t max_indices) {
    //verificando os parâmetros
    if (banco == NULL || indices == NULL) {
        return 0;
    }

    //percorrendo o banco e guardando as posições do nível pedido
    size_t quantidade_de_encontradas = 0;
    for (size_t i = 0; i < banco->quantidade && quantidade_de_encontradas < max_indices; i++) {
        if (banco->itens[i].nivel == nivel) {
            indices[quantidade_de_encontradas] = i;
            quantidade_de_encontradas++;
        }
    }

    return quantidade_de_encontradas;
}
