/**
 * @file menu.h
 * @brief Menu de texto: ponto de entrada do programa (chamado por main) e da opção "Jogar".
 */

#ifndef MENU_H
#define MENU_H

#include "banco.h"

/**
 * @brief Laço do menu até "Sair": cadastrar, listar, pesquisar, alterar, excluir, navegar por nível,
 *        exportar CSV e jogar (jogo_executar; sem USE_RAYLIB só avisa como compilar).
 * @param banco Banco editado pelo menu (main salva ao sair).
 */
void menu_principal(Banco *banco);

#endif /* MENU_H */
