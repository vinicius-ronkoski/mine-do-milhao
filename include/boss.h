/**
 * @file boss.h
 * @brief Um boss por nível, que perde 1 de vida a cada acerto (partida_responder).
 *
 * Sem raylib: o desenho fica em assets_desenhar_boss e tela_desenhar_boss, pelo BossId.
 */
#ifndef BOSS_H
#define BOSS_H

#include "config.h"

/** Bosses na ordem dos níveis 1 a 5 (trocar aqui exige trocar também em assets.c). */
typedef enum {
    BOSS_CREEPER = 0,   /**< Nível 1 (muito fácil). */
    BOSS_WITHER,        /**< Nível 2 (fácil). */
    BOSS_WARDEN,        /**< Nível 3 (médio). */
    BOSS_ENDER_DRAGON,  /**< Nível 4 (difícil). */
    BOSS_HEROBRINE,     /**< Nível 5 (muito difícil). */
    BOSS_QUANTIDADE     /**< Quantidade de bosses; também é o valor de "boss inválido". */
} BossId;

/* Erro de compilação se NUM_NIVEIS e a quantidade de bosses divergirem. */
typedef char boss_verifica_quantidade[(BOSS_QUANTIDADE == NUM_NIVEIS) ? 1 : -1];

/** Estado de um boss durante a partida. */
typedef struct {
    BossId id;          /**< Qual boss é. */
    int nivel;          /**< Nível de dificuldade que ele representa. */
    int vida_maxima;    /**< Perguntas do nível na partida (partida_perguntas_no_nivel). */
    int vida_atual;     /**< 0 = derrotado. */
} Boss;

/**
 * @brief Inicializa um boss com vida cheia (chamado por partida_iniciar).
 * @return STATUS_OK ou STATUS_ERRO_PARAMETRO (NULL, id inválido ou vida_maxima <= 0).
 */
Status boss_iniciar(Boss *boss, BossId id, int vida_maxima);

/** @brief BossId do nível; BOSS_QUANTIDADE se inválido (boss_iniciar acusa o erro). */
BossId boss_id_do_nivel(int nivel);

/** @brief Nome exibível ("Creeper", "Wither"...). Nunca NULL ("Desconhecido" se inválido). */
const char *boss_nome(BossId id);

/**
 * @brief Tira `dano` de vida (nunca abaixo de 0); um acerto = 1.
 * @return STATUS_OK ou STATUS_ERRO_PARAMETRO.
 */
Status boss_receber_dano(Boss *boss, int dano);

/** @brief true se a vida chegou a 0 (false se NULL). */
bool boss_derrotado(const Boss *boss);

/** @brief Vida restante de 0.0 a 1.0 para a barra (tela_desenhar_barra_vida); 0.0 se inválido. */
float boss_calculo_vida(const Boss *boss);

#endif /* BOSS_H */
