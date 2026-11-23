/**
 * @file jogo.h
 * @brief Laço principal visual (raylib): janela, troca de telas e orquestração dos módulos.
 *
 * Quem chama: a opção "8 - JOGAR" do menu de texto (menu.c) chama jogo_executar().
 * O Jogo guarda TODO o estado (assets, partida, diálogo, botões): nenhuma variável global.
 *
 * Cada quadro (60 por segundo):
 *   jogo_atualizar()  -> lê entrada da tela ativa (tela_X_atualizar) e chama partida.h
 *   jogo_desenhar()   -> desenha a tela ativa (tela_X_desenhar), sem alterar nada
 *
 * Telas: DIALOGO -> PERGUNTA <-> REVELACAO -> ... -> RESULTADO. ESC ou fechar a janela volta ao menu.
 * Só compila com USE_RAYLIB=1.
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

/** Segundos em que uma tela recém-aberta ignora cliques (evita o clique da tela anterior "vazar"). */
#define ATRASO_ENTRADA_TELA 0.20f

/** Telas do jogo base (menu gráfico e minigame ficam fora desta etapa). */
typedef enum {
    TELA_DIALOGO = 0,   /**< Cena de diálogo (intro, entrada de boss, final). */
    TELA_PERGUNTA,      /**< Pergunta + alternativas + ajudas + botão PARAR. */
    TELA_REVELACAO,     /**< Mostra certo/errado e o dano no boss. */
    TELA_RESULTADO,     /**< Prêmio final em esmeraldas. */
    TELA_SAIR           /**< Encerra o laço e volta ao menu de texto. */
} TelaJogo;

/** Estado completo da parte gráfica. */
typedef struct {
    const Banco *banco;                   /**< Banco carregado (somente leitura durante o jogo). */
    Assets assets;                        /**< Imagens, sons, fonte (com fallback). */
    Partida partida;                      /**< Regras e estado da partida (partida.h). */
    Dialogo dialogo;                      /**< Falas em reprodução. */

    Botao alternativas[NUM_ALTERNATIVAS]; /**< Botões A-D (rótulos mudam a cada pergunta). */
    Botao botao_parar;                    /**< "Parar e levar as esmeraldas" (tecla P). */
    Botao botao_dica;                     /**< Ajuda: comprar a dica com esmeraldas (tecla H). */
    Botao botao_eliminar;                 /**< Ajuda: eliminar duas alternativas (tecla E). */
    Botao botao_continuar;                /**< Revelação e resultado (tecla ENTER). */

    TelaJogo tela;                        /**< Tela ativa. */
    TelaJogo tela_apos_dialogo;           /**< Para onde ir quando o diálogo acabar. */
    float tempo_na_tela;                  /**< Segundos desde que a tela atual começou. */
    float tempo_total;                    /**< Segundos desde o início (animação contínua dos bosses). */
    bool efeito_da_tela_tocado;           /**< Para tocar um som só uma vez por tela (zera ao trocar). */
    int cursor_atual;                     /**< Cursor do mouse em uso (MOUSE_CURSOR_*). */
    Status ultimo_erro;                   /**< Último erro não fatal, mostrado num banner. */
} Jogo;

/**
 * @brief Abre a janela, roda o jogo e volta quando o jogador fecha (chamada pelo menu).
 *
 * Antes de abrir a janela confere se o banco tem perguntas suficientes por nível
 * (o cadastro permite excluir perguntas). Assets ausentes NÃO impedem o jogo.
 *
 * @param banco Banco carregado por dados_carregar() (o main continua salvando ao sair).
 * @return STATUS_OK; STATUS_ASSET_FALLBACK (jogou com substitutos); STATUS_NAO_ENCONTRADO
 *         (perguntas insuficientes); STATUS_ERRO_GRAFICO; STATUS_ERRO_PARAMETRO.
 */
Status jogo_executar(const Banco *banco);

/**
 * @brief Prepara o Jogo depois que a janela existe: assets, botões, música e primeira partida.
 * @return STATUS_OK, STATUS_ASSET_FALLBACK (continua jogável) ou erro fatal.
 */
Status jogo_iniciar(Jogo *jogo, const Banco *banco);

/** @brief Libera os assets e devolve o cursor normal. */
void jogo_encerrar(Jogo *jogo);

/**
 * @brief Começa uma partida nova: sorteia as perguntas, prepara os botões e toca
 *        a introdução seguida da entrada do primeiro boss.
 */
Status jogo_nova_partida(Jogo *jogo);

/** @brief Atualiza música, relógios, a tela ativa (tela_X_atualizar) e o cursor. */
void jogo_atualizar(Jogo *jogo, float dt);

/** @brief Desenha a tela ativa (tela_X_desenhar) e o banner de erro, se houver. */
void jogo_desenhar(const Jogo *jogo);

/** @brief Troca de tela, zera tempo_na_tela e efeito_da_tela_tocado e solta os botões. */
void jogo_trocar_tela(Jogo *jogo, TelaJogo nova_tela);

/**
 * @brief Começa a tocar uma cena e, quando acabar, vai para `depois`.
 * Para tocar mais de uma cena seguida, chame jogo_anexar_cena() logo em seguida.
 * Se a cena falhar, registra o erro e vai direto para `depois` (o jogo não trava).
 */
Status jogo_tocar_cena(Jogo *jogo, CenaId cena, TelaJogo depois);

/** @brief Acrescenta uma cena ao diálogo que está tocando (ex.: boss derrotado + próximo boss). */
Status jogo_anexar_cena(Jogo *jogo, CenaId cena);

/**
 * @brief Depois de partida_responder(): toca a reação do apresentador (acerto ou erro, variação
 *        sorteada, citando a resposta certa) e, quando ela acaba, vai para TELA_REVELACAO.
 */
Status jogo_tocar_reacao(Jogo *jogo);

/**
 * @brief Prepara os botões da pergunta atual: rótulos "A) ...", alternativas eliminadas
 *        desabilitadas, ajudas já usadas desabilitadas, sem destaque.
 *        Chame ao entrar na tela de pergunta e depois de usar uma ajuda.
 */
Status jogo_preparar_botoes_pergunta(Jogo *jogo);

/**
 * @brief Registra erro não fatal: guarda em ultimo_erro (banner) e escreve no terminal.
 * @param contexto Texto curto ("carregar cena", "responder").
 */
void jogo_registrar_erro(Jogo *jogo, Status status, const char *contexto);

/** @brief true se o jogador pediu para avançar neste quadro: ESPAÇO, ENTER ou clique (ao soltar). */
bool jogo_jogador_quer_avancar(void);

/**
 * @brief Atualiza um botão da tela ativa e diz se ele foi acionado, já tocando o SOM_CLIQUE.
 *
 * É o jeito padrão de ler botões nas telas (em vez de chamar botao_atualizar direto):
 * todo botão do jogo faz o mesmo som, e cliques nos primeiros ATRASO_ENTRADA_TELA segundos
 * de uma tela são ignorados (o estado de hover continua sendo atualizado).
 *
 * @return true se o botão foi acionado neste quadro.
 */
bool jogo_botao_acionado(Jogo *jogo, Botao *botao);

#endif /* JOGO_H */
