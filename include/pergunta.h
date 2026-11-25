/**
 * @file pergunta.h
 * @brief Estrutura Pergunta e operações sobre uma pergunta.
 *
 * Textos em vetores fixos (não ponteiros) para a struct ir direto ao .dat com fwrite().
 */

#ifndef PERGUNTA_H
#define PERGUNTA_H

#include "config.h"

/** @brief Uma pergunta do jogo (uma linha do CSV). */
typedef struct {
    char enunciado[TAM_ENUNCIADO];                         /**< Texto da pergunta. */
    char alternativas[NUM_ALTERNATIVAS][TAM_ALTERNATIVA];  /**< Alternativas A a D. */
    char correta;                                          /**< 'A', 'B', 'C' ou 'D'. */
    int  nivel;                                            /**< NIVEL_MIN a NIVEL_MAX. */
    char dica[TAM_DICA];                                   /**< Dica comprada com esmeraldas no jogo. */
} Pergunta;

/** @brief Nome do nível ("Muito fácil" ... "Muito difícil"); "Inválido" fora de 1 a 5. */
const char *pergunta_nome_nivel(int nivel);

/**
 * @brief Confere textos não vazios, letra correta A-D e nível válido (usada por banco_inserir/alterar).
 * @return STATUS_OK ou STATUS_ERRO_PARAMETRO (inclusive se NULL).
 */
Status pergunta_validar(const Pergunta *pergunta);

/** @brief Imprime a pergunta formatada e colorida no terminal (menu). */
void pergunta_imprimir(const Pergunta *pergunta);

#endif /* PERGUNTA_H */
