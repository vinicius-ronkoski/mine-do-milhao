/**
 * @file tela.h
 * @brief Entrada (atualizar) e desenho (desenhar) de cada tela + desenhos compartilhados.
 *
 * Padrão de toda tela:
 *   tela_X_atualizar(Jogo*, dt) -> lê botões/teclas e chama funções de partida.h
 *   tela_X_desenhar(const Jogo*) -> SÓ desenha (nunca altera a partida)
 *
 * Ferramentas disponíveis para as telas:
 *   - texto:  assets_desenhar_texto / _centralizado / _quebrado / _ajustado (assets.h)
 *   - imagem: assets_desenhar_fundo (fundo da fase), assets_desenhar_imagem, assets_desenhar_painel,
 *             assets_desenhar_boss (assets.h)
 *   - fase:   tela_boss_da_fase(jogo) diz qual boss está em jogo (para fundo, cores, sprite)
 *   - som:    assets_tocar_som (assets.h); o clique dos botões já é tocado por jogo_botao_acionado
 *   - botões: jogo_botao_acionado (jogo.h) para ler, botao_desenhar (botao.h) para desenhar;
 *             estão em jogo->alternativas, jogo->botao_parar, botao_dica, botao_eliminar, botao_continuar
 *   - regras: partida_responder, partida_parar, partida_comprar_dica, partida_eliminar_duas,
 *             partida_confirmar_revelacao, partida_premio_final... (partida.h)
 *   - fluxo:  jogo_trocar_tela, jogo_tocar_cena, jogo_anexar_cena, jogo_preparar_botoes_pergunta
 *
 * Layout sugerido (1280x720), o mesmo usado para posicionar os botões em jogo.c:
 *   HUD y 16-56 | boss x 30, y 70, 180x180 (nome e barra à direita dele)
 *   enunciado x 30, y 270, 920x150 | alternativas 2x2 a partir de y 435
 *   PARAR / DICA / ELIMINAR em y 645 | escada de prêmios x 980, y 16, 280x688
 *
 * Só compila com USE_RAYLIB=1.
 */
#ifndef TELA_H
#define TELA_H

#include "raylib.h"
#include "jogo.h"

/* ---------- Tela de diálogo (IMPLEMENTADA) ---------- */

/** @brief Avança o diálogo com ESPAÇO/ENTER/clique; ao terminar vai para jogo->tela_apos_dialogo. */
void tela_dialogo_atualizar(Jogo *jogo, float dt);

/** @brief Fundo IMG_FUNDO_DIALOGO, boss da rodada ao centro e a caixa de diálogo. */
void tela_dialogo_desenhar(const Jogo *jogo);

/* ---------- Tela de pergunta (A IMPLEMENTAR) ---------- */

/**
 * @brief Lê os botões da pergunta. Passos sugeridos (todo botão via jogo_botao_acionado, que já
 *        toca o clique e ignora cliques nos primeiros ATRASO_ENTRADA_TELA segundos):
 *   1. Para cada alternativa: se jogo_botao_acionado(jogo, &jogo->alternativas[posicao])
 *      -> partida_responder(posicao) e jogo_trocar_tela(TELA_REVELACAO).
 *   2. botao_parar -> partida_parar(); jogo_tocar_cena(CENA_PARADA, TELA_RESULTADO).
 *   3. botao_dica -> partida_comprar_dica(); jogo_preparar_botoes_pergunta() (desabilita o botão).
 *      O botão já vem desabilitado sem saldo (partida_pode_comprar_dica) e mostra o preço.
 *   4. botao_eliminar -> partida_eliminar_duas(); jogo_preparar_botoes_pergunta().
 *   5. Status diferente de STATUS_OK -> jogo_registrar_erro().
 */
void tela_pergunta_atualizar(Jogo *jogo, float dt);

/**
 * @brief Fundo da fase (assets_desenhar_fundo com tela_boss_da_fase), HUD, boss, painel do enunciado
 *        (assets_desenhar_texto_ajustado), botões, dica (partida_dica_atual, se não for NULL) e escada de prêmios.
 */
void tela_pergunta_desenhar(const Jogo *jogo);

/* ---------- Tela de revelação (A IMPLEMENTAR) ---------- */

/**
 * @brief Mostra o resultado da resposta. Passos sugeridos:
 *   1. Se !jogo->efeito_da_tela_tocado: tocar SOM_ACERTO ou SOM_ERRO (partida.ultima_acertou),
 *      marcar destaque dos botões (DESTAQUE_CORRETO na posicao_correta, DESTAQUE_ERRADO na escolha errada)
 *      e efeito_da_tela_tocado = true.
 *   2. jogo_botao_acionado(jogo, &jogo->botao_continuar) -> partida_confirmar_revelacao(); então:
 *      - partida.boss_derrotado_agora (NÃO use boss_derrotado(partida_boss_atual()): depois de
 *        confirmar, o boss "atual" já é o do próximo nível) -> jogo_tocar_cena(CENA_BOSS_DERROTADO, ...)
 *        e, se a partida continua, jogo_anexar_cena(dialogo_cena_do_boss(próximo boss));
 *      - partida_encerrada() -> cena CENA_VITORIA ou CENA_DERROTA e depois TELA_RESULTADO;
 *      - senão -> jogo_preparar_botoes_pergunta() e TELA_PERGUNTA.
 */
void tela_revelacao_atualizar(Jogo *jogo, float dt);

/** @brief Igual à tela de pergunta, com as cores de certo/errado e o boss piscando ao tomar dano. */
void tela_revelacao_desenhar(const Jogo *jogo);

/* ---------- Tela de resultado (A IMPLEMENTAR) ---------- */

/** @brief jogo_botao_acionado(jogo, &jogo->botao_continuar) ("Jogar de novo") -> jogo_nova_partida(); ESC fecha a janela e volta ao menu. */
void tela_resultado_atualizar(Jogo *jogo, float dt);

/** @brief Fundo IMG_FUNDO_RESULTADO (assets_desenhar_fundo com BOSS_QUANTIDADE: sem fundo de fase),
 *        partida_premio_final() em esmeraldas e mensagem por estado. */
void tela_resultado_desenhar(const Jogo *jogo);

/* ---------- Desenhos compartilhados ---------- */

/**
 * @brief (IMPLEMENTADA) Boss da fase atual (o da rodada em jogo), para escolher fundo, cores e sprite.
 *
 * Uso típico, que troca o cenário sozinho a cada boss:
 *   assets_desenhar_fundo(&jogo->assets, IMG_FUNDO_PERGUNTA, tela_boss_da_fase(jogo));
 *
 * @return BossId da rodada; BOSS_QUANTIDADE se não há partida válida (o fundo usa então o padrão).
 */
BossId tela_boss_da_fase(const Jogo *jogo);

/** @brief (A IMPLEMENTAR) "Pergunta N/15" (partida_numero_da_rodada), nível (pergunta_nome_nivel) e esmeraldas. */
void tela_desenhar_hud(const Jogo *jogo);

/** @brief (A IMPLEMENTAR) Boss da rodada (assets_desenhar_boss) + nome (boss_nome) + barra (boss_calculo_vida). */
void tela_desenhar_boss(const Jogo *jogo, Rectangle area);

/** @brief (IMPLEMENTADA) Barra horizontal de vida: fundo, preenchimento proporcional a `fracao` (0 a 1), borda. */
void tela_desenhar_barra_vida(Rectangle area, float fracao, Color cor);

/** @brief (A IMPLEMENTAR) As 15 rodadas com prêmios (partida_premio_da_rodada), marcando a atual e os marcos. */
void tela_desenhar_escada_premios(const Jogo *jogo, Rectangle area);

/** @brief (IMPLEMENTADA) Faixa vermelha no rodapé com utils_mensagem_status(jogo->ultimo_erro). */
void tela_desenhar_banner_erro(const Jogo *jogo);

#endif /* TELA_H */
