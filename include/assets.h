/**
 * @file assets.h
 * @brief Acesso centralizado a imagens, sons, música, fonte e frames (com FALLBACK).
 *
 * REGRA DE OURO: nenhum outro módulo chama LoadTexture/LoadSound direto. Tudo passa por aqui.
 * Se um arquivo não existir ou não for reconhecido, o jogo CONTINUA:
 *   - imagem  -> textura xadrez magenta/preto (ou cor sólida) gerada em memória;
 *   - som     -> silêncio (Efeito.valido = false);
 *   - fonte   -> fonte padrão da raylib;
 *   - frames  -> um único quadro gerado.
 *
 * COMO ADICIONAR UM ASSET NOVO (3 passos):
 *   1. Coloque o arquivo em assets/sprites, assets/sounds ou assets/fonts.
 *   2. Acrescente o id no enum correspondente abaixo (ANTES do *_QUANTIDADE).
 *   3. Acrescente o nome do arquivo em assets_nome_arquivo_imagem() / _som() (assets.c).
 *
 * Só compila com USE_RAYLIB=1.
 */
#ifndef ASSETS_H
#define ASSETS_H

#include <stdbool.h>
#include "raylib.h"
#include "config.h"
#include "boss.h"

/** Imagens estáticas. */
typedef enum {
    IMG_FUNDO_PERGUNTA = 0,   /**< Fundo da tela de perguntas. */
    IMG_FUNDO_DIALOGO,        /**< Fundo das cenas de diálogo. */
    IMG_FUNDO_RESULTADO,      /**< Fundo da tela final. */
    IMG_PAINEL_ENUNCIADO,     /**< Moldura atrás do enunciado. */
    IMG_BOTAO_NORMAL,         /**< Skin do botão em repouso. */
    IMG_BOTAO_HOVER,          /**< Skin com o mouse em cima. */
    IMG_BOTAO_PRESSIONADO,    /**< Skin pressionado. */
    IMG_BOTAO_DESABILITADO,   /**< Skin desabilitado. */
    IMG_CAIXA_DIALOGO,        /**< Caixa de fala. */
    IMG_ESMERALDA,            /**< Ícone da esmeralda (HUD). */
    IMG_BOSS_CREEPER,
    IMG_BOSS_WITHER,
    IMG_BOSS_WARDEN,
    IMG_BOSS_ENDER_DRAGON,
    IMG_BOSS_HEROBRINE,
    IMG_RETRATO_ALDEAO,       /**< Retrato do apresentador (diálogos). */
    IMG_QUANTIDADE
} ImagemId;

/** Efeitos sonoros curtos. */
typedef enum {
    SOM_CLIQUE = 0,
    SOM_ACERTO,
    SOM_ERRO,
    SOM_DANO_BOSS,
    SOM_BOSS_DERROTADO,
    SOM_VITORIA,
    SOM_QUANTIDADE
} SomId;

/** Animações por frames (arquivos nome_00.png, nome_01.png...). */
typedef enum {
    ANIM_ESMERALDA = 0,
    ANIM_BOSS_CREEPER,
    ANIM_BOSS_WITHER,
    ANIM_BOSS_WARDEN,
    ANIM_BOSS_ENDER_DRAGON,
    ANIM_BOSS_HEROBRINE,
    ANIM_QUANTIDADE
} AnimacaoId;

/** Uma imagem e se ela é substituta (fallback). */
typedef struct { Texture2D textura; bool fallback; } Imagem;

/** Um efeito sonoro; valido = false significa "tocar silêncio". */
typedef struct { Sound som; bool valido; } Efeito;

/** Sequência de quadros. */
typedef struct {
    Texture2D *quadros;          /**< Vetor alocado dinamicamente (assets_descarregar libera). */
    int quantidade;              /**< Número de quadros (>= 1 mesmo em fallback). */
    float segundos_por_quadro;   /**< Duração de cada quadro. */
    bool fallback;
} Animacao;

/** Todos os recursos do jogo. Vive dentro de Jogo (jogo.h): sem variável global. */
typedef struct {
    Imagem imagens[IMG_QUANTIDADE];
    Efeito sons[SOM_QUANTIDADE];
    Animacao animacoes[ANIM_QUANTIDADE];
    Music musica;
    bool musica_valida;
    Font fonte;
    bool fonte_propria;          /**< false = usando a fonte padrão da raylib. */
    bool audio_ativo;            /**< false = sem dispositivo de áudio (tudo silencioso). */
    int total_fallbacks;         /**< Quantos assets caíram no fallback (para aviso no log). */
} Assets;

/**
 * @brief Carrega TODOS os assets, aplicando fallback onde faltar arquivo.
 *
 * Passos: IsWindowReady() (senão STATUS_ERRO_GRAFICO); InitAudioDevice() (falhou: audio_ativo=false);
 * fonte; imagens; sons; música; animações. Cada falha individual incrementa total_fallbacks e
 * escreve um aviso em stderr — NUNCA aborta.
 *
 * @return STATUS_OK (tudo carregado), STATUS_ASSET_FALLBACK (jogável, mas com substitutos),
 *         STATUS_ERRO_PARAMETRO ou STATUS_ERRO_GRAFICO (janela ainda não criada).
 */
Status assets_carregar(Assets *assets);

/** @brief Libera tudo (UnloadTexture/UnloadSound/UnloadMusicStream/UnloadFont, CloseAudioDevice, free dos quadros). */
void assets_descarregar(Assets *assets);

/** @brief Caminho do arquivo de uma imagem (monta com PASTA_SPRITES). Tabela local, sem global. Nunca NULL. */
const char *assets_nome_arquivo_imagem(ImagemId id);

/** @brief Caminho do arquivo de um som (monta com PASTA_SONS). Nunca NULL. */
const char *assets_nome_arquivo_som(SomId id);

/**
 * @brief Carrega uma imagem; se falhar (arquivo ausente ou IsImageValid() falso), gera fallback.
 * @return STATUS_OK ou STATUS_ASSET_FALLBACK (alvo preenchido nos dois casos); STATUS_ERRO_PARAMETRO se NULL.
 */
Status assets_carregar_imagem(Imagem *alvo, ImagemId id);

/**
 * @brief Gera a textura substituta (xadrez magenta/preto, estilo "textura faltando").
 * Para mudar o visual do fallback, edite só esta função.
 */
Texture2D assets_gerar_fallback(int largura, int altura);

/** @brief Carrega um efeito sonoro; falhou -> valido=false. Mesmo retorno de assets_carregar_imagem. */
Status assets_carregar_som(Efeito *alvo, SomId id);

/** @brief Carrega a música de fundo (stream). Falhou -> musica_valida=false. */
Status assets_carregar_musica(Assets *assets);

/** @brief Carrega ARQUIVO_FONTE com codepoints 32..255 (acentos do português). Falhou -> fonte padrão. */
Status assets_carregar_fonte(Assets *assets);

/**
 * @brief Carrega frames "prefixo_00.png".."prefixo_NN.png" até o primeiro que não existir
 *        (PASTA_FRAMES). Nenhum encontrado -> 1 quadro de fallback.
 */
Status assets_carregar_animacao(Animacao *alvo, const char *prefixo, float segundos_por_quadro);

/** @brief Textura de uma imagem (nunca inválida: usa o fallback se preciso). */
Texture2D assets_imagem(const Assets *assets, ImagemId id);

/**
 * @brief Desenha uma imagem esticada para caber em `destino` (DrawTexturePro).
 * Todas as telas usam esta função: trocar o PNG não exige mudar código.
 */
void assets_desenhar_imagem(const Assets *assets, ImagemId id, Rectangle destino, Color tint);

/** @brief Toca um efeito; ignora silenciosamente se !valido ou !audio_ativo. */
void assets_tocar_som(const Assets *assets, SomId id);

/** @brief Inicia a música de fundo em loop (se válida). */
void assets_tocar_musica(Assets *assets);

/** @brief Chamar UMA vez por frame: UpdateMusicStream(). Sem isso a música para. */
void assets_atualizar_musica(Assets *assets);

/** @brief Quadro de uma animação para um instante `tempo` (segundos): (tempo / segundos_por_quadro) % quantidade. */
Texture2D assets_quadro_animacao(const Assets *assets, AnimacaoId id, float tempo);

/** @brief Imagem estática do boss (BOSS_CREEPER -> IMG_BOSS_CREEPER...). */
ImagemId assets_imagem_do_boss(BossId id);

/** @brief Animação do boss (BOSS_CREEPER -> ANIM_BOSS_CREEPER...). */
AnimacaoId assets_animacao_do_boss(BossId id);

#endif /* ASSETS_H */
