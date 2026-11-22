/**
 * @file dialogo.c
 * @brief Implementação dos diálogos (ver dialogo.h). Compila só com USE_RAYLIB.
 *
 * Organização: 1) falas de cada cena (PERSONALIZE AQUI), 2) controle do diálogo, 3) desenho.
 */
#ifdef USE_RAYLIB
#include <string.h>
#include "dialogo.h"

/** Nome do apresentador nas falas. */
#define NOME_APRESENTADOR "Silvio Aldeão"

/** @name Layout da caixa de diálogo (parte de baixo da janela) */
/**@{*/
#define ALTURA_CAIXA_DIALOGO  200.0f
#define MARGEM_CAIXA_DIALOGO   24.0f
#define LADO_RETRATO          150.0f
#define FONTE_NOME_DIALOGO     26.0f
#define FONTE_TEXTO_DIALOGO    24.0f
#define FONTE_AVISO_DIALOGO    18.0f
/**@}*/

/** Número grande de letras: usado para "completar" a fala de uma vez. */
#define LETRAS_FALA_COMPLETA 100000

/* ========================================================================= */
/* 1. Falas de cada cena — TEXTO PROVISÓRIO: personalize à vontade           */
/* ========================================================================= */

Status dialogo_anexar_cena(Dialogo *dialogo, CenaId cena)
{
    Status status = STATUS_OK;

    if (dialogo == NULL) {
        return STATUS_ERRO_PARAMETRO;
    }

/* Atalho só deste switch: acrescenta uma fala e guarda o primeiro erro */
#define FALA(personagem, texto, retrato)                                          \
    do {                                                                          \
        if (status == STATUS_OK) {                                                \
            status = dialogo_adicionar_fala(dialogo, personagem, texto, retrato); \
        }                                                                         \
    } while (0)

    switch (cena) {
    case CENA_INTRO:
        FALA(NOME_APRESENTADOR, "Hmmm! Bem-vindo ao Mine do Milhão!", IMG_RETRATO_ALDEAO);
        FALA(NOME_APRESENTADOR, "São 15 perguntas. Cada acerto vale esmeraldas, quer ficar rico?",
             IMG_RETRATO_ALDEAO);
        FALA(NOME_APRESENTADOR, "Nas perguntas 5 e 10 suas esmeraldas ficam garantidas. "
             "Você pode parar a qualquer momento e levar o que juntou.", IMG_RETRATO_ALDEAO);
        break;
    case CENA_BOSS_CREEPER:
        FALA(NOME_APRESENTADOR, "Espera... que som é esse?", IMG_RETRATO_ALDEAO);
        break;
    case CENA_BOSS_WITHER:
        FALA(NOME_APRESENTADOR, "O que é o que é? Tem três cabeças e vai te explodir?!", IMG_RETRATO_ALDEAO);
        break;
    case CENA_BOSS_WARDEN:
        FALA(NOME_APRESENTADOR, "Você fez barulho demais! Lá vem ele!.",
             IMG_RETRATO_ALDEAO);
        break;
    case CENA_BOSS_ENDER_DRAGON:
        FALA(NOME_APRESENTADOR, "Até agora foi tudo fácil demais...", IMG_RETRATO_ALDEAO);
        FALA(NOME_APRESENTADOR, "Você consegue derrotar o seu próprio fim?", IMG_RETRATO_ALDEAO);
        FALA(NOME_APRESENTADOR, "ou melhor... o dragão do fim.", IMG_RETRATO_ALDEAO);
        FALA(NOME_APRESENTADOR, "ha.", IMG_RETRATO_ALDEAO);
        FALA(NOME_APRESENTADOR, "ha. ha.", IMG_RETRATO_ALDEAO);
        FALA(NOME_APRESENTADOR, "ha. ha. ha.", IMG_RETRATO_ALDEAO);
        FALA(NOME_APRESENTADOR, "Deveriam me pagar mais por fazer essas piadas horríveis", IMG_RETRATO_ALDEAO);
        break;
    case CENA_BOSS_HEROBRINE:
        FALA(NOME_APRESENTADOR, "...",IMG_RETRATO_ALDEAO);
        FALA(NOME_APRESENTADOR, "...",IMG_RETRATO_ALDEAO);
        FALA(NOME_APRESENTADOR, "...boa sorte.",IMG_RETRATO_ALDEAO);
        break;
    case CENA_BOSS_DERROTADO:
        FALA(NOME_APRESENTADOR, "Ufa, essa foi por pouco! Mais esmeraldas para a sua coleção.", 
            IMG_RETRATO_ALDEAO);
        break;
    case CENA_VITORIA:
        FALA(NOME_APRESENTADOR, "Inacreditável! Você é realmente muito forte!",
             IMG_RETRATO_ALDEAO);
        break;
    case CENA_DERROTA:
        FALA(NOME_APRESENTADOR, "Que pena... a resposta estava errada.", IMG_RETRATO_ALDEAO);
        break;
    case CENA_PARADA:
        FALA(NOME_APRESENTADOR, "Decisão sábia! Leve suas esmeraldas e fuja daqui!", IMG_RETRATO_ALDEAO);
        break;
    default:
        status = STATUS_ERRO_PARAMETRO;  /* cena inexistente */
        break;
    }

#undef FALA

    if (status == STATUS_OK && dialogo->quantidade > 0 && dialogo->indice < dialogo->quantidade) {
        dialogo->terminou = false;  /* havia terminado e ganhou falas novas: volta a tocar */
    }
    return status;
}

/* ========================================================================= */
/* 2. Controle do diálogo                                                    */
/* ========================================================================= */

void dialogo_limpar(Dialogo *dialogo)
{
    if (dialogo == NULL) {
        return;
    }
    memset(dialogo, 0, sizeof(*dialogo));
    dialogo->terminou = true;  /* vazio = nada para mostrar */
}

Status dialogo_adicionar_fala(Dialogo *dialogo, const char *personagem, const char *texto, ImagemId retrato)
{
    Fala *nova_fala;

    if (dialogo == NULL || personagem == NULL || texto == NULL
        || dialogo->quantidade >= DIALOGO_MAX_FALAS
        || strlen(personagem) >= TAM_NOME_PERSONAGEM || strlen(texto) >= TAM_FALA) {
        return STATUS_ERRO_PARAMETRO;
    }

    nova_fala = &dialogo->falas[dialogo->quantidade];
    strcpy(nova_fala->personagem, personagem);
    strcpy(nova_fala->texto, texto);
    nova_fala->retrato = retrato;
    dialogo->quantidade++;
    dialogo->terminou = false;
    return STATUS_OK;
}

Status dialogo_carregar_cena(Dialogo *dialogo, CenaId cena)
{
    if (dialogo == NULL) {
        return STATUS_ERRO_PARAMETRO;
    }
    dialogo_limpar(dialogo);
    return dialogo_anexar_cena(dialogo, cena);
}

CenaId dialogo_cena_do_boss(BossId id)
{
    if ((int)id < 0 || id >= BOSS_QUANTIDADE) {
        return CENA_QUANTIDADE;
    }
    return (CenaId)(CENA_BOSS_CREEPER + (int)id);
}

const Fala *dialogo_fala_atual(const Dialogo *dialogo)
{
    if (dialogo == NULL || dialogo->terminou || dialogo->indice >= dialogo->quantidade) {
        return NULL;
    }
    return &dialogo->falas[dialogo->indice];
}

int dialogo_letras_visiveis(const Dialogo *dialogo)
{
    float letras = 0.0f;

    if (dialogo == NULL) {
        return 0;
    }
    letras = dialogo->tempo_na_fala * (float)DIALOGO_LETRAS_POR_SEGUNDO;
    return letras > (float)LETRAS_FALA_COMPLETA ? LETRAS_FALA_COMPLETA : (int)letras;
}

bool dialogo_atualizar(Dialogo *dialogo, float dt, bool jogador_avancou)
{
    const Fala *fala;

    if (dialogo == NULL) {
        return true;
    }
    fala = dialogo_fala_atual(dialogo);
    if (fala == NULL) {
        dialogo->terminou = true;
        return true;
    }

    dialogo->tempo_na_fala += dt;
    if (!jogador_avancou) {
        return false;
    }

    if (dialogo_letras_visiveis(dialogo) < assets_contar_letras(fala->texto)) {
        /* 1º toque: mostra a fala inteira de uma vez */
        dialogo->tempo_na_fala = (float)LETRAS_FALA_COMPLETA / (float)DIALOGO_LETRAS_POR_SEGUNDO;
        return false;
    }

    /* 2º toque: próxima fala (ou fim) */
    dialogo->indice++;
    dialogo->tempo_na_fala = 0.0f;
    if (dialogo->indice >= dialogo->quantidade) {
        dialogo->terminou = true;
    }
    return dialogo->terminou;
}

/* ========================================================================= */
/* 3. Desenho                                                                */
/* ========================================================================= */

void dialogo_desenhar(const Dialogo *dialogo, const Assets *assets)
{
    const Fala *fala = dialogo_fala_atual(dialogo);
    Rectangle caixa, area_retrato, area_texto, area_nome;
    bool tem_retrato;

    if (fala == NULL || assets == NULL) {
        return;
    }

    /* Caixa no rodapé da janela */
    caixa.x = MARGEM_CAIXA_DIALOGO;
    caixa.width = LARGURA_JANELA - 2.0f * MARGEM_CAIXA_DIALOGO;
    caixa.height = ALTURA_CAIXA_DIALOGO;
    caixa.y = ALTURA_JANELA - ALTURA_CAIXA_DIALOGO - MARGEM_CAIXA_DIALOGO;
    assets_desenhar_painel(assets, IMG_CAIXA_DIALOGO, caixa, PALETA_PAINEL, WHITE);

    /* Retrato à esquerda (se a fala tiver um) */
    tem_retrato = (fala->retrato != IMG_NENHUMA);
    area_retrato.x = caixa.x + MARGEM_CAIXA_DIALOGO;
    area_retrato.y = caixa.y + (caixa.height - LADO_RETRATO) / 2.0f;
    area_retrato.width = LADO_RETRATO;
    area_retrato.height = LADO_RETRATO;
    if (tem_retrato) {
        assets_desenhar_imagem(assets, fala->retrato, area_retrato, WHITE);
    }

    /* Nome e texto à direita do retrato */
    area_nome.x = tem_retrato ? area_retrato.x + LADO_RETRATO + MARGEM_CAIXA_DIALOGO
                              : caixa.x + MARGEM_CAIXA_DIALOGO;
    area_nome.y = caixa.y + MARGEM_CAIXA_DIALOGO * 0.75f;
    area_nome.width = caixa.x + caixa.width - MARGEM_CAIXA_DIALOGO - area_nome.x;
    area_nome.height = FONTE_NOME_DIALOGO;
    assets_desenhar_texto(assets, fala->personagem, (Vector2){ area_nome.x, area_nome.y },
                          FONTE_NOME_DIALOGO, PALETA_DESTAQUE);

    area_texto.x = area_nome.x;
    area_texto.y = area_nome.y + FONTE_NOME_DIALOGO * 1.5f;
    area_texto.width = area_nome.width;
    area_texto.height = caixa.y + caixa.height - MARGEM_CAIXA_DIALOGO * 1.5f - area_texto.y;
    assets_desenhar_texto_quebrado(assets, fala->texto, area_texto, FONTE_TEXTO_DIALOGO,
                                   PALETA_TEXTO, dialogo_letras_visiveis(dialogo));

    /* Aviso para continuar, só com a fala completa */
    if (dialogo_letras_visiveis(dialogo) >= assets_contar_letras(fala->texto)) {
        const char *aviso = "ESPAÇO ou clique para continuar";
        Vector2 posicao_aviso;

        posicao_aviso.x = caixa.x + caixa.width - MARGEM_CAIXA_DIALOGO
                        - assets_largura_texto(assets, aviso, FONTE_AVISO_DIALOGO);
        posicao_aviso.y = caixa.y + caixa.height - MARGEM_CAIXA_DIALOGO - FONTE_AVISO_DIALOGO * 0.5f;
        assets_desenhar_texto(assets, aviso, posicao_aviso, FONTE_AVISO_DIALOGO, PALETA_TEXTO_SUAVE);
    }
}

#endif /* USE_RAYLIB */
