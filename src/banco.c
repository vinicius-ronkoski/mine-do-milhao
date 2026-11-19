/**
 * @file banco.c
 * @author Paulo Valadares (paulovaladares@alunos.utfpr.edu.br)
 * @brief implementação do banco de perguntas (vetor dinâmico com cadastro completo)
 * @version 0.1
 * @date 2026-11-18
 *
 * @copyright Copyright (c) 2026
 *
 */
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "banco.h"

#define PASSO_DE_CRESCIMENTO 10

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
        size_t nova_capacidade = banco->capacidade + PASSO_DE_CRESCIMENTO;

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

    //puxando cada pergunta seguinte uma posição para trás
    for (size_t i = indice; i + 1 < banco->quantidade; i++) {
        banco->itens[i] = banco->itens[i + 1];
    }
    banco->quantidade--;

    return STATUS_OK;
}

size_t banco_buscar_texto(const Banco *banco, const char *trecho,
                          size_t *indices, size_t max_indices) {
    char trecho_minusculo[TAM_ENUNCIADO];
    char enunciado_minusculo[TAM_ENUNCIADO];
    size_t tamanho_do_trecho = 0;
    size_t quantidade_de_encontradas = 0;

    //verificando os parâmetros
    if (banco == NULL || trecho == NULL || indices == NULL) {
        return 0;
    }

    //passando o trecho para minúsculo
    while (trecho[tamanho_do_trecho] != '\0' && tamanho_do_trecho < TAM_ENUNCIADO - 1) {
        trecho_minusculo[tamanho_do_trecho] = (char)tolower((unsigned char)trecho[tamanho_do_trecho]);
        tamanho_do_trecho++;
    }
    trecho_minusculo[tamanho_do_trecho] = '\0';

    //percorrendo o banco e guardando as posições que contêm o trecho
    for (size_t i = 0; i < banco->quantidade && quantidade_de_encontradas < max_indices; i++) {
        size_t tamanho_do_enunciado = 0;

        //passando o enunciado para minúsculo
        while (banco->itens[i].enunciado[tamanho_do_enunciado] != '\0' &&
               tamanho_do_enunciado < TAM_ENUNCIADO - 1) {
            enunciado_minusculo[tamanho_do_enunciado] =
                (char)tolower((unsigned char)banco->itens[i].enunciado[tamanho_do_enunciado]);
            tamanho_do_enunciado++;
        }
        enunciado_minusculo[tamanho_do_enunciado] = '\0';

        //procurando o trecho dentro do enunciado
        if (strstr(enunciado_minusculo, trecho_minusculo) != NULL) {
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
