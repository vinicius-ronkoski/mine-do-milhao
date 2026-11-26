/**
 * @file menu.h
 * @brief Menu de texto: ponto de entrada do programa (chamado por main) e da opção "Jogar".
 */

#ifndef MENU_H
#define MENU_H

#include <stddef.h>
#include <stdbool.h>
#include "banco.h"

/**
 * @brief Desenha as opções do menu principal.
 */
void exibir_menu(void);

/**
 * @brief Pergunta ao usuário um número de resultado (1 a total).
 *
 * @return true se escolheu; false se cancelou (digitou 0) ou a entrada acabou.
 */
bool escolher_resultado(const char *prompt, size_t total, size_t *escolhido);

/**
 * @brief Lista resumida dos resultados (posição, nível e enunciado).
 *
 * @param banco   Banco consultado.
 * @param indices Posições dos resultados dentro do banco.
 * @param total   Quantidade de resultados.
 */
void listar_resultados(const Banco *banco, const size_t *indices, size_t total);

/**
 * @brief Pede um trecho do enunciado, busca no banco e deixa o usuário escolher um resultado.
 *
 * Usada por pesquisar, alterar e excluir, pois as três começam do mesmo jeito.
 *
 * @param banco     Banco consultado.
 * @param acao      Verbo mostrado no prompt (ex.: "alterar").
 * @param indices   Vetor de saída (precisa ter banco->quantidade posições).
 * @param escolhido Recebe a posição, DENTRO DO BANCO, da pergunta escolhida.
 * @return true se o usuário escolheu uma pergunta; false se nada foi escolhido.
 */
bool buscar_e_escolher(const Banco *banco, const char *acao, size_t *indices, size_t *escolhido);

/**
 * @brief Lê um campo de texto obrigatório; ao alterar, ENTER mantém o valor atual.
 *
 * @param rotulo   Nome do campo.
 * @param destino  Buffer que recebe o texto.
 * @param tamanho  Tamanho do buffer.
 * @param atual    Valor atual (NULL ao cadastrar: então o campo é obrigatório).
 * @return true se o campo ficou preenchido; false se a entrada terminou.
 */
bool ler_campo(const char *rotulo, char *destino, size_t tamanho, const char *atual);

/**
 * @brief Lê a letra da alternativa correta (A a D).
 *
 * Se *correta já contém uma letra válida (alterar), ENTER mantém o valor atual.
 * Se não (cadastrar, com a Pergunta zerada), o campo é obrigatório.
 *
 * @param correta Letra atual (entrada) e letra lida (saída).
 * @return true se leu; false se a entrada terminou.
 */
bool ler_correta(char *correta);

/**
 * @brief Lê o nível de dificuldade.
 *
 * Se *nivel já contém um nível válido (alterar), ENTER mantém o valor atual.
 * Se não (cadastrar, com a Pergunta zerada), o campo é obrigatório.
 *
 * @param nivel Nível atual (entrada) e nível lido (saída).
 * @return true se leu; false se a entrada terminou.
 */
bool ler_nivel(int *nivel);

/**
 * @brief Mostra perguntas página a página e deixa o usuário navegar entre elas.
 *
 * Usada por "Listar todas" (várias perguntas por tela) e por "Navegar por nível"
 * (uma pergunta por tela). Comandos: P (ou ENTER) próxima, A anterior, S sai e um
 * número pula direto para aquela pergunta.
 *
 * @param banco      Banco consultado.
 * @param indices    Posições, dentro do banco, das perguntas a mostrar.
 * @param total      Quantidade de posições em indices (maior que zero).
 * @param titulo     Título exibido no topo da tela.
 * @param por_pagina Quantas perguntas aparecem por tela (maior que zero).
 */
void exibir_paginas(const Banco *banco, const size_t *indices, size_t total, const char *titulo, size_t por_pagina);

/**
 * @brief Tela "Cadastrar pergunta": lê todos os campos e insere no banco.
 *
 * A Pergunta começa zerada, então todos os campos são obrigatórios.
 */
void tela_cadastrar(Banco *banco);

/**
 * @brief Tela "Listar todas as perguntas": mostra o banco inteiro, página a página.
 */
void tela_listar(const Banco *banco);

/**
 * @brief Tela "Pesquisar pergunta": busca por trecho do enunciado e mostra a escolhida.
 */
void tela_pesquisar(const Banco *banco);

/**
 * @brief Tela "Alterar pergunta": busca, mostra a atual e pede os novos dados.
 *
 * Trabalha numa CÓPIA da pergunta: só grava no banco se tudo for válido.
 */
void tela_alterar(Banco *banco);

/**
 * @brief Tela "Excluir pergunta": busca, mostra a escolhida e pede confirmação.
 */
void tela_excluir(Banco *banco);

/**
 * @brief Tela "Navegar por nível": escolhe um nível e percorre suas perguntas uma a uma.
 */
void tela_navegar_nivel(const Banco *banco);

/**
 * @brief Confere se o nome do arquivo termina com ".csv".
 *
 * @param nome Nome do arquivo.
 * @return true se termina com ".csv"; false caso contrário.
 */
bool termina_com_csv(const char *nome);

/**
 * @brief Tela "Exportar CSV": grava o banco atual em CAMINHO_EXPORTADOS.
 *
 * O arquivo gerado tem o mesmo formato do CSV inicial, então pode ser lido de volta.
 */
void tela_exportar(const Banco *banco);

/**
 * @brief Tela "Jogar": abre a janela do jogo (raylib) e volta ao menu quando ela fecha.
 *
 * Sem USE_RAYLIB o jogo não existe no executável; avisa como compilar.
 * Assets que faltarem não impedem o jogo (fallback), só geram um aviso.
 */
void tela_jogar(const Banco *banco);

/**
 * @brief Laço do menu até "Sair": cadastrar, listar, pesquisar, alterar, excluir, navegar por nível,
 *        exportar CSV e jogar (jogo_executar; sem USE_RAYLIB só avisa como compilar).
 * @param banco Banco editado pelo menu (main salva ao sair).
 */
void menu_principal(Banco *banco);

#endif /* MENU_H */
