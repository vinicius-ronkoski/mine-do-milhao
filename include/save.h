/**
 * @file save.h
 * @brief Save do jogo: o boss em que o jogador vai continuar, o saldo de esmeraldas e se
 *        "eliminar duas" já foi usada.
 *
 * Fluxo: ao derrotar um boss, tela_revelacao_atualizar grava o próximo (save_gravar, antes da
 * história e da luta); o botão "Carregar jogo" do menu só fica habilitado se save_ler der certo
 * e chama jogo_continuar_save. A vitória apaga o save (save_apagar); a derrota o mantém.
 * Arquivo de texto (CAMINHO_SAVE): uma linha de comentário e "boss esmeraldas eliminar_usada".
 * Não usa raylib.
 */
#ifndef SAVE_H
#define SAVE_H

#include "config.h"
#include "boss.h"

/** @brief Conteúdo do save. */
typedef struct {
    BossId boss;            /**< Boss em que a partida continua (Wither a Herobrine). */
    int esmeraldas;         /**< Saldo no instante do save (esmeraldas - gastas). */
    bool eliminacao_usada;  /**< "Eliminar duas" já gasta na partida. */
} Save;

/**
 * @brief Grava o save (via temporário + arquivo_substituir: o save antigo só some se der certo).
 * @return STATUS_OK, STATUS_ERRO_PARAMETRO ou STATUS_ERRO_ARQUIVO.
 */
Status save_gravar(const char *caminho, const Save *save);

/**
 * @brief Lê e valida o save: boss de Wither a Herobrine, saldo entre 0 e o prêmio acumulado
 *        até aquele boss, flag 0 ou 1.
 * @return STATUS_OK; STATUS_ERRO_ARQUIVO (não existe); STATUS_ERRO_FORMATO (corrompido ou fora da faixa).
 */
Status save_ler(const char *caminho, Save *save);

/** @brief Apaga o save (não existir também é STATUS_OK). */
Status save_apagar(const char *caminho);

#endif /* SAVE_H */
