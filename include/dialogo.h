/**
 * @file dialogo.h
 * @brief Diálogos de personagens (falas em sequência, efeito máquina de escrever).
 *
 * Uma Cena é uma lista de falas. As cenas existentes ficam em dialogo_carregar_cena().
 * COMO ADICIONAR UMA CENA: (1) novo id em CenaId; (2) novo `case` em dialogo_carregar_cena()
 * chamando dialogo_adicionar_fala(). FUTURO: ler de assets/dialogos.txt trocando só aquela função.
 * Só compila com USE_RAYLIB=1.
 */
#ifndef DIALOGO_H
#define DIALOGO_H

#include <stdbool.h>
#include "raylib.h"
#include "config.h"
#include "assets.h"

/** Uma fala. */
typedef struct {
    char personagem[TAM_NOME_PERSONAGEM];  /**< Nome mostrado acima do texto. */
    char texto[TAM_FALA];                  /**< Conteúdo (UTF-8, com acentos). */
    ImagemId retrato;                      /**< Retrato (IMG_*). Fallback automático se faltar. */
} Fala;

/** Cenas disponíveis. */
typedef enum {
    CENA_INTRO = 0,         /**< Abertura da partida. */
    CENA_BOSS_CREEPER,      /**< Entrada de cada boss: CENA_BOSS_x = CENA_BOSS_CREEPER + BossId. */
    CENA_BOSS_WITHER,
    CENA_BOSS_WARDEN,
    CENA_BOSS_ENDER_DRAGON,
    CENA_BOSS_HEROBRINE,
    CENA_BOSS_DERROTADO,    /**< Reação genérica após derrotar um boss. */
    CENA_VITORIA,
    CENA_DERROTA,
    CENA_PARADA,
    CENA_QUANTIDADE
} CenaId;

/** Sequência de falas em reprodução. */
typedef struct {
    Fala falas[DIALOGO_MAX_FALAS];
    int quantidade;        /**< Falas preenchidas. */
    int indice;            /**< Fala atual. */
    float tempo;           /**< Segundos desde o início da fala atual (máquina de escrever). */
    bool terminou;         /**< true após avançar além da última fala. */
} Dialogo;

/** @brief Zera o diálogo. */
void dialogo_limpar(Dialogo *dialogo);

/**
 * @brief Acrescenta uma fala ao fim.
 * @return STATUS_OK ou STATUS_ERRO_PARAMETRO (NULL, textos longos demais ou DIALOGO_MAX_FALAS excedido).
 */
Status dialogo_adicionar_fala(Dialogo *dialogo, const char *personagem, const char *texto, ImagemId retrato);

/**
 * @brief Monta as falas de uma cena (dialogo_limpar + dialogo_adicionar_fala...).
 * @param dialogo Destino.
 * @param cena    Cena desejada.
 * @return STATUS_OK ou STATUS_ERRO_PARAMETRO (cena inexistente).
 */
Status dialogo_carregar_cena(Dialogo *dialogo, CenaId cena);

/** @brief Cena de entrada de um boss (CENA_BOSS_CREEPER + id). */
CenaId dialogo_cena_do_boss(BossId id);

/**
 * @brief Atualiza: soma dt; clique/ESPAÇO/ENTER primeiro completa a fala, depois avança.
 * @param dt Tempo do frame (GetFrameTime()).
 * @return true quando o diálogo acabou (terminou == true).
 */
bool dialogo_atualizar(Dialogo *dialogo, float dt);

/**
 * @brief Desenha caixa (IMG_CAIXA_DIALOGO), retrato, nome e texto parcial (letras = tempo * DIALOGO_LETRAS_POR_SEGUNDO),
 *        com quebra de linha (tela_desenhar_texto_com_quebra).
 */
void dialogo_desenhar(const Dialogo *dialogo, const Assets *assets);

/** @brief 1 se terminou ou nunca teve falas. */
int dialogo_terminou(const Dialogo *dialogo);

#endif /* DIALOGO_H */
