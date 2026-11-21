/**
 * @file botao.h
 * @brief Botões clicáveis (mouse) com atalho de teclado, skin por assets e fallback sem imagem.
 *
 * Decisão: botão PRÓPRIO em vez de raygui. Motivo: precisa de skin por PNG, atalho A-D/1-4,
 * som de clique e fallback; raygui desenharia com estilo próprio e fonte própria.
 * Só compila com USE_RAYLIB=1.
 */
#ifndef BOTAO_H
#define BOTAO_H

#include <stdbool.h>
#include "raylib.h"
#include "config.h"
#include "assets.h"

/** Estado visual do botão (escolhe a skin). */
typedef enum { BOTAO_NORMAL = 0, BOTAO_HOVER, BOTAO_PRESSIONADO, BOTAO_DESABILITADO } EstadoBotao;

/** Botão de tela. */
typedef struct {
    Rectangle area;                              /**< Posição e tamanho em pixels. */
    char rotulo[TAM_ALTERNATIVA + 16];           /**< Texto (cabe "A) " + alternativa). */
    int tecla_atalho;                            /**< KEY_* da raylib, ou KEY_NULL. */
    bool habilitado;                             /**< false: desenha cinza e ignora cliques. */
    bool visivel;                                /**< false: não desenha nem reage. */
    EstadoBotao estado;
} Botao;

/**
 * @brief Inicializa um botão.
 * @return STATUS_OK ou STATUS_ERRO_PARAMETRO (NULL ou rótulo maior que o buffer).
 */
Status botao_criar(Botao *botao, Rectangle area, const char *rotulo, int tecla_atalho);

/** @brief Troca o texto (ex.: ao mudar de pergunta). STATUS_ERRO_PARAMETRO se NULL ou longo demais. */
Status botao_definir_rotulo(Botao *botao, const char *rotulo);

/**
 * @brief Posiciona 4 botões em grade 2x2 na faixa inferior da tela (alternativas A-D).
 * Para mudar o layout (1 coluna, outra posição), altere só esta função.
 * @param botoes Vetor com NUM_ALTERNATIVAS botões já criados.
 */
void botao_layout_alternativas(Botao botoes[NUM_ALTERNATIVAS]);

/**
 * @brief Atualiza estado (hover/pressionado) e detecta clique OU atalho. Chamar 1x por frame.
 * Passos: ignorar se !visivel/!habilitado; CheckCollisionPointRec(GetMousePosition(), area);
 * IsMouseButtonDown/Released; IsKeyPressed(tecla_atalho); hover -> SetMouseCursor(POINTING_HAND).
 * @return true se foi acionado neste frame.
 */
bool botao_atualizar(Botao *botao);

/**
 * @brief Desenha o botão. Usa IMG_BOTAO_* conforme o estado; se a imagem for fallback,
 *        desenha retângulo + borda coloridos (DrawRectangleRec) para continuar legível.
 *        Texto centralizado com assets->fonte.
 */
void botao_desenhar(const Botao *botao, const Assets *assets);

#endif /* BOTAO_H */
