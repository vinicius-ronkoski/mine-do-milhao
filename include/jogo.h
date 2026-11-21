/**
 * @file jogo.h
 * @brief Loop principal visual (raylib): janela, troca de telas e orquestração dos módulos.
 *
 * O Jogo guarda TODO o estado (assets, partida, diálogo, botões): nenhuma variável global.
 * Só compila com USE_RAYLIB=1.
 */
#ifndef JOGO_H
#define JOGO_H

#include <stdbool.h>
#include "raylib.h"
#include "config.h"
#include "banco.h"
#include "assets.h"
#include "partida.h"
#include "dialogo.h"
#include "botao.h"

/** Telas do jogo base (menu inicial e minigame ficam fora desta etapa). */
typedef enum {
    TELA_DIALOGO = 0,   /**< Cena de diálogo (intro, entrada de boss, final). */
    TELA_PERGUNTA,      /**< Pergunta + alternativas + botão PARAR. */
    TELA_REVELACAO,     /**< Mostra certo/errado e o dano no boss. */
    TELA_RESULTADO,     /**< Prêmio final em esmeraldas. */
    TELA_SAIR           /**< Encerra o loop. */
} TelaJogo;

/** Estado completo da aplicação gráfica. */
typedef struct {
    const Banco *banco;                   /**< Banco carregado (somente leitura durante o jogo). */
    Assets assets;
    Partida partida;
    Dialogo dialogo;
    Botao alternativas[NUM_ALTERNATIVAS]; /**< Botões A-D (rótulos mudam a cada pergunta). */
    Botao botao_parar;                    /**< "Parar e levar as esmeraldas". */
    Botao botao_continuar;                /**< Usado na revelação e no resultado. */
    TelaJogo tela;                        /**< Tela ativa. */
    TelaJogo tela_apos_dialogo;           /**< Para onde ir quando o diálogo acabar. */
    float tempo_na_tela;                  /**< Segundos desde que a tela atual começou (animações). */
    Status ultimo_erro;                   /**< Último erro não fatal, exibido num banner. */
} Jogo;

/**
 * @brief Ponto de entrada visual, chamado pelo main.c quando USE_RAYLIB está definido.
 *
 * Passos: SetConfigFlags(FLAG_VSYNC_HINT); InitWindow(LARGURA_JANELA, ALTURA_JANELA, TITULO_JANELA);
 * SetTargetFPS(FPS_ALVO); sorteio_semear(time(NULL)); jogo_iniciar(); loop
 * `while (!WindowShouldClose() && jogo.tela != TELA_SAIR)` { jogo_atualizar; BeginDrawing; jogo_desenhar;
 * EndDrawing }; jogo_encerrar(); CloseWindow().
 *
 * @param banco Banco carregado por dados_carregar() (main.c continua salvando ao sair).
 * @return STATUS_OK ou erro fatal (STATUS_ERRO_GRAFICO, STATUS_NAO_ENCONTRADO se o CSV não tem
 *         perguntas suficientes por nível, STATUS_ERRO_MEMORIA).
 */
Status jogo_executar(const Banco *banco);

/**
 * @brief Prepara o Jogo (depois que a janela existe): assets_carregar, botões, nova partida, cena de intro.
 * @return STATUS_OK, STATUS_ASSET_FALLBACK (continua jogável) ou erro fatal.
 */
Status jogo_iniciar(Jogo *jogo, const Banco *banco);

/** @brief Libera assets e zera o Jogo. */
void jogo_encerrar(Jogo *jogo);

/** @brief Inicia partida nova: partida_iniciar() + cena CENA_INTRO + tela_apos_dialogo = TELA_PERGUNTA. */
Status jogo_nova_partida(Jogo *jogo);

/**
 * @brief Atualiza a tela ativa (despacha para tela_*_atualizar), a música e o cronômetro da tela.
 * @param dt GetFrameTime().
 */
void jogo_atualizar(Jogo *jogo, float dt);

/** @brief Desenha a tela ativa (despacha para tela_*_desenhar) e o banner de erro, se houver. */
void jogo_desenhar(const Jogo *jogo);

/** @brief Troca de tela e zera tempo_na_tela. */
void jogo_trocar_tela(Jogo *jogo, TelaJogo nova_tela);

/**
 * @brief Toca uma cena de diálogo e, ao terminar, vai para `depois`.
 * @return STATUS_OK ou erro de dialogo_carregar_cena().
 */
Status jogo_tocar_cena(Jogo *jogo, CenaId cena, TelaJogo depois);

/**
 * @brief Preenche os rótulos dos botões A-D da pergunta atual
 *        ("A) " + sorteio_texto_alternativa()) e reabilita todos.
 */
Status jogo_preparar_botoes_pergunta(Jogo *jogo);

/**
 * @brief Registra erro não fatal: guarda em ultimo_erro, escreve utils_mensagem_status() em stderr.
 * @param contexto Texto curto ("carregar fonte", "sortear perguntas").
 */
void jogo_registrar_erro(Jogo *jogo, Status status, const char *contexto);

#endif /* JOGO_H */
