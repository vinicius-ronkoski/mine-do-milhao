/**
 * @file boss.h
 * @brief Estrutura básica de BOSS: um por nível de dificuldade, perde vida a cada acerto.
 *
 * NÃO depende de raylib. O sprite/animação do boss é escolhido em assets.h
 * (assets_imagem_do_boss), usando o BossId daqui.
 */
#ifndef BOSS_H
#define BOSS_H

#include "config.h"

/** Bosses, na ordem dos níveis 1 a 5. Para trocar/adicionar boss: edite aqui E em assets.h. */
typedef enum {
    BOSS_CREEPER = 0,   /**< Nível 1 (muito fácil). */
    BOSS_WITHER,        /**< Nível 2 (fácil). */
    BOSS_WARDEN,        /**< Nível 3 (médio). */
    BOSS_ENDER_DRAGON,  /**< Nível 4 (difícil). */
    BOSS_HEROBRINE,     /**< Nível 5 (muito difícil). */
    BOSS_QUANTIDADE
} BossId;

/* Falha de compilação se NUM_NIVEIS (config.h) e a quantidade de bosses divergirem. */
typedef char boss_verifica_quantidade[(BOSS_QUANTIDADE == NUM_NIVEIS) ? 1 : -1];

/** Estado de um boss durante a partida. */
typedef struct {
    BossId id;          /**< Qual boss é. */
    int nivel;          /**< Nível de dificuldade que ele representa. */
    int vida_maxima;    /**< = quantidade de perguntas do nível na partida (partida_perguntas_no_nivel). */
    int vida_atual;     /**< 0 = derrotado. */
} Boss;

/**
 * @brief Inicializa um boss.
 * @return STATUS_OK ou STATUS_ERRO_PARAMETRO (NULL, id inválido ou vida_maxima <= 0).
 */
Status boss_iniciar(Boss *boss, BossId id, int vida_maxima);

/** @brief Boss associado a um nível (NIVEL_MIN..NIVEL_MAX). Fora do intervalo: BOSS_CREEPER. */
BossId boss_id_do_nivel(int nivel);

/** @brief Nome exibível ("Creeper", "Wither"...). Nunca NULL ("Desconhecido" se inválido). */
const char *boss_nome(BossId id);

/**
 * @brief Tira vida do boss (nunca abaixo de 0).
 * @param dano Normalmente 1 (um acerto = um ponto de dano).
 * @return STATUS_OK ou STATUS_ERRO_PARAMETRO.
 */
Status boss_receber_dano(Boss *boss, int dano);

/** @brief 1 se vida_atual <= 0; 0 caso contrário (inclusive NULL). */
int boss_derrotado(const Boss *boss);

/** @brief Fração de vida restante de 0.0 a 1.0 (para a barra de vida na tela). */
int boss_calculo_vida(const Boss *boss);

#endif /* BOSS_H */
