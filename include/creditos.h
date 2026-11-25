/**
 * @file creditos.h
 * @brief Conteúdo da tela de créditos: uma tabela de linhas de texto em creditos.c.
 *
 * Fluxo: o botão "Créditos" do menu e o botão da tela de vitória abrem TELA_CREDITOS; tela_creditos_desenhar
 * (tela.c) percorre as linhas com creditos_linha e as faz subir. Separado de tela.c, como historia.c,
 * para editar o texto sem mexer no código das telas.
 */
#ifndef CREDITOS_H
#define CREDITOS_H

/** @brief Quantas linhas os créditos têm (inclui as vazias). */
int creditos_quantidade_linhas(void);

/**
 * @brief Linha `indice` (0 = primeira). 1º caractere: '#' título, '~' rótulo, "" linha vazia.
 * @return "" se `indice` estiver fora da tabela.
 */
const char *creditos_linha(int indice);

#endif /* CREDITOS_H */
