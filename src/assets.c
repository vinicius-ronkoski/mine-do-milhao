/**
 * @file assets.c
 * @brief Carga de assets com fallback, desenho de imagens, som, música e texto (ver assets.h).
 *
 * Organização do arquivo:
 *   1. Constantes e tabelas de nomes de arquivo (locais às funções: sem variável global)
 *   2. Funções auxiliares internas (static)
 *   3. Carga e liberação
 *   4. Desenho de imagens
 *   5. Som e música
 *   6. Texto
 *
 * Compila só com USE_RAYLIB=1.
 */
#ifdef USE_RAYLIB
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "assets.h"

/* ---- 1. Constantes ---- */

/** Maior caminho de arquivo montado aqui (pasta + nome). */
#define TAM_CAMINHO_ASSET 160

/** Primeiro caractere carregado na fonte (espaço). */
#define CODEPOINT_INICIAL 32

/** Caracteres carregados na fonte: 32 a 255 (Latin-1, inclui á, ç, õ...). */
#define NUM_CODEPOINTS 224

/** Lado (em pixels) da textura xadrez gerada quando uma imagem falta. */
#define LADO_FALLBACK 16

/** Quantos quadrados o xadrez de fallback tem por lado. */
#define QUADRADOS_FALLBACK 4

/** Espessura da borda dos painéis desenhados como fallback. */
#define BORDA_PAINEL 3.0f

/** Imagens com pelo menos esta largura são cenários (suavizados); as menores são pixel art (nítidas).
 *  Os fundos têm 854 px ou mais; retratos e ícones, no máximo 600 px. */
#define LARGURA_MINIMA_SUAVIZADA 800

/** 1 = fundos suavizados (BILINEAR); 0 = nítidos (POINT). */
#define SUAVIZAR_FUNDOS 1

/** Maior ampliação usada para reler a fonte (4x = 128 px: monitores 4K e acima reaproveitam este atlas). */
#define ZOOM_MAXIMO_FONTE 4.0f

/** Distância entre linhas de texto, em múltiplos do tamanho da fonte. */
#define ESPACO_ENTRE_LINHAS 1.25f

/** Maior linha de texto montada na quebra automática. */
#define TAM_LINHA_TEXTO 512

/** Quantidade de elementos de um vetor declarado (não funciona com ponteiro). */
#define QUANTIDADE_DE(vetor) (sizeof(vetor) / sizeof((vetor)[0]))

/**
 * @brief Nome do arquivo de cada imagem (em CAMINHO_SPRITES, SEM extensão), na ordem do enum ImagemId.
 * PARA TROCAR UMA IMAGEM: troque só o nome na lista (ou salve a imagem com este nome).
 * Extensões aceitas: ver achar_imagem().
 */
static const char *nome_arquivo_imagem(ImagemId id)
{

    static const char *const nomes[IMG_QUANTIDADE] = {
        /* Objetos */
        "painel_enunciado",         /* IMG_PAINEL_ENUNCIADO       */
        "botao",                    /* IMG_BOTAO                  */
        "caixa_dialogo",            /* IMG_CAIXA_DIALOGO          */
        "esmeralda",                /* IMG_ESMERALDA              */
        "titulo",                   /* IMG_TITULO                 */

        /* Fundos */
        "fundo_menu",               /* IMG_FUNDO_MENU             */
        "fundo_pergunta",           /* IMG_FUNDO_PERGUNTA         */
        "fundo_dialogo",            /* IMG_FUNDO_DIALOGO          */
        "palco",                    /* IMG_FUNDO_PALCO            */
        "fundo_portal",             /* IMG_FUNDO_PORTAL           */
        "fundo_fim",                /* IMG_FUNDO_FIM              */
        "auditorio",                /* IMG_FUNDO_AUDITORIO        */
        "fundo_preto",              /* IMG_FUNDO_PRETO            */
        "fundo_vitoria",            /* IMG_FUNDO_VITORIA          */
        "fundo_derrota",            /* IMG_FUNDO_DERROTA          */
        "fundo_resultado",          /* IMG_FUNDO_RESULTADO        */

        /* Ícones */
        "icone_aldeao00",           /* IMG_ICONE_ALDEAO           */
        "icone_aldeao01",           /* IMG_ICONE_ALDEAO_ASSUSTADO */
        "icone_desconhecido00",     /* IMG_ICONE_DESCONHECIDO     */
        "icone_desconhecido01",     /* IMG_ICONE_DESCONHECIDO     */
        "retrato_creeper",          /* IMG_ICONE_CREEPER          */
        "retrato_wither",           /* IMG_ICONE_WITHER           */
        "retrato_warden",           /* IMG_ICONE_WARDEN           */
        "retrato_ender_dragon",     /* IMG_ICONE_ENDER_DRAGON     */
        "retrato_steve",            /* IMG_ICONE_STEVE            */
        "retrato_herobrine",        /* IMG_ICONE_HEROBRINE        */

        /* História 1 */
        "h1_00", "h1_01","h1_02",   /* IMG_H1_CENA_1...3          */

        /* História 2 */
        "h2_00", "h2_01", "h2_02",  /* IMG_H2_CENA_1...3          */
        "h2_03", "h2_04", "h2_05",  /* IMG_H1_CENA_4...6          */
        "h2_06", "h2_07", "h2_08",  /* IMG_H1_CENA_7...9          */

        /* História 3 */
        "h3_00", "h3_01", "h3_02",  /* IMG_H3_CENA_1...3          */
        "h3_03", "h3_04",           /* IMG_H3_CENA_4...5          */

        /* História 4 */
        "h4_00", "h4_01", "h4_02",  /* IMG_H4_CENA_1...3          */
        "h4_03", "h4_04",           /* IMG_H4_CENA_4...5          */

        /* História 5 */
        "h5_00", "h5_01", "h5_02",  /* IMG_H5_CENA_1...3          */

        /* História 6 (final) */
        "h6_00", "h6_01", "h6_02",  /* IMG_H6_CENA_1...3          */
        "h6_03", "h6_04", "h6_05",  /* IMG_H6_CENA_4...6          */
        "h6_06", "h6_07", "h6_08",  /* IMG_H6_CENA_7...9          */
        "h6_09"                     /* IMG_H6_CENA_10             */
    };

    if ((int)id < 0 || id >= IMG_QUANTIDADE || nomes[id] == NULL) {
        return "";
    }
    return nomes[id];
}

/** @brief Nome do arquivo de cada som (em CAMINHO_SONS, SEM extensão), na ordem do enum SomId. */
static const char *nome_arquivo_musica(MusicaId id)
{
    static const char *const nomes[MUSICA_QUANTIDADE] = {
        "musica",                /* MUSICA_PRINCIPAL     */
        "musica_creeper",        /* MUSICA_CREEPER       */
        "musica_wither",         /* MUSICA_WITHER        */
        "musica_warden",         /* MUSICA_WARDEN        */
        "musica_end",            /* MUSICA_END           */
        "musica_moog_city",      /* MUSICA_MOOG_CITY     */
        "musica_herobrine",      /* MUSICA_HEROBRINE     */
        "musica_equinoxe"        /* MUSICA_EQUINOXE      */
    };

    if ((int)id < 0 || id >= MUSICA_QUANTIDADE || nomes[id] == NULL) {
        return "";
    }
    return nomes[id];
}

static const char *nome_arquivo_som(SomId id)
{
    static const char *const nomes[SOM_QUANTIDADE] = {
        "clique",                /* SOM_CLIQUE               */
        "acerto",                /* SOM_ACERTO               */
        "erro",                  /* SOM_ERRO                 */
        "dano",                  /* SOM_DANO_BOSS            */
        "boss_derrotado",        /* SOM_BOSS_DERROTADO       */
        "vitoria",               /* SOM_VITORIA              */
        "plateia",               /* SOM_PLATEIA              */
        "entrada_creeper",       /* SOM_CREEPER              */
        "portal",                /* SOM_PORTAL               */
        "aldeao",                /* SOM_ALDEAO               */
        "sculk",                 /* SOM_SCULK                */
        "ender_dragon_00",       /* SOM_ENDER_DRAGON_VOO     */
        "ender_dragon_01",       /* SOM_ENDER_DRAGON_IDLE    */
        "flint_and_steel",       /* SOM_FLINT_AND_STEEL      */
        "the_nether",            /* SOM_THE_NETHER           */
        "receba"                 /* SOM_RECEBA               */
    };

    if ((int)id < 0 || id >= SOM_QUANTIDADE || nomes[id] == NULL) {
        return "";
    }
    return nomes[id];
}

/**
 * @brief Nome-base dos arquivos de cada boss: frames (CAMINHO_FRAMES/<prefixo>_00...) e
 *        fundo da fase (CAMINHO_FUNDOS/<prefixo>), sempre + extensão de imagem.
 */
static const char *prefixo_do_boss(BossId id)
{
    static const char *const prefixos[BOSS_QUANTIDADE] = {
        "creeper", "wither", "warden", "ender_dragon", "herobrine"
    };

    if ((int)id < 0 || id >= BOSS_QUANTIDADE) {
        return "";
    }
    return prefixos[id];
}

/**
 * @brief Cor-tema de cada fase, usada no degradê quando não há imagem de fundo.
 * PARA MUDAR O CLIMA DE UMA FASE SEM ARTE: troque a cor aqui.
 */
static Color cor_tema_boss(BossId id)
{
    static const Color cores[BOSS_QUANTIDADE] = {
        {  40,  90,  40, 255 },   /* Creeper: verde da grama   */
        {  45,  40,  55, 255 },   /* Wither: cinza escuro     */
        {  10,  45,  55, 255 },   /* Warden: deep dark azulado */
        {  50,  20,  70, 255 },   /* Ender Dragon: roxo do End */
        {  70,  15,  15, 255 }    /* Herobrine: vermelho-escuro */
    };

    if ((int)id < 0 || id >= BOSS_QUANTIDADE) {
        return PALETA_FUNDO;
    }
    return cores[id];
}

/* ---- 2. Funções auxiliares internas ---- */

/**
 * @brief Registra no terminal que um asset caiu no fallback e conta.
 * @param caminho Arquivo procurado (ou encontrado, se era inválido).
 * @param motivo  "não encontrado" ou "inválido".
 */
static void avisar_fallback(Assets *assets, const char *caminho, const char *motivo)
{
    fprintf(stderr, "Aviso: asset %s: %s (usando substituto)\n", motivo, caminho);
    assets->total_fallbacks++;
}

/**
 * @brief Procura `base` + cada extensão da lista e devolve o primeiro arquivo que existe.
 *
 * @param base        Caminho sem extensão (ex.: "assets/sounds/clique").
 * @param extensoes   Extensões aceitas, na ordem de preferência (ex.: ".wav").
 * @param quantidade  Quantas extensões há na lista.
 * @param caminho     Recebe o caminho encontrado; se nada existir, recebe só a `base`
 *                    (para o aviso dizer qual arquivo faltou; as extensões estão no assets/README.md).
 * @param capacidade  Tamanho do buffer `caminho`.
 * @return true se achou um arquivo.
 */
static bool achar_arquivo(const char *base, const char *const extensoes[], size_t quantidade,
                          char *caminho, size_t capacidade)
{
    size_t indice;

    for (indice = 0; indice < quantidade; indice++) {
        int escritos = snprintf(caminho, capacidade, "%s%s", base, extensoes[indice]);

        /* Caminho cortado (maior que o buffer) apontaria para outro arquivo: pula */
        if (escritos >= 0 && (size_t)escritos < capacidade && FileExists(caminho)) {
            return true;
        }
    }

    /* Nada encontrado: devolve só a base, para o aviso dizer qual arquivo faltou */
    snprintf(caminho, capacidade, "%s", base);
    return false;
}

/** @brief achar_arquivo() com as extensões de imagem que a raylib lê. */
static bool achar_imagem(const char *base, char *caminho, size_t capacidade)
{
    static const char *const extensoes[] = { ".png", ".jpg", ".jpeg", ".bmp" };

    return achar_arquivo(base, extensoes, QUANTIDADE_DE(extensoes), caminho, capacidade);
}

/** @brief achar_arquivo() com as extensões de áudio que a raylib lê (sons e música). */
static bool achar_audio(const char *base, char *caminho, size_t capacidade)
{
    static const char *const extensoes[] = { ".wav", ".ogg", ".mp3" };

    return achar_arquivo(base, extensoes, QUANTIDADE_DE(extensoes), caminho, capacidade);
}

/** @brief achar_arquivo() com as extensões de fonte. */
static bool achar_fonte(const char *base, char *caminho, size_t capacidade)
{
    static const char *const extensoes[] = { ".ttf", ".otf" };

    return achar_arquivo(base, extensoes, QUANTIDADE_DE(extensoes), caminho, capacidade);
}

/** @brief Gera a textura substituta: xadrez magenta/preto, a "textura faltando" do Minecraft. */
static Texture2D gerar_textura_fallback(void)
{
    Color magenta = { 248, 0, 248, 255 };
    Image xadrez = GenImageChecked(LADO_FALLBACK, LADO_FALLBACK,
                                   LADO_FALLBACK / QUADRADOS_FALLBACK,
                                   LADO_FALLBACK / QUADRADOS_FALLBACK, magenta, BLACK);
    Texture2D textura = LoadTextureFromImage(xadrez);

    UnloadImage(xadrez);
    return textura;
}

/**
 * @brief Carrega uma textura de um arquivo que já se sabe que existe.
 * @return true se a raylib reconheceu a imagem; false caso contrário.
 */
static bool carregar_textura_de_arquivo(const char *caminho, Texture2D *destino)
{
    Image imagem = LoadImage(caminho);

    if (!IsImageValid(imagem)) {
        return false;  /* arquivo existe, mas não é uma imagem que a raylib entende */
    }

    *destino = LoadTextureFromImage(imagem);
    UnloadImage(imagem);
    if (!IsTextureValid(*destino)) {
        return false;
    }

    /* Fundos grandes podem ser suavizados (SUAVIZAR_FUNDOS); pixel art pequena fica sempre POINT (nítida) */
    if (SUAVIZAR_FUNDOS && destino->width >= LARGURA_MINIMA_SUAVIZADA) {
        SetTextureFilter(*destino, TEXTURE_FILTER_BILINEAR);
    }
    return true;
}

/** @brief Carrega uma imagem estática; se falhar, usa o xadrez. */
static void carregar_imagem(Assets *assets, ImagemId id)
{
    char base[TAM_CAMINHO_ASSET], caminho[TAM_CAMINHO_ASSET];
    Imagem *imagem = &assets->imagens[id];
    bool achou;

    snprintf(base, sizeof(base), "%s%s", CAMINHO_SPRITES, nome_arquivo_imagem(id));
    achou = achar_imagem(base, caminho, sizeof(caminho));
    imagem->fallback = !achou || !carregar_textura_de_arquivo(caminho, &imagem->textura);
    if (imagem->fallback) {
        imagem->textura = gerar_textura_fallback();
        avisar_fallback(assets, caminho, achou ? "inválido" : "não encontrado");
    }
}

/**
 * @brief Carrega o fundo de uma fase (opcional). Se não existir, fica sem textura e
 *        assets_desenhar_fundo usa o fundo padrão da tela. Só um arquivo corrompido gera aviso.
 * @return true se o fundo foi carregado.
 */
static bool carregar_fundo_boss(Assets *assets, BossId id)
{
    char base[TAM_CAMINHO_ASSET], caminho[TAM_CAMINHO_ASSET];
    Imagem *fundo = &assets->fundos_boss[id];
    bool achou;

    snprintf(base, sizeof(base), "%s%s", CAMINHO_FUNDOS, prefixo_do_boss(id));
    achou = achar_imagem(base, caminho, sizeof(caminho));
    fundo->fallback = !achou || !carregar_textura_de_arquivo(caminho, &fundo->textura);
    if (fundo->fallback) {
        fundo->textura.id = 0;  /* sem textura: nada a descarregar */
        if (achou) {
            avisar_fallback(assets, caminho, "inválido");
        }
    }
    return !fundo->fallback;
}

/** @brief Carrega os frames de um boss até o primeiro que faltar; nenhum -> 1 quadro sem textura. */
static void carregar_animacao_boss(Assets *assets, BossId id)
{
    char base[TAM_CAMINHO_ASSET], caminho[TAM_CAMINHO_ASSET], primeiro[TAM_CAMINHO_ASSET];
    Animacao *animacao = &assets->bosses[id];

    animacao->quantidade = 0;
    primeiro[0] = '\0';
    for (int indice_quadro = 0; indice_quadro < MAX_QUADROS_ANIMACAO; indice_quadro++) {
        snprintf(base, sizeof(base), "%s%s_%02d", CAMINHO_FRAMES, prefixo_do_boss(id), indice_quadro);
        if (!achar_imagem(base, caminho, sizeof(caminho))
            || !carregar_textura_de_arquivo(caminho, &animacao->quadros[indice_quadro])) {
            if (indice_quadro == 0) {
                snprintf(primeiro, sizeof(primeiro), "%s", caminho);  /* para o aviso */
            }
            break;  /* a sequência termina no primeiro quadro que falta */
        }
        animacao->quantidade++;
    }

    animacao->fallback = (animacao->quantidade == 0);
    if (animacao->fallback) {
        animacao->quadros[0] = gerar_textura_fallback();
        animacao->quantidade = 1;
        avisar_fallback(assets, primeiro, "não encontrado");
    }
}

/** @brief Carrega um efeito sonoro; se faltar, fica em silêncio. */
static void carregar_som(Assets *assets, SomId id)
{
    char base[TAM_CAMINHO_ASSET], caminho[TAM_CAMINHO_ASSET];
    Efeito *efeito = &assets->sons[id];
    bool achou;

    efeito->valido = false;
    if (!assets->audio_ativo) {
        printf("Erro ao carregar som: Audio Inativo\n");
        return;  /* sem placa de som: o aviso único já foi dado em assets_carregar */
    }

    snprintf(base, sizeof(base), "%s%s", CAMINHO_SONS, nome_arquivo_som(id));
    achou = achar_audio(base, caminho, sizeof(caminho));
    if (achou) {
        efeito->som = LoadSound(caminho);
        efeito->valido = IsSoundValid(efeito->som);
        if (efeito->valido) {
            SetSoundVolume(efeito->som, VOLUME_EFEITOS);
        }
    }
    if (!efeito->valido) {
        avisar_fallback(assets, caminho, achou ? "inválido" : "não encontrado");
    }
}

/**
 * @brief Carrega uma música; se faltar, ela fica marcada como inválida (trocar para ela não faz nada).
 * @return true se carregou.
 */
static bool carregar_musica(Assets *assets, MusicaId id)
{
    char base[TAM_CAMINHO_ASSET], caminho[TAM_CAMINHO_ASSET];
    bool achou;

    assets->musica_valida[id] = false;
    if (!assets->audio_ativo) {
        return false;
    }

    snprintf(base, sizeof(base), "%s%s", CAMINHO_SONS, nome_arquivo_musica(id));
    achou = achar_audio(base, caminho, sizeof(caminho));
    if (achou) {
        assets->musicas[id] = LoadMusicStream(caminho);
        assets->musica_valida[id] = IsMusicValid(assets->musicas[id]);
    }
    if (assets->musica_valida[id]) {
        assets->musicas[id].looping = true;  /* recomeça sozinha ao terminar */
        SetMusicVolume(assets->musicas[id], VOLUME_MUSICA);
    } else if (id == MUSICA_PRINCIPAL) {
        avisar_fallback(assets, caminho, achou ? "inválido" : "não encontrado");
    }
    return assets->musica_valida[id];
}

/** @brief Carrega ARQUIVO_FONTE (.ttf/.otf) com os caracteres 32..255; se faltar, usa a fonte padrão. */
/**
 * @brief Lê a fonte do jogo em `tamanho` pixels. Devolve false se o arquivo falta ou é inválido.
 *
 * O tamanho é o da textura com as letras (atlas): desenhar o texto bem maior que isso o amplia
 * e borra; por isso assets_ajustar_fonte() relê a fonte quando a tela cheia muda o zoom.
 */
static bool ler_fonte(int tamanho, Font *saida)
{
    char caminho[TAM_CAMINHO_ASSET];
    int codepoints[NUM_CODEPOINTS];
    int indice;
    Font fonte_lida;

    if (!achar_fonte(ARQUIVO_FONTE, caminho, sizeof(caminho))) {
        return false;
    }
    for (indice = 0; indice < NUM_CODEPOINTS; indice++) {
        codepoints[indice] = CODEPOINT_INICIAL + indice;
    }
    fonte_lida = LoadFontEx(caminho, tamanho, codepoints, NUM_CODEPOINTS);
    /* Se a leitura falha, a raylib devolve a fonte padrão: não pode ser descarregada depois */
    if (!IsFontValid(fonte_lida) || fonte_lida.texture.id == GetFontDefault().texture.id) {
        return false;
    }
    *saida = fonte_lida;
    return true;
}

static void carregar_fonte(Assets *assets)
{
    assets->fonte = GetFontDefault();
    assets->fonte_propria = ler_fonte(TAM_FONTE_BASE, &assets->fonte);
    assets->zoom_fonte = 1.0f;

    if (!assets->fonte_propria) {
        assets->fonte = GetFontDefault();
        avisar_fallback(assets, ARQUIVO_FONTE, "não encontrado ou inválido");
    }
}

void assets_ajustar_fonte(Assets *assets, float zoom)
{
    Font nova;
    int tamanho;

    if (assets == NULL || !assets->fonte_propria || zoom <= 0.0f) {
        return;
    }
    if (zoom > ZOOM_MAXIMO_FONTE) {
        zoom = ZOOM_MAXIMO_FONTE;
    }
    if (fabsf(zoom - assets->zoom_fonte) < 0.01f) {
        return;   /* mesmo zoom: nada a fazer (a fonte só é relida quando a janela muda de tamanho) */
    }

    /* Atlas no tamanho real da tela (32 px x 1,5 = 48 px em 1920x1080): o texto não é ampliado.
     * As medidas não mudam, pois DrawTextEx usa o tamanho pedido, não o do atlas. */
    tamanho = (int)roundf((float)TAM_FONTE_BASE * zoom);
    if (!ler_fonte(tamanho, &nova)) {
        return;   /* mantém a fonte atual */
    }
    UnloadFont(assets->fonte);
    assets->fonte = nova;
    assets->zoom_fonte = zoom;
}

/** @brief Espaço entre letras proporcional ao tamanho (igual ao que a raylib usa na fonte padrão). */
static float espacamento_letras(float tamanho)
{
    return tamanho / 10.0f;
}

/* ---- 3. Carga e liberação ---- */

Status assets_carregar(Assets *assets)
{
    int indice, fundos_encontrados, musicas_encontradas;

    if (assets == NULL) {
        return STATUS_ERRO_PARAMETRO;
    }
    memset(assets, 0, sizeof(*assets));
    if (!IsWindowReady()) {
        return STATUS_ERRO_GRAFICO;  /* texturas precisam da janela (contexto OpenGL) */
    }

    /* Áudio: sem dispositivo o jogo continua mudo */
    InitAudioDevice();
    assets->audio_ativo = IsAudioDeviceReady();
    if (!assets->audio_ativo) {
        fprintf(stderr, "Aviso: nenhum dispositivo de áudio encontrado (jogo sem som)\n");
    }

    carregar_fonte(assets);
    for (indice = 0; indice < IMG_QUANTIDADE; indice++) {
        carregar_imagem(assets, (ImagemId)indice);
    }
    for (indice = 0; indice < BOSS_QUANTIDADE; indice++) {
        carregar_animacao_boss(assets, (BossId)indice);
    }

    /* Fundos de fase são opcionais: um aviso só, com quantos foram encontrados */
    fundos_encontrados = 0;
    for (indice = 0; indice < BOSS_QUANTIDADE; indice++) {
        fundos_encontrados += carregar_fundo_boss(assets, (BossId)indice) ? 1 : 0;
    }
    if (fundos_encontrados < BOSS_QUANTIDADE) {
        fprintf(stderr, "Aviso: %d de %d fundos de fase em " CAMINHO_FUNDOS
                " (opcional: os outros usam o fundo padrão da tela)\n",
                fundos_encontrados, BOSS_QUANTIDADE);
    }

    for (indice = 0; indice < SOM_QUANTIDADE; indice++) {
        carregar_som(assets, (SomId)indice);
    }
    /* Músicas: a principal avisa se faltar; as das seções são opcionais (um aviso só) */
    assets->musica_atual = MUSICA_MANTER;
    if (!assets->audio_ativo) {
        printf("Erro ao carregar música: Audio Inativo\n");
    } else {
        musicas_encontradas = 0;
        for (indice = 0; indice < MUSICA_QUANTIDADE; indice++) {
            bool carregou = carregar_musica(assets, (MusicaId)indice);

            if (carregou && indice != MUSICA_PRINCIPAL) {
                musicas_encontradas++;
            }
        }
        if (musicas_encontradas < MUSICA_QUANTIDADE - 1) {
            fprintf(stderr, "Aviso: %d de %d músicas de seção em " CAMINHO_SONS
                    " (opcional: sem ela a música atual continua)\n",
                    musicas_encontradas, MUSICA_QUANTIDADE - 1);
        }
    }

    return assets->total_fallbacks > 0 ? STATUS_ASSET_FALLBACK : STATUS_OK;
}

void assets_descarregar(Assets *assets)
{
    int indice, quadro;

    // Confere os parâmetros
    if (assets == NULL) {
        return;
    }

    // Descarrega as imagens
    for (indice = 0; indice < IMG_QUANTIDADE; indice++) {
        if (assets->imagens[indice].textura.id > 0) {
            UnloadTexture(assets->imagens[indice].textura);
        }
    }

    // Descarrega as texturas dos bosses
    for (indice = 0; indice < BOSS_QUANTIDADE; indice++) {
        if (assets->fundos_boss[indice].textura.id > 0) {
            UnloadTexture(assets->fundos_boss[indice].textura);
        }
    }

    // Descarrega os frames dos bosses
    for (indice = 0; indice < BOSS_QUANTIDADE; indice++) {
        for (quadro = 0; quadro < assets->bosses[indice].quantidade; quadro++) {
            UnloadTexture(assets->bosses[indice].quadros[quadro]);
        }
    }

    // Descarrega os sons
    for (indice = 0; indice < SOM_QUANTIDADE; indice++) {
        if (assets->sons[indice].valido) {
            UnloadSound(assets->sons[indice].som);
        }
    }

    // Descarrega as músicas
    for (indice = 0; indice < MUSICA_QUANTIDADE; indice++) {
        if (assets->musica_valida[indice]) {
            UnloadMusicStream(assets->musicas[indice]);
        }
    }

    // Descarrega a fonte
    if (assets->fonte_propria) {
        UnloadFont(assets->fonte);  /* a fonte padrão é da raylib: não se descarrega */
    }
    // Fecha a saída de audio
    if (assets->audio_ativo) {
        CloseAudioDevice();
    }

    memset(assets, 0, sizeof(*assets));
}

/* ---- 4. Desenho de imagens ---- */

bool assets_imagem_fallback(const Assets *assets, ImagemId id)
{
    if (assets == NULL || (int)id < 0 || id >= IMG_QUANTIDADE) {
        return true;
    }
    return assets->imagens[id].fallback;
}

/** @brief Desenha uma textura inteira esticada no retângulo de destino. */
static void desenhar_textura_esticada(const Escala *escala, Texture2D textura, Rectangle destino, Color tint)
{
    Rectangle origem = { 0.0f, 0.0f, (float)textura.width, (float)textura.height };
    Vector2 sem_rotacao = { 0.0f, 0.0f };

    /* Bordas em pixels inteiros da tela: em tela cheia (zoom 1,5) nada fica "entre" dois pixels */
    DrawTexturePro(textura, origem, escala_alinhar_aos_pixels(escala, destino), sem_rotacao, 0.0f, tint);
}

/**
 * @brief Desenha pixel art o maior possível dentro de `destino` SEM distorcer (mantém a proporção)
 *        e centralizada. Um frame quadrado ocupa o quadrado inteiro; um retangular sobra nas laterais
 *        (ou em cima e embaixo). Usada nos bosses, onde esticar deformaria o desenho.
 */
static void desenhar_textura_encaixada(const Escala *escala, Texture2D textura, Rectangle destino, Color tint)
{
    Rectangle origem = { 0.0f, 0.0f, (float)textura.width, (float)textura.height };
    Vector2 sem_rotacao = { 0.0f, 0.0f };

    if (textura.width <= 0 || textura.height <= 0) {
        return;
    }
    /* Escala INTEIRA em pixels da tela (6x na janela, 9x em 1920x1080): todo pixel da arte
     * vira um bloco do mesmo tamanho, em qualquer resolução (ver escala_alinhar_pixel_art). */
    DrawTexturePro(textura, origem, escala_alinhar_pixel_art(escala, destino, textura.width, textura.height),
                   sem_rotacao, 0.0f, tint);
}

void assets_desenhar_fundo(const Assets *assets, ImagemId fundo_padrao, BossId boss)
{
    Rectangle janela = { 0.0f, 0.0f, (float)LARGURA_JANELA, (float)ALTURA_JANELA };
    bool boss_valido = (int)boss >= 0 && boss < BOSS_QUANTIDADE;

    if (assets == NULL) {
        return;
    }

    /* 1. Fundo próprio da fase */
    if (boss_valido && !assets->fundos_boss[boss].fallback) {
        desenhar_textura_esticada(&assets->escala, assets->fundos_boss[boss].textura, janela, WHITE);
        return;
    }

    /* 2. Fundo padrão da tela */
    if (!assets_imagem_fallback(assets, fundo_padrao)) {
        desenhar_textura_esticada(&assets->escala, assets->imagens[fundo_padrao].textura, janela, WHITE);
        return;
    }

    /* 3. Degradê na cor da fase (o xadrez em tela cheia seria ilegível) */
    DrawRectangleGradientV(0, 0, LARGURA_JANELA, ALTURA_JANELA,
                           boss_valido ? cor_tema_boss(boss) : PALETA_FUNDO, BLACK);
}

void assets_desenhar_imagem(const Assets *assets, ImagemId id, Rectangle destino, Color tint)
{
    if (assets == NULL || (int)id < 0 || id >= IMG_QUANTIDADE) {
        return;
    }
    desenhar_textura_esticada(&assets->escala, assets->imagens[id].textura, destino, tint);
}

void assets_desenhar_painel(const Assets *assets, ImagemId id, Rectangle destino,
                            Color cor_se_faltar, Color tint)
{
    if (!assets_imagem_fallback(assets, id)) {
        desenhar_textura_esticada(&assets->escala, assets->imagens[id].textura, destino, tint);
        return;
    }

    /* Sem imagem: retângulo com borda escura e um brilho no topo (relevo de botão do Minecraft) */
    DrawRectangleRec(destino, cor_se_faltar);
    DrawRectangle((int)destino.x, (int)destino.y, (int)destino.width, (int)BORDA_PAINEL,
                  Fade(WHITE, 0.25f));
    DrawRectangleLinesEx(destino, BORDA_PAINEL, PALETA_BORDA);
}

void assets_desenhar_boss(const Assets *assets, BossId id, float tempo, Rectangle destino, Color tint)
{
    const Animacao *animacao;
    int quadro_atual;

    // Confere parametros
    if (assets == NULL || (int)id < 0 || id >= BOSS_QUANTIDADE) {
        return;
    }
    animacao = &assets->bosses[id];
    if (animacao->quantidade <= 0) {
        return;
    }

    quadro_atual = (int)(tempo / SEGUNDOS_POR_QUADRO) % animacao->quantidade;
    if (quadro_atual < 0) {
        quadro_atual = 0;
    }
    desenhar_textura_encaixada(&assets->escala, animacao->quadros[quadro_atual], destino, tint);
}

/* ---- 5. Som e música ---- */

void assets_tocar_som(const Assets *assets, SomId id)
{
    if (assets == NULL || !assets->audio_ativo || (int)id < 0 || id >= SOM_QUANTIDADE
        || !assets->sons[id].valido) {
        return;
    }
    PlaySound(assets->sons[id].som);
}

void assets_trocar_musica(Assets *assets, MusicaId id)
{
    if (assets == NULL || !assets->audio_ativo) {
        return;
    }
    /* Silêncio: para a música atual e fica sem nenhuma até a próxima troca */
    if (id == MUSICA_SILENCIO) {
        if (assets->musica_atual < MUSICA_QUANTIDADE) {
            StopMusicStream(assets->musicas[assets->musica_atual]);
        }
        assets->musica_atual = MUSICA_MANTER;   /* "nenhuma tocando" */
        return;
    }
    /* MUSICA_MANTER (ou qualquer valor fora da lista), a mesma que já toca, ou arquivo faltando:
     * não faz nada e a atual continua. Sem isto, musicas[id] seria lido fora do vetor. */
    if ((int)id < 0 || id >= MUSICA_QUANTIDADE || id == assets->musica_atual
        || !assets->musica_valida[id]) {
        return;
    }
    if (assets->musica_atual < MUSICA_QUANTIDADE) {
        StopMusicStream(assets->musicas[assets->musica_atual]);
    }
    PlayMusicStream(assets->musicas[id]);
    assets->musica_atual = id;
}

void assets_atualizar_musica(const Assets *assets)
{
    if (assets == NULL || !assets->audio_ativo || assets->musica_atual >= MUSICA_QUANTIDADE) {
        return;
    }
    UpdateMusicStream(assets->musicas[assets->musica_atual]);
}

/* ---- 6. Texto ---- */

int assets_contar_letras(const char *texto)
{
    int letras = 0, bytes_da_letra = 0;

    // Confere parâmetros
    if (texto == NULL) {
        return 0;
    }

    // Pega os caracteres um a um
    while (*texto != '\0') {
        /*
        * GetCodepointNext  pega o Unicode e registra em Bytes (UTF-8)
        * Toda essa seção é utilizada para registrar caracteres com acentos ou não convencionais
        * strlen por exemplo só lê em bytes o tamanho total (quando por exemplo, 'é' é 2 bytes)
        * então para 'é':
        * strlen -> 2 bytes (2 char)
        * este trecho -> 1 caracter
        */
        GetCodepointNext(texto, &bytes_da_letra);
        texto += bytes_da_letra;
        letras++;
    }
    return letras;
}

/**
 * @brief Copia as primeiras `letras` letras UTF-8 de `origem` para `destino`.
 * Nunca corta no meio de um caractere acentuado (que ocupa 2 bytes em UTF-8).
 */
static void copiar_letras(char *destino, size_t capacidade, const char *origem, int letras)
{
    size_t bytes = 0;
    int bytes_da_letra = 0;

    while (origem[bytes] != '\0' && letras > 0) {
        GetCodepointNext(origem + bytes, &bytes_da_letra);
        if (bytes + (size_t)bytes_da_letra >= capacidade) {
            break;
        }
        bytes += (size_t)bytes_da_letra;
        letras--;
    }
    memcpy(destino, origem, bytes);
    destino[bytes] = '\0';
}

float assets_largura_texto(const Assets *assets, const char *texto, float tamanho)
{
    if (assets == NULL || texto == NULL) {
        return 0.0f;
    }
    return MeasureTextEx(assets->fonte, texto, tamanho, espacamento_letras(tamanho)).x;
}

void assets_desenhar_texto(const Assets *assets, const char *texto, Vector2 posicao, float tamanho, Color cor)
{
    if (assets == NULL || texto == NULL) {
        return;
    }
    DrawTextEx(assets->fonte, texto, posicao, tamanho, espacamento_letras(tamanho), cor);
}

void assets_desenhar_texto_centralizado(const Assets *assets, const char *texto, Rectangle area,
                                        float tamanho, Color cor)
{
    Vector2 posicao;

    if (assets == NULL || texto == NULL) {
        return;
    }
    posicao.x = area.x + (area.width - assets_largura_texto(assets, texto, tamanho)) / 2.0f;
    posicao.y = area.y + (area.height - tamanho) / 2.0f;
    assets_desenhar_texto(assets, texto, posicao, tamanho, cor);
}

/** @brief Avança `texto` até o primeiro caractere que não é espaço. */
static const char *pular_espacos(const char *texto)
{
    while (*texto == ' ') {
        texto++;
    }
    return texto;
}

/** @brief Conta as letras UTF-8 de `inicio` até `fim` (sem incluir `fim`). */
static int contar_letras_ate(const char *inicio, const char *fim)
{
    int letras = 0, bytes_da_letra = 0;

    while (inicio < fim) {
        GetCodepointNext(inicio, &bytes_da_letra);
        inicio += bytes_da_letra;
        letras++;
    }
    return letras;
}

/**
 * @brief Palavra maior que a linha inteira: copia para `linha` o maior começo dela que cabe.
 *
 * Cresce letra por letra a partir da primeira e para na primeira que não cabe. Assim o custo
 * depende só do que cabe na linha, e não do tamanho da palavra (nem do resto do texto).
 * A primeira letra entra sempre, mesmo sem caber: garante que o texto avança.
 *
 * @return Onde a palavra continua na linha de baixo.
 */
static const char *cortar_palavra(const Assets *assets, const char *inicio_palavra, const char *fim_palavra,
                                  float largura_maxima, float tamanho, char *linha, size_t capacidade)
{
    int letras_da_palavra = contar_letras_ate(inicio_palavra, fim_palavra);
    int letras = (letras_da_palavra > 0) ? 1 : 0;

    copiar_letras(linha, capacidade, inicio_palavra, letras);
    while (letras < letras_da_palavra) {
        copiar_letras(linha, capacidade, inicio_palavra, letras + 1);
        if (assets_largura_texto(assets, linha, tamanho) > largura_maxima) {
            copiar_letras(linha, capacidade, inicio_palavra, letras);  /* volta ao último que cabia */
            break;
        }
        letras++;
    }
    return inicio_palavra + strlen(linha);
}

/**
 * @brief Monta em `linha` o maior trecho de `inicio` que cabe em `largura_maxima`.
 *
 * Acrescenta uma palavra por vez (com os espaços antes dela) direto em `linha` e mede;
 * se passar da largura, desfaz e a palavra vai para a linha de baixo. Uma palavra maior
 * que a linha inteira é cortada por letras (cortar_palavra). Um '\n' no texto força a quebra.
 *
 * @return Onde começa a próxima linha (espaços e o '\n' da quebra já pulados).
 */
static const char *montar_linha(const Assets *assets, const char *inicio, float largura_maxima,
                                float tamanho, char *linha, size_t capacidade)
{
    const char *cursor = inicio;
    size_t usado = 0;  /* bytes já ocupados em `linha` */

    linha[0] = '\0';
    while (*cursor != '\0' && *cursor != '\n') {
        const char *inicio_palavra = pular_espacos(cursor);
        const char *inicio_trecho = (usado == 0) ? inicio_palavra : cursor;  /* linha não começa com espaço */
        const char *fim_palavra = inicio_palavra;
        size_t bytes_trecho;

        while (*fim_palavra != '\0' && *fim_palavra != ' ' && *fim_palavra != '\n') {
            fim_palavra++;
        }
        bytes_trecho = (size_t)(fim_palavra - inicio_trecho);

        /* Acrescenta o trecho direto em `linha` (+1 pelo '\0'); se passar da largura, desfaz */
        if (usado + bytes_trecho + 1 <= capacidade) {
            memcpy(linha + usado, inicio_trecho, bytes_trecho);
            linha[usado + bytes_trecho] = '\0';
            if (assets_largura_texto(assets, linha, tamanho) <= largura_maxima) {
                usado += bytes_trecho;
                cursor = fim_palavra;
                continue;
            }
            linha[usado] = '\0';
        }

        if (usado == 0) {
            return cortar_palavra(assets, inicio_palavra, fim_palavra, largura_maxima, tamanho,
                                  linha, capacidade);
        }
        break;  /* a palavra vai para a linha de baixo */
    }

    cursor = pular_espacos(cursor);
    if (*cursor == '\n') {
        cursor++;
    }
    return cursor;
}

/**
 * @brief Percorre o texto linha a linha; desenha se `desenhar` for true.
 * @return Quantidade de linhas que o texto inteiro ocupa.
 */
static int percorrer_texto_quebrado(const Assets *assets, const char *texto, Rectangle area,
                                    float tamanho, Color cor, int limite_letras, bool desenhar)
{
    char linha[TAM_LINHA_TEXTO];
    char linha_parcial[TAM_LINHA_TEXTO];
    const char *cursor = texto;
    float altura_linha = tamanho * ESPACO_ENTRE_LINHAS;
    int total_linhas = 0;
    int letras_restantes = limite_letras;

    while (*cursor != '\0') {
        Vector2 posicao = { area.x, area.y + (float)total_linhas * altura_linha };
        const char *proxima = montar_linha(assets, cursor, area.width, tamanho, linha, sizeof(linha));

        if (proxima == cursor) {
            break;  /* proteção: nada coube (área estreita demais) */
        }
        cursor = proxima;

        if (desenhar && posicao.y + tamanho <= area.y + area.height + 1.0f && letras_restantes != 0) {
            if (letras_restantes < 0) {
                assets_desenhar_texto(assets, linha, posicao, tamanho, cor);
            } else {
                int letras_da_linha = assets_contar_letras(linha);
                int letras_desenhadas = letras_da_linha < letras_restantes ? letras_da_linha : letras_restantes;

                copiar_letras(linha_parcial, sizeof(linha_parcial), linha, letras_desenhadas);
                assets_desenhar_texto(assets, linha_parcial, posicao, tamanho, cor);
                letras_restantes -= letras_desenhadas;
            }
        }
        total_linhas++;
    }
    return total_linhas;
}

int assets_desenhar_texto_quebrado(const Assets *assets, const char *texto, Rectangle area,
                                   float tamanho, Color cor, int limite_letras)
{
    if (assets == NULL || texto == NULL) {
        return 0;
    }
    return percorrer_texto_quebrado(assets, texto, area, tamanho, cor, limite_letras, true);
}

void assets_desenhar_texto_ajustado(const Assets *assets, const char *texto, Rectangle area,
                                    float tamanho_maximo, float tamanho_minimo, Color cor)
{
    float tamanho, altura_texto;
    int linhas = 0;
    Rectangle area_centralizada = area;

    if (assets == NULL || texto == NULL) {
        return;
    }

    /* Procura o maior tamanho em que todas as linhas cabem na altura da área */
    for (tamanho = tamanho_maximo; tamanho > tamanho_minimo; tamanho -= 2.0f) {
        linhas = percorrer_texto_quebrado(assets, texto, area, tamanho, cor, -1, false);
        if ((float)linhas * tamanho * ESPACO_ENTRE_LINHAS <= area.height) {
            break;
        }
    }
    if (tamanho < tamanho_minimo) {
        tamanho = tamanho_minimo;
    }

    /* Centraliza o bloco de linhas na vertical (a última linha não precisa do espaço extra) */
    linhas = percorrer_texto_quebrado(assets, texto, area, tamanho, cor, -1, false);
    altura_texto = (float)(linhas - 1) * tamanho * ESPACO_ENTRE_LINHAS + tamanho;
    if (altura_texto < area.height) {
        area_centralizada.y += (area.height - altura_texto) / 2.0f;
        area_centralizada.height = altura_texto + 1.0f;
    }
    percorrer_texto_quebrado(assets, texto, area_centralizada, tamanho, cor, -1, true);
}

#endif /* USE_RAYLIB */
