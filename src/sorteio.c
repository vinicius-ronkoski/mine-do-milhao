/** @file sorteio.c @brief Esqueleto do sistema aleatorizador (ver sorteio.h). */
#include <stdlib.h>
#include "sorteio.h"
#include "partida.h"

void sorteio_semear(unsigned int semente)
{
    /* TODO: srand(semente); */
    (void)semente;
}

int sorteio_inteiro(int minimo, int maximo)
{
    /* TODO: minimo + rand() % (maximo - minimo + 1), tratando minimo > maximo. */
    (void)maximo;
    return minimo;
}

void sorteio_embaralhar_vetor(size_t* vetor, size_t tamanho)
{
    /* TODO: Fisher-Yates usando sorteio_inteiro(). */
    (void)vetor; (void)tamanho;
}

Status sorteio_alternativas(const Pergunta *p, PerguntaSorteada *saida)
{
    /* TODO: validar com pergunta_validar(); ordem = {0,1,2,3}; embaralhar; achar posicao_correta. */
    (void)p; (void)saida;
    return STATUS_NAO_IMPLEMENTADO;
}

Status sorteio_perguntas_do_nivel(const Banco *banco, int nivel, size_t quantidade, size_t *indices_out)
{
    /* TODO: malloc(banco->quantidade) -> banco_filtrar_nivel -> embaralhar -> copiar -> free.
     *       Se total < quantidade devolver STATUS_NAO_ENCONTRADO (CSV com perguntas insuficientes). */
    (void)banco; (void)nivel; (void)quantidade; (void)indices_out;
    return STATUS_NAO_IMPLEMENTADO;
}

Status sorteio_montar_partida(const Banco *banco, PerguntaSorteada saida[PERGUNTAS_POR_PARTIDA])
{
    /* TODO: laço nível NIVEL_MIN..NIVEL_MAX; banco_obter(banco, indice) para cada índice sorteado. */
    (void)banco; (void)saida;
    return STATUS_NAO_IMPLEMENTADO;
}

const char *sorteio_texto_alternativa(const PerguntaSorteada *ps, int posicao)
{
    /* TODO: retornar ps->origem->alternativas[ps->ordem[posicao]] com checagem de limites. */
    (void)ps; (void)posicao;
    return "";
}
