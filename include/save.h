/**
 * @file save.h
 * @brief Save automático: a cena da história em que o jogador está, o saldo de esmeraldas e se
 *        "eliminar duas" já foi usada.
 *
 * Fluxo: quando começa uma cena que grava (HISTORIA_2 a 5 e a entrada dos bosses do Wither em
 * diante, ver historia_boss_da_cena_salva), tela_dialogo_atualizar chama save_gravar. Sair no meio
 * da luta volta, ao carregar, para a entrada daquele boss (a última cena gravada). O botão
 * "Carregar jogo" só fica habilitado se save_ler der certo e chama jogo_continuar_save.
 * Parar e vitória apagam o save (save_apagar); a derrota o mantém.
 * Arquivo de texto (CAMINHO_SAVE): uma linha de comentário e "NOME_DA_CENA esmeraldas eliminar_usada",
 * ex. "BOSS_WITHER 2 0". Só compila com USE_RAYLIB=1 (usa historia.h).
 */
#ifndef SAVE_H
#define SAVE_H

#include "config.h"
#include "historia.h"

/** @brief Conteúdo do save. */
typedef struct {
    CenaId cena;            /**< Cena em que continuar (uma das que gravam o save). */
    int esmeraldas;         /**< Saldo quando a cena começou (esmeraldas - gastas). */
    bool eliminacao_usada;  /**< "Eliminar duas" já gasta na partida. */
} Save;

/**
 * @brief Grava o save (via temporário + arquivo_substituir: o save antigo só some se der certo).
 * @return STATUS_OK, STATUS_ERRO_PARAMETRO (conteúdo inválido) ou STATUS_ERRO_ARQUIVO.
 */
Status save_gravar(const char *caminho, const Save *save);

/**
 * @brief Lê e valida o save: cena que grava (historia_boss_da_cena_salva), saldo entre 0 e o
 *        prêmio acumulado até aquele boss, flag 0 ou 1.
 * @return STATUS_OK; STATUS_ERRO_ARQUIVO (não existe); STATUS_ERRO_FORMATO (corrompido ou fora da faixa).
 */
Status save_ler(const char *caminho, Save *save);

/** @brief Apaga o save (não existir também é STATUS_OK). */
Status save_apagar(const char *caminho);

#endif /* SAVE_H */
