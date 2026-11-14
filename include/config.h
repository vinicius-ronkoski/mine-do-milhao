/**
 * @file config.h
 * @brief Constantes e tipos compartilhados por todo o projeto Mine do Milhão.
 *
 * Tudo que é "número mágico" ou caminho de arquivo fica aqui, para que
 * nenhum outro módulo precise de variáveis globais.
 */

#ifndef CONFIG_H
#define CONFIG_H

/** Quantidade de alternativas por pergunta (A, B, C e D). */
#define NUM_ALTERNATIVAS 4

/** Tamanho máximo (com '\0') do enunciado de uma pergunta. */
#define TAM_ENUNCIADO 256

/** Tamanho máximo (com '\0') de cada alternativa. */
#define TAM_ALTERNATIVA 128

/** Tamanho máximo (com '\0') da dica de uma pergunta. */
#define TAM_DICA 192

/** Menor nível de dificuldade (1 = muito fácil). */
#define NIVEL_MIN 1

/** Maior nível de dificuldade (5 = muito difícil). */
#define NIVEL_MAX 5

/** Caminho do CSV inicial (lido só na primeira execução). */
#define CAMINHO_CSV "data/perguntas.csv"

/** Caminho do arquivo binário (lido e salvo a cada execução). */
#define CAMINHO_BIN "data/perguntas.bin"

/** Pasta onde a função "exportar CSV" grava seus arquivos. */
#define PASTA_EXPORTADOS "data/exportados/"

/**
 * @brief Códigos de retorno padronizados das funções que podem falhar.
 */
typedef enum {
    STATUS_OK = 0,              /**< Operação concluída com sucesso. */
    STATUS_ERRO_ARQUIVO,        /**< Falha ao abrir, ler ou gravar arquivo. */
    STATUS_ERRO_MEMORIA,        /**< Falha de alocação (malloc/realloc). */
    STATUS_ERRO_PARAMETRO,      /**< Argumento inválido (NULL, índice fora etc.). */
    STATUS_ERRO_FORMATO,        /**< Conteúdo do arquivo fora do formato esperado. */
    STATUS_NAO_ENCONTRADO,      /**< Pergunta procurada não existe. */
    STATUS_NAO_IMPLEMENTADO     /**< Função ainda é um esqueleto (TODO). */
} Status;

#endif /* CONFIG_H */
