/**
 * @file escala.h
 * @brief Tela cheia (F11) e janela redimensionável sem refazer o layout.
 *
 * O jogo é desenhado em coordenadas virtuais de 1280x720 (layout.h). A cada quadro, este módulo
 * calcula o zoom que faz esse quadro caber na janela real sem distorcer (faixas pretas se não
 * for 16:9), aplica-o com uma Camera2D e converte o mouse de volta para 1280x720.
 * Desenhar direto na resolução da tela (e não numa textura esticada) permite manter a pixel art
 * em escala inteira (escala_alinhar_pixel_art).
 *
 * Só compila com USE_RAYLIB=1.
 */

#ifndef ESCALA_H
#define ESCALA_H

#include "raylib.h"
#include "config.h"

/** Menor tamanho da janela redimensionada (metade do virtual). */
#define LARGURA_MINIMA_JANELA (LARGURA_JANELA / 2)
#define ALTURA_MINIMA_JANELA  (ALTURA_JANELA / 2)

/** Como o quadro virtual (1280x720) está posicionado na janela real. */
typedef struct {
    Camera2D camera;      /**< Zoom e deslocamento aplicados ao desenho. */
    Rectangle area_tela;  /**< Onde o quadro 1280x720 aparece na janela, em pixels reais. */
} Escala;

/**
 * @brief Recalcula zoom, faixas pretas e a conversão do mouse. Chame 1x por quadro, antes dos botões.
 * @param largura_tela GetScreenWidth().
 * @param altura_tela  GetScreenHeight().
 */
void escala_atualizar(Escala *escala, int largura_tela, int altura_tela);

/** @brief Depois de BeginDrawing(): limpa de preto, recorta a área do jogo e ativa a câmera. */
void escala_comecar_desenho(const Escala *escala);

/** @brief Antes de EndDrawing(): desativa câmera e recorte. */
void escala_terminar_desenho(void);

/**
 * @brief Ajusta um retângulo virtual para que as bordas caiam em pixels inteiros da tela
 *        (com zoom 1,5, x = 793 cairia no pixel 1189,5).
 */
Rectangle escala_alinhar_aos_pixels(const Escala *escala, Rectangle destino);

/**
 * @brief Maior retângulo dentro de `destino` em que cada pixel da arte vira um bloco inteiro de
 *        pixels da tela, centralizado (boss 64x64 em 384: 6x na janela, 9x em 1920x1080).
 *        Se a arte não cabe nem em 1x, só encaixa mantendo a proporção.
 * @param largura_arte Largura da imagem em pixels (ex.: 64).
 * @param altura_arte  Altura da imagem em pixels.
 */
Rectangle escala_alinhar_pixel_art(const Escala *escala, Rectangle destino,
                                   int largura_arte, int altura_arte);

#endif /* ESCALA_H */
