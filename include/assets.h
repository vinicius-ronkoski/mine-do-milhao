/**
 * @file assets.h
 * @brief Acesso centralizado a imagens, sons, música, fonte, frames e texto (com FALLBACK).
 *
 * Todo carregamento de arquivo da pasta assets/ e todo desenho de texto passam por aqui.
 * Se um arquivo não existir ou não for reconhecido, o jogo CONTINUA:
 *   - imagem de cenário/sprite -> textura xadrez magenta/preta ("textura faltando" do Minecraft);
 *   - imagem de interface (botão, painel, caixa) -> retângulo colorido com borda (assets_desenhar_painel);
 *   - fundo de tela -> fundo da fase (boss), senão o fundo padrão da tela, senão degradê na cor do boss
 *     (tudo dentro de assets_desenhar_fundo);
 *   - som/música -> silêncio;
 *   - fonte -> fonte padrão da raylib;
 *   - frames do boss -> um único quadro xadrez.
 *
 * PARA ADICIONAR UMA IMAGEM OU SOM NOVO:
 *   1. Coloque o arquivo em assets/sprites/ ou assets/sounds/ (extensões aceitas: assets/README.md).
 *   2. Acrescente o id no enum ImagemId ou SomId abaixo (ANTES do *_QUANTIDADE).
 *   3. Acrescente o nome SEM extensão na tabela de assets.c (mesma ordem do enum).
 *
 * FRAMES DOS BOSSES: assets/sprites/frames/<boss>_00, <boss>_01 ... (até MAX_QUADROS_ANIMACAO).
 * FUNDO DE CADA FASE (opcional): assets/sprites/fundos/<boss> (ex.: fundos/creeper.png).
 * Os prefixos <boss> ficam em assets.c (creeper, wither, warden, ender_dragon, herobrine).
 *
 * Só compila com USE_RAYLIB=1.
 */
#ifndef ASSETS_H
#define ASSETS_H

#include "raylib.h"
#include "config.h"
#include "boss.h"

/* ------------------------------------------------------------------------- */
/* Paleta de cores da interface (usada também como fallback de imagens)      */
/* ------------------------------------------------------------------------- */

/** @name Paleta (estilo Minecraft) */
/**@{*/
#define PALETA_FUNDO        (Color){  24,  28,  38, 255 }  /**< Fundo quando não há imagem. */
#define PALETA_PAINEL       (Color){  56,  56,  56, 235 }  /**< Painéis e caixas de texto. */
#define PALETA_BORDA        (Color){  16,  16,  16, 255 }  /**< Borda de painéis e botões. */
#define PALETA_BOTAO        (Color){ 111, 111, 111, 255 }  /**< Botão em repouso (cinza de pedra). */
#define PALETA_BOTAO_HOVER  (Color){ 126, 140, 190, 255 }  /**< Botão com o mouse em cima (azul do menu). */
#define PALETA_BOTAO_OFF    (Color){  60,  60,  60, 255 }  /**< Botão desabilitado. */
#define PALETA_TEXTO        (Color){ 240, 240, 240, 255 }  /**< Texto principal. */
#define PALETA_TEXTO_SUAVE  (Color){ 160, 160, 160, 255 }  /**< Texto secundário/desabilitado. */
#define PALETA_DESTAQUE     (Color){ 255, 222,  89, 255 }  /**< Amarelo (títulos, rodada atual). */
#define PALETA_ESMERALDA    (Color){  23, 221,  98, 255 }  /**< Verde esmeralda (prêmios). */
#define PALETA_ACERTO       (Color){  46, 160,  67, 255 }  /**< Alternativa certa. */
#define PALETA_ERRO         (Color){ 192,  57,  43, 255 }  /**< Alternativa errada / erros. */
#define PALETA_MARCO        (Color){ 102, 191, 255, 255 }  /**< Marcos de segurança (5 e 10). */
/**@}*/

/* ------------------------------------------------------------------------- */
/* Identificadores dos assets                                                */
/* ------------------------------------------------------------------------- */

/** Imagens estáticas (arquivos em assets/sprites/, nomes na tabela de assets.c). */
typedef enum {
    IMG_FUNDO_PERGUNTA = 0,   /**< Fundo da tela de perguntas (1280x720). */
    IMG_FUNDO_DIALOGO,        /**< Fundo das cenas de diálogo. */
    IMG_FUNDO_RESULTADO,      /**< Fundo da tela final. */
    IMG_PAINEL_ENUNCIADO,     /**< Moldura atrás do enunciado (fallback: painel cinza). */
    IMG_BOTAO,                /**< Skin única dos botões; o estado só muda a cor (fallback: retângulo). */
    IMG_CAIXA_DIALOGO,        /**< Caixa de fala (fallback: painel cinza). */
    IMG_ESMERALDA,            /**< Ícone da esmeralda (HUD e escada de prêmios). */
    IMG_RETRATO_ALDEAO,       /**< Retrato do apresentador nos diálogos. */
    IMG_QUANTIDADE            /**< Quantidade de imagens (não é uma imagem). */
} ImagemId;

/** Use em Fala.retrato quando a fala não tem retrato. */
#define IMG_NENHUMA IMG_QUANTIDADE

/** Efeitos sonoros curtos (arquivos em assets/sounds/). */
typedef enum {
    SOM_CLIQUE = 0,       /**< Clique em botão. */
    SOM_ACERTO,           /**< Resposta certa. */
    SOM_ERRO,             /**< Resposta errada. */
    SOM_DANO_BOSS,        /**< Boss perdeu vida. */
    SOM_BOSS_DERROTADO,   /**< Boss caiu. */
    SOM_VITORIA,          /**< Prêmio máximo. */
    SOM_QUANTIDADE        /**< Quantidade de sons (não é um som). */
} SomId;

/** Mais quadros por animação de boss (boss_00.png ... boss_07.png). */
#define MAX_QUADROS_ANIMACAO 8

/** Velocidade das animações dos bosses (segundos que cada quadro fica na tela). */
#define SEGUNDOS_POR_QUADRO 0.15f

/* ------------------------------------------------------------------------- */
/* Estruturas                                                                */
/* ------------------------------------------------------------------------- */

/** Uma imagem na GPU e se ela é substituta (fallback). */
typedef struct {
    Texture2D textura;   /**< Textura carregada do arquivo ou gerada. */
    bool fallback;       /**< true = o arquivo faltou e a textura foi gerada. */
} Imagem;

/** Um efeito sonoro; valido = false significa "tocar silêncio". */
typedef struct {
    Sound som;           /**< Som carregado. */
    bool valido;         /**< false = arquivo ausente ou sem áudio. */
} Efeito;

/** Sequência de quadros de um boss (vetor fixo: sem malloc). */
typedef struct {
    Texture2D quadros[MAX_QUADROS_ANIMACAO]; /**< Quadros na ordem de exibição. */
    int quantidade;                          /**< Quadros em uso (>= 1 depois de carregar). */
    bool fallback;                           /**< true = nenhum quadro encontrado (1 quadro gerado). */
} Animacao;

/** Todos os recursos do jogo. Vive dentro de Jogo (jogo.h): sem variável global. */
typedef struct {
    Imagem imagens[IMG_QUANTIDADE];          /**< Imagens estáticas (ImagemId). */
    Efeito sons[SOM_QUANTIDADE];             /**< Efeitos sonoros (SomId). */
    Animacao bosses[BOSS_QUANTIDADE];        /**< Frames de cada boss (índice = BossId). */
    Imagem fundos_boss[BOSS_QUANTIDADE];     /**< Fundo de cada fase (opcional; fallback = true se não existe). */
    Music musica;                            /**< Música de fundo (stream). */
    bool musica_valida;                      /**< false = sem música. */
    Font fonte;                              /**< Fonte do jogo. */
    bool fonte_propria;                      /**< false = usando a fonte padrão da raylib. */
    bool audio_ativo;                        /**< false = sem dispositivo de áudio (tudo em silêncio). */
    int total_fallbacks;                     /**< Quantos assets caíram no fallback (aviso no terminal). */
} Assets;

/* ------------------------------------------------------------------------- */
/* Carga e liberação                                                         */
/* ------------------------------------------------------------------------- */

/**
 * @brief Carrega TODOS os assets, aplicando fallback onde faltar arquivo.
 *
 * Deve ser chamada DEPOIS de InitWindow(). Inicia o áudio, carrega fonte, imagens,
 * sons, música e frames. Cada arquivo que falta escreve um aviso no terminal e
 * soma em total_fallbacks — NUNCA aborta o jogo.
 *
 * @return STATUS_OK (tudo carregado), STATUS_ASSET_FALLBACK (jogável, com substitutos),
 *         STATUS_ERRO_PARAMETRO (NULL) ou STATUS_ERRO_GRAFICO (janela ainda não criada).
 */
Status assets_carregar(Assets *assets);

/** @brief Libera texturas, sons, música e fonte e fecha o áudio. Chame antes de CloseWindow(). */
void assets_descarregar(Assets *assets);

/* ------------------------------------------------------------------------- */
/* Imagens                                                                   */
/* ------------------------------------------------------------------------- */

/**
 * @brief Desenha o fundo cobrindo a janela inteira, escolhido pela fase (boss) atual.
 *
 * Ordem de escolha, da mais específica para a mais genérica:
 *   1. o fundo da fase: assets/sprites/fundos/<boss> (se existir);
 *   2. a imagem `fundo_padrao` da tela (ex.: IMG_FUNDO_PERGUNTA), se existir;
 *   3. um degradê escuro na cor do boss (ou neutro, sem boss) — nunca o xadrez.
 * Assim o cenário muda a cada boss mesmo antes de existir arte para ele.
 *
 * @param fundo_padrao Fundo genérico da tela (IMG_FUNDO_PERGUNTA, IMG_FUNDO_DIALOGO...).
 * @param boss         Boss da fase, ou BOSS_QUANTIDADE para não usar fundo de fase (ex.: resultado).
 */
void assets_desenhar_fundo(const Assets *assets, ImagemId fundo_padrao, BossId boss);

/** @brief true se a imagem é substituta (arquivo ausente) ou o id é inválido. */
bool assets_imagem_fallback(const Assets *assets, ImagemId id);

/**
 * @brief Desenha uma imagem esticada para caber em `destino`.
 * Trocar o PNG não exige mudar código. Id inválido: não desenha nada.
 * @param tint Cor de multiplicação (WHITE = cores originais).
 */
void assets_desenhar_imagem(const Assets *assets, ImagemId id, Rectangle destino, Color tint);

/**
 * @brief Desenha um elemento de interface (painel, botão, caixa de diálogo).
 *
 * Se a imagem existe, desenha a imagem multiplicada por `tint`. Se faltou, desenha um
 * retângulo em `cor_se_faltar` com borda escura — nunca o xadrez magenta, que ficaria ilegível.
 *
 * @param cor_se_faltar Cor do retângulo quando não há imagem (ex.: PALETA_BOTAO_HOVER).
 * @param tint          Cor de multiplicação da imagem (WHITE = original; mais escura = desabilitado).
 */
void assets_desenhar_painel(const Assets *assets, ImagemId id, Rectangle destino,
                            Color cor_se_faltar, Color tint);

/**
 * @brief Desenha o quadro atual da animação de um boss.
 * @param tempo Segundos desde o início (a animação roda em loop, SEGUNDOS_POR_QUADRO por quadro).
 */
void assets_desenhar_boss(const Assets *assets, BossId id, float tempo, Rectangle destino, Color tint);

/* ------------------------------------------------------------------------- */
/* Som e música                                                              */
/* ------------------------------------------------------------------------- */

/** @brief Toca um efeito; não faz nada se o som faltou ou não há áudio. */
void assets_tocar_som(const Assets *assets, SomId id);

/** @brief Inicia a música de fundo em loop (se existir). */
void assets_tocar_musica(const Assets *assets);

/** @brief Chamar UMA vez por quadro: alimenta o stream da música (sem isso ela para). */
void assets_atualizar_musica(const Assets *assets);

/* ------------------------------------------------------------------------- */
/* Texto (sempre com a fonte do jogo; aceita UTF-8 com acentos)               */
/* A fonte carrega os caracteres 32 a 255 (Latin-1): letras acentuadas e ç     */
/* funcionam; travessão (—), aspas curvas (“ ”) e reticências (…) viram "?".   */
/* ------------------------------------------------------------------------- */

/** @brief Largura em pixels de um texto de uma linha. */
float assets_largura_texto(const Assets *assets, const char *texto, float tamanho);

/** @brief Desenha uma linha de texto a partir de `posicao` (canto superior esquerdo). */
void assets_desenhar_texto(const Assets *assets, const char *texto, Vector2 posicao, float tamanho, Color cor);

/** @brief Desenha uma linha de texto centralizada (horizontal e vertical) em `area`. */
void assets_desenhar_texto_centralizado(const Assets *assets, const char *texto, Rectangle area,
                                        float tamanho, Color cor);

/**
 * @brief Desenha texto com quebra de linha automática dentro de `area` (o que não cabe na altura é cortado).
 *
 * @param limite_letras Se >= 0, desenha só as primeiras N letras (efeito máquina de escrever;
 *                      conta letras UTF-8, então "ç" vale 1). Use -1 para o texto inteiro.
 * @return Quantidade de linhas que o texto inteiro ocupa nessa largura.
 */
int assets_desenhar_texto_quebrado(const Assets *assets, const char *texto, Rectangle area,
                                   float tamanho, Color cor, int limite_letras);

/**
 * @brief Como assets_desenhar_texto_quebrado(), mas diminui a fonte até o texto caber inteiro
 *        e centraliza as linhas na vertical. Tenta tamanho_maximo, tamanho_maximo - 2, ... até tamanho_minimo.
 */
void assets_desenhar_texto_ajustado(const Assets *assets, const char *texto, Rectangle area,
                                    float tamanho_maximo, float tamanho_minimo, Color cor);

/** @brief Quantidade de letras (codepoints UTF-8) de um texto. "ação" = 4. */
int assets_contar_letras(const char *texto);

#endif /* ASSETS_H */
