/**
 * @file utils.h
 * @brief Funções auxiliares de terminal: leitura segura, cores e beep.
 */

#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>
#include "config.h"

/** @name Cores ANSI (o terminal precisa suportar sequências de escape). */
/**@{*/
#define COR_RESET    "\033[0m"
#define COR_VERMELHO "\033[1;31m"
#define COR_VERDE    "\033[1;32m"
#define COR_AMARELO  "\033[1;33m"
#define COR_AZUL     "\033[1;34m"
#define COR_CIANO    "\033[1;36m"
/**@}*/

/**
 * @brief Prepara o terminal (no Windows, ativa UTF-8 para acentos).
 */
void utils_configurar_terminal(void);

/**
 * @brief Limpa a tela do terminal.
 */
void utils_limpar_tela(void);

/**
 * @brief Emite um beep sonoro no terminal.
 */
void utils_beep(void);

/**
 * @brief Espera o usuário pressionar ENTER.
 */
void utils_pausar(void);

/**
 * @brief Lê uma linha de texto com segurança (sem estourar o buffer).
 *
 * Remove o '\n' final e descarta o excedente se a linha for maior que o buffer.
 *
 * @param prompt   Texto exibido antes da leitura (pode ser NULL).
 * @param destino  Buffer que recebe o texto.
 * @param tamanho  Tamanho do buffer em bytes.
 * @return 1 se leu uma linha; 0 se a entrada terminou (EOF).
 */
int utils_ler_linha(const char *prompt, char *destino, size_t tamanho);

/**
 * @brief Lê um inteiro dentro de um intervalo, repetindo até ser válido.
 *
 * Mostra mensagem de erro e beep a cada entrada inválida.
 *
 * @param prompt  Texto exibido antes da leitura.
 * @param minimo  Menor valor aceito.
 * @param maximo  Maior valor aceito.
 * @param valor   Recebe o número lido.
 * @return 1 se leu um valor válido; 0 se a entrada terminou (EOF).
 */
int utils_ler_inteiro(const char *prompt, int minimo, int maximo, int *valor);

/**
 * @brief Traduz um Status para uma mensagem legível.
 *
 * @param status Código de retorno.
 * @return Texto constante em português (nunca NULL).
 */
const char *utils_mensagem_status(Status status);

#endif /* UTILS_H */
