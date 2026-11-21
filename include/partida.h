/**
 * @file partida.h
 * @brief Regras do Jogo do Milhão (SBT) SEM nenhum código gráfico.
 *
 * Toda a regra de negócio fica aqui; a parte visual (tela.h/jogo.h) só LÊ o estado
 * e chama estas funções. Assim dá para testar a regra sem abrir janela.
 * Convenção: rodada é INDICE de 0 a PERGUNTAS_POR_PARTIDA-1 (a "pergunta 5" do enunciado é o índice 4).
 *
 * Fluxo: RESPONDENDO --partida_responder--> REVELANDO --partida_confirmar_revelacao-->
 *        RESPONDENDO (próxima) | FIM_ERRO | FIM_VITORIA.
 *        partida_parar() vale em RESPONDENDO ("a qualquer momento") e leva a FIM_PARADA.
 */
#ifndef PARTIDA_H
#define PARTIDA_H

#include <stdbool.h>
#include "config.h"
#include "banco.h"
#include "boss.h"
#include "sorteio.h"

/** Estados da partida. */
typedef enum {
    ESTADO_RESPONDENDO = 0, /**< Aguardando o jogador escolher A-D ou parar. */
    ESTADO_REVELANDO,       /**< Resposta dada; a tela mostra certo/errado antes de seguir. */
    ESTADO_FIM_PARADA,      /**< Jogador parou e levou o acumulado. */
    ESTADO_FIM_ERRO,        /**< Errou: prêmio cai conforme REGRA_ERRO_VOLTA_AO_MARCO. */
    ESTADO_FIM_VITORIA      /**< Acertou a rodada 15: prêmio máximo e Herobrine derrotado. */
} EstadoPartida;

/** Tudo que descreve uma partida em andamento. */
typedef struct {
    PerguntaSorteada perguntas[PERGUNTAS_POR_PARTIDA]; /**< Sorteadas em partida_iniciar(). */
    Boss bosses[NUM_NIVEIS];                           /**< bosses[nivel - NIVEL_MIN]. */
    int rodada;                  /**< Índice da pergunta atual (0..14). */
    int esmeraldas;              /**< Acumulado (prêmio de parar). */
    int esmeraldas_garantidas;   /**< Último marco atingido (prêmio de errar). */
    int escolha;                 /**< Posição (0-3) escolhida na rodada, -1 se nenhuma. */
    bool ultima_acertou;         /**< Resultado da última resposta. */
    EstadoPartida estado;
} Partida;

/**
 * @brief Prepara uma partida nova.
 *
 * Passos: zerar a struct; sorteio_montar_partida(); para cada nível iniciar o Boss com
 * vida_maxima = partida_perguntas_no_nivel(); rodada = 0; estado = ESTADO_RESPONDENDO.
 *
 * @param partida Partida a preencher.
 * @param banco   Banco carregado (dados_carregar).
 * @return STATUS_OK, STATUS_ERRO_PARAMETRO, STATUS_ERRO_MEMORIA ou STATUS_NAO_ENCONTRADO
 *         (CSV sem perguntas suficientes em algum nível).
 */
Status partida_iniciar(Partida *partida, const Banco *banco);

/** @brief Nível (NIVEL_MIN..NIVEL_MAX) de uma rodada: 0-1→1, 2-3→2, 4-7→3, 8-11→4, 12-14→5. Inválida: NIVEL_MIN. */
int partida_nivel_da_rodada(int rodada);

/** @brief Quantas rodadas pertencem a um nível (2,2,4,4,3). É a vida inicial do boss. 0 se inválido. */
int partida_perguntas_no_nivel(int nivel);

/** @brief 1 se a rodada é marco de segurança (índices 4 e 9, ou seja, perguntas 5 e 10). */
int partida_eh_marco(int rodada);

/**
 * @brief Esmeraldas acumuladas ao ACERTAR uma rodada (tabela PREMIOS_ESMERALDAS do config.h).
 * @return Valor; 0 se a rodada for inválida.
 */
int partida_premio_da_rodada(int rodada);

/** @brief Pergunta atual (já embaralhada) ou NULL se a partida terminou/é inválida. */
const PerguntaSorteada *partida_pergunta_atual(const Partida *partida);

/** @brief Boss do nível da rodada atual (para desenhar a barra de vida) ou NULL. */
const Boss *partida_boss_atual(const Partida *partida);

/**
 * @brief Registra a resposta do jogador na rodada atual.
 *
 * Passos: exigir ESTADO_RESPONDENDO; guardar escolha; comparar com posicao_correta;
 * se acertou: esmeraldas = partida_premio_da_rodada(), boss_receber_dano(boss do nível, 1),
 * atualizar esmeraldas_garantidas se for marco; se errou: nada muda aqui.
 * Termina em ESTADO_REVELANDO.
 *
 * @param posicao_escolhida Posição 0 a 3 (a tela traduz clique/tecla para isso).
 * @return STATUS_OK ou STATUS_ERRO_PARAMETRO (posição inválida / estado errado).
 */
Status partida_responder(Partida *partida, int posicao_escolhida);

/**
 * @brief Sai de REVELANDO: acerto avança a rodada (ou vitória na 15ª); erro vai a FIM_ERRO.
 * @return STATUS_OK ou STATUS_ERRO_PARAMETRO (estado errado).
 */
Status partida_confirmar_revelacao(Partida *partida);

/**
 * @brief Jogador decide parar e leva o acumulado (estado FIM_PARADA).
 * @return STATUS_OK ou STATUS_ERRO_PARAMETRO (só vale em ESTADO_RESPONDENDO).
 */
Status partida_parar(Partida *partida);

/** @brief 1 se o estado é FIM_PARADA, FIM_ERRO ou FIM_VITORIA. */
int partida_encerrada(const Partida *partida);

/**
 * @brief Esmeraldas que o jogador leva embora.
 * PARADA/VITORIA: esmeraldas. ERRO: esmeraldas_garantidas (ou 0 se REGRA_ERRO_VOLTA_AO_MARCO == 0).
 */
int partida_premio_final(const Partida *partida);

#endif /* PARTIDA_H */
