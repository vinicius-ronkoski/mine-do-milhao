/**
 * @file config.h
 * @brief Constantes e tipos compartilhados por todo o projeto Mine do Milhão.
 *
 * Tudo que é "número mágico" ou caminho de arquivo usado por mais de um módulo
 * fica aqui, para que nenhum outro módulo precise de variáveis globais.
 * Constantes usadas por um módulo só ficam no .c dele (ex.: OPCAO_SAIR em menu.c).
 *
 * CONVENÇÕES DO PROJETO (valem para todo código novo):
 *   - Nomes: funções e variáveis em minúsculas_com_underscore, prefixadas pelo módulo
 *     (banco_inserir, partida_responder); macros e constantes em MAIÚSCULAS;
 *     tipos em PascalCase (Pergunta, Banco, Status). Nada de nomes de uma letra,
 *     exceto contadores de laço (i, j).
 *   - Retornos:
 *       Status -> a função pode falhar por motivos diferentes (STATUS_OK = deu certo);
 *       bool   -> pergunta de sim/não (boss_derrotado, partida_encerrada);
 *       int / size_t / float -> quantidade ou valor calculado.
 *     Nunca devolva 1/0 numa função do tipo Status: STATUS_OK vale 0.
 *   - sizeof sempre com parênteses: sizeof(x).
 *   - Caminhos: CAMINHO_* = pasta ou arquivo; ARQUIVO_* = arquivo sem extensão
 *     (assets.c tenta as extensões aceitas).
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <stdbool.h>  /* bool, true, false em todo o projeto */

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

/** Caminho do arquivo binário .dat (criado na 1ª execução; lido e salvo nas demais). */
#define CAMINHO_DAT "data/perguntas.dat"

/** Pasta onde a função "exportar CSV" grava seus arquivos. */
#define CAMINHO_EXPORTADOS "data/exportados/"

/** @name Regras da partida (2ª entrega) */
/**@{*/
/** Perguntas de uma partida (enunciado: 15). */
#define PERGUNTAS_POR_PARTIDA   15

/** Quantidade de níveis de dificuldade (e de bosses). */
#define NUM_NIVEIS              (NIVEL_MAX - NIVEL_MIN + 1)

/** Marcos de segurança, contados a partir de 1: "pergunta 5" é o índice 4 da partida. */
#define RODADA_MARCO_1          5
#define RODADA_MARCO_2          10

/** Quantas perguntas de cada nível entram na partida (níveis 1 a 5). A soma deve ser PERGUNTAS_POR_PARTIDA. */
#define PERGUNTAS_POR_NIVEL     { 2, 2, 4, 4, 3 }

/** Esmeraldas ACUMULADAS ao acertar cada rodada (15 valores, uma por pergunta). */
#define PREMIOS_ESMERALDAS      { 1, 2, 3, 5, 10, 15, 20, 25, 30, 40, 50, 70, 100, 150, 300 }

/** Preço da dica em esmeraldas, por nível (1 a 5). O valor gasto sai do prêmio final. */
#define CUSTO_DICA_POR_NIVEL    { 1, 2, 5, 10, 20 }

/** Preço de "eliminar duas" por nível: o dobro da dica (ajuda mais forte). Também sai do prêmio final. */
#define CUSTO_ELIMINAR_POR_NIVEL { 2, 4, 10, 20, 40 }

/** 1: errar leva o último marco (5 ou 10); 0: errar perde tudo. */
#define REGRA_ERRO_VOLTA_AO_MARCO 1
/**@}*/

/** @name Janela (raylib) */
/**@{*/
#define LARGURA_JANELA   1280
#define ALTURA_JANELA    720
#define FPS_ALVO         60
#define TITULO_JANELA    "Mine do Milhão"
/**@}*/

/** @name Áudio (0.0 = mudo, 1.0 = volume original do arquivo) */
/**@{*/
#define VOLUME_MUSICA    0.35f   /**< Música de fundo: baixa, para não abafar falas e efeitos. */
#define VOLUME_EFEITOS   0.80f   /**< Clique, acerto, erro, dano no boss... */
/**@}*/

/** @name Pastas e arquivos de assets (sempre com a barra final nas pastas) */
/**@{*/
#define CAMINHO_SPRITES  "assets/sprites/"
#define CAMINHO_FRAMES   "assets/sprites/frames/"
#define CAMINHO_FUNDOS   "assets/sprites/fundos/"   /* fundo de cada fase: <boss> (creeper, wither...) */
#define CAMINHO_SONS     "assets/sounds/"
#define CAMINHO_FONTES   "assets/fonts/"
#define ARQUIVO_FONTE    CAMINHO_FONTES "minecraft"  /* .ttf ou .otf */
#define ARQUIVO_MUSICA   CAMINHO_SONS "musica"      /* .ogg, .mp3 ou .wav */
#define TAM_FONTE_BASE   32
/**@}*/

#define DIALOGO_MAX_FALAS          16
#define TAM_FALA                   256
#define TAM_NOME_PERSONAGEM        32
#define DIALOGO_LETRAS_POR_SEGUNDO 40

/**
 * @brief Códigos de retorno padronizados das funções que podem falhar.
 */
typedef enum {
    STATUS_OK = 0,              /**< Operação concluída com sucesso. */
    STATUS_ERRO_ARQUIVO,        /**< Falha ao abrir, ler ou gravar arquivo (inclui o fim da entrada do teclado, EOF). */
    STATUS_ERRO_MEMORIA,        /**< Falha de alocação (malloc/realloc). */
    STATUS_ERRO_PARAMETRO,      /**< Argumento inválido (NULL, índice fora etc.). */
    STATUS_ERRO_FORMATO,        /**< Conteúdo do arquivo fora do formato esperado. */
    STATUS_NAO_ENCONTRADO,      /**< Pergunta procurada não existe. */
    STATUS_NAO_IMPLEMENTADO,    /**< Função ainda é um esqueleto (TODO). */
    STATUS_ERRO_GRAFICO,        /**< Janela/OpenGL não pôde ser criada. */
    STATUS_SALDO_INSUFICIENTE,  /**< Esmeraldas insuficientes para comprar uma ajuda. */
    STATUS_ASSET_FALLBACK      /**< Não é erro: jogo rodando com substitutos de assets. */
} Status;

#endif /* CONFIG_H */
