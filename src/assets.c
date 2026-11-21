/** @file assets.c @brief Esqueleto do módulo de assets (ver assets.h). Compila só com USE_RAYLIB. */
#ifdef USE_RAYLIB
#include <stdio.h>
#include <stdlib.h>
#include "assets.h"
#include "utils.h"

Status assets_carregar(Assets *assets)
{
    /* TODO: memset; checar IsWindowReady(); chamar as funções assets_carregar_* abaixo;
     *       somar total_fallbacks; retornar STATUS_ASSET_FALLBACK se total_fallbacks > 0. */
    (void)assets;
    return STATUS_NAO_IMPLEMENTADO;
}

void assets_descarregar(Assets *assets)
{
    /* TODO: Unload* de tudo que estiver válido; free(quadros); CloseAudioDevice() se audio_ativo. */
    (void)assets;
}

const char *assets_nome_arquivo_imagem(ImagemId id)
{
    /* TODO: static const char *const nomes[IMG_QUANTIDADE] LOCAL; ex.: CAMINHO_SPRITES "fundo_pergunta.png".
     *       PARA TROCAR UM ASSET: altere só o nome aqui. */
    (void)id;
    return "";
}

const char *assets_nome_arquivo_som(SomId id)
{
    /* TODO: idem, com CAMINHO_SONS (.wav para efeitos; .ogg/.mp3 para música). */
    (void)id;
    return "";
}

Status assets_carregar_imagem(Imagem *alvo, ImagemId id)
{
    /* TODO: FileExists -> LoadImage -> IsImageValid -> LoadTextureFromImage -> UnloadImage;
     *       falhou: alvo->textura = assets_gerar_fallback(w, h); alvo->fallback = true. */
    (void)alvo; (void)id;
    return STATUS_NAO_IMPLEMENTADO;
}

Texture2D assets_gerar_fallback(int largura, int altura)
{
    /* TODO: GenImageChecked(largura, altura, 32, 32, MAGENTA, BLACK) -> textura -> UnloadImage. */
    Texture2D vazia = { 0 };
    (void)largura; (void)altura;
    return vazia;
}

Status assets_carregar_som(Efeito *alvo, SomId id)
{
    /* TODO: FileExists -> LoadSound -> IsSoundValid; falhou: valido = false. */
    (void)alvo; (void)id;
    return STATUS_NAO_IMPLEMENTADO;
}

Status assets_carregar_musica(Assets *assets)
{
    /* TODO: LoadMusicStream(CAMINHO_SONS "musica.ogg"); IsMusicValid. */
    (void)assets;
    return STATUS_NAO_IMPLEMENTADO;
}

Status assets_carregar_fonte(Assets *assets)
{
    /* TODO: int pontos[224]; preencher com 32..255; LoadFontEx(ARQUIVO_FONTE, TAM_FONTE_BASE, pontos, 224);
     *       IsFontValid? senão GetFontDefault() e fonte_propria = false. */
    (void)assets;
    return STATUS_NAO_IMPLEMENTADO;
}

Status assets_carregar_animacao(Animacao *alvo, const char *prefixo, float segundos_por_quadro)
{
    /* TODO: laço com TextFormat("%s%s_%02d.png", CAMINHO_FRAMES, prefixo, i) + realloc do vetor de quadros.
     *       ALTERNATIVA futura: spritesheet única + DrawTextureRec (trocar só esta função). */
    (void)alvo; (void)prefixo; (void)segundos_por_quadro;
    return STATUS_NAO_IMPLEMENTADO;
}

Texture2D assets_imagem(const Assets *assets, ImagemId id)
{
    /* TODO: retornar assets->imagens[id].textura com checagem de intervalo. */
    Texture2D vazia = { 0 };
    (void)assets; (void)id;
    return vazia;
}

void assets_desenhar_imagem(const Assets *assets, ImagemId id, Rectangle destino, Color tint)
{
    /* TODO: DrawTexturePro(textura, {0,0,w,h}, destino, {0,0}, 0, tint). */
    (void)assets; (void)id; (void)destino; (void)tint;
}

void assets_tocar_som(const Assets *assets, SomId id)
{
    /* TODO: if (audio_ativo && sons[id].valido) PlaySound(...). */
    (void)assets; (void)id;
}

void assets_tocar_musica(Assets *assets)
{
    /* TODO: PlayMusicStream + SetMusicVolume. */
    (void)assets;
}

void assets_atualizar_musica(Assets *assets)
{
    /* TODO: UpdateMusicStream(assets->musica) se válida. */
    (void)assets;
}

Texture2D assets_quadro_animacao(const Assets *assets, AnimacaoId id, float tempo)
{
    /* TODO: índice = (int)(tempo / segundos_por_quadro) % quantidade. */
    Texture2D vazia = { 0 };
    (void)assets; (void)id; (void)tempo;
    return vazia;
}

ImagemId assets_imagem_do_boss(BossId id)
{
    /* TODO: IMG_BOSS_CREEPER + id (enum na mesma ordem). */
    (void)id;
    return IMG_BOSS_CREEPER;
}

AnimacaoId assets_animacao_do_boss(BossId id)
{
    /* TODO: ANIM_BOSS_CREEPER + id. */
    (void)id;
    return ANIM_BOSS_CREEPER;
}
#endif /* USE_RAYLIB */
