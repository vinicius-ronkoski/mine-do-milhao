/**
 * @file tela.h
 * @brief Cada tela do jogo: tela_X_atualizar lê botões e chama partida.h; tela_X_desenhar só desenha.
 *
 * Chamadas pelo switch de jogo_atualizar/jogo_desenhar (jogo.c). Posições em layout.h.
 * Só compila com USE_RAYLIB=1.
 */
#ifndef TELA_H
#define TELA_H

#include "raylib.h"
#include "jogo.h"

/* ---------- Menu principal ---------- */

/** @brief Toca a música principal ao entrar; Novo jogo -> carregamento e jogo_nova_partida; Sair -> TELA_SAIR. */
void tela_menu_atualizar(Jogo *jogo, float dt);

/** @brief Fundo (IMG_FUNDO_MENU), logo (IMG_TITULO ou texto), os 4 botões e TEXTO_VERSAO. */
void tela_menu_desenhar(const Jogo *jogo);

/* ---------- Carregamento (placeholder) ---------- */

/** @brief Após DURACAO_CARREGANDO vai para tela_apos_carregar (TELA_DIALOGO = jogo_nova_partida). */
void tela_carregando_atualizar(Jogo *jogo, float dt);

/** @brief Fundo do menu escurecido, "Carregando..." e uma barra enchendo. */
void tela_carregando_desenhar(const Jogo *jogo);

/* ---------- Tela de diálogo ---------- */

/** @brief Avança o diálogo, toca som/música de cada fala nova e, no fim, vai para tela_apos_dialogo. */
void tela_dialogo_atualizar(Jogo *jogo, float dt);

/** @brief Cenário da fala (ou fundo da fase com o boss) e a caixa de diálogo. */
void tela_dialogo_desenhar(const Jogo *jogo);

/* ---------- Tela de pergunta ---------- */

/**
 * @brief Lê os botões da pergunta (via jogo_botao_acionado):
 *   - alternativa -> partida_responder() e jogo_tocar_reacao() (a revelação vem depois da fala);
 *   - Parar -> partida_parar() e CENA_PARADA; Dica/Eliminar -> compra e jogo_preparar_botoes_pergunta().
 *   Na 1ª pergunta de cada boss toca o som de entrada dele. Erros vão para jogo_registrar_erro().
 */
void tela_pergunta_atualizar(Jogo *jogo, float dt);

/** @brief Esquerda: enunciado (e dica), alternativas e ajudas; direita: HUD e boss. */
void tela_pergunta_desenhar(const Jogo *jogo);

/* ---------- Tela de revelação ---------- */

/**
 * @brief Som de dano ao entrar; Continuar -> partida_confirmar_revelacao e decide o destino:
 *        erro -> CENA_DERROTA e resultado; vitória -> CENA_HISTORIA_FIM e resultado;
 *        boss caiu -> CENA_HISTORIA_2..5 + próximo boss; senão, próxima pergunta.
 */
void tela_revelacao_atualizar(Jogo *jogo, float dt);

/** @brief Boss no centro (animação de dano), mensagem do resultado, saldo e Continuar. */
void tela_revelacao_desenhar(const Jogo *jogo);

/* ---------- Tela de resultado ---------- */

/** @brief "Menu" -> carregamento -> menu principal. */
void tela_resultado_atualizar(Jogo *jogo, float dt);

/** @brief Título pelo estado final, prêmio (partida_premio_final) e de onde ele veio. */
void tela_resultado_desenhar(const Jogo *jogo);

/* ---------- Desenhos compartilhados ---------- */

/** @brief Boss da rodada atual (fundo da fase em assets_desenhar_fundo); BOSS_QUANTIDADE se não há. */
BossId tela_boss_da_fase(const Jogo *jogo);

/** @brief Saldo de esmeraldas, "Pergunta N/15" e nível. */
void tela_desenhar_hud(const Jogo *jogo);

/** @brief Nome, sprite (quadrado da largura de `area`), barra e "Vida x/y" do boss da rodada. */
void tela_desenhar_boss(const Jogo *jogo, Rectangle area);

/** @brief Barra de vida preenchida em `fracao` (0 a 1). */
void tela_desenhar_barra_vida(Rectangle area, float fracao, Color cor);

/** @brief Faixa vermelha no rodapé com o último erro (jogo_registrar_erro). */
void tela_desenhar_banner_erro(const Jogo *jogo);

#endif /* TELA_H */
