/**
 * @file menu.h
 * @brief Menu principal em modo texto (1ª entrega, sem parte gráfica).
 */

#ifndef MENU_H
#define MENU_H

#include "banco.h"

/**
 * @brief Exibe o menu principal e trata as opções até o usuário sair.
 *
 * Opções: cadastrar, listar, pesquisar, alterar, excluir, navegar por nível
 * de dificuldade e exportar CSV. Entradas inválidas geram mensagem e beep.
 *
 * @param banco Banco de perguntas manipulado pelo menu.
 */
void menu_principal(Banco *banco);

#endif /* MENU_H */
