/**
 * @file banco.c
 * @brief Implementação do banco de perguntas (vetor dinâmico).
 *
 * TODO (equipe): implementar as funções marcadas abaixo.
 */

#include <stdlib.h>

#include "banco.h"

void banco_iniciar(Banco *banco)
{
    if (banco == NULL) {
        return;
    }
    banco->itens = NULL;
    banco->quantidade = 0;
    banco->capacidade = 0;
}

void banco_liberar(Banco *banco)
{
    if (banco == NULL) {
        return;
    }
    free(banco->itens);
    banco_iniciar(banco);
}

const Pergunta *banco_obter(const Banco *banco, size_t indice)
{
    if (banco == NULL || indice >= banco->quantidade) {
        return NULL;
    }
    return &banco->itens[indice];
}

Status banco_inserir(Banco *banco, const Pergunta *pergunta)
{
    /* TODO: validar com pergunta_valida(); se quantidade == capacidade,
     *       dobrar a capacidade com realloc (começando em, por ex., 16);
     *       copiar a pergunta e incrementar quantidade. */
    (void)banco;
    (void)pergunta;
    return STATUS_NAO_IMPLEMENTADO;
}

Status banco_alterar(Banco *banco, size_t indice, const Pergunta *pergunta)
{
    /* TODO: validar índice e pergunta; sobrescrever banco->itens[indice]. */
    (void)banco;
    (void)indice;
    (void)pergunta;
    return STATUS_NAO_IMPLEMENTADO;
}

Status banco_remover(Banco *banco, size_t indice)
{
    /* TODO: validar índice; deslocar os itens seguintes (memmove) e
     *       decrementar quantidade; opcionalmente encolher com realloc. */
    (void)banco;
    (void)indice;
    return STATUS_NAO_IMPLEMENTADO;
}

size_t banco_buscar_texto(const Banco *banco, const char *trecho,
                          size_t *indices, size_t max_indices)
{
    /* TODO: percorrer o vetor comparando o trecho com o enunciado
     *       (ignorar maiúsculas/minúsculas) e guardar as posições. */
    (void)banco;
    (void)trecho;
    (void)indices;
    (void)max_indices;
    return 0;
}

size_t banco_filtrar_nivel(const Banco *banco, int nivel,
                           size_t *indices, size_t max_indices)
{
    /* TODO: percorrer o vetor guardando as posições cujo nivel == nivel. */
    (void)banco;
    (void)nivel;
    (void)indices;
    (void)max_indices;
    return 0;
}
