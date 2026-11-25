/**
 * @file dialogo.h
 * @brief Falas dos personagens em sequência, com efeito de máquina de escrever.
 *
 * Uma CENA é uma lista de falas escrita num `case` de dialogo_anexar_cena() (dialogo.c); o Dialogo
 * é o que está tocando e pode juntar cenas (jogo_tocar_cena + jogo_anexar_cena). Dentro do `case`:
 * FALA(...) adiciona uma fala; CENARIO(IMG_X) troca o fundo das falas seguintes (sem o boss);
 * SOM(SOM_X) antes de uma FALA toca quando ela aparece; MUSICA(MUSICA_X) troca a música na próxima fala.
 * As cenas CENA_HISTORIA_* são chamadas em jogo_nova_partida (jogo.c) e tela_revelacao_atualizar (tela.c).
 * O módulo só guarda os dados: tela.c lê a entrada, toca os efeitos e desenha. Só com USE_RAYLIB=1.
 */
#ifndef DIALOGO_H
#define DIALOGO_H

#include "raylib.h"
#include "config.h"
#include "assets.h"
#include "boss.h"

/** Variações de reação para acerto e para erro (dialogo_anexar_reacao). */
#define REACOES_POR_RESULTADO 4

/** Uma fala. */
typedef struct {
    char personagem[TAM_NOME_PERSONAGEM];  /**< Nome mostrado acima do texto. */
    char texto[TAM_FALA];                  /**< Conteúdo (UTF-8, com acentos). */
    ImagemId retrato;                      /**< Retrato (IMG_*) ou IMG_NENHUMA. */
    SomId som;                             /**< Som ao aparecer (SOM) ou SOM_NENHUM. */
    ImagemId fundo;                        /**< Cenário (CENARIO) ou IMG_NENHUMA = fundo da fase. */
    MusicaId musica;                       /**< Música que começa aqui (MUSICA) ou MUSICA_MANTER. */
} Fala;

/** Cenas (as de boss seguem a ordem de BossId). */
typedef enum {
    CENA_INTRO = 0,         /**< Abertura da partida. */
    CENA_BOSS_CREEPER,      /**< Entrada de cada boss: CENA_BOSS_CREEPER + BossId. */
    CENA_BOSS_WITHER,
    CENA_BOSS_WARDEN,
    CENA_BOSS_ENDER_DRAGON,
    CENA_BOSS_HEROBRINE,
    CENA_DERROTA,           /**< Errou. */
    CENA_PARADA,            /**< Parou e levou as esmeraldas. */
    CENA_HISTORIA_1,        /**< Parte introdutória com o creeper. */
    CENA_HISTORIA_2,        /**< Indo para o Nether. */
    CENA_HISTORIA_3,        /**< Indo para o Warden. */
    CENA_HISTORIA_4,        /**< Indo para o End. */
    CENA_HISTORIA_5,        /**< Voltando do End e checando o Steve. */
    CENA_HISTORIA_FIM,      /**< Vitória, paz e entrega dos prêmios. */
    CENA_QUANTIDADE         /**< Quantidade de cenas (também "cena inválida"). */
} CenaId;

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
 * @brief Acrescenta uma fala sem som, cenário nem música (as macros do `case` preenchem depois).
 * @return STATUS_OK ou STATUS_ERRO_PARAMETRO (texto longo demais ou DIALOGO_MAX_FALAS excedido).
 */
Status dialogo_adicionar_fala(Dialogo *dialogo, const char *personagem, const char *texto,
                              ImagemId retrato);

/** @brief Fala recém-aparecida, entregue uma vez (senão NULL): tela_dialogo_atualizar toca som e música dela. */
const Fala *dialogo_fala_nova(Dialogo *dialogo);

/**
 * @brief Acrescenta as falas da cena ao fim do diálogo (é aqui que as cenas são escritas).
 * @return STATUS_OK ou STATUS_ERRO_PARAMETRO (cena inválida ou falas demais).
 */
Status dialogo_anexar_cena(Dialogo *dialogo, CenaId cena);

/**
 * @brief Reação a uma resposta (jogo_tocar_reacao): resposta certa, frase do boss e, no acerto,
 *        o saldo. Cada fala é sorteada pela `variante`; texto "" nas tabelas pula a fala.
 * @param resposta_certa Texto da alternativa certa (%s da 1ª fala).
 * @param esmeraldas     Saldo atual (%d da fala das esmeraldas).
 * @param variante       Qualquer inteiro (usa-se |variante| % REACOES_POR_RESULTADO).
 * @return STATUS_OK ou STATUS_ERRO_PARAMETRO.
 */
Status dialogo_anexar_reacao(Dialogo *dialogo, bool acertou, const char *resposta_certa,
                             BossId boss_id, int esmeraldas, int variante);

/** @brief dialogo_limpar + dialogo_anexar_cena. */
Status dialogo_carregar_cena(Dialogo *dialogo, CenaId cena);

/** @brief Cena de entrada do boss; CENA_QUANTIDADE se inválido. */
CenaId dialogo_cena_do_boss(BossId id);

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
