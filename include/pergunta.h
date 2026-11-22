/**
 * @file pergunta.h
 * @brief Estrutura Pergunta e operações sobre uma única pergunta.
 *
 * Os campos de texto usam vetores de tamanho fixo (e não ponteiros) para que
 * a estrutura possa ser gravada direto no arquivo binário com fwrite().
 */

#ifndef PERGUNTA_H
#define PERGUNTA_H

#include "config.h"

/**
 * @brief Uma pergunta do Jogo do Milhão.
 */
typedef struct {
    char enunciado[TAM_ENUNCIADO];                         /**< Texto da pergunta. */
    char alternativas[NUM_ALTERNATIVAS][TAM_ALTERNATIVA];  /**< Alternativas A a D. */
    char correta;                                          /**< Letra correta: 'A', 'B', 'C' ou 'D'. */
    int  nivel;                                            /**< Dificuldade, de NIVEL_MIN a NIVEL_MAX. */
    char dica[TAM_DICA];                                   /**< Dica única da pergunta (usada na parte criativa). */
} Pergunta;

/**
 * @brief Devolve o nome do nível de dificuldade.
 *
 * @param nivel Nível de 1 a 5.
 * @return "Muito fácil", "Fácil", "Médio", "Difícil" ou "Muito difícil";
 *         "Inválido" se o nível estiver fora do intervalo.
 */
const char *pergunta_nome_nivel(int nivel);

/**
 * @brief Verifica se todos os campos da pergunta estão coerentes.
 *
 * Confere enunciado e alternativas não vazios, letra correta entre A e D e
 * nível entre NIVEL_MIN e NIVEL_MAX.
 *
 * @param pergunta Pergunta a validar.
 * @return STATUS_OK (0) se a pergunta é válida; STATUS_ERRO_PARAMETRO se
 *         algum campo estiver inválido (inclusive se pergunta for NULL).
 */
Status pergunta_validar(const Pergunta *pergunta);

/**
 * @brief Imprime a pergunta formatada e colorida no terminal.
 *
 * @param pergunta Pergunta a exibir.
 */
void pergunta_imprimir(const Pergunta *pergunta);

#endif /* PERGUNTA_H */
