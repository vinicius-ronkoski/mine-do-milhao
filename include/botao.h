/**
 * @file botao.h
 * @brief Botões clicáveis (mouse) com atalho de teclado, skin por imagem e fallback sem imagem.
 *
 * Decisão: botão PRÓPRIO em vez de raygui. Motivo: precisa de skin por PNG, atalho A-D,
 * destaque de certo/errado e fallback; raygui desenharia com estilo e fonte próprios.
 *
 * Uma única imagem (IMG_BOTAO) serve para todos os estados: o estado só muda a cor.
 * O botão NÃO mexe no cursor do mouse; quem decide é jogo_atualizar(), uma vez por quadro.
 *
 * Só compila com USE_RAYLIB=1.
 */
#ifndef BOTAO_H
#define BOTAO_H

#include "raylib.h"
#include "config.h"
#include "assets.h"

/** Estado do botão no quadro atual (calculado por botao_atualizar). */
typedef enum {
    BOTAO_NORMAL = 0,      /**< Mouse longe. */
    BOTAO_HOVER,           /**< Mouse em cima. */
    BOTAO_PRESSIONADO,     /**< Botão esquerdo segurado em cima. */
    BOTAO_DESABILITADO     /**< habilitado == false. */
} EstadoBotao;

/** Destaque usado na revelação da resposta (tem prioridade sobre o estado). */
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
} Botao;

/**
 * @brief Inicializa um botão (visível, habilitado, sem destaque).
 * @return STATUS_OK ou STATUS_ERRO_PARAMETRO (NULL ou rótulo maior que o buffer).
 */
Status botao_criar(Botao *botao, Rectangle area, const char *rotulo, int tecla_atalho);

/** @brief Troca o texto. STATUS_ERRO_PARAMETRO se NULL ou longo demais (o texto antigo fica). */
Status botao_definir_rotulo(Botao *botao, const char *rotulo);

/**
 * @brief Atualiza o estado e detecta o acionamento. Chame 1x por quadro para cada botão da tela ativa.
 *
 * Acionado = soltou o botão esquerdo do mouse em cima dele, OU apertou a tecla de atalho.
 * Botão invisível ou desabilitado nunca é acionado.
 *
 * @return true se foi acionado neste quadro.
 */
bool botao_atualizar(Botao *botao);

/** @brief true se o mouse está em cima (estado HOVER ou PRESSIONADO): usado para o cursor de "mãozinha". */
bool botao_sob_mouse(const Botao *botao);

/**
 * @brief Desenha o botão: skin IMG_BOTAO (ou retângulo, se faltar) na cor do estado/destaque,
 *        com o rótulo diminuindo a fonte até caber.
 * @param tamanho_fonte Tamanho máximo do texto.
 */
void botao_desenhar(const Botao *botao, const Assets *assets, float tamanho_fonte);

#endif /* BOTAO_H */
