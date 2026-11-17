/**
 * @file main.c
 * @brief Ponto de entrada do Mine do Milhão (1ª entrega: parte básica).
 */

#include <stdio.h>

#include "arquivo.h"
#include "banco.h"
#include "menu.h"
#include "utils.h"

/**
 * @brief Carrega os dados, roda o menu e salva tudo ao sair.
 *
 * @return 0 ao terminar normalmente.
 */
int main(void)
{
    Banco banco;
    Status status;

    utils_configurar_terminal();
    banco_iniciar(&banco);

    status = dados_carregar(&banco);
    if (status != STATUS_OK) {
        printf(COR_VERMELHO "Erro ao carregar dados: %s\n" COR_RESET,
        utils_mensagem_status(status));
        banco_liberar(&banco);
        return 1;   /* não chama dados_salvar: preserva o .bin */
    }

    menu_principal(&banco);

    status = dados_salvar(&banco);
    if (status != STATUS_OK) {
        printf(COR_AMARELO "Aviso ao salvar dados: %s\n" COR_RESET,
               utils_mensagem_status(status));
    }

    banco_liberar(&banco);
    return 0;
}
