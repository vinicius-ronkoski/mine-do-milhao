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

/**
 * @brief Carrega os dados, roda o menu e salva tudo ao sair.
 *
 * O menu é SEMPRE o ponto de entrada; a opção "Jogar" dele chama o jogo
 * visual (jogo.h) quando o programa é compilado com USE_RAYLIB=1.
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
        return EXIT_FAILURE;   /* não chama dados_salvar: preserva o .dat */
    }

    // Menu principal (cadastro completo e, com raylib, a opção de jogar)
    menu_principal(&banco);

    // Sai do programa, verificando se os dados foram salvos
    status = dados_salvar(&banco);
    if (status != STATUS_OK) {
        printf(COR_AMARELO "Aviso ao salvar dados: %s\n" COR_RESET,
               utils_mensagem_status(status));
    }

    banco_liberar(&banco);
    return EXIT_SUCCESS;
}
