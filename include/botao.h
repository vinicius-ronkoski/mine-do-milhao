/**
 * @file botao.h
 * @brief Botões clicáveis com atalho de teclado, skin IMG_BOTAO (cor muda pelo estado) e fallback.
 *
 * Criados em jogo.c (criar_botoes), lidos pelas telas via jogo_botao_acionado e desenhados com
 * botao_desenhar. Próprio em vez de raygui para ter skin PNG, atalhos A-D e destaque certo/errado.
 * Só compila com USE_RAYLIB=1.
 */
#ifndef BOTAO_H
#define BOTAO_H

#include "raylib.h"
#include "config.h"
#include "assets.h"

/** Estado do botão no quadro atual (botao_atualizar). */
typedef enum {
    BOTAO_NORMAL = 0,      /**< Mouse longe. */
    BOTAO_HOVER,           /**< Mouse em cima. */
    BOTAO_PRESSIONADO,     /**< Botão esquerdo segurado em cima. */
    BOTAO_DESABILITADO     /**< habilitado == false. */
} EstadoBotao;

/** Destaque da revelação (vale mais que o estado). */
typedef enum {
    DESTAQUE_NENHUM = 0,   /**< Cores normais. */
    DESTAQUE_CORRETO,      /**< Verde: era a resposta certa. */
    DESTAQUE_ERRADO        /**< Vermelho: o jogador escolheu esta e errou. */
} DestaqueBotao;

/** Tamanho do texto do botão: cabe "A) " + uma alternativa inteira. */
#define TAM_ROTULO_BOTAO (TAM_ALTERNATIVA + 16)

/** Botão de tela. */
typedef struct {
    Rectangle area;                   /**< Posição e tamanho em pixels. */
    char rotulo[TAM_ROTULO_BOTAO];    /**< Texto (UTF-8). */
    int tecla_atalho;                 /**< KEY_* da raylib, ou KEY_NULL para nenhum. */
    bool habilitado;                  /**< false: desenha apagado e ignora cliques. */
    bool visivel;                     /**< false: não desenha nem reage. */
    EstadoBotao estado;               /**< Atualizado por botao_atualizar(). */
    DestaqueBotao destaque;           /**< Certo/errado na revelação. */
    ImagemId icone;                   /**< Ícone após o texto (ex.: IMG_ESMERALDA no preço) ou IMG_NENHUMA. */
    ImagemId skin;                    /**< Textura do fundo; IMG_BOTAO por padrão (botao_criar). */
} Botao;

/**
 * @brief Inicializa um botão (visível, habilitado, sem destaque e sem ícone).
 * @return STATUS_OK ou STATUS_ERRO_PARAMETRO (NULL ou rótulo maior que o buffer).
 */
Status botao_criar(Botao *botao, Rectangle area, const char *rotulo, int tecla_atalho);

/** @brief Troca o texto. STATUS_ERRO_PARAMETRO se NULL ou longo demais (o texto antigo fica). */
Status botao_definir_rotulo(Botao *botao, const char *rotulo);

/**
 * @brief Atualiza o estado; 1x por quadro por botão (as telas usam jogo_botao_acionado).
 * @return true se acionado: soltou o mouse em cima ou apertou o atalho (nunca se invisível/desabilitado).
 */
bool botao_atualizar(Botao *botao);

/** @brief true se o mouse está em cima (cursor de "mãozinha" em jogo_atualizar). */
bool botao_sob_mouse(const Botao *botao);

/**
 * @brief Desenha skin e rótulo na cor do estado/destaque, reduzindo a fonte até caber.
 * @param tamanho_fonte Tamanho máximo do texto.
 */
void botao_desenhar(const Botao *botao, const Assets *assets, float tamanho_fonte);

#endif /* BOTAO_H */
