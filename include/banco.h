/**
 * @file banco.h
 * @brief Banco de perguntas: vetor dinâmico com cadastro completo (CRUD).
 *
 * O vetor cresce e diminui em tempo de execução (malloc/realloc/free).
 * Os índices usados nas funções começam em 0.
 */

#ifndef BANCO_H
#define BANCO_H

#include <stddef.h>
#include "config.h"
#include "pergunta.h"

/**
 * @brief Vetor dinâmico de perguntas.
 */
typedef struct {
    Pergunta *itens;    /**< Vetor alocado dinamicamente (NULL se vazio). */
    size_t quantidade;  /**< Quantas perguntas estão em uso. */
    size_t capacidade;  /**< Quantas perguntas cabem sem realocar. */
} Banco;

/**
 * @brief Deixa o banco vazio e pronto para uso.
 *
 * @param banco Banco a inicializar.
 */
void banco_iniciar(Banco *banco);

/**
 * @brief Libera a memória do vetor e zera o banco.
 *
 * @param banco Banco a liberar (pode já estar vazio).
 */
void banco_liberar(Banco *banco);

/**
 * @brief Obtém uma pergunta pelo índice.
 *
 * @param banco  Banco consultado.
 * @param indice Posição da pergunta (0 a quantidade-1).
 * @return Ponteiro para a pergunta, ou NULL se o índice for inválido.
 */
const Pergunta *banco_obter(const Banco *banco, size_t indice);

/**
 * @brief Insere uma cópia da pergunta no final do banco.
 *
 * Deve aumentar a capacidade com realloc quando o vetor estiver cheio.
 *
 * @param banco    Banco que recebe a pergunta.
 * @param pergunta Pergunta a copiar (deve ser válida).
 * @return STATUS_OK, STATUS_ERRO_PARAMETRO ou STATUS_ERRO_MEMORIA.
 */
Status banco_inserir(Banco *banco, const Pergunta *pergunta);

/**
 * @brief Substitui a pergunta de uma posição.
 *
 * @param banco    Banco alterado.
 * @param indice   Posição a substituir.
 * @param pergunta Novos dados (devem ser válidos).
 * @return STATUS_OK ou STATUS_ERRO_PARAMETRO.
 */
Status banco_alterar(Banco *banco, size_t indice, const Pergunta *pergunta);

/**
 * @brief Remove a pergunta de uma posição, deslocando as seguintes.
 *
 * Pode reduzir a capacidade com realloc quando sobrar muito espaço.
 *
 * @param banco  Banco alterado.
 * @param indice Posição a remover.
 * @return STATUS_OK ou STATUS_ERRO_PARAMETRO.
 */
Status banco_remover(Banco *banco, size_t indice);

/**
 * @brief Procura perguntas cujo enunciado contém um trecho (sem diferenciar maiúsculas).
 *
 * @param banco          Banco consultado.
 * @param trecho         Texto procurado.
 * @param indices        Vetor de saída com as posições encontradas.
 * @param max_indices    Tamanho do vetor de saída (use banco->quantidade).
 * @return Quantidade de perguntas encontradas.
 */
size_t banco_buscar_texto(const Banco *banco, const char *trecho,
                          size_t *indices, size_t max_indices);

/**
 * @brief Coleta as posições das perguntas de um nível de dificuldade.
 *
 * @param banco       Banco consultado.
 * @param nivel       Nível de 1 a 5.
 * @param indices     Vetor de saída com as posições encontradas.
 * @param max_indices Tamanho do vetor de saída (use banco->quantidade).
 * @return Quantidade de perguntas do nível.
 */
size_t banco_filtrar_nivel(const Banco *banco, int nivel,
                           size_t *indices, size_t max_indices);

#endif /* BANCO_H */
