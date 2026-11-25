/**
 * @file utils.h
 * @brief Auxiliares de terminal (menu de texto): leitura segura, cores ANSI, beep e mensagens de Status.
 */

#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>
#include "config.h"

/** @name Cores ANSI */
/**@{*/
#define COR_RESET    "\033[0m"
#define COR_VERMELHO "\033[1;31m"
#define COR_VERDE    "\033[1;32m"
#define COR_AMARELO  "\033[1;33m"
#define COR_AZUL     "\033[1;34m"
#define COR_CIANO    "\033[1;36m"
/**@}*/

/**
 * @brief Descarta o resto da linha digitada até o ENTER (ou EOF).
 *        Só use quando sobrou texto (como em utils_ler_linha); senão espera uma linha nova.
 */
void utils_limpar_buffer(void);

/** @brief Prepara o terminal (no Windows, UTF-8 para os acentos). Chamada por main. */
void utils_configurar_terminal(void);

/** @brief Limpa a tela do terminal. */
void utils_limpar_tela(void);

/** @brief Beep no terminal (entradas inválidas). */
void utils_beep(void);

/** @brief Espera o usuário pressionar ENTER. */
void utils_pausar(void);

/**
 * @brief Lê uma linha sem estourar `destino`: tira o '\n' e descarta o excedente.
 * @param prompt Texto exibido antes (pode ser NULL).
 * @return STATUS_OK; STATUS_ERRO_ARQUIVO (EOF); STATUS_ERRO_PARAMETRO (destino NULL ou tamanho 0).
 */
Status utils_ler_linha(const char *prompt, char *destino, size_t tamanho);

/**
 * @brief Lê um inteiro em [minimo, maximo], repetindo com erro e beep até ser válido.
 * @return STATUS_OK; STATUS_ERRO_ARQUIVO (EOF); STATUS_ERRO_PARAMETRO (valor NULL).
 */
Status utils_ler_int(const char *prompt, int minimo, int maximo, int *valor);

/** @brief Texto em português de um Status (nunca NULL); usado no menu e no banner de erro do jogo. */
const char *utils_mensagem_status(Status status);

#endif /* UTILS_H */
