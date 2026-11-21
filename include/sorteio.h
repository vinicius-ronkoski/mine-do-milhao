/**
 * @file sorteio.h
 * @brief Sistema aleatorizador: sorteia perguntas do Banco e embaralha alternativas.
 *
 * NÃO depende de raylib (compila também sem USE_RAYLIB). Usa só rand()/srand().
 * NUNCA altera o Banco: trabalha com cópias de índices e com a ordem de exibição.
 */
#ifndef SORTEIO_H
#define SORTEIO_H

#include <stddef.h>
#include "config.h"
#include "banco.h"
#include "pergunta.h"

/**
 * @brief Uma pergunta pronta para ser exibida na partida (alternativas já embaralhadas).
 *
 * `origem` aponta para a Pergunta dentro do Banco (não é cópia: o Banco precisa
 * continuar vivo e sem remoções durante a partida).
 */
typedef struct {
    const Pergunta *origem;              /**< Pergunta original (banco_obter). */
    int ordem[NUM_ALTERNATIVAS];         /**< ordem[i] = índice original da alternativa mostrada na posição i. */
    int posicao_correta;                 /**< Posição (0 a 3) onde a alternativa correta ficou após embaralhar. */
} PerguntaSorteada;

/**
 * @brief Semeia o gerador aleatório.
 * @param semente Valor da semente (use time(NULL) no início do jogo; fixo para testes).
 */
void sorteio_semear(unsigned int semente);

/**
 * @brief Sorteia um inteiro em [minimo, maximo].
 * @return Valor sorteado; se minimo > maximo, devolve minimo.
 */
int sorteio_inteiro(int minimo, int maximo);

/**
 * @brief Embaralha um vetor de inteiros (Fisher-Yates).
 * @param vetor   Vetor a embaralhar.
 * @param tamanho Quantidade de elementos.
 */
void sorteio_embaralhar_vetor(int *vetor, size_t tamanho);

/**
 * @brief Embaralha as alternativas de uma pergunta e preenche a PerguntaSorteada.
 *
 * Passos: ordem = {0,1,2,3}; embaralhar; achar a posição cuja alternativa original
 * é (p->correta - 'A') e guardar em posicao_correta.
 *
 * @param p     Pergunta do banco.
 * @param saida Recebe origem, ordem e posicao_correta.
 * @return STATUS_OK ou STATUS_ERRO_PARAMETRO (NULL ou pergunta_validar() falhou).
 */
Status sorteio_alternativas(const Pergunta *p, PerguntaSorteada *saida);

/**
 * @brief Sorteia `quantidade` perguntas DISTINTAS de um nível.
 *
 * Passos: alocar vetor de banco->quantidade posições; banco_filtrar_nivel();
 * embaralhar os índices; copiar os primeiros `quantidade`; liberar o vetor.
 *
 * @param banco       Banco consultado.
 * @param nivel       Nível (NIVEL_MIN a NIVEL_MAX).
 * @param quantidade  Quantas perguntas sortear.
 * @param indices_out Vetor de saída com `quantidade` posições (índices do Banco).
 * @return STATUS_OK; STATUS_ERRO_PARAMETRO; STATUS_ERRO_MEMORIA;
 *         STATUS_NAO_ENCONTRADO se o nível tem menos perguntas que `quantidade`.
 */
Status sorteio_perguntas_do_nivel(const Banco *banco, int nivel, size_t quantidade, size_t *indices_out);

/**
 * @brief Monta as 15 perguntas de uma partida (ordem crescente de dificuldade).
 *
 * Para cada nível, usa partida_perguntas_no_nivel() (partida.h) e
 * sorteio_perguntas_do_nivel(); depois aplica sorteio_alternativas() em cada uma.
 *
 * @param banco Banco já carregado (dados_carregar).
 * @param saida Vetor com PERGUNTAS_POR_PARTIDA posições.
 * @return STATUS_OK ou o primeiro erro encontrado (a partida NÃO deve começar se falhar).
 */
Status sorteio_montar_partida(const Banco *banco, PerguntaSorteada saida[PERGUNTAS_POR_PARTIDA]);

/**
 * @brief Texto da alternativa exibida em uma posição.
 * @param ps      Pergunta sorteada.
 * @param posicao Posição de 0 a NUM_ALTERNATIVAS-1.
 * @return Texto (origem->alternativas[ordem[posicao]]) ou "" se inválido. Nunca NULL.
 */
const char *sorteio_texto_alternativa(const PerguntaSorteada *ps, int posicao);

#endif /* SORTEIO_H */
