/**
 * @file main.c
 * @brief Ponto de entrada do Mine do Milhão (1ª entrega: parte básica).
 */

#include <stdio.h>
#include <stdlib.h>

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

    // Inicializa o terminal e o Banco de Dados
    utils_configurar_terminal();
    banco_iniciar(&banco);

    // Carrega os dados e confere o status
    status = dados_carregar(&banco);
    if (status != STATUS_OK) {
        printf(COR_VERMELHO "Erro ao carregar dados: %s\n" COR_RESET,
        utils_mensagem_status(status));
        banco_liberar(&banco);
        return EXIT_FAILURE;   /* não chama dados_salvar: preserva o .bin */
    }

    // Verifica se RAYLIB está sendo utilizado
    #ifdef USE_RAYLIB
        status = jogo_executar(&banco);
        
    #else
        menu_principal(&banco); // caso contrário inicia o menu principal
    #endif

    // Sai do programa, verificando se os dados foram salvos
    status = dados_salvar(&banco);
    if (status != STATUS_OK) {
        printf(COR_AMARELO "Aviso ao salvar dados: %s\n" COR_RESET,
               utils_mensagem_status(status));
    }

    banco_liberar(&banco);
    return EXIT_SUCCESS;
}
