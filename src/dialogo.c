/**
 * @file dialogo.c
 * @brief Motor dos diálogos (ver dialogo.h): falas em sequência, máquina de escrever e desenho.
 *
 * As falas de cada cena ficam em historia.c. Compila só com USE_RAYLIB.
 */
#ifdef USE_RAYLIB
#include <stdio.h>
#include <string.h>
#include "dialogo.h"
#include "layout.h"

/** Número grande de letras: usado para "completar" a fala de uma vez. */
#define LETRAS_FALA_COMPLETA 100000

/* ---- 1. Controle do diálogo ---- */

void dialogo_limpar(Dialogo *dialogo)
{
    if (dialogo == NULL) {
        return;
    }
    memset(dialogo, 0, sizeof(*dialogo));
    dialogo->terminou = true;          /* vazio = nada para mostrar */
    dialogo->indice_fala_anunciada = -1;  /* depois do memset: 0 diria "a 1ª fala já foi entregue" */
}

Status dialogo_adicionar_fala(Dialogo *dialogo, const char *personagem, const char *texto,
                              ImagemId retrato)
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
    nova_fala->som = SOM_NENHUM;        /* a macro FALA troca se houver SOM(...) antes */
    nova_fala->fundo = IMG_NENHUMA;     /* a macro FALA troca pelo CENARIO atual */
    nova_fala->musica = MUSICA_MANTER;  /* a macro FALA troca se houver MUSICA(...) antes */
    nova_fala->cena = FALA_SEM_CENA;    /* a macro FALA marca a cena de origem */
    dialogo->quantidade++;
    dialogo->terminou = false;
    return STATUS_OK;
}

const Fala *dialogo_fala_nova(Dialogo *dialogo)
{
    /* Nada novo: diálogo vazio/acabado ou esta fala já foi entregue */
    if (dialogo == NULL || dialogo->terminou || dialogo->indice >= dialogo->quantidade
        || dialogo->indice == dialogo->indice_fala_anunciada) {
        return NULL;
    }
    dialogo->indice_fala_anunciada = dialogo->indice;
    return &dialogo->falas[dialogo->indice];
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

/* ---- 2. Desenho ---- */

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
    area_retrato.y = caixa.y + (caixa.height - LADO_RETRATO_DIALOGO) / 2.0f;
    area_retrato.width = LADO_RETRATO_DIALOGO;
    area_retrato.height = LADO_RETRATO_DIALOGO;
    if (tem_retrato) {
        assets_desenhar_imagem(assets, fala->retrato, area_retrato, WHITE);
    }

    /* Nome e texto à direita do retrato */
    area_nome.x = tem_retrato ? area_retrato.x + LADO_RETRATO_DIALOGO + MARGEM_CAIXA_DIALOGO
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
