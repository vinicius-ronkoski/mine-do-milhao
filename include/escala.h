/**
 * @file escala.h
 * @brief Tela cheia e janela redimensionável sem refazer o layout.
 *
 * O jogo inteiro é desenhado em coordenadas VIRTUAIS de LARGURA_JANELA x ALTURA_JANELA
 * (1280x720, ver layout.h). Este módulo calcula, a cada quadro, como esse quadro virtual cabe
 * na janela real (que pode ser 1920x1080 em tela cheia, ou qualquer tamanho se o jogador
 * redimensionar):
 *   - zoom = o maior fator que cabe sem distorcer (mantém 16:9);
 *   - o que sobra vira faixas pretas (letterbox), centralizando a imagem;
 *   - o mouse é convertido de volta para coordenadas virtuais (SetMouseOffset/SetMouseScale),
 *     então os botões continuam funcionando sem nenhuma mudança.
 *
 * O desenho usa uma Camera2D com esse zoom, em vez de desenhar numa textura de 1280x720 e
 * esticá-la: assim cada imagem é desenhada direto na resolução da tela e a pixel art pode ser
 * alinhada a pixels inteiros (escala_alinhar_pixel_art), sem perder nitidez.
 *
 * Só compila com USE_RAYLIB=1.
 */

#ifndef ESCALA_H
#define ESCALA_H

#include "raylib.h"
#include "config.h"

/** Menor tamanho da janela quando o jogador a redimensiona (metade do tamanho virtual). */
#define LARGURA_MINIMA_JANELA (LARGURA_JANELA / 2)
#define ALTURA_MINIMA_JANELA  (ALTURA_JANELA / 2)

/** Como o quadro virtual (1280x720) está posicionado na janela real. */
typedef struct {
    Camera2D camera;      /**< zoom e deslocamento usados entre escala_comecar_desenho e escala_terminar_desenho. */
    Rectangle area_tela;  /**< Onde o quadro virtual aparece na janela, em pixels reais (fora dele = faixas pretas). */
} Escala;

/**
 * @brief Recalcula zoom, faixas pretas e a conversão do mouse para o tamanho atual da janela.
 *
 * Chame uma vez por quadro, ANTES de ler botões (o mouse convertido é usado por botao_atualizar).
 *
 * @param escala       Escala a atualizar.
 * @param largura_tela Largura real da janela (GetScreenWidth()).
 * @param altura_tela  Altura real da janela (GetScreenHeight()).
 */
void escala_atualizar(Escala *escala, int largura_tela, int altura_tela);

/**
 * @brief Começa o desenho em coordenadas virtuais: limpa a janela de preto, recorta a área do
 *        jogo (nada vaza para as faixas pretas) e ativa a câmera com o zoom.
 *        Chame logo depois de BeginDrawing().
 */
void escala_comecar_desenho(const Escala *escala);

/** @brief Termina o desenho virtual (desativa câmera e recorte). Chame antes de EndDrawing(). */
void escala_terminar_desenho(void);

/**
 * @brief Ajusta um retângulo virtual para que suas bordas caiam em pixels INTEIROS da tela.
 *
 * Sem isso, com zoom 1,5 um retângulo em x = 793 começaria no pixel 1189,5 e as colunas de
 * uma imagem ficariam com larguras diferentes. Use para qualquer imagem nítida.
 */
Rectangle escala_alinhar_aos_pixels(const Escala *escala, Rectangle destino);

/**
 * @brief Maior retângulo dentro de `destino` em que cada pixel da arte vira um bloco INTEIRO
 *        de pixels da tela (2x2, 3x3, 9x9...), centralizado e já alinhado aos pixels.
 *
 * Garante pixel art nítida em qualquer resolução: na janela 1280x720 um boss 64x64 em 384 px
 * fica 6x; em tela cheia 1920x1080, 9x. Numa resolução "quebrada" o boss fica um pouco menor,
 * mas nunca com pixels de tamanhos diferentes. Se a arte não cabe nem em 1x, apenas encaixa
 * mantendo a proporção.
 *
 * @param largura_arte Largura da imagem original em pixels (ex.: 64).
 * @param altura_arte  Altura da imagem original em pixels.
 */
Rectangle escala_alinhar_pixel_art(const Escala *escala, Rectangle destino,
                                   int largura_arte, int altura_arte);

#endif /* ESCALA_H */
