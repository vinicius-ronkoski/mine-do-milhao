/**
 * @file config.h
 * @brief Constantes, caminhos e o enum Status compartilhados por todos os módulos (sem globais).
 *
 * Constantes de um módulo só ficam no .c dele. Convenções: funções módulo_ação em
 * minúsculas_com_underscore, macros em MAIÚSCULAS, tipos em PascalCase; retorno Status quando
 * a função pode falhar (STATUS_OK = 0), bool para sim/não; CAMINHO_* = pasta ou arquivo,
 * ARQUIVO_* = arquivo sem extensão (assets.c tenta as aceitas).
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <stdbool.h>

/** Quantidade de alternativas por pergunta (A, B, C e D). */
#define NUM_ALTERNATIVAS 4

/** Tamanhos de texto sem o '\0'. Só mexa nos MAX_*: TAM_*, o buffer do CSV e o sscanf de arquivo.c derivam deles. */
/**@{*/
#define MAX_ENUNCIADO   255
#define MAX_ALTERNATIVA 127
#define MAX_DICA        191
/**@}*/

// O +1 serve para o \0
#define TAM_ENUNCIADO   (MAX_ENUNCIADO + 1)
#define TAM_ALTERNATIVA (MAX_ALTERNATIVA + 1)
#define TAM_DICA        (MAX_DICA + 1)

/** Buffer de uma linha do CSV: campos + aspas, vírgulas, nível, letra e \r\n. */
#define TAM_LINHA_CSV   (TAM_ENUNCIADO + NUM_ALTERNATIVAS * TAM_ALTERNATIVA + TAM_DICA + 64)

/** Cabeçalho do CSV (ordem das colunas lidas e gravadas). */
#define CSV_CABECALHO "nivel,enunciado,alternativa_a,alternativa_b,alternativa_c,alternativa_d,correta,dica"

/** Níveis de dificuldade: 1 = muito fácil ... 5 = muito difícil. */
#define NIVEL_MIN 1

#define NIVEL_MAX 5

/** CSV inicial (lido só na 1ª execução). */
#define CAMINHO_CSV "data/perguntas.csv"

/** Binário das perguntas (criado na 1ª execução; lido e salvo nas demais). */
#define CAMINHO_DAT "data/perguntas.dat"

/** Pasta da opção "Exportar CSV". */
#define CAMINHO_EXPORTADOS "data/exportados/"

/** Save do jogo (texto: boss, esmeraldas e "eliminar duas" usada; ver save.h). Fora do Git. */
#define CAMINHO_SAVE "data/save.txt"

/** @name Regras da partida (partida.c) */
/**@{*/
/** Perguntas por partida. */
#define PERGUNTAS_POR_PARTIDA   15

/** Quantidade de níveis de dificuldade (e de bosses). */
#define NUM_NIVEIS              (NIVEL_MAX - NIVEL_MIN + 1)

/** Marcos de segurança, contados a partir de 1 (pergunta 5 = rodada de índice 4). */
#define RODADA_MARCO_1          5
#define RODADA_MARCO_2          10

/** Perguntas de cada nível na partida (soma = PERGUNTAS_POR_PARTIDA); também é a vida de cada boss. */
#define PERGUNTAS_POR_NIVEL     { 2, 2, 4, 4, 3 }

/** Esmeraldas acumuladas ao acertar cada rodada (não somam: substituem o valor anterior). */
#define PREMIOS_ESMERALDAS      { 1, 2, 3, 5, 10, 15, 20, 25, 30, 40, 50, 70, 100, 150, 384 }

/** Preço da dica por nível; o gasto sai do prêmio final. */
#define CUSTO_DICA_POR_NIVEL    { 1, 2, 5, 10, 20 }

/** Preço de "eliminar duas" por nível (dobro da dica); também sai do prêmio final. */
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
#define TEXTO_VERSAO     "Mine do Milhão 0.1b"   /**< Canto inferior esquerdo do menu. */
/**@}*/

/** @name Áudio (0.0 = mudo, 1.0 = volume original do arquivo) */
/**@{*/
#define VOLUME_MUSICA    0.35f   /**< Baixa, para não abafar os efeitos. */
#define VOLUME_EFEITOS   0.80f
/**@}*/

/** @name Pastas e arquivos de assets (pastas com a barra final) */
/**@{*/
#define CAMINHO_SPRITES  "assets/sprites/"
#define CAMINHO_FRAMES   "assets/sprites/frames/"
#define CAMINHO_FUNDOS   "assets/sprites/fundos/"   /* fundo de cada fase: <boss>.png */
#define CAMINHO_SONS     "assets/sounds/"
#define CAMINHO_FONTES   "assets/fonts/"
#define ARQUIVO_FONTE    CAMINHO_FONTES "minecraft"  /* .ttf ou .otf */
#define TAM_FONTE_BASE   32
/**@}*/

/** Limites do diálogo (dialogo.h); MAX_FALAS soma as cenas encadeadas. */
#define DIALOGO_MAX_FALAS          64
#define TAM_FALA                   256
#define TAM_NOME_PERSONAGEM        32
#define DIALOGO_LETRAS_POR_SEGUNDO 40

/** @brief Retorno das funções que podem falhar (STATUS_OK = 0); texto em utils_mensagem_status. */
typedef enum {
    STATUS_OK = 0,              /**< Deu certo. */
    STATUS_ERRO_ARQUIVO,        /**< Falha ao abrir, ler ou gravar arquivo (inclui EOF do teclado). */
    STATUS_ERRO_MEMORIA,        /**< Falha de alocação (malloc/realloc). */
    STATUS_ERRO_PARAMETRO,      /**< Argumento inválido (NULL, índice fora etc.). */
    STATUS_ERRO_FORMATO,        /**< Conteúdo do arquivo fora do formato esperado. */
    STATUS_NAO_ENCONTRADO,      /**< Não existe (ex.: perguntas insuficientes num nível). */
    STATUS_NAO_IMPLEMENTADO,    /**< Função ainda é um esqueleto (TODO). */
    STATUS_ERRO_GRAFICO,        /**< Janela/OpenGL não pôde ser criada. */
    STATUS_SALDO_INSUFICIENTE,  /**< Esmeraldas insuficientes para comprar uma ajuda. */
    STATUS_ASSET_FALLBACK      /**< Não é erro: algum asset faltou e foi substituído. */
} Status;

#endif /* CONFIG_H */
