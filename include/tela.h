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
 * Posições e tamanhos de todas as telas (e dos botões criados em jogo.c): include/layout.h,
 * que também tem um mapa da tela de pergunta.
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

/* ---------- Tela de pergunta (IMPLEMENTADA) ---------- */

/**
 * @brief Lê os botões da pergunta (todo botão via jogo_botao_acionado, que já toca o clique e
 *        ignora cliques nos primeiros ATRASO_ENTRADA_TELA segundos):
 *   0. Na 1ª pergunta de cada boss (vida cheia), toca SOM_ENTRADA_CREEPER + boss->id, uma vez.
 *   1. Para cada alternativa: se jogo_botao_acionado(jogo, &jogo->alternativas[posicao])
 *      -> partida_responder(posicao) e jogo_trocar_tela(TELA_REVELACAO).
 *   2. botao_parar -> partida_parar(); jogo_tocar_cena(CENA_PARADA, TELA_RESULTADO).
 *   3. botao_dica -> partida_comprar_dica(); jogo_preparar_botoes_pergunta() (desabilita o botão).
 *      O botão já vem desabilitado sem saldo (partida_pode_comprar_dica) e mostra o preço.
 *   4. botao_eliminar -> partida_eliminar_duas(); jogo_preparar_botoes_pergunta()
 *      (também pago: desabilitado sem saldo por partida_pode_eliminar, mostra o preço).
 *   5. Status diferente de STATUS_OK -> jogo_registrar_erro().
 */
void tela_pergunta_atualizar(Jogo *jogo, float dt);

/**
 * @brief Fundo da fase; à esquerda o enunciado (com a dica comprada numa faixa embaixo dele),
 *        as alternativas e as ajudas (preço = número + ícone da esmeralda); à direita o HUD
 *        e o boss com nome e barra de vida.
 */
void tela_pergunta_desenhar(const Jogo *jogo);

/* ---------- Tela de revelação (IMPLEMENTADA) ---------- */

/**
 * @brief Vem depois da reação do apresentador (jogo_tocar_reacao). Ao entrar, toca SOM_DANO_BOSS
 *        ("dano") se acertou. Continuar -> partida_confirmar_revelacao() e então:
 *   - errou (ESTADO_FIM_ERRO) -> CENA_DERROTA (agradecimento) e TELA_RESULTADO;
 *   - venceu (ESTADO_FIM_VITORIA) -> CENA_BOSS_DERROTADO + CENA_VITORIA e TELA_RESULTADO;
 *   - boss caiu (boss_derrotado_agora) -> CENA_BOSS_DERROTADO + entrada do próximo boss e TELA_PERGUNTA;
 *   - senão -> TELA_PERGUNTA.
 */
void tela_revelacao_atualizar(Jogo *jogo, float dt);

/** @brief Sem pergunta nem alternativas: boss no centro (a barra desce e ele pisca se tomou dano),
 *         "Acertou! -1 de vida" / "<boss> derrotado!" / "Errou! Nenhum dano." e o botão Continuar. */
void tela_revelacao_desenhar(const Jogo *jogo);

/* ---------- Tela de resultado (IMPLEMENTADA) ---------- */

/** @brief Botão "Reiniciar (ENTER)" -> jogo_nova_partida() (volta à introdução); ESC volta ao menu. */
void tela_resultado_atualizar(Jogo *jogo, float dt);

/** @brief Título por estado (Fim de jogo / Você parou / Vitória!), retrato e agradecimento do apresentador,
 *         prêmio final (partida_premio_final) e de onde ele veio (marco de segurança, dicas descontadas). */
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

/** @brief (IMPLEMENTADA) Saldo de esmeraldas (canto superior direito), "Pergunta N/15" e o nome do nível. */
void tela_desenhar_hud(const Jogo *jogo);

/**
 * @brief (IMPLEMENTADA) Boss da rodada dentro de `area`: nome em cima, sprite quadrado (lado = largura
 *        da área), barra de vida e "Vida x/y" embaixo. A altura usada é largura + 40 + 15 + 24 + 30.
 */
void tela_desenhar_boss(const Jogo *jogo, Rectangle area);

/** @brief (IMPLEMENTADA) Barra horizontal de vida: fundo, preenchimento proporcional a `fracao` (0 a 1), borda. */
void tela_desenhar_barra_vida(Rectangle area, float fracao, Color cor);

/** @brief (A IMPLEMENTAR) As 15 rodadas com prêmios (partida_premio_da_rodada), marcando a atual e os marcos. */
void tela_desenhar_escada_premios(const Jogo *jogo, Rectangle area);

/** @brief (IMPLEMENTADA) Faixa vermelha no rodapé com utils_mensagem_status(jogo->ultimo_erro). */
void tela_desenhar_banner_erro(const Jogo *jogo);

#endif /* TELA_H */
