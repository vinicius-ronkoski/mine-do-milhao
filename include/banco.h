/**
 * @file banco.h
 * @brief Banco de perguntas: vetor dinâmico com cadastro completo (CRUD).
 *
 * Preenchido por arquivo.h, editado pelo menu (menu.c) e lido pelo jogo (sorteio.h).
 * Índices começam em 0.
 */

#ifndef BANCO_H
#define BANCO_H

#include <stddef.h>
#include "config.h"
#include "pergunta.h"

/**
 * @brief Vetor de perguntas que cresce e encolhe (malloc/realloc/free).
 *
 * Invariantes: quantidade <= capacidade; itens == NULL se e só se capacidade == 0.
 * @warning Não grave o Banco com fwrite(): itens é ponteiro (ver dat_salvar).
 */
typedef struct {
    /** Vetor alocado dinamicamente (NULL se vazio). */
    Pergunta *itens;
    /** Perguntas em uso. */
    size_t quantidade;
    /** Perguntas que cabem sem realocar. */
    size_t capacidade;
} Banco;

/** @brief Deixa o banco vazio e pronto para uso. */
void banco_iniciar(Banco *banco);

/** @brief Libera o vetor e zera o banco (pode já estar vazio). */
void banco_liberar(Banco *banco);

/** @brief Pergunta da posição `indice`, ou NULL se o índice for inválido. */
const Pergunta *banco_obter(const Banco *banco, size_t indice);

/**
 * @brief Copia a pergunta para o fim do banco (dobra a capacidade quando cheio).
 * @return STATUS_OK, STATUS_ERRO_PARAMETRO (inválida, ver pergunta_validar) ou STATUS_ERRO_MEMORIA.
 */
Status banco_inserir(Banco *banco, const Pergunta *pergunta);

/**
 * @brief Substitui a pergunta da posição `indice`.
 * @return STATUS_OK ou STATUS_ERRO_PARAMETRO.
 */
Status banco_alterar(Banco *banco, size_t indice, const Pergunta *pergunta);

/**
 * @brief Remove a pergunta da posição `indice`, deslocando as seguintes (encolhe se sobrar espaço).
 * @return STATUS_OK ou STATUS_ERRO_PARAMETRO.
 */
Status banco_remover(Banco *banco, size_t indice);

/**
 * @brief Posições das perguntas cujo enunciado contém `trecho` (sem diferenciar maiúsculas, com acentos).
 * @param indices     Vetor de saída.
 * @param max_indices Tamanho de `indices` (use banco->quantidade).
 * @return Quantidade encontrada.
 */
size_t banco_buscar_texto(const Banco *banco, const char *trecho,
                          size_t *indices, size_t max_indices);

/**
 * @brief Posições das perguntas do nível `nivel` (navegação do menu e sorteio da partida).
 * @param indices     Vetor de saída.
 * @param max_indices Tamanho de `indices` (use banco->quantidade).
 * @return Quantidade encontrada.
 */
size_t banco_filtrar_nivel(const Banco *banco, int nivel,
                           size_t *indices, size_t max_indices);

#endif /* BANCO_H */
