/**
 * @file partida.h
 * @brief Regras do Jogo do Milhão (SBT), sem código gráfico: as telas só leem o estado e chamam estas funções.
 *
 * Fluxo: RESPONDENDO --partida_responder--> REVELANDO --partida_confirmar_revelacao-->
 *        RESPONDENDO (próxima) | FIM_ERRO | FIM_VITORIA; partida_parar() leva a FIM_PARADA.
 * Rodada é índice 0-14 (a "pergunta 5" é o índice 4; para exibir use partida_numero_da_rodada).
 * Ajudas pagas com esmeraldas, só em RESPONDENDO: dica (uma vez por pergunta) e eliminar duas
 * (uma vez por partida). Saldo = esmeraldas - esmeraldas_gastas; o gasto sai do prêmio final.
 */
#ifndef PARTIDA_H
#define PARTIDA_H

#include "config.h"
#include "banco.h"
#include "boss.h"
#include "sorteio.h"

/** Estados da partida. */
typedef enum {
    ESTADO_RESPONDENDO = 0, /**< Esperando A-D, ajuda ou parar. */
    ESTADO_REVELANDO,       /**< Resposta dada; a tela mostra o resultado. */
    ESTADO_FIM_PARADA,      /**< Parou e levou o acumulado. */
    ESTADO_FIM_ERRO,        /**< Errou: leva o último marco (REGRA_ERRO_VOLTA_AO_MARCO). */
    ESTADO_FIM_VITORIA      /**< Acertou as 15. */
} EstadoPartida;

/** Estado completo de uma partida (vive em Jogo). */
typedef struct {
    PerguntaSorteada perguntas[PERGUNTAS_POR_PARTIDA]; /**< Sorteadas em partida_iniciar(). */
    Boss bosses[NUM_NIVEIS];                           /**< bosses[nivel - NIVEL_MIN]. */
    int rodada;                  /**< Índice da pergunta atual (0..14). */
    int esmeraldas;              /**< Acumulado (prêmio de parar). */
    int esmeraldas_garantidas;   /**< Último marco atingido (prêmio de errar). */
    int escolha;                 /**< Posição (0-3) escolhida na rodada, -1 se nenhuma. */
    bool ultima_acertou;         /**< Resultado da última resposta. */
    bool boss_derrotado_agora;   /**< A última resposta derrotou o boss (vale até a próxima resposta). */
    int esmeraldas_gastas;       /**< Total pago em ajudas. */
    bool dica_visivel;           /**< Dica comprada nesta rodada. */
    bool eliminacao_usada;       /**< "Eliminar duas" já usada na partida. */
    bool alternativa_eliminada[NUM_ALTERNATIVAS]; /**< Posições desabilitadas na rodada atual. */
    EstadoPartida estado;        /**< Ver o fluxo no topo do arquivo. */
} Partida;

/**
 * @brief Partida nova: sorteio_montar_partida + um boss por nível (boss_iniciar). Chamada por
 *        jogo_nova_partida (e por jogo_executar, para conferir o banco). Se falhar, `partida` não muda.
 * @return STATUS_OK, STATUS_ERRO_PARAMETRO, STATUS_ERRO_MEMORIA ou STATUS_NAO_ENCONTRADO
 *         (algum nível sem perguntas suficientes).
 */
Status partida_iniciar(Partida *partida, const Banco *banco);

/** @brief Nível da rodada pela PERGUNTAS_POR_NIVEL (0-1→1, 2-3→2, 4-7→3, 8-11→4, 12-14→5); 0 se inválida. */
int partida_nivel_da_rodada(int rodada);

/** @brief Rodadas do nível (= vida inicial do boss); 0 se inválido. */
int partida_perguntas_no_nivel(int nivel);

/** @brief Número para exibir (1 a 15); 0 se NULL. */
int partida_numero_da_rodada(const Partida *partida);

/** @brief true nas perguntas 5 e 10 (marcos de segurança). */
bool partida_eh_marco(int rodada);

/** @brief Esmeraldas acumuladas ao acertar a rodada (PREMIOS_ESMERALDAS); 0 se inválida. */
int partida_premio_da_rodada(int rodada);

/** @brief Pergunta atual ou NULL se a partida acabou. */
const PerguntaSorteada *partida_pergunta_atual(const Partida *partida);

/**
 * @brief Boss do nível da rodada atual (NULL se NULL). Na revelação ainda é o da pergunta respondida;
 *        depois de partida_confirmar_revelacao já é o próximo (veja boss_derrotado_agora).
 */
const Boss *partida_boss_atual(const Partida *partida);

/**
 * @brief Registra a resposta (tela de pergunta). Acerto: prêmio da rodada, dano no boss e,
 *        nos marcos, atualiza o garantido. Vai para ESTADO_REVELANDO.
 * @param posicao_escolhida 0 a 3.
 * @return STATUS_OK ou STATUS_ERRO_PARAMETRO (posição inválida/eliminada ou estado errado).
 */
Status partida_responder(Partida *partida, int posicao_escolhida);

/** @brief Esmeraldas disponíveis para gastar (0 se NULL). */
int partida_saldo(const Partida *partida);

/** @brief Preço da dica na rodada atual; 0 se NULL ou encerrada. */
int partida_custo_dica(const Partida *partida);

/** @brief true se dá para comprar a dica agora (habilita o botão em jogo_preparar_botoes_pergunta). */
bool partida_pode_comprar_dica(const Partida *partida);

/**
 * @brief Paga a dica e a deixa visível até o fim da rodada.
 * @return STATUS_OK; STATUS_SALDO_INSUFICIENTE (nada é cobrado); STATUS_ERRO_PARAMETRO.
 */
Status partida_comprar_dica(Partida *partida);

/** @brief Dica da pergunta atual se comprada; senão NULL. */
const char *partida_dica_atual(const Partida *partida);

/** @brief Preço de "eliminar duas" na rodada atual; 0 se NULL ou encerrada. */
int partida_custo_eliminar(const Partida *partida);

/** @brief true se dá para usar "eliminar duas" agora (habilita o botão). */
bool partida_pode_eliminar(const Partida *partida);

/**
 * @brief Paga e desabilita duas alternativas erradas sorteadas (uma vez por partida).
 * @return STATUS_OK; STATUS_SALDO_INSUFICIENTE (nada muda); STATUS_ERRO_PARAMETRO.
 */
Status partida_eliminar_duas(Partida *partida);

/**
 * @brief Sai da revelação (botão Continuar): acerto avança (ou vitória na 15ª); erro vai a FIM_ERRO.
 * @return STATUS_OK ou STATUS_ERRO_PARAMETRO (estado errado).
 */
Status partida_confirmar_revelacao(Partida *partida);

/**
 * @brief Para e leva o acumulado (FIM_PARADA).
 * @return STATUS_OK ou STATUS_ERRO_PARAMETRO (só em ESTADO_RESPONDENDO).
 */
Status partida_parar(Partida *partida);

/** @brief true em FIM_PARADA, FIM_ERRO ou FIM_VITORIA. */
bool partida_encerrada(const Partida *partida);

/**
 * @brief Prêmio final (tela de resultado): acumulado ao parar/vencer, último marco ao errar,
 *        menos o gasto em ajudas (nunca negativo). 0 com a partida em andamento.
 */
int partida_premio_final(const Partida *partida);

#endif /* PARTIDA_H */
