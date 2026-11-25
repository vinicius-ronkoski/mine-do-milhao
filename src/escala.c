/**
 * @file escala.c
 * @brief Tela cheia, janela redimensionável e pixel art nítida (ver escala.h). Só com USE_RAYLIB.
 */

#ifdef USE_RAYLIB

#include <math.h>
#include <stddef.h>
#include "escala.h"

/* ========================================================================= */
/* Conversão virtual <-> tela                                                */
/* ========================================================================= */

/** @brief Coordenada virtual -> pixel da tela (mesma conta que a Camera2D faz). */
static float virtual_para_tela(float valor, float zoom, float deslocamento)
{
    return valor * zoom + deslocamento;
}

/** @brief Pixel da tela -> coordenada virtual (conta inversa). */
static float tela_para_virtual(float valor, float zoom, float deslocamento)
{
    return (valor - deslocamento) / zoom;
}

/* ========================================================================= */
/* Funções públicas                                                          */
/* ========================================================================= */

void escala_atualizar(Escala *escala, int largura_tela, int altura_tela)
{
    float zoom_x, zoom_y, zoom, largura_jogo, altura_jogo;

    if (escala == NULL || largura_tela <= 0 || altura_tela <= 0) {
        return;   /* janela minimizada: mantém a escala anterior */
    }

    /* Maior zoom que cabe nas duas direções (a menor das duas razões) */
    zoom_x = (float)largura_tela / (float)LARGURA_JANELA;
    zoom_y = (float)altura_tela / (float)ALTURA_JANELA;
    zoom = zoom_x < zoom_y ? zoom_x : zoom_y;

    /* Área do jogo centralizada; o deslocamento é arredondado para começar num pixel inteiro */
    largura_jogo = (float)LARGURA_JANELA * zoom;
    altura_jogo = (float)ALTURA_JANELA * zoom;
    escala->area_tela.x = fmaxf(0.0f, roundf(((float)largura_tela - largura_jogo) / 2.0f));
    escala->area_tela.y = fmaxf(0.0f, roundf(((float)altura_tela - altura_jogo) / 2.0f));
    escala->area_tela.width = largura_jogo;
    escala->area_tela.height = altura_jogo;

    /* Câmera: tela = virtual * zoom + offset */
    escala->camera.target = (Vector2){ 0.0f, 0.0f };
    escala->camera.offset = (Vector2){ escala->area_tela.x, escala->area_tela.y };
    escala->camera.rotation = 0.0f;
    escala->camera.zoom = zoom;

    /* Mouse: a raylib devolve (posição_real + offset) * scale -> coordenada virtual.
     * Assim botao_atualizar compara o mouse com as áreas do layout.h sem saber da escala. */
    SetMouseOffset((int)-escala->area_tela.x, (int)-escala->area_tela.y);
    SetMouseScale(1.0f / zoom, 1.0f / zoom);
}

void escala_comecar_desenho(const Escala *escala)
{
    if (escala == NULL) {
        return;
    }
    ClearBackground(BLACK);   /* cor das faixas laterais (ou de cima e de baixo) */
    BeginScissorMode((int)escala->area_tela.x, (int)escala->area_tela.y,
                     (int)ceilf(escala->area_tela.width), (int)ceilf(escala->area_tela.height));
    BeginMode2D(escala->camera);
}

void escala_terminar_desenho(void)
{
    EndMode2D();
    EndScissorMode();
}

Rectangle escala_alinhar_aos_pixels(const Escala *escala, Rectangle destino)
{
    float zoom, desloc_x, desloc_y, esquerda, topo, direita, base;

    if (escala == NULL || escala->camera.zoom <= 0.0f) {
        return destino;
    }
    zoom = escala->camera.zoom;
    desloc_x = escala->camera.offset.x;
    desloc_y = escala->camera.offset.y;

    /* Arredonda as quatro bordas em pixels da tela e volta para coordenadas virtuais */
    esquerda = roundf(virtual_para_tela(destino.x, zoom, desloc_x));
    topo = roundf(virtual_para_tela(destino.y, zoom, desloc_y));
    direita = roundf(virtual_para_tela(destino.x + destino.width, zoom, desloc_x));
    base = roundf(virtual_para_tela(destino.y + destino.height, zoom, desloc_y));

    destino.x = tela_para_virtual(esquerda, zoom, desloc_x);
    destino.y = tela_para_virtual(topo, zoom, desloc_y);
    destino.width = (direita - esquerda) / zoom;
    destino.height = (base - topo) / zoom;
    return destino;
}

Rectangle escala_alinhar_pixel_art(const Escala *escala, Rectangle destino,
                                   int largura_arte, int altura_arte)
{
    float zoom, largura_tela, altura_tela, bloco_x, bloco_y, bloco, centro_x, centro_y;
    float esquerda, topo;
    Rectangle resultado;

    if (escala == NULL || escala->camera.zoom <= 0.0f || largura_arte <= 0 || altura_arte <= 0) {
        return destino;
    }
    zoom = escala->camera.zoom;

    /* Tamanho do destino em pixels REAIS e quantos pixels da tela cabem por pixel da arte */
    largura_tela = destino.width * zoom;
    altura_tela = destino.height * zoom;
    bloco_x = largura_tela / (float)largura_arte;
    bloco_y = altura_tela / (float)altura_arte;
    bloco = floorf(bloco_x < bloco_y ? bloco_x : bloco_y);   /* inteiro: 6, 9, 12... */

    if (bloco < 1.0f) {
        /* Arte maior que o espaço: não dá escala inteira; só encaixa mantendo a proporção */
        bloco = bloco_x < bloco_y ? bloco_x : bloco_y;
    }

    /* Centraliza o retângulo inteiro (em pixels da tela) dentro do destino */
    centro_x = virtual_para_tela(destino.x + destino.width / 2.0f, zoom, escala->camera.offset.x);
    centro_y = virtual_para_tela(destino.y + destino.height / 2.0f, zoom, escala->camera.offset.y);
    esquerda = roundf(centro_x - (float)largura_arte * bloco / 2.0f);
    topo = roundf(centro_y - (float)altura_arte * bloco / 2.0f);

    resultado.x = tela_para_virtual(esquerda, zoom, escala->camera.offset.x);
    resultado.y = tela_para_virtual(topo, zoom, escala->camera.offset.y);
    resultado.width = (float)largura_arte * bloco / zoom;
    resultado.height = (float)altura_arte * bloco / zoom;
    return resultado;
}

#endif /* USE_RAYLIB */
