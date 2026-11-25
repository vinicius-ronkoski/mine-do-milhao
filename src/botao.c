/**
 * @file botao.c
 * @brief Implementação dos botões (ver botao.h). Compila só com USE_RAYLIB.
 */
#ifdef USE_RAYLIB
#include <string.h>
#include "botao.h"
#include "layout.h"

/** Margem entre a borda do botão e o texto. */
#define MARGEM_TEXTO_BOTAO 12.0f

/** Menor tamanho de fonte aceito para o rótulo caber. */
#define FONTE_MINIMA_BOTAO 14.0f

/** Espaço entre o texto e o ícone. */
#define ESPACO_TEXTO_ICONE 6.0f

/* ---- Criação ---- */

Status botao_criar(Botao *botao, Rectangle area, const char *rotulo, int tecla_atalho)
{
    if (botao == NULL || rotulo == NULL) {
        return STATUS_ERRO_PARAMETRO;
    }

    memset(botao, 0, sizeof(*botao));
    botao->area = area;
    botao->tecla_atalho = tecla_atalho;
    botao->habilitado = true;
    botao->visivel = true;
    botao->estado = BOTAO_NORMAL;
    botao->destaque = DESTAQUE_NENHUM;
    botao->icone = IMG_NENHUMA;
    return botao_definir_rotulo(botao, rotulo);
}

Status botao_definir_rotulo(Botao *botao, const char *rotulo)
{
    if (botao == NULL || rotulo == NULL || strlen(rotulo) >= sizeof(botao->rotulo)) {
        return STATUS_ERRO_PARAMETRO;
    }
    strcpy(botao->rotulo, rotulo);
    return STATUS_OK;
}

/* ---- Entrada ---- */

bool botao_atualizar(Botao *botao)
{
    bool mouse_em_cima;

    if (botao == NULL) {
        return false;
    }
    if (!botao->visivel || !botao->habilitado) {
        botao->estado = BOTAO_DESABILITADO;
        return false;
    }

    mouse_em_cima = CheckCollisionPointRec(GetMousePosition(), botao->area);
    if (mouse_em_cima && IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        botao->estado = BOTAO_PRESSIONADO;
    } else if (mouse_em_cima) {
        botao->estado = BOTAO_HOVER;
    } else {
        botao->estado = BOTAO_NORMAL;
    }

    if (mouse_em_cima && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
        return true;
    }
    return botao->tecla_atalho != KEY_NULL && IsKeyPressed(botao->tecla_atalho);
}

bool botao_sob_mouse(const Botao *botao)
{
    return botao != NULL && botao->visivel
        && (botao->estado == BOTAO_HOVER || botao->estado == BOTAO_PRESSIONADO);
}

/* ---- Desenho ---- */

/**
 * @brief Rótulo + ícone numa linha, centralizados em `area`. Diminui a fonte (até FONTE_MINIMA_BOTAO)
 *        enquanto texto + ícone não couberem. Desabilitado: o ícone fica acinzentado como o texto.
 */
static void desenhar_texto_com_icone(const Botao *botao, const Assets *assets, Rectangle area,
                                     float tamanho_fonte, Color cor_texto)
{
    float tamanho = tamanho_fonte;
    float largura_texto = assets_largura_texto(assets, botao->rotulo, tamanho);
    float largura_total = largura_texto + ESPACO_TEXTO_ICONE + LADO_ICONE_ESMERALDA;
    Rectangle icone;
    float x;

    while (largura_total > area.width && tamanho > FONTE_MINIMA_BOTAO) {
        tamanho -= 1.0f;
        largura_texto = assets_largura_texto(assets, botao->rotulo, tamanho);
        largura_total = largura_texto + ESPACO_TEXTO_ICONE + LADO_ICONE_ESMERALDA;
    }

    x = area.x + (area.width - largura_total) / 2.0f;
    assets_desenhar_texto(assets, botao->rotulo, (Vector2){ x, area.y + (area.height - tamanho) / 2.0f },
                          tamanho, cor_texto);

    icone.x = x + largura_texto + ESPACO_TEXTO_ICONE;
    icone.y = area.y + (area.height - LADO_ICONE_ESMERALDA) / 2.0f;
    icone.width = LADO_ICONE_ESMERALDA;
    icone.height = LADO_ICONE_ESMERALDA;
    assets_desenhar_imagem(assets, botao->icone, icone, botao->habilitado ? WHITE : GRAY);
}

void botao_desenhar(const Botao *botao, const Assets *assets, float tamanho_fonte)
{
    Color cor_fundo = PALETA_BOTAO;      /* cor do retângulo se a imagem faltar */
    Color tint_imagem = WHITE;           /* cor que multiplica a imagem, se ela existir */
    Color cor_texto = PALETA_TEXTO;
    Rectangle area_texto;

    if (botao == NULL || !botao->visivel) {
        return;
    }

    /* Destaque de certo/errado vale mais que o estado do mouse */
    if (botao->destaque == DESTAQUE_CORRETO) {
        cor_fundo = PALETA_ACERTO;
        tint_imagem = PALETA_ACERTO;
    } else if (botao->destaque == DESTAQUE_ERRADO) {
        cor_fundo = PALETA_ERRO;
        tint_imagem = PALETA_ERRO;
    } else if (!botao->habilitado) {
        cor_fundo = PALETA_BOTAO_OFF;
        tint_imagem = GRAY;
        cor_texto = PALETA_TEXTO_SUAVE;
    } else if (botao->estado == BOTAO_PRESSIONADO) {
        cor_fundo = PALETA_BOTAO_HOVER;
        tint_imagem = LIGHTGRAY;
    } else if (botao->estado == BOTAO_HOVER) {
        cor_fundo = PALETA_BOTAO_HOVER;
        tint_imagem = (Color){ 200, 210, 255, 255 };
    }

    assets_desenhar_painel(assets, IMG_BOTAO, botao->area, cor_fundo, tint_imagem);

    area_texto.x = botao->area.x + MARGEM_TEXTO_BOTAO;
    area_texto.y = botao->area.y + MARGEM_TEXTO_BOTAO / 2.0f;
    area_texto.width = botao->area.width - 2.0f * MARGEM_TEXTO_BOTAO;
    area_texto.height = botao->area.height - MARGEM_TEXTO_BOTAO;

    if (botao->icone != IMG_NENHUMA) {
        desenhar_texto_com_icone(botao, assets, area_texto, tamanho_fonte, cor_texto);
        return;
    }

    /* Uma linha que cabe: centraliza. Senão quebra e diminui a fonte. */
    if (assets_largura_texto(assets, botao->rotulo, tamanho_fonte) <= area_texto.width) {
        assets_desenhar_texto_centralizado(assets, botao->rotulo, area_texto, tamanho_fonte, cor_texto);
    } else {
        assets_desenhar_texto_ajustado(assets, botao->rotulo, area_texto, tamanho_fonte,
                                       FONTE_MINIMA_BOTAO, cor_texto);
    }
}

#endif /* USE_RAYLIB */
