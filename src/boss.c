/** 
 * @file boss.c 
 * 
 * @brief Esqueleto da estrutura de BOSS (ver boss.h). 
 * 
 * */
#include "boss.h"

Status boss_iniciar(Boss *boss, BossId id, int vida_maxima)
{
    /* TODO: validar; id, nivel = id + NIVEL_MIN, vida_maxima, vida_atual = vida_maxima. */
    (void)boss; (void)id; (void)vida_maxima;
    return STATUS_NAO_IMPLEMENTADO;
}

BossId boss_id_do_nivel(int nivel)
{
    /* TODO: (BossId)(nivel - NIVEL_MIN) com checagem de intervalo. */
    (void)nivel;
    return BOSS_CREEPER;
}

const char *boss_nome(BossId id)
{
    /* TODO: tabela static const char *const nomes[] LOCAL à função (sem global). */
    (void)id;
    return "Desconhecido";
}

Status boss_receber_dano(Boss *boss, int dano)
{
    /* TODO: vida_atual -= dano; limitar em 0. */
    (void)boss; (void)dano;
    return STATUS_NAO_IMPLEMENTADO;
}

int boss_derrotado(const Boss *boss)
{
    /* TODO */
    (void)boss;
    return 0;
}

float boss_calculo_vida(const Boss *boss)
{
    /* TODO: vida_atual / vida_maxima. */
    (void)boss;
    return 0.0f;
}
