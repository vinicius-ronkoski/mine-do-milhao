/**
 * @file historia.h
 * @brief Conteúdo da história: as falas de cada cena e as reações do apresentador.
 *
 * Cada CenaId é um `case` de historia_anexar_cena (historia.c), escrito com as macros:
 * FALA(...) adiciona uma fala; CENARIO(IMG_X) troca o fundo das falas seguintes (sem o boss);
 * RETRATO(IMG_X) troca o retrato; SOM(SOM_X) toca na próxima fala; MUSICA(MUSICA_X) troca a música
 * na próxima fala. As cenas são tocadas por jogo_tocar_cena/jogo_anexar_cena (jogo.c), chamadas em
 * jogo_nova_partida e tela_revelacao_atualizar. Quem mostra as falas é o motor de dialogo.h.
 * Só compila com USE_RAYLIB=1.
 */
#ifndef HISTORIA_H
#define HISTORIA_H

#include "config.h"
#include "boss.h"
#include "dialogo.h"

/** Variações de reação para acerto e para erro (historia_anexar_reacao). */
#define REACOES_POR_RESULTADO 4

/** Cenas da história (as de boss seguem a ordem de BossId). */
typedef enum {
    CENA_INTRO = 0,         /**< Abertura da partida. */
    CENA_BOSS_CREEPER,      /**< Entrada de cada boss: CENA_BOSS_CREEPER + BossId. */
    CENA_BOSS_WITHER,
    CENA_BOSS_WARDEN,
    CENA_BOSS_ENDER_DRAGON,
    CENA_BOSS_HEROBRINE,
    CENA_DERROTA,           /**< Errou. */
    CENA_PARADA,            /**< Parou e levou as esmeraldas. */
    CENA_HISTORIA_1,        /**< Parte introdutória com o creeper. */
    CENA_HISTORIA_2,        /**< Indo para o Nether. */
    CENA_HISTORIA_3,        /**< Indo para o Warden. */
    CENA_HISTORIA_4,        /**< Indo para o End. */
    CENA_HISTORIA_5,        /**< Voltando do End e checando o Steve. */
    CENA_HISTORIA_FIM,      /**< Vitória, paz e entrega dos prêmios. */
    CENA_QUANTIDADE         /**< Quantidade de cenas (também "cena inválida"). */
} CenaId;

/**
 * @brief Acrescenta as falas da cena ao fim do diálogo (cada cena é um `case` em historia.c).
 * @return STATUS_OK ou STATUS_ERRO_PARAMETRO (cena inválida ou falas demais).
 */
Status historia_anexar_cena(Dialogo *dialogo, CenaId cena);

/**
 * @brief Reação a uma resposta (jogo_tocar_reacao): resposta certa, frase do boss e, no acerto,
 *        o saldo. Cada fala é sorteada pela `variante`; texto "" nas tabelas pula a fala.
 * @param resposta_certa Texto da alternativa certa (%s da 1ª fala).
 * @param esmeraldas     Saldo atual (%d da fala das esmeraldas).
 * @param variante       Qualquer inteiro (usa-se |variante| % REACOES_POR_RESULTADO).
 * @return STATUS_OK ou STATUS_ERRO_PARAMETRO.
 */
Status historia_anexar_reacao(Dialogo *dialogo, bool acertou, const char *resposta_certa,
                              BossId boss_id, int esmeraldas, int variante);

/** @brief dialogo_limpar + historia_anexar_cena. */
Status historia_carregar_cena(Dialogo *dialogo, CenaId cena);

/** @brief Cena de entrada do boss; CENA_QUANTIDADE se inválido. */
CenaId historia_cena_do_boss(BossId id);

/**
 * @brief A cena de história que antecede a luta (Wither <- HISTORIA_2 ...). Usada pela revelação
 *        (o que tocar quando um boss cai) e por jogo_continuar_save (história + entrada do boss).
 * @return A cena, ou CENA_QUANTIDADE se o boss não tem checkpoint (Creeper ou inválido).
 */
CenaId historia_cena_antes_do_boss(BossId boss);

/**
 * @brief Boss ao qual pertence uma cena que grava o save: HISTORIA_2 a 5 e a entrada dos bosses
 *        do Wither em diante (BOSS_WITHER ... BOSS_HEROBRINE).
 * @return O boss, ou BOSS_QUANTIDADE se a cena não grava o save.
 */
BossId historia_boss_da_cena_salva(CenaId cena);

/** @brief Nome da cena no save ("HISTORIA_2", "BOSS_WITHER"...); "" se inválida. */
const char *historia_nome_cena(CenaId cena);

/** @brief Cena pelo nome (o contrário de historia_nome_cena); CENA_QUANTIDADE se não existe. */
CenaId historia_cena_pelo_nome(const char *nome);

#endif /* HISTORIA_H */
