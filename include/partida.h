/**
 * @file partida.h
 * @brief Regras do Jogo do Milhão (SBT) SEM nenhum código gráfico.
 *
 * Toda a regra de negócio fica aqui; a parte visual (tela.h/jogo.h) só LÊ o estado
 * e chama estas funções. Assim dá para testar a regra sem abrir janela.
 * Convenção: rodada é INDICE de 0 a PERGUNTAS_POR_PARTIDA-1 (a "pergunta 5" do enunciado é o índice 4).
 * Para mostrar "Pergunta N/15" use partida_numero_da_rodada(), que já devolve 1 a 15.
 * Valores inválidos NUNCA viram valores "válidos" em silêncio: veja o retorno de cada função.
 *
 * Fluxo: RESPONDENDO --partida_responder--> REVELANDO --partida_confirmar_revelacao-->
 *        RESPONDENDO (próxima) | FIM_ERRO | FIM_VITORIA.
 *        partida_parar() vale em RESPONDENDO ("a qualquer momento") e leva a FIM_PARADA.
 *
 * Ajudas (parte criativa), só em RESPONDENDO:
 *   - partida_comprar_dica(): paga CUSTO_DICA_POR_NIVEL esmeraldas e mostra a dica da pergunta
 *     (campo `dica` do CSV) até a rodada acabar. Pode comprar em várias perguntas, uma vez em cada.
 *     O que foi gasto (esmeraldas_gastas) é descontado do prêmio final;
 *   - partida_eliminar_duas(): paga CUSTO_ELIMINAR_POR_NIVEL esmeraldas, UMA vez por partida;
 *     desabilita duas alternativas erradas. Também conta em esmeraldas_gastas.
 *
 * Saldo = esmeraldas (prêmio acumulado) - esmeraldas_gastas. É o que o jogador pode gastar.
 * Na primeira pergunta o saldo é 0, então a dica só pode ser comprada a partir da segunda.
 */
#ifndef PARTIDA_H
#define PARTIDA_H

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
    bool boss_derrotado_agora;   /**< true se a ÚLTIMA resposta derrotou o boss do nível. Vale da resposta até a próxima
                                      (inclusive depois de partida_confirmar_revelacao(), quando o boss "atual" já é outro). */
    int esmeraldas_gastas;       /**< Total pago em ajudas (dica e eliminar; descontado do prêmio final). */
    bool dica_visivel;           /**< Dica comprada na rodada atual (volta a false na próxima rodada). */
    bool eliminacao_usada;       /**< A ajuda "eliminar duas" já foi gasta nesta partida. */
    bool alternativa_eliminada[NUM_ALTERNATIVAS]; /**< Posições desabilitadas na rodada atual. */
    EstadoPartida estado;        /**< Estado atual (ver fluxo no topo do arquivo). */
} Partida;

/**
 * @brief Prepara uma partida nova.
 *
 * Passos: montar tudo numa partida local (por_nivel[] vem de partida_perguntas_no_nivel();
 * sorteio_montar_partida(); um Boss por nível com vida_maxima = perguntas do nível; rodada = 0;
 * estado = ESTADO_RESPONDENDO) e só então copiar para `partida`. Se falhar, `partida` fica INALTERADA.
 *
 * @param partida Partida a preencher.
 * @param banco   Banco carregado (dados_carregar).
 * @return STATUS_OK, STATUS_ERRO_PARAMETRO, STATUS_ERRO_MEMORIA ou STATUS_NAO_ENCONTRADO
 *         (CSV sem perguntas suficientes em algum nível).
 */
Status partida_iniciar(Partida *partida, const Banco *banco);

/**
 * @brief Nível de uma rodada (índice 0-14), pela distribuição PERGUNTAS_POR_NIVEL (2,2,4,4,3):
 *        0-1→1, 2-3→2, 4-7→3, 8-11→4, 12-14→5.
 * @return Nível de NIVEL_MIN a NIVEL_MAX; 0 (inválido) se a rodada estiver fora de 0..14.
 */
int partida_nivel_da_rodada(int rodada);

/** @brief Quantas rodadas pertencem a um nível (2,2,4,4,3). É a vida inicial do boss. 0 se o nível for inválido. */
int partida_perguntas_no_nivel(int nivel);

/** @brief Número da pergunta para exibir (1 a 15) = rodada + 1; 0 se partida for NULL. */
int partida_numero_da_rodada(const Partida *partida);

/** @brief true se a rodada é marco de segurança (índices 4 e 9, ou seja, perguntas 5 e 10). */
bool partida_eh_marco(int rodada);

/**
 * @brief Esmeraldas acumuladas ao ACERTAR uma rodada (tabela PREMIOS_ESMERALDAS do config.h).
 * @return Valor; 0 se a rodada for inválida.
 */
int partida_premio_da_rodada(int rodada);

/** @brief Pergunta atual (já embaralhada) ou NULL se a partida terminou/é inválida. */
const PerguntaSorteada *partida_pergunta_atual(const Partida *partida);

/**
 * @brief Boss do nível da rodada atual (para desenhar a barra de vida) ou NULL se partida for NULL.
 *
 * Em ESTADO_REVELANDO ainda é o boss da pergunta respondida (já com o dano aplicado), então a tela
 * pode mostrar o boss caindo. Depois de partida_confirmar_revelacao() passa a ser o do próximo nível;
 * para saber se o boss anterior caiu use partida->boss_derrotado_agora.
 */
const Boss *partida_boss_atual(const Partida *partida);

/**
 * @brief Registra a resposta do jogador na rodada atual.
 *
 * Passos: exigir ESTADO_RESPONDENDO; guardar escolha; comparar com posicao_correta;
 * se acertou: esmeraldas = partida_premio_da_rodada(), boss_receber_dano(boss do nível, 1),
 * boss_derrotado_agora se a vida chegou a 0, esmeraldas_garantidas se for marco;
 * se errou: nada muda além de escolha/ultima_acertou. Termina em ESTADO_REVELANDO.
 *
 * @param posicao_escolhida Posição 0 a 3 (a tela traduz clique/tecla para isso).
 * @return STATUS_OK ou STATUS_ERRO_PARAMETRO (posição inválida, eliminada ou estado errado).
 */
Status partida_responder(Partida *partida, int posicao_escolhida);

/** @brief Esmeraldas que o jogador pode gastar agora: esmeraldas - esmeraldas_gastas (0 se NULL). */
int partida_saldo(const Partida *partida);

/** @brief Preço da dica na rodada atual (CUSTO_DICA_POR_NIVEL); 0 se a partida for NULL ou acabou. */
int partida_custo_dica(const Partida *partida);

/**
 * @brief true se a dica pode ser comprada agora: ESTADO_RESPONDENDO, ainda não comprada nesta
 *        rodada e saldo suficiente. Use para habilitar/desabilitar o botão.
 */
bool partida_pode_comprar_dica(const Partida *partida);

/**
 * @brief Ajuda "dica": paga partida_custo_dica() e deixa a dica da pergunta atual visível.
 * @return STATUS_OK; STATUS_SALDO_INSUFICIENTE (nada é cobrado); STATUS_ERRO_PARAMETRO
 *         (NULL, fora de ESTADO_RESPONDENDO ou já comprada nesta rodada).
 */
Status partida_comprar_dica(Partida *partida);

/** @brief Texto da dica da pergunta atual se ela estiver visível; NULL caso contrário. */
const char *partida_dica_atual(const Partida *partida);

/** @brief Preço de "eliminar duas" na rodada atual (CUSTO_ELIMINAR_POR_NIVEL); 0 se NULL ou encerrada. */
int partida_custo_eliminar(const Partida *partida);

/**
 * @brief true se "eliminar duas" pode ser usada agora: ESTADO_RESPONDENDO, ainda não usada na partida
 *        e saldo suficiente. Use para habilitar/desabilitar o botão.
 */
bool partida_pode_eliminar(const Partida *partida);

/**
 * @brief Ajuda "eliminar duas": paga partida_custo_eliminar() e marca duas alternativas ERRADAS
 *        sorteadas como eliminadas (uma vez por partida).
 * @return STATUS_OK; STATUS_SALDO_INSUFICIENTE (nada é cobrado nem eliminado); STATUS_ERRO_PARAMETRO
 *         (já usada, NULL ou fora de ESTADO_RESPONDENDO).
 */
Status partida_eliminar_duas(Partida *partida);

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

/** @brief true se o estado é FIM_PARADA, FIM_ERRO ou FIM_VITORIA (false se NULL). */
bool partida_encerrada(const Partida *partida);

/**
 * @brief Esmeraldas que o jogador leva embora, já descontado o que gastou em dicas (nunca negativo).
 * PARADA/VITORIA: esmeraldas. ERRO: esmeraldas_garantidas (ou 0 se REGRA_ERRO_VOLTA_AO_MARCO == 0).
 * Partida em andamento (ou NULL): 0, porque ainda não há prêmio final.
 */
int partida_premio_final(const Partida *partida);

#endif /* PARTIDA_H */
