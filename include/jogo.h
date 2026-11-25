/**
 * @file jogo.h
 * @brief Jogo visual (raylib): janela, laço principal, troca de telas e orquestração dos módulos.
 *
 * Fluxo: menu de texto (opção Jogar) -> jogo_executar -> a cada quadro jogo_atualizar
 * (escala, tela_X_atualizar -> partida.h) e jogo_desenhar (tela_X_desenhar).
 * Telas: DIALOGO -> PERGUNTA -> (reação) -> REVELACAO -> ... -> RESULTADO. ESC volta ao menu; F11 = tela cheia.
 * O Jogo guarda todo o estado (sem globais). Só compila com USE_RAYLIB=1.
 */
#ifndef JOGO_H
#define JOGO_H

#include "raylib.h"
#include "config.h"
#include "banco.h"
#include "assets.h"
#include "partida.h"
#include "dialogo.h"
#include "botao.h"

/** Alterna a tela cheia (escala.h). */
#define TECLA_TELA_CHEIA KEY_F11

/** Segundos que o banner de erro fica visível. */
#define DURACAO_BANNER_ERRO 5.0f

/** Segundos em que uma tela nova ignora cliques (o clique anterior não "vaza"). */
#define ATRASO_ENTRADA_TELA 0.20f

/** Telas do jogo. */
typedef enum {
    TELA_DIALOGO = 0,   /**< Falas (história, reação, entrada de boss). */
    TELA_PERGUNTA,      /**< Pergunta, alternativas, ajudas e Parar. */
    TELA_REVELACAO,     /**< Resultado da resposta e dano no boss. */
    TELA_RESULTADO,     /**< Prêmio final. */
    TELA_SAIR           /**< Encerra o laço (volta ao menu de texto). */
} TelaJogo;

/** Estado completo da parte gráfica (vive em jogo_executar). */
typedef struct {
    const Banco *banco;                   /**< Só leitura durante o jogo. */
    Assets assets;                        /**< Imagens, sons, fonte e escala da tela. */
    Partida partida;                      /**< Regras e estado (partida.h). */
    Dialogo dialogo;                      /**< Falas em reprodução. */

    Botao alternativas[NUM_ALTERNATIVAS]; /**< A-D (teclas A-D). */
    Botao botao_parar;                    /**< Tecla P. */
    Botao botao_dica;                     /**< Tecla H. */
    Botao botao_eliminar;                 /**< Tecla E. */
    Botao botao_continuar;                /**< Revelação e resultado (ENTER). */

    TelaJogo tela;                        /**< Tela ativa. */
    TelaJogo tela_apos_dialogo;           /**< Para onde ir quando o diálogo acabar. */
    float tempo_na_tela;                  /**< Segundos na tela atual. */
    float tempo_total;                    /**< Segundos desde o início (animação dos bosses). */
    bool efeito_da_tela_tocado;           /**< Som de entrada da tela já tocado. */
    int cursor_atual;                     /**< MOUSE_CURSOR_* em uso. */
    Status ultimo_erro;                   /**< Erro não fatal mostrado no banner. */
    int ultima_variante_reacao;           /**< Para não repetir a reação seguida (-1 = nenhuma). */
    float tempo_do_erro;                  /**< Tempo restante do banner. */
} Jogo;

/**
 * @brief Chamada pelo menu (tela_jogar): confere o banco, abre a janela, roda o laço e fecha.
 * @return STATUS_OK; STATUS_ASSET_FALLBACK; STATUS_NAO_ENCONTRADO (perguntas insuficientes);
 *         STATUS_ERRO_GRAFICO; STATUS_ERRO_PARAMETRO.
 */
Status jogo_executar(const Banco *banco);

/**
 * @brief Depois da janela: assets_carregar, botões, música e jogo_nova_partida.
 * @return STATUS_OK, STATUS_ASSET_FALLBACK (jogável) ou erro fatal.
 */
Status jogo_iniciar(Jogo *jogo, const Banco *banco);

/** @brief Libera os assets e devolve o cursor normal. */
void jogo_encerrar(Jogo *jogo);

/** @brief partida_iniciar, prepara os botões e toca intro + HISTORIA_1 + entrada do 1º boss (também no Reiniciar). */
Status jogo_nova_partida(Jogo *jogo);

/** @brief Por quadro: F11, escala, fonte, música, relógios, tela_X_atualizar e cursor. */
void jogo_atualizar(Jogo *jogo, float dt);

/** @brief Desenha a tela ativa (tela_X_desenhar) e o banner de erro. */
void jogo_desenhar(const Jogo *jogo);

/** @brief Troca de tela, zera os relógios da tela e solta os botões. */
void jogo_trocar_tela(Jogo *jogo, TelaJogo nova_tela);

/**
 * @brief Toca a cena e depois vai para `depois` (encadeie outras com jogo_anexar_cena).
 *        Se a cena falhar, registra o erro e pula direto para `depois`.
 */
Status jogo_tocar_cena(Jogo *jogo, CenaId cena, TelaJogo depois);

/** @brief Acrescenta uma cena ao diálogo atual (ex.: história + entrada do próximo boss). */
Status jogo_anexar_cena(Jogo *jogo, CenaId cena);

/** @brief Após partida_responder: toca a reação (dialogo_anexar_reacao) e depois vai para TELA_REVELACAO. */
Status jogo_tocar_reacao(Jogo *jogo);

/** @brief Rótulos "A) ..." e preços, habilita/desabilita alternativas e ajudas. Chame a cada pergunta e após uma ajuda. */
Status jogo_preparar_botoes_pergunta(Jogo *jogo);

/**
 * @brief Erro não fatal: mostra no banner (DURACAO_BANNER_ERRO) e escreve no terminal.
 * @param contexto Texto curto ("carregar cena", "pergunta").
 */
void jogo_registrar_erro(Jogo *jogo, Status status, const char *contexto);

/** @brief true com ESPAÇO, ENTER ou clique (ao soltar) neste quadro. */
bool jogo_jogador_quer_avancar(void);

/**
 * @brief Jeito padrão das telas lerem um botão: botao_atualizar + SOM_CLIQUE, ignorando cliques
 *        nos primeiros ATRASO_ENTRADA_TELA segundos da tela.
 * @return true se acionado neste quadro.
 */
bool jogo_botao_acionado(Jogo *jogo, Botao *botao);

#endif /* JOGO_H */
