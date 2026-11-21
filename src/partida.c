/** @file partida.c @brief Esqueleto das regras da partida (ver partida.h). */
#include <string.h>
#include "partida.h"

Status partida_iniciar(Partida *partida, const Banco *banco)
{
    /* TODO: memset; sorteio_montar_partida(); boss_iniciar() por nível; rodada=0; escolha=-1. */
    (void)partida; (void)banco;
    return STATUS_NAO_IMPLEMENTADO;
}

int partida_nivel_da_rodada(int rodada)
{
    /* TODO: faixas 0-1,2-3,4-7,8-11,12-14 (enunciado: 1-2,3-4,5-8,9-12,13-15). */
    (void)rodada;
    return NIVEL_MIN;
}

int partida_perguntas_no_nivel(int nivel)
{
    /* TODO: contar rodadas com partida_nivel_da_rodada() == nivel. */
    (void)nivel;
    return 0;
}

int partida_eh_marco(int rodada)
{
    /* TODO: rodada + 1 == RODADA_MARCO_1 || rodada + 1 == RODADA_MARCO_2. */
    (void)rodada;
    return 0;
}

int partida_premio_da_rodada(int rodada)
{
    /* TODO: static const int tabela[PERGUNTAS_POR_PARTIDA] = PREMIOS_ESMERALDAS; (LOCAL, sem global). */
    (void)rodada;
    return 0;
}

const PerguntaSorteada *partida_pergunta_atual(const Partida *partida)
{
    /* TODO: NULL se encerrada; senão &partida->perguntas[partida->rodada]. */
    (void)partida;
    return NULL;
}

const Boss *partida_boss_atual(const Partida *partida)
{
    /* TODO: &partida->bosses[partida_nivel_da_rodada(rodada) - NIVEL_MIN]. */
    (void)partida;
    return NULL;
}

Status partida_responder(Partida *partida, int posicao_escolhida)
{
    /* TODO: ver passos em partida.h. */
    (void)partida; (void)posicao_escolhida;
    return STATUS_NAO_IMPLEMENTADO;
}

Status partida_confirmar_revelacao(Partida *partida)
{
    /* TODO: acerto -> rodada++ (ou FIM_VITORIA); erro -> FIM_ERRO. */
    (void)partida;
    return STATUS_NAO_IMPLEMENTADO;
}

Status partida_parar(Partida *partida)
{
    /* TODO: exigir ESTADO_RESPONDENDO; estado = ESTADO_FIM_PARADA. */
    (void)partida;
    return STATUS_NAO_IMPLEMENTADO;
}

int partida_encerrada(const Partida *partida)
{
    /* TODO */
    (void)partida;
    return 0;
}

int partida_premio_final(const Partida *partida)
{
    /* TODO: ver regra em partida.h. */
    (void)partida;
    return 0;
}
