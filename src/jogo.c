/** @file jogo.c @brief Esqueleto do loop visual (ver jogo.h). Compila só com USE_RAYLIB. */
#ifdef USE_RAYLIB
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "jogo.h"
#include "tela.h"
#include "utils.h"

Status jogo_executar(const Banco *banco)
{
    /* TODO: ver passos em jogo.h. Garantir CloseWindow() em TODOS os caminhos de erro. */
    (void)banco;
    return STATUS_NAO_IMPLEMENTADO;
}

Status jogo_iniciar(Jogo *jogo, const Banco *banco)
{
    /* TODO: memset; assets_carregar (STATUS_ASSET_FALLBACK não é erro); botao_criar x6;
     *       botao_layout_alternativas; jogo_nova_partida. */
    (void)jogo; (void)banco;
    return STATUS_NAO_IMPLEMENTADO;
}

void jogo_encerrar(Jogo *jogo)
{
    /* TODO: assets_descarregar(&jogo->assets). */
    (void)jogo;
}

Status jogo_nova_partida(Jogo *jogo)
{
    /* TODO: partida_iniciar; jogo_preparar_botoes_pergunta; jogo_tocar_cena(CENA_INTRO, TELA_PERGUNTA). */
    (void)jogo;
    return STATUS_NAO_IMPLEMENTADO;
}

void jogo_atualizar(Jogo *jogo, float dt)
{
    /* TODO: assets_atualizar_musica; tempo_na_tela += dt; switch (jogo->tela) -> tela_*_atualizar. */
    (void)jogo; (void)dt;
}

void jogo_desenhar(const Jogo *jogo)
{
    /* TODO: ClearBackground; switch -> tela_*_desenhar; tela_desenhar_banner_erro se ultimo_erro != STATUS_OK. */
    (void)jogo;
}

void jogo_trocar_tela(Jogo *jogo, TelaJogo nova_tela)
{
    /* TODO: tela = nova_tela; tempo_na_tela = 0. */
    (void)jogo; (void)nova_tela;
}

Status jogo_tocar_cena(Jogo *jogo, CenaId cena, TelaJogo depois)
{
    /* TODO: dialogo_carregar_cena; tela_apos_dialogo = depois; jogo_trocar_tela(TELA_DIALOGO). */
    (void)jogo; (void)cena; (void)depois;
    return STATUS_NAO_IMPLEMENTADO;
}

Status jogo_preparar_botoes_pergunta(Jogo *jogo)
{
    /* TODO: TextFormat("%c) %s", 'A' + i, sorteio_texto_alternativa(...)); habilitado = true. */
    (void)jogo;
    return STATUS_NAO_IMPLEMENTADO;
}

void jogo_registrar_erro(Jogo *jogo, Status status, const char *contexto)
{
    /* TODO: fprintf(stderr, "%s: %s\n", contexto, utils_mensagem_status(status)); jogo->ultimo_erro = status. */
    (void)jogo; (void)status; (void)contexto;
}
#endif /* USE_RAYLIB */
