/**
 * @file dialogo.h
 * @brief Motor dos diálogos: falas em sequência, com efeito de máquina de escrever.
 *
 * O conteúdo (cenas e reações) fica em historia.h/.c, que preenche o Dialogo com
 * dialogo_adicionar_fala. Este módulo só guarda e mostra as falas: tela.c lê a entrada,
 * toca som/música de cada fala nova e chama dialogo_desenhar. Só compila com USE_RAYLIB=1.
 */
#ifndef DIALOGO_H
#define DIALOGO_H

#include "raylib.h"
#include "config.h"
#include "assets.h"

/** Uma fala. */
typedef struct {
    char personagem[TAM_NOME_PERSONAGEM];  /**< Nome mostrado acima do texto. */
    char texto[TAM_FALA];                  /**< Conteúdo (UTF-8, com acentos). */
    ImagemId retrato;                      /**< Retrato (IMG_*) ou IMG_NENHUMA. */
    SomId som;                             /**< Som ao aparecer (SOM) ou SOM_NENHUM. */
    ImagemId fundo;                        /**< Cenário (CENARIO) ou IMG_NENHUMA = fundo da fase. */
    MusicaId musica;                       /**< Música que começa aqui (MUSICA) ou MUSICA_MANTER. */
} Fala;

/** Sequência de falas em reprodução. */
typedef struct {
    Fala falas[DIALOGO_MAX_FALAS];  /**< Falas, na ordem. */
    int quantidade;                 /**< Falas preenchidas. */
    int indice;                     /**< Fala atual (0 a quantidade-1). */
    float tempo_na_fala;            /**< Segundos na fala atual (máquina de escrever). */
    bool terminou;                  /**< Passou da última fala (ou vazio). */
    int indice_fala_anunciada;      /**< Última fala entregue por dialogo_fala_nova (-1 = nenhuma). */
} Dialogo;

/** @brief Esvazia o diálogo (terminou = true, pois não há falas). */
void dialogo_limpar(Dialogo *dialogo);

/**
 * @brief Acrescenta uma fala sem som, cenário nem música (as macros de historia.c preenchem depois).
 * @return STATUS_OK ou STATUS_ERRO_PARAMETRO (texto longo demais ou DIALOGO_MAX_FALAS excedido).
 */
Status dialogo_adicionar_fala(Dialogo *dialogo, const char *personagem, const char *texto,
                              ImagemId retrato);

/** @brief Fala recém-aparecida, entregue uma vez (senão NULL): tela_dialogo_atualizar toca som e música dela. */
const Fala *dialogo_fala_nova(Dialogo *dialogo);

/**
 * @brief Avança o relógio da fala. Pedido de avançar: completa a fala sendo digitada ou passa à próxima.
 * @param jogador_avancou ESPAÇO, ENTER ou clique neste quadro (jogo_jogador_quer_avancar).
 * @return true quando o diálogo acabou.
 */
bool dialogo_atualizar(Dialogo *dialogo, float dt, bool jogador_avancou);

/** @brief Fala sendo mostrada, ou NULL se o diálogo terminou. */
const Fala *dialogo_fala_atual(const Dialogo *dialogo);

/** @brief Letras da fala atual já visíveis. */
int dialogo_letras_visiveis(const Dialogo *dialogo);

/** @brief Desenha caixa, retrato, nome e texto parcial no rodapé (e o aviso de continuar). */
void dialogo_desenhar(const Dialogo *dialogo, const Assets *assets);

#endif /* DIALOGO_H */
