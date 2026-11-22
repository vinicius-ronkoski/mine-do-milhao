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
#include <stdlib.h>
#include <string.h>
#include "banco.h"

#define CAPACIDADE_INICIAL 8
#define FATOR_DE_CRESCIMENTO 2
#define FATOR_PARA_ENCOLHER 4

/**
 * @brief converte uma letra ASCII maiúscula em minúscula (os outros bytes ficam iguais)
 *
 * Não usamos o tolower() porque ele depende da configuração de idioma do sistema.
 *
 * @param letra byte a converter
 * @return o byte em minúsculo
 */
static unsigned char minuscula_ascii(unsigned char letra) {
    //somando 32 só nas letras de A a Z
    if (letra >= 'A' && letra <= 'Z') {
        return (unsigned char)(letra + 32);
    }

    return letra;
}

/**
 * @brief converte o segundo byte de uma letra acentuada do UTF-8 para minúsculo
 *
 * No UTF-8, as letras de À a Þ ocupam 2 bytes: o primeiro é sempre 0xC3 e o segundo
 * vai de 0x80 a 0x9E (menos o 0x97, que é o sinal de multiplicação). A minúscula tem o
 * segundo byte 0x20 maior (É = C3 89, é = C3 A9).
 *
 * @param segundo_byte segundo byte da letra
 * @return o segundo byte da letra em minúsculo
 */
static unsigned char segundo_byte_em_minusculo(unsigned char segundo_byte) {
    //somando 0x20 só nas letras maiúsculas acentuadas
    if (segundo_byte >= 0x80 && segundo_byte <= 0x9E && segundo_byte != 0x97) {
        return (unsigned char)(segundo_byte + 0x20);
    }

    return segundo_byte;
}

/**
 * @brief compara um caractere de cada texto sem diferenciar maiúsculas de minúsculas
 *
 * @param primeiro_texto início do caractere do primeiro texto (não pode ser '\0')
 * @param segundo_texto início do caractere do segundo texto (não pode ser '\0')
 * @param bytes_do_caractere recebe quantos bytes foram comparados (1 ou 2)
 * @return true se os caracteres são iguais, false caso contrário
 */
static bool caracteres_iguais(const char *primeiro_texto, const char *segundo_texto, size_t *bytes_do_caractere) {
    unsigned char primeiro_byte_do_primeiro = (unsigned char)primeiro_texto[0];
    unsigned char primeiro_byte_do_segundo = (unsigned char)segundo_texto[0];

    //tratando as letras acentuadas, que ocupam 2 bytes
    if (primeiro_byte_do_primeiro == 0xC3 && primeiro_byte_do_segundo == 0xC3 &&
        primeiro_texto[1] != '\0' && segundo_texto[1] != '\0') {
        *bytes_do_caractere = 2;
        return segundo_byte_em_minusculo((unsigned char)primeiro_texto[1]) ==
               segundo_byte_em_minusculo((unsigned char)segundo_texto[1]);
    }

    //tratando os outros caracteres byte a byte
    *bytes_do_caractere = 1;
    return minuscula_ascii(primeiro_byte_do_primeiro) == minuscula_ascii(primeiro_byte_do_segundo);
}

/**
 * @brief verifica se o texto contém o trecho, sem diferenciar maiúsculas de minúsculas
 *
 * Funciona com letras acentuadas em UTF-8 ("médio" acha "MÉDIO"), mas não ignora o acento:
 * "medio" não acha "médio".
 *
 * @param texto texto onde vamos procurar
 * @param trecho texto procurado
 * @return true se o trecho aparece no texto, false caso contrário
 */
static bool texto_contem_trecho(const char *texto, const char *trecho) {
    //aceitando o trecho vazio (ele aparece em qualquer texto)
    if (trecho[0] == '\0') {
        return true;
    }

    //testando o trecho a partir de cada posição do texto
    for (size_t posicao_inicial = 0; texto[posicao_inicial] != '\0'; posicao_inicial++) {
        size_t deslocamento = 0;

        //comparando caractere por caractere até acabar o trecho ou o texto
        while (trecho[deslocamento] != '\0' && texto[posicao_inicial + deslocamento] != '\0') {
            size_t bytes_do_caractere = 0;

            if (!caracteres_iguais(&texto[posicao_inicial + deslocamento], &trecho[deslocamento],
                                   &bytes_do_caractere)) {
                break;
            }

            deslocamento += bytes_do_caractere;
        }

        //achando o trecho inteiro
        if (trecho[deslocamento] == '\0') {
            return true;
        }
    }

    return false;
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

    //liberando o vetor e zerando o banco
    free(banco->itens); // Libera as perguntas
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
