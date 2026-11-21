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

/** Tamanhos de texto
 *  Só mexa nos MAX_*: os TAM_* (vetores da Pergunta) e o buffer do CSV derivam deles,
 *  e o formato do sscanf em arquivo.c também. MAX_* não conta o '\0' final. */
/**@{*/
#define MAX_ENUNCIADO   255
#define MAX_ALTERNATIVA 127
#define MAX_DICA        191
/**@}*/

// O +1 serve para o \0
#define TAM_ENUNCIADO   (MAX_ENUNCIADO + 1)
#define TAM_ALTERNATIVA (MAX_ALTERNATIVA + 1)
#define TAM_DICA        (MAX_DICA + 1)

/** Buffer de uma linha do CSV : todos os campos + aspas, vírgulas, nível, letra e \r\n. */
#define TAM_LINHA_CSV   (TAM_ENUNCIADO + NUM_ALTERNATIVAS * TAM_ALTERNATIVA + TAM_DICA + 64)

/** Primeira linha do CSV: nomes das colunas, na ordem em que são lidas e gravadas. */
#define CSV_CABECALHO "nivel,enunciado,alternativa_a,alternativa_b,alternativa_c,alternativa_d,correta,dica"

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


#define PERGUNTAS_POR_PARTIDA   15
#define NUM_NIVEIS              (NIVEL_MAX - NIVEL_MIN + 1)
#define RODADA_MARCO_1          5     /* pergunta 5  (índice 4) */
#define RODADA_MARCO_2          10    /* pergunta 10 (índice 9) */

/* Esmeraldas ACUMULADAS ao acertar cada rodada (15 valores).*/
#define PREMIOS_ESMERALDAS { 1, 2, 3, 5, 10, 15, 20, 25, 30, 40, 50, 70, 100, 150, 300 }

/* 1: errar leva o último marco (5 ou 10); 0: errar perde tudo.*/
#define REGRA_ERRO_VOLTA_AO_MARCO 1

#define LARGURA_JANELA   1280
#define ALTURA_JANELA    720
#define FPS_ALVO         60
#define TITULO_JANELA    "Mine do Milhão"

#define CAMINHO_SPRITES    "assets/sprites/"
#define CAMINHO_FRAMES     "assets/sprites/frames/"
#define CAMINHO_SONS       "assets/sounds/"
#define CAMINHO_FONTES     "assets/fonts/"
#define ARQUIVO_FONTE    PASTA_FONTES "minecraft.ttf"
#define TAM_FONTE_BASE   32

#define DIALOGO_MAX_FALAS          16
#define TAM_FALA                   256
#define TAM_NOME_PERSONAGEM        32
#define DIALOGO_LETRAS_POR_SEGUNDO 40

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
    STATUS_NAO_IMPLEMENTADO,    /**< Função ainda é um esqueleto (TODO). */
    STATUS_ERRO_GRAFICO,        /**< Janela/OpenGL não pôde ser criada. */
    STATUS_ASSET_FALLBACK      /**< Não é erro: jogo rodando com substitutos de assets. */
} Status;

#endif /* CONFIG_H */
