/**
 * @file sorteio.c
 * @brief Implementação do sistema aleatorizador (ver sorteio.h). Sem raylib.
 */
#include <stdlib.h>
#include "sorteio.h"

void sorteio_semear(unsigned int semente)
{
    srand(semente);
}

int sorteio_inteiro(int minimo, int maximo)
{
    unsigned int faixa;

    if (minimo >= maximo) {
        return minimo;
    }

    /* A faixa é pequena (alternativas e perguntas), então o viés do % é desprezível. */
    faixa = (unsigned int)(maximo - minimo) + 1u;
    return minimo + (int)((unsigned int)rand() % faixa);
}

void sorteio_embaralhar_vetor(size_t *vetor, size_t tamanho)
{
    size_t i, j, temporario;

    if (vetor == NULL) {
        return;
    }

    /* Fisher-Yates: do fim para o começo, troca cada posição com uma sorteada entre 0 e ela mesma. */
    for (i = tamanho; i > 1; i--) {
        j = (size_t)sorteio_inteiro(0, (int)(i - 1));
        temporario = vetor[i - 1];
        vetor[i - 1] = vetor[j];
        vetor[j] = temporario;
    }
}

Status sorteio_alternativas(const Pergunta *pergunta, PerguntaSorteada *saida)
{
    size_t ordem[NUM_ALTERNATIVAS];
    int i, original_correta;

    if (pergunta == NULL || saida == NULL || pergunta_validar(pergunta) != STATUS_OK) {
        return STATUS_ERRO_PARAMETRO;
    }

    for (i = 0; i < NUM_ALTERNATIVAS; i++) {
        ordem[i] = (size_t)i;
    }
    sorteio_embaralhar_vetor(ordem, NUM_ALTERNATIVAS);

    /* Descobre em que posição a alternativa que era a correta foi parar */
    original_correta = pergunta->correta - 'A';
    saida->origem = pergunta;
    saida->posicao_correta = 0;
    for (i = 0; i < NUM_ALTERNATIVAS; i++) {
        saida->ordem[i] = (int)ordem[i];
        if (saida->ordem[i] == original_correta) {
            saida->posicao_correta = i;
        }
    }
    return STATUS_OK;
}

Status sorteio_perguntas_do_nivel(const Banco *banco, int nivel, size_t quantidade, size_t *indices_saida)
{
    size_t *candidatas;
    size_t total, i;

    if (banco == NULL || indices_saida == NULL || nivel < NIVEL_MIN || nivel > NIVEL_MAX) {
        return STATUS_ERRO_PARAMETRO;
    }
    if (quantidade == 0) {
        return STATUS_OK;
    }
    if (banco->quantidade < quantidade) {
        return STATUS_NAO_ENCONTRADO;  /* nem somando todos os níveis chega */
    }

    candidatas = malloc(banco->quantidade * sizeof(*candidatas));
    if (candidatas == NULL) {
        return STATUS_ERRO_MEMORIA;
    }

    total = banco_filtrar_nivel(banco, nivel, candidatas, banco->quantidade);
    if (total < quantidade) {
        free(candidatas);
        return STATUS_NAO_ENCONTRADO;  /* o CSV/cadastro tem poucas perguntas neste nível */
    }

    sorteio_embaralhar_vetor(candidatas, total);
    for (i = 0; i < quantidade; i++) {
        indices_saida[i] = candidatas[i];
    }

    free(candidatas);
    return STATUS_OK;
}

Status sorteio_montar_partida(const Banco *banco, const int por_nivel[NUM_NIVEIS],
                              PerguntaSorteada saida[PERGUNTAS_POR_PARTIDA])
{
    PerguntaSorteada montada[PERGUNTAS_POR_PARTIDA];
    size_t indices[PERGUNTAS_POR_PARTIDA];
    int soma = 0, nivel, k, usadas = 0;
    Status status;

    if (banco == NULL || por_nivel == NULL || saida == NULL) {
        return STATUS_ERRO_PARAMETRO;
    }
    for (nivel = NIVEL_MIN; nivel <= NIVEL_MAX; nivel++) {
        if (por_nivel[nivel - NIVEL_MIN] < 0) {
            return STATUS_ERRO_PARAMETRO;
        }
        soma += por_nivel[nivel - NIVEL_MIN];
    }
    if (soma != PERGUNTAS_POR_PARTIDA) {
        return STATUS_ERRO_PARAMETRO;
    }

    for (nivel = NIVEL_MIN; nivel <= NIVEL_MAX; nivel++) {
        int quantidade = por_nivel[nivel - NIVEL_MIN];

        status = sorteio_perguntas_do_nivel(banco, nivel, (size_t)quantidade, indices);
        if (status != STATUS_OK) {
            return status;  /* saida não foi tocada: a partida não começa */
        }
        for (k = 0; k < quantidade; k++) {
            status = sorteio_alternativas(banco_obter(banco, indices[k]), &montada[usadas]);
            if (status != STATUS_OK) {
                return status;
            }
            usadas++;
        }
    }

    for (k = 0; k < PERGUNTAS_POR_PARTIDA; k++) {
        saida[k] = montada[k];
    }
    return STATUS_OK;
}

const char *sorteio_texto_alternativa(const PerguntaSorteada *sorteada, int posicao)
{
    if (sorteada == NULL || sorteada->origem == NULL || posicao < 0 || posicao >= NUM_ALTERNATIVAS) {
        return "";
    }
    return sorteada->origem->alternativas[sorteada->ordem[posicao]];
}
