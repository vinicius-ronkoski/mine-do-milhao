/**
 * @file menu.h
 * @brief Menu principal em modo texto: ponto de entrada do programa e da opção "Jogar".
 */

#ifndef MENU_H
#define MENU_H

#include "banco.h"

/**
 * @brief Exibe o menu principal e trata as opções até o usuário sair.
 *
 * Opções: cadastrar, listar, pesquisar, alterar, excluir, navegar por nível
 * de dificuldade, exportar CSV e jogar (abre o jogo visual de jogo.h; sem
 * USE_RAYLIB só avisa como compilar). Entradas inválidas geram mensagem e beep.
 *
 * @param banco Banco de perguntas manipulado pelo menu.
 */
void menu_principal(Banco *banco);

#endif /* MENU_H */
