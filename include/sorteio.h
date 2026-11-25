/**
 * @file sorteio.h
 * @brief Sorteia as perguntas da partida por nível e embaralha as alternativas (rand/srand).
 *
 * Chamado por partida_iniciar (sorteio_montar_partida). Sem raylib; nunca altera o Banco.
 */
#ifndef SORTEIO_H
#define SORTEIO_H

#include <stddef.h>
#include "config.h"
#include "banco.h"
#include "pergunta.h"

/**
 * @brief Pergunta pronta para a partida, com as alternativas embaralhadas.
 *        `origem` aponta para dentro do Banco: ele não pode mudar durante a partida.
 */
typedef struct {
    const Pergunta *origem;              /**< Pergunta original (banco_obter). */
    int ordem[NUM_ALTERNATIVAS];         /**< ordem[i] = alternativa original mostrada na posição i. */
    int posicao_correta;                 /**< Posição (0 a 3) da correta depois de embaralhar. */
} PerguntaSorteada;

/** @brief Semeia o gerador (jogo_executar usa time(NULL)). */
void sorteio_semear(unsigned int semente);

/** @brief Inteiro sorteado em [minimo, maximo]; devolve minimo se minimo > maximo. */
int sorteio_inteiro(int minimo, int maximo);

/** @brief Embaralha um vetor de índices do Banco (Fisher-Yates); ignora NULL. */
void sorteio_embaralhar_vetor(size_t *vetor, size_t tamanho);

/**
 * @brief Embaralha as alternativas e preenche origem, ordem e posicao_correta de `saida`.
 * @return STATUS_OK ou STATUS_ERRO_PARAMETRO (NULL ou pergunta inválida).
 */
Status sorteio_alternativas(const Pergunta *pergunta, PerguntaSorteada *saida);

/**
 * @brief Sorteia `quantidade` perguntas distintas do nível (banco_filtrar_nivel + embaralhar).
 * @param indices_saida Recebe `quantidade` índices do Banco.
 * @return STATUS_OK; STATUS_ERRO_PARAMETRO; STATUS_ERRO_MEMORIA;
 *         STATUS_NAO_ENCONTRADO (o nível tem perguntas de menos).
 */
Status sorteio_perguntas_do_nivel(const Banco *banco, int nivel, size_t quantidade, size_t *indices_saida);

/**
 * @brief Monta as 15 perguntas em ordem crescente de nível (sorteio_perguntas_do_nivel +
 *        sorteio_alternativas). A distribuição vem de partida.h por parâmetro.
 * @param por_nivel Perguntas de cada nível (soma = PERGUNTAS_POR_PARTIDA).
 * @param saida     Só é escrita se tudo deu certo.
 * @return STATUS_OK; STATUS_ERRO_PARAMETRO; STATUS_NAO_ENCONTRADO; STATUS_ERRO_MEMORIA.
 */
Status sorteio_montar_partida(const Banco *banco, const int por_nivel[NUM_NIVEIS],
                              PerguntaSorteada saida[PERGUNTAS_POR_PARTIDA]);

/** @brief Texto da alternativa mostrada em `posicao` (0 a 3); "" se inválida, nunca NULL. */
const char *sorteio_texto_alternativa(const PerguntaSorteada *sorteada, int posicao);

#endif /* SORTEIO_H */
