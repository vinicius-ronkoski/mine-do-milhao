/**
 * @file tela.h
 * @brief Entrada (atualizar) e desenho (desenhar) de cada tela + helpers visuais compartilhados.
 *
 * Padrão: tela_X_atualizar(Jogo*, dt) lê entrada e chama funções de partida.h;
 * tela_X_desenhar(const Jogo*) só desenha (nunca altera a partida).
 * Só compila com USE_RAYLIB=1.
 */
#ifndef TELA_H
#define TELA_H

#include "raylib.h"
#include "jogo.h"

/* ---------- Tela de diálogo ---------- */

/** @brief dialogo_atualizar(); ao terminar -> jogo_trocar_tela(jogo->tela_apos_dialogo). */
void tela_dialogo_atualizar(Jogo *jogo, float dt);

/** @brief Fundo IMG_FUNDO_DIALOGO + dialogo_desenhar(). */
void tela_dialogo_desenhar(const Jogo *jogo);

/* ---------- Tela de pergunta ---------- */

/**
 * @brief Lê botões A-D e PARAR.
 * Alternativa acionada -> partida_responder(pos), assets_tocar_som(SOM_CLIQUE), tela REVELAÇÃO.
 * PARAR -> partida_parar(), tela RESULTADO (CENA_PARADA antes, se desejado).
 * Erros de Status -> jogo_registrar_erro().
 */
void tela_pergunta_atualizar(Jogo *jogo, float dt);

/** @brief Fundo, HUD, painel do enunciado, botões A-D, botão PARAR, boss e escada de prêmios. */
void tela_pergunta_desenhar(const Jogo *jogo);

/* ---------- Tela de revelação ---------- */

/**
 * @brief Toca SOM_ACERTO/SOM_ERRO uma vez (tempo_na_tela ~ 0); espera botão continuar/ENTER.
 * Ao continuar: partida_confirmar_revelacao(); depois decide:
 *   - boss derrotado (boss_derrotado) -> jogo_tocar_cena(CENA_BOSS_DERROTADO) e, se o próximo
 *     nível tem outro boss, cena dialogo_cena_do_boss(); 
 *   - partida_encerrada -> TELA_RESULTADO (CENA_VITORIA/CENA_DERROTA);
 *   - senão -> jogo_preparar_botoes_pergunta() e TELA_PERGUNTA.
 */
void tela_revelacao_atualizar(Jogo *jogo, float dt);

/** @brief Mesma tela da pergunta, com a alternativa correta em verde e a escolhida errada em vermelho; boss piscando ao tomar dano. */
void tela_revelacao_desenhar(const Jogo *jogo);

/* ---------- Tela de resultado ---------- */

/** @brief Botão "Jogar de novo" -> jogo_nova_partida(); ESC/janela fecha -> TELA_SAIR. */
void tela_resultado_atualizar(Jogo *jogo, float dt);

/** @brief Fundo IMG_FUNDO_RESULTADO, partida_premio_final() em esmeraldas, mensagem por estado (parou/errou/venceu). */
void tela_resultado_desenhar(const Jogo *jogo);

/* ---------- Helpers de desenho ---------- */

/** @brief Desenha o fundo `id` cobrindo a janela inteira (assets_desenhar_imagem). */
void tela_desenhar_fundo(const Assets *assets, ImagemId id);

/** @brief Texto centralizado horizontalmente em `area` com a fonte do jogo. */
void tela_desenhar_texto_centralizado(const Assets *assets, const char *texto, Rectangle area, float tamanho, Color cor);

/**
 * @brief Texto com quebra automática de linha dentro de `area` (usa MeasureTextEx palavra a palavra).
 * @param limite_caracteres Se > 0, desenha só os N primeiros caracteres (efeito máquina de escrever;
 *        cuidado para não cortar no meio de um caractere UTF-8: usar GetCodepointNext).
 */
void tela_desenhar_texto_com_quebra(const Assets *assets, const char *texto, Rectangle area,
                                    float tamanho, Color cor, int limite_caracteres);

/** @brief HUD: "Pergunta N/15", nível (pergunta_nome_nivel), ícone + esmeraldas acumuladas e garantidas. */
void tela_desenhar_hud(const Jogo *jogo);

/** @brief Boss atual (animação assets_quadro_animacao) + nome + barra de vida (boss_fracao_vida). */
void tela_desenhar_boss(const Jogo *jogo, Rectangle area);

/** @brief Barra horizontal de vida: fundo, preenchimento proporcional, borda. */
void tela_desenhar_barra_vida(Rectangle area, float fracao, Color cor);

/** @brief Escada lateral com as 15 rodadas e prêmios (partida_premio_da_rodada), marcando a atual e os marcos. */
void tela_desenhar_escada_premios(const Jogo *jogo, Rectangle area);

/** @brief Faixa vermelha no rodapé com utils_mensagem_status(jogo->ultimo_erro). */
void tela_desenhar_banner_erro(const Jogo *jogo);

#endif /* TELA_H */
