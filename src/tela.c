/**
 * @file tela.c
 * @brief Telas do jogo (ver tela.h). Compila só com USE_RAYLIB.
 *
 * Organização: 1) desenhos compartilhados, 2) tela de diálogo (pronta),
 * 3) telas de pergunta/revelação/resultado (A IMPLEMENTAR pela equipe).
 */
#ifdef USE_RAYLIB
#include "tela.h"
#include "utils.h"

/** Lado do boss desenhado no centro da tela de diálogo. */
#define LADO_BOSS_DIALOGO 260.0f

/** Altura da faixa de erro no rodapé. */
#define ALTURA_BANNER_ERRO 34.0f

/** Tamanho do texto provisório das telas ainda não implementadas. */
#define FONTE_PROVISORIA 30.0f

/* ========================================================================= */
/* 1. Desenhos compartilhados                                                */
/* ========================================================================= */

BossId tela_boss_da_fase(const Jogo *jogo)
{
    const Boss *boss;

    if (jogo == NULL) {
        return BOSS_QUANTIDADE;
    }
    boss = partida_boss_atual(&jogo->partida);
    return boss != NULL ? boss->id : BOSS_QUANTIDADE;
}

void tela_desenhar_barra_vida(Rectangle area, float fracao, Color cor)
{
    Rectangle preenchimento = area;

    if (fracao < 0.0f) {
        fracao = 0.0f;
    } else if (fracao > 1.0f) {
        fracao = 1.0f;
    }
    preenchimento.width = area.width * fracao;

    DrawRectangleRec(area, Fade(BLACK, 0.6f));
    DrawRectangleRec(preenchimento, cor);
    DrawRectangleLinesEx(area, 2.0f, PALETA_BORDA);
}

void tela_desenhar_banner_erro(const Jogo *jogo)
{
    Rectangle faixa = { 0.0f, (float)ALTURA_JANELA - ALTURA_BANNER_ERRO,
                        (float)LARGURA_JANELA, ALTURA_BANNER_ERRO };

    if (jogo == NULL || jogo->ultimo_erro == STATUS_OK) {
        return;
    }
    DrawRectangleRec(faixa, Fade(PALETA_ERRO, 0.9f));
    assets_desenhar_texto_centralizado(&jogo->assets, utils_mensagem_status(jogo->ultimo_erro),
                                       faixa, 20.0f, PALETA_TEXTO);
}

void tela_desenhar_hud(const Jogo *jogo)
{
    /* TODO (equipe): ver tela.h. */
    (void)jogo;
}

void tela_desenhar_boss(const Jogo *jogo, Rectangle area)
{
    /* TODO (equipe): ver tela.h. */
    (void)jogo; (void)area;
}

void tela_desenhar_escada_premios(const Jogo *jogo, Rectangle area)
{
    /* TODO (equipe): ver tela.h. */
    (void)jogo; (void)area;
}

/* ========================================================================= */
/* 2. Tela de diálogo (pronta)                                               */
/* ========================================================================= */

void tela_dialogo_atualizar(Jogo *jogo, float dt)
{
    bool avancou;

    if (jogo == NULL) {
        return;
    }
    avancou = jogo->tempo_na_tela >= ATRASO_ENTRADA_TELA && jogo_jogador_quer_avancar();
    if (avancou) {
        assets_tocar_som(&jogo->assets, SOM_CLIQUE);
    }
    if (dialogo_atualizar(&jogo->dialogo, dt, avancou)) {
        jogo_trocar_tela(jogo, jogo->tela_apos_dialogo);
    }
}

void tela_dialogo_desenhar(const Jogo *jogo)
{
    const Boss *boss;
    Rectangle area_boss;

    if (jogo == NULL) {
        return;
    }

    assets_desenhar_fundo(&jogo->assets, IMG_FUNDO_DIALOGO, tela_boss_da_fase(jogo));

    /* Boss da rodada no centro, acima da caixa de fala */
    boss = partida_boss_atual(&jogo->partida);
    if (boss != NULL && !boss_derrotado(boss)) {
        area_boss.width = LADO_BOSS_DIALOGO;
        area_boss.height = LADO_BOSS_DIALOGO;
        area_boss.x = ((float)LARGURA_JANELA - LADO_BOSS_DIALOGO) / 2.0f;
        area_boss.y = 90.0f;
        assets_desenhar_boss(&jogo->assets, boss->id, jogo->tempo_total, area_boss, WHITE);
    }

    dialogo_desenhar(&jogo->dialogo, &jogo->assets);
}

/* ========================================================================= */
/* 3. Telas de jogo — A IMPLEMENTAR (passos em tela.h)                       */
/*    Enquanto não existem, mostram só o fundo e um aviso.                   */
/* ========================================================================= */

/** @brief Aviso provisório no centro da tela (apague quando a tela for implementada). */
static void desenhar_aviso_provisorio(const Jogo *jogo, ImagemId fundo, BossId boss, const char *texto)
{
    Rectangle janela = { 0.0f, 0.0f, (float)LARGURA_JANELA, (float)ALTURA_JANELA };

    assets_desenhar_fundo(&jogo->assets, fundo, boss);
    assets_desenhar_texto_centralizado(&jogo->assets, texto, janela, FONTE_PROVISORIA, PALETA_DESTAQUE);
}

void tela_pergunta_atualizar(Jogo *jogo, float dt)
{
    /* TODO (equipe): ver passos em tela.h. */
    (void)jogo; (void)dt;
}

void tela_pergunta_desenhar(const Jogo *jogo)
{
    /* TODO (equipe): substituir o aviso pela tela de verdade. */
    desenhar_aviso_provisorio(jogo, IMG_FUNDO_PERGUNTA, tela_boss_da_fase(jogo), "Tela de pergunta: implementar em tela.c (ESC volta ao menu)");
}

void tela_revelacao_atualizar(Jogo *jogo, float dt)
{
    /* TODO (equipe): ver passos em tela.h. */
    (void)jogo; (void)dt;
}

void tela_revelacao_desenhar(const Jogo *jogo)
{
    desenhar_aviso_provisorio(jogo, IMG_FUNDO_PERGUNTA, tela_boss_da_fase(jogo), "Tela de revelação: implementar em tela.c");
}

void tela_resultado_atualizar(Jogo *jogo, float dt)
{
    /* TODO (equipe): ver passos em tela.h. */
    (void)jogo; (void)dt;
}

void tela_resultado_desenhar(const Jogo *jogo)
{
    desenhar_aviso_provisorio(jogo, IMG_FUNDO_RESULTADO, BOSS_QUANTIDADE, "Tela de resultado: implementar em tela.c");
}

#endif /* USE_RAYLIB */
