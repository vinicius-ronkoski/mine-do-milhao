/**
 * @file boss.c
 * @brief Implementação da estrutura de BOSS (ver boss.h). Sem raylib.
 */
#include <stddef.h>
#include "boss.h"

Status boss_iniciar(Boss *boss, BossId id, int vida_maxima)
{
    if (boss == NULL || (int)id < 0 || id >= BOSS_QUANTIDADE || vida_maxima <= 0) {
        return STATUS_ERRO_PARAMETRO;
    }

    boss->id = id;
    boss->nivel = (int)id + NIVEL_MIN;
    boss->vida_maxima = vida_maxima;
    boss->vida_atual = vida_maxima;
    return STATUS_OK;
}

BossId boss_id_do_nivel(int nivel)
{
    if (nivel < NIVEL_MIN || nivel > NIVEL_MAX) {
        return BOSS_QUANTIDADE;
    }
    return (BossId)(nivel - NIVEL_MIN);
}

const char *boss_nome(BossId id)
{
    /* Tabela local à função (sem variável global); mesma ordem do enum BossId. */
    static const char *const nomes[BOSS_QUANTIDADE] = {
        "Creeper", "Wither", "Warden", "Ender Dragon", "Herobrine"
    };

    if ((int)id < 0 || id >= BOSS_QUANTIDADE) {
        return "Desconhecido";
    }
    return nomes[id];
}

Status boss_receber_dano(Boss *boss, int dano)
{
    if (boss == NULL || dano < 0) {
        return STATUS_ERRO_PARAMETRO;
    }

    boss->vida_atual -= dano;
    if (boss->vida_atual < 0) {
        boss->vida_atual = 0;
    }
    return STATUS_OK;
}

bool boss_derrotado(const Boss *boss)
{
    return boss != NULL && boss->vida_atual <= 0;
}

float boss_calculo_vida(const Boss *boss)
{
    if (boss == NULL || boss->vida_maxima <= 0) {
        return 0.0f;
    }
    return (float)boss->vida_atual / (float)boss->vida_maxima;
}
