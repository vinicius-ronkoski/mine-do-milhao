/**
 * @file menu.c
 * @brief Implementação do menu principal em modo texto.
 *
 * TODO (equipe): trocar cada chamada a tela_em_construcao() pela função
 * real da opção (cadastrar, listar, pesquisar...).
 */

#include <stdio.h>

#include "menu.h"
#include "utils.h"

/** Opção "sair" do menu. */
#define OPCAO_SAIR 0

/** Maior número de opção do menu. */
#define OPCAO_MAX 7

/**
 * @brief Desenha as opções do menu principal.
 */
static void exibir_menu(void)
{
    printf(COR_VERDE "=====================================\n");
    printf("        MINE DO MILHÃO - MENU\n");
    printf("=====================================\n" COR_RESET);
    printf(" 1 - Cadastrar pergunta\n");
    printf(" 2 - Listar todas as perguntas\n");
    printf(" 3 - Pesquisar pergunta\n");
    printf(" 4 - Alterar pergunta\n");
    printf(" 5 - Excluir pergunta\n");
    printf(" 6 - Navegar por nível de dificuldade\n");
    printf(" 7 - Exportar perguntas para CSV\n");
    printf(" 0 - Sair\n\n");
}

/**
 * @brief Tela provisória de uma opção ainda não implementada.
 *
 * @param titulo Nome da opção.
 * @param banco  Banco de perguntas (usado só para mostrar o total).
 */
static void tela_em_construcao(const char *titulo, const Banco *banco)
{
    utils_limpar_tela();
    printf(COR_AMARELO "== %s ==\n" COR_RESET, titulo);
    printf("Perguntas no banco: %zu\n", banco->quantidade);
    printf("Em construção...\n");
    utils_pausar();
}

void menu_principal(Banco *banco)
{
    int opcao = OPCAO_SAIR;

    do {
        utils_limpar_tela();
        exibir_menu();

        if (!utils_ler_inteiro("Escolha uma opção: ", OPCAO_SAIR, OPCAO_MAX, &opcao)) {
            break; /* fim da entrada (EOF) */
        }

        switch (opcao) {
        case 1: tela_em_construcao("Cadastrar pergunta", banco); break;
        case 2: tela_em_construcao("Listar perguntas", banco); break;
        case 3: tela_em_construcao("Pesquisar pergunta", banco); break;
        case 4: tela_em_construcao("Alterar pergunta", banco); break;
        case 5: tela_em_construcao("Excluir pergunta", banco); break;
        case 6: tela_em_construcao("Navegar por nível", banco); break;
        case 7: tela_em_construcao("Exportar CSV", banco); break;
        default: break;
        }
    } while (opcao != OPCAO_SAIR);
}
