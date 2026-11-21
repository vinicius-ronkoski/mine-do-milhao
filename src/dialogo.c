/** @file dialogo.c @brief Esqueleto dos diálogos (ver dialogo.h). Compila só com USE_RAYLIB. */
#ifdef USE_RAYLIB
#include <string.h>
#include "dialogo.h"

void dialogo_limpar(Dialogo *dialogo)
{
    /* TODO: memset(dialogo, 0, sizeof *dialogo). */
    (void)dialogo;
}

Status dialogo_adicionar_fala(Dialogo *dialogo, const char *personagem, const char *texto, ImagemId retrato)
{
    /* TODO: validar tamanhos; copiar; quantidade++. */
    (void)dialogo; (void)personagem; (void)texto; (void)retrato;
    return STATUS_NAO_IMPLEMENTADO;
}

Status dialogo_carregar_cena(Dialogo *dialogo, CenaId cena)
{
    /* TODO: switch (cena) com as falas de cada cena (ver comentário do arquivo .h). */
    (void)dialogo; (void)cena;
    return STATUS_NAO_IMPLEMENTADO;
}

CenaId dialogo_cena_do_boss(BossId id)
{
    /* TODO: (CenaId)(CENA_BOSS_CREEPER + id). */
    (void)id;
    return CENA_BOSS_CREEPER;
}

bool dialogo_atualizar(Dialogo *dialogo, float dt)
{
    /* TODO: ver dialogo.h. */
    (void)dialogo; (void)dt;
    return true;
}

void dialogo_desenhar(const Dialogo *dialogo, const Assets *assets)
{
    /* TODO: ver dialogo.h. */
    (void)dialogo; (void)assets;
}

int dialogo_terminou(const Dialogo *dialogo)
{
    /* TODO */
    (void)dialogo;
    return 1;
}
#endif /* USE_RAYLIB */
