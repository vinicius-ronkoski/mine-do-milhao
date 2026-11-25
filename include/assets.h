/**
 * @file assets.h
 * @brief Todo arquivo de assets/ (imagens, sons, músicas, fonte, frames) e todo desenho de texto.
 *
 * Carregado uma vez em jogo_iniciar (assets_carregar) e usado pelas telas, botões e diálogos.
 * Arquivo faltando não para o jogo: vira xadrez, painel colorido, degradê, silêncio ou fonte padrão.
 * Imagem/som novo: arquivo na pasta, id no enum (antes do *_QUANTIDADE) e nome na tabela de assets.c.
 * Frames dos bosses: frames/<boss>_00..07; fundo de cada fase: fundos/<boss>.
 * Só compila com USE_RAYLIB=1.
 */
#ifndef ASSETS_H
#define ASSETS_H

#include "raylib.h"
#include "config.h"
#include "boss.h"
#include "escala.h"

/* ---- Paleta de cores da interface (usada também como fallback de imagens) ---- */

/** @name Paleta (estilo Minecraft; também é a cor dos fallbacks) */
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

/* ---- Identificadores dos assets ---- */

/** Imagens de assets/sprites/ (nomes em nome_arquivo_imagem, assets.c). */
typedef enum {
    // Objetos
    IMG_PAINEL_ENUNCIADO = 0,         /**< Moldura atrás do enunciado (fallback: painel cinza). */
    IMG_BOTAO,                        /**< Skin única dos botões; o estado só muda a cor (fallback: retângulo). */
    IMG_CAIXA_DIALOGO,                /**< Caixa de fala (fallback: painel cinza). */
    IMG_ESMERALDA,                    /**< Ícone da esmeralda (HUD e preços das ajudas). */
    IMG_TITULO,                       /**< Logo do menu (fallback: TITULO_JANELA em texto). */

    // Fundos
    IMG_FUNDO_MENU,                   /**< Fundo do menu e da tela de carregamento. */
    IMG_FUNDO_PERGUNTA,               /**< Fundo da tela de perguntas (1280x720). */
    IMG_FUNDO_DIALOGO,                /**< Fundo das cenas de diálogo. */
    IMG_FUNDO_PALCO,                  /**< Imagem do palco (Minecraft) */
    IMG_FUNDO_PORTAL,                 /**< Imagem do portal do Nether */
    IMG_FUNDO_FIM,                    /**< Imagem de fundo do fim */
    IMG_FUNDO_AUDITORIO,              /**< Imagem do auditório (Minecraft)*/
    IMG_FUNDO_PRETO,                  /**< Fundo Preto */
    IMG_FUNDO_VITORIA,                /**< Fundo com vitória */
    IMG_FUNDO_DERROTA,                /**< Fundo com derrota */
    IMG_FUNDO_RESULTADO,              /**< Fundo da tela final. (Fundo de terra) */

    // Icones
    IMG_ICONE_ALDEAO,                 /**< Icone Villager*/
    IMG_ICONE_ALDEAO_ASSUSTADO,       /**< Icone Villager asustado*/
    IMG_ICONE_DESCONHECIDO_1,         /**< Icone desconhecido (Herobrine) */
    IMG_ICONE_DESCONHECIDO_2,         /**< Icone desconhecido (Sem interrogação) */
    IMG_ICONE_CREEPER,                /**< Icone Creeper */
    IMG_ICONE_WITHER,                 /**< Icone Wither */
    IMG_ICONE_WARDEN,                 /**< Icone Warden (Herobrine) */
    IMG_ICONE_ENDER_DRAGON,           /**< Icone Ender Dragon */
    IMG_ICONE_STEVE,                  /**< Icone Steve */
    IMG_ICONE_HEROBRINE,              /**< Icone Herobrine */

    // Historia 1
    IMG_H1_CENA_1,                     /*< Frame 1 do Creeper auditorio */
    IMG_H1_CENA_2,                     /*< Frame 2 do Creeper auditorio */
    IMG_H1_CENA_3,                     /*< Apresentador olha para o Creeper */
    // Cena Creeper

    // Historia 2
    IMG_H2_CENA_1,                     /*< Creeper dropa item */
    IMG_H2_CENA_2,                     /*< Leitura do livro com enigma */
    IMG_H2_CENA_3,                     /*< Villager olha para o jogador */
    IMG_H2_CENA_4,                     /*< Portal do nether desativado */
    IMG_H2_CENA_5,                     /*< Portal do nether ativado */
    IMG_H2_CENA_6,                     /*< Entram no nether, com portal no fundo */
    IMG_H2_CENA_7,                     /*< Villager olha para o lado */
    IMG_H2_CENA_8,                     /*< Villager com Wither sem estar montado */
    // Cena_Wither
    IMG_H2_CENA_9,                     /*< Villager olha pra cima, chamar img nenhuma  */

    // Historia 3
    IMG_H3_CENA_1,                     /*< Villager aliviado olha para o jogador */
    IMG_H3_CENA_2,                     /*< Voltam, dentro de caverna, Villager olha para o portal */
    IMG_H3_CENA_3,                     /*< Villager olha para o jogador na caverna */
    IMG_H3_CENA_4,                     /*< Sensor de Sculk ativa, chamando o warden */
    IMG_H3_CENA_5,                     /*< Mesmo fundo do warden */

    // Historia 4
    IMG_H4_CENA_1,                     /*< Villager aponta para saída com sua seta mágica */
    IMG_H4_CENA_2,                     /*< Villager encontra um portal convenientemente colocado ali */
    IMG_H4_CENA_3,                     /*< Jogador olha para o redomoinho */
    IMG_H4_CENA_4,                     /*< Villager mostra sua roupa nova e zomba */
    IMG_H4_CENA_5,                     /*< Barulho do ender dragon */
    // Cena Ender Dragon

    // Historia 5
    IMG_H5_CENA_1,                     /*< Cena do fim do jogo */
    IMG_H5_CENA_2,                     /*< Villager e Player no mapa do herobrine */
    IMG_H5_CENA_3,                     /*< Fica escuro e villager pergunta se está tudo bem */
    // Cena Herobrine

    // Historia 6 (Final)
    IMG_H6_CENA_1,                     /*< Frame 1 - Villager na paz */
    IMG_H6_CENA_2,                     /*< Frame 2 - Villager na paz */
    IMG_H6_CENA_3,                     /*< Frame 3 - Villager na paz */
    IMG_H6_CENA_4,                     /*< Frame 4 - Villager na paz */
    IMG_H6_CENA_5,                     /*< De volta ao palco, olhar baixo */
    IMG_H6_CENA_6,                     /*< No palco, anunciando do vencedor*/
    IMG_H6_CENA_7,                     /*< Frame 1 - Steve pulando*/
    IMG_H6_CENA_8,                     /*< Frame 2 - Steve pulando*/
    IMG_H6_CENA_9,                     /*< Frame 1 - Steve pulando com esmeralda*/
    IMG_H6_CENA_10,                    /*< Frame 2 - Steve pulando com esmeralda*/

    // Quantidade (SEMPRE MANTER)
    IMG_QUANTIDADE                      /**< Quantidade de imagens (não é uma imagem). */
} ImagemId;

/** Fala sem retrato/cenário. */
#define IMG_NENHUMA IMG_QUANTIDADE


/** Efeitos de assets/sounds/ (nomes em nome_arquivo_som). */
typedef enum {
    SOM_CLIQUE = 0,       /**< Clique em botão. */
    SOM_ACERTO,           /**< Resposta certa. */
    SOM_ERRO,             /**< Resposta errada. */
    SOM_DANO_BOSS,        /**< Boss perdeu vida. */
    SOM_BOSS_DERROTADO,   /**< Boss caiu. */
    SOM_VITORIA,          /**< Prêmio máximo. */
    SOM_PLATEIA,          /**< Som de entrada da plateia*/
    SOM_CREEPER,          /**< Som do creeper */
    SOM_PORTAL,           /**< Som do portal do Nether */
    SOM_ALDEAO,           /**< Som de aldeão */
    SOM_SCULK,            /**< Som de Sculk */
    SOM_ENDER_DRAGON_VOO, /**< Som do Ender Dragon Voando */
    SOM_ENDER_DRAGON_IDLE,/**< Som do ender dragon */
    SOM_FLINT_AND_STEEL,  /**< Som do Minecraft Movie */
    SOM_THE_NETHER,       /**< Som do Minecraft Movie */
    SOM_RECEBA,           /**< Receba (Luva de Pedreiro) */
    SOM_QUANTIDADE        /**< Quantidade de sons (não é um som). */
} SomId;

/** Fala sem som (assets_tocar_som ignora). */
#define SOM_NENHUM SOM_QUANTIDADE

/** Músicas de fundo: uma por vez, em loop, até assets_trocar_musica ou MUSICA(...) numa cena. */
typedef enum {
    MUSICA_PRINCIPAL = 0,     /**< "musica": abertura e telas fora das seções. */
    MUSICA_CREEPER,           /**< Seção de cada boss: MUSICA_CREEPER + BossId. */
    MUSICA_WITHER,
    MUSICA_WARDEN,
    MUSICA_END,               /**< C418 original The End, placeholder */
    MUSICA_MOOG_CITY,         /**< C418 original Moog City, placeholder */
    MUSICA_HEROBRINE,
    MUSICA_EQUINOXE,           /**< C418 original Equinoxe, placeholder */
    MUSICA_QUANTIDADE,        /**< Quantidade (não é uma música). */
    MUSICA_SILENCIO           /**< Comando: para a música atual. */
} MusicaId;

/** Fala que não troca a música. */
#define MUSICA_MANTER MUSICA_QUANTIDADE

/** Máximo de quadros por boss (<boss>_00 ... _07). */
#define MAX_QUADROS_ANIMACAO 8

/** Segundos de cada quadro da animação dos bosses. */
#define SEGUNDOS_POR_QUADRO 0.15f

/* ---- Estruturas ---- */

/** Textura na GPU e se é substituta. */
typedef struct {
    Texture2D textura;   /**< Do arquivo ou gerada. */
    bool fallback;       /**< true = arquivo faltou. */
} Imagem;

/** Efeito sonoro (valido = false: silêncio). */
typedef struct {
    Sound som;
    bool valido;
} Efeito;

/** Quadros da animação de um boss (vetor fixo). */
typedef struct {
    Texture2D quadros[MAX_QUADROS_ANIMACAO];
    int quantidade;                          /**< Quadros em uso (>= 1). */
    bool fallback;                           /**< Nenhum quadro encontrado (1 xadrez). */
} Animacao;

/** Todos os recursos; vive dentro de Jogo (sem globais). */
typedef struct {
    Imagem imagens[IMG_QUANTIDADE];          /**< Imagens estáticas (ImagemId). */
    Efeito sons[SOM_QUANTIDADE];             /**< Efeitos sonoros (SomId). */
    Animacao bosses[BOSS_QUANTIDADE];        /**< Frames de cada boss (índice = BossId). */
    Imagem fundos_boss[BOSS_QUANTIDADE];     /**< Fundo de cada fase (opcional). */
    Music musicas[MUSICA_QUANTIDADE];        /**< Streams (MusicaId). */
    bool musica_valida[MUSICA_QUANTIDADE];   /**< false = arquivo faltou. */
    MusicaId musica_atual;                   /**< MUSICA_MANTER = nenhuma. */
    Font fonte;
    bool fonte_propria;                      /**< false = fonte padrão da raylib. */
    float zoom_fonte;                        /**< Zoom da última leitura da fonte. */
    bool audio_ativo;                        /**< false = sem dispositivo de áudio. */
    int total_fallbacks;                     /**< Assets substituídos (aviso no terminal). */
    Escala escala;                           /**< Atualizada por jogo_atualizar; alinha as imagens à tela. */
} Assets;

/* ---- Carga e liberação ---- */

/**
 * @brief Inicia o áudio e carrega tudo, com fallback (chamada em jogo_iniciar, depois de InitWindow).
 * @return STATUS_OK; STATUS_ASSET_FALLBACK (jogável); STATUS_ERRO_PARAMETRO; STATUS_ERRO_GRAFICO.
 */
Status assets_carregar(Assets *assets);

/** @brief Relê a fonte em TAM_FONTE_BASE x zoom quando o zoom muda (texto nítido em tela cheia). */
void assets_ajustar_fonte(Assets *assets, float zoom);

/** @brief Libera tudo e fecha o áudio (jogo_encerrar, antes de CloseWindow). */
void assets_descarregar(Assets *assets);

/* ---- Imagens ---- */

/**
 * @brief Fundo da tela inteira: o da fase (fundos/<boss>), senão `fundo_padrao`, senão degradê na cor do boss.
 * @param boss BOSS_QUANTIDADE para não usar fundo de fase (ex.: resultado).
 */
void assets_desenhar_fundo(const Assets *assets, ImagemId fundo_padrao, BossId boss);

/** @brief true se a imagem faltou ou o id é inválido. */
bool assets_imagem_fallback(const Assets *assets, ImagemId id);

/**
 * @brief Desenha a imagem esticada em `destino` (id inválido: nada).
 * @param tint WHITE = cores originais.
 */
void assets_desenhar_imagem(const Assets *assets, ImagemId id, Rectangle destino, Color tint);

/**
 * @brief Painel, botão ou caixa: a imagem com `tint` ou, se faltou, retângulo em `cor_se_faltar` com borda.
 */
void assets_desenhar_painel(const Assets *assets, ImagemId id, Rectangle destino,
                            Color cor_se_faltar, Color tint);

/**
 * @brief Quadro atual da animação do boss, em escala inteira (escala_alinhar_pixel_art).
 * @param tempo Segundos desde o início (loop de SEGUNDOS_POR_QUADRO por quadro).
 */
void assets_desenhar_boss(const Assets *assets, BossId id, float tempo, Rectangle destino, Color tint);

/* ---- Som e música ---- */

/** @brief Toca um efeito (nada se faltou ou não há áudio). */
void assets_tocar_som(const Assets *assets, SomId id);

/** @brief Troca para `id` em loop (ignora se já toca, MUSICA_MANTER ou arquivo ausente); MUSICA_SILENCIO para. */
void assets_trocar_musica(Assets *assets, MusicaId id);

/** @brief 1x por quadro (jogo_atualizar): alimenta o stream da música. */
void assets_atualizar_musica(const Assets *assets);

/* ---- Texto (fonte do jogo, UTF-8; só Latin-1: travessão, aspas curvas e reticências viram "?") ---- */

/** @brief Largura de uma linha de texto. */
float assets_largura_texto(const Assets *assets, const char *texto, float tamanho);

/** @brief Uma linha a partir de `posicao` (canto superior esquerdo). */
void assets_desenhar_texto(const Assets *assets, const char *texto, Vector2 posicao, float tamanho, Color cor);

/** @brief Uma linha centralizada em `area`. */
void assets_desenhar_texto_centralizado(const Assets *assets, const char *texto, Rectangle area,
                                        float tamanho, Color cor);

/**
 * @brief Texto com quebra de linha em `area` (o que passa da altura é cortado).
 * @param limite_letras Só as N primeiras letras (máquina de escrever do diálogo); -1 = tudo.
 * @return Linhas que o texto inteiro ocupa.
 */
int assets_desenhar_texto_quebrado(const Assets *assets, const char *texto, Rectangle area,
                                   float tamanho, Color cor, int limite_letras);

/** @brief Como _quebrado, mas reduz a fonte (de 2 em 2) até caber e centraliza na vertical. */
void assets_desenhar_texto_ajustado(const Assets *assets, const char *texto, Rectangle area,
                                    float tamanho_maximo, float tamanho_minimo, Color cor);

/** @brief Letras UTF-8 do texto ("ação" = 4). */
int assets_contar_letras(const char *texto);

#endif /* ASSETS_H */
