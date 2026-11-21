/**
 * @file main.c
 * @brief Ponto de entrada do Mine do Milhão (1ª entrega: parte básica).
 */

#include <stdio.h>

#include "arquivo.h"
#include "banco.h"
#include "menu.h"
#include "utils.h"
#ifdef USE_RAYLIB
    #include "jogo.h"
#endif

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

    
    #ifdef USE_RAYLIB
        status = jogo_executar(&banco);
        if (status != STATUS_OK && status != STATUS_ASSET_FALLBACK)
        printf(COR_VERMELHO "Erro no jogo: %s\n" COR_RESET, utils_mensagem_status(status));
    #else
        menu_principal(&banco);
    #endif

    status = dados_salvar(&banco);
    if (status != STATUS_OK) {
        printf(COR_AMARELO "Aviso ao salvar dados: %s\n" COR_RESET,
               utils_mensagem_status(status));
    }

    banco_liberar(&banco);
    return 0;
}
