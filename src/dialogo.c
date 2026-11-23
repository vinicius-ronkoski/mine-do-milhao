/**
 * @file dialogo.c
 * @brief Implementação dos diálogos (ver dialogo.h). Compila só com USE_RAYLIB.
 *
 * Organização: 1) falas de cada cena (PERSONALIZE AQUI), 2) controle do diálogo, 3) desenho.
 */
#ifdef USE_RAYLIB
#include <stdio.h>
#include <string.h>
#include "dialogo.h"
#include "layout.h"

/** Nome do apresentador nas falas. */
#define NOME_APRESENTADOR "Silvio Aldeão"

/** Número grande de letras: usado para "completar" a fala de uma vez. */
#define LETRAS_FALA_COMPLETA 100000

/* ========================================================================= */
/* 1. Falas de cada cena — TEXTO PROVISÓRIO: personalize à vontade           */
/* ========================================================================= */

Status dialogo_anexar_cena(Dialogo *dialogo, CenaId cena)
{
    Status status = STATUS_OK;
    ImagemId cenario = IMG_NENHUMA;          /* toda cena começa com o fundo da fase */
    MusicaId musica_proxima = MUSICA_MANTER; /* MUSICA(...) marca a troca na próxima fala */

    if (dialogo == NULL) {
        return STATUS_ERRO_PARAMETRO;
    }

/* Atalhos só deste switch: acrescentam uma fala e guardam o primeiro erro.
 *   FALA(personagem, texto, retrato)          fala sem som
 *   FALA_SOM(personagem, texto, retrato, som) fala que toca `som` (SOM_*) quando aparece
 *   CENARIO(imagem)                           as falas seguintes usam este fundo (IMG_*);
 *                                             CENARIO(IMG_NENHUMA) volta ao fundo da fase
 *   MUSICA(musica)                            a música troca quando a PRÓXIMA fala aparecer
 *                                             (MUSICA_*); continua tocando depois da cena */
#define FALA_SOM(personagem, texto, retrato, som)                                          \
    do {                                                                                   \
        if (status == STATUS_OK) {                                                         \
            status = dialogo_adicionar_fala(dialogo, personagem, texto, retrato, som);     \
            if (status == STATUS_OK) {                                                     \
                dialogo->falas[dialogo->quantidade - 1].fundo = cenario;                   \
                dialogo->falas[dialogo->quantidade - 1].musica = musica_proxima;           \
                musica_proxima = MUSICA_MANTER;  /* a troca vale só para esta fala */      \
            }                                                                              \
        }                                                                                  \
    } while (0)
#define FALA(personagem, texto, retrato) FALA_SOM(personagem, texto, retrato, SOM_NENHUM)
#define CENARIO(imagem) (cenario = (imagem))
#define MUSICA(musica) (musica_proxima = (musica))

    switch (cena) {
    case CENA_INTRO:
        MUSICA(MUSICA_SILENCIO);
        // Entrada no palco
        CENARIO(IMG_FUNDO_PALCO);
        FALA_SOM(NOME_APRESENTADOR, "Muito obrigado, muito obrigado! Hmm!", IMG_RETRATO_ALDEAO, SOM_PLATEIA);
        FALA(
            NOME_APRESENTADOR, 
            "Boa noite senhoras e senhores! Boa noite moças e rapazes! Boa noite meninas e meninos!", 
            IMG_RETRATO_ALDEAO
        );
        FALA(NOME_APRESENTADOR, "Auditório, boa noite!", IMG_RETRATO_ALDEAO);

        // Auditorio
        CENARIO(IMG_FUNDO_AUDITORIO);
        FALA("Plateia", "BOA NOITE!", IMG_NENHUMA);

        // Apresentador
        CENARIO(IMG_FUNDO_PALCO);
        FALA(NOME_APRESENTADOR, "Bem vindos ao Mine do Milhão! Vamos conhecer as regras...", IMG_RETRATO_ALDEAO);
        FALA(NOME_APRESENTADOR, "São 15 perguntas. Cada acerto vale esmeraldas, quem quer ficar rico?",
             IMG_RETRATO_ALDEAO);

        // Auditório
        CENARIO(IMG_FUNDO_AUDITORIO);
        FALA("Plateia", "EU!", IMG_NENHUMA);

        // Foreshadowing
        CENARIO(IMG_FUNDO_PRETO);
        FALA("???", "E quem ganha algo com isso?", IMG_RETRATO_DESCONHECIDO);

        // Apresentador
        CENARIO(IMG_FUNDO_PALCO);
        FALA(NOME_APRESENTADOR, "Não sei se comento... mas ok.", IMG_RETRATO_ALDEAO);
        FALA(NOME_APRESENTADOR, "Bem, nas perguntas 5 e 10 suas esmeraldas ficam garantidas. "
             "Você pode parar a qualquer momento e levar o que juntou!", IMG_RETRATO_ALDEAO);
        FALA(NOME_APRESENTADOR, "Então vamos para o Mine do-", IMG_RETRATO_ALDEAO);
        
        FALA("Comissão Técnica", "EI, VOCÊ NÃO PODE ENTRAR AQUI!", IMG_NENHUMA);
        break;
    case CENA_BOSS_CREEPER:
        break;
    case CENA_BOSS_WITHER:
        MUSICA(MUSICA_WITHER);
        FALA(NOME_APRESENTADOR, "O que é o que é? Tem três cabeças e vai te explodir?!", IMG_RETRATO_ALDEAO);
        break;
    case CENA_BOSS_WARDEN:
        MUSICA(MUSICA_WARDEN);
        FALA(NOME_APRESENTADOR, "Você fez barulho demais! Lá vem ele!.",
             IMG_RETRATO_ALDEAO);
        break;
    case CENA_BOSS_ENDER_DRAGON:
        MUSICA(MUSICA_ENDER_DRAGON);
        FALA(NOME_APRESENTADOR, "Até agora foi tudo fácil demais...", IMG_RETRATO_ALDEAO);
        FALA(NOME_APRESENTADOR, "Você consegue derrotar o seu próprio fim?", IMG_RETRATO_ALDEAO);
        FALA(NOME_APRESENTADOR, "ou melhor... o dragão do fim.", IMG_RETRATO_ALDEAO);
        FALA(NOME_APRESENTADOR, "ha.", IMG_RETRATO_ALDEAO);
        FALA(NOME_APRESENTADOR, "ha. ha.", IMG_RETRATO_ALDEAO);
        FALA(NOME_APRESENTADOR, "ha. ha. ha.", IMG_RETRATO_ALDEAO);
        FALA(NOME_APRESENTADOR, "Deveriam me pagar mais por fazer essas piadas horríveis", IMG_RETRATO_ALDEAO);
        break;
    case CENA_BOSS_HEROBRINE:
        MUSICA(MUSICA_HEROBRINE);
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
        FALA(NOME_APRESENTADOR, "Fim de jogo para você... mas foi uma bela tentativa!", IMG_RETRATO_ALDEAO);
        FALA(NOME_APRESENTADOR, "Obrigado por jogar o Mine do Milhão. Volte quando quiser!", IMG_RETRATO_ALDEAO);
        break;
    case CENA_PARADA:
        FALA(NOME_APRESENTADOR, "Decisão sábia! Leve suas esmeraldas e fuja daqui!", IMG_RETRATO_ALDEAO);
        break;

    /* --- História (quem chama cada uma: jogo_nova_partida em jogo.c e a revelação em tela.c) ---
     * Exemplo com cenário e som:
     *     CENARIO(IMG_CENARIO_VILA);
     *     FALA(NOME_APRESENTADOR, "...", IMG_RETRATO_ALDEAO);
     *     CENARIO(IMG_CENARIO_NETHER);                   (o fundo muda a partir daqui)
     *     FALA_SOM(NOME_APRESENTADOR, "...", IMG_RETRATO_ALDEAO, SOM_PORTAL);             */
    case CENA_HISTORIA_1:
        MUSICA(MUSICA_PRINCIPAL);
        CENARIO(IMG_FUNDO_PALCO);
        FALA(NOME_APRESENTADOR, "Quem é você? POR QUE VOCÊ TA SUBINDO NO-", IMG_RETRATO_ALDEAO);
        break;
    case CENA_HISTORIA_2:
        FALA(NOME_APRESENTADOR, "[História 2: escrever as falas em dialogo.c]", IMG_RETRATO_ALDEAO);
        break;
    case CENA_HISTORIA_3:
        FALA(NOME_APRESENTADOR, "[História 3: escrever as falas em dialogo.c]", IMG_RETRATO_ALDEAO);
        break;
    case CENA_HISTORIA_4:
        FALA(NOME_APRESENTADOR, "[História 4: escrever as falas em dialogo.c]", IMG_RETRATO_ALDEAO);
        break;
    case CENA_HISTORIA_5:
        FALA(NOME_APRESENTADOR, "[História 5: escrever as falas em dialogo.c]", IMG_RETRATO_ALDEAO);
        break;
    case CENA_HISTORIA_FIM:
        FALA(NOME_APRESENTADOR, "[História final: escrever as falas em dialogo.c]", IMG_RETRATO_ALDEAO);
        break;
    default:
        status = STATUS_ERRO_PARAMETRO;  /* cena inexistente */
        break;
    }

#undef FALA
#undef FALA_SOM
#undef CENARIO
#undef MUSICA

    if (status == STATUS_OK && dialogo->quantidade > 0 && dialogo->indice < dialogo->quantidade) {
        dialogo->terminou = false;  /* havia terminado e ganhou falas novas: volta a tocar */
    }
    return status;
}

/**
 * @brief Monta uma fala do apresentador trocando o único %s de `formato` por `texto` e a acrescenta.
 *        Se não couber em TAM_FALA, o texto é cortado (snprintf nunca passa do buffer).
 */
static Status anexar_fala_formatada(Dialogo *dialogo, const char *formato, const char *texto)
{
    char fala[TAM_FALA];
    int escritos = snprintf(fala, sizeof(fala), formato, texto);

    if (escritos < 0) {
        return STATUS_ERRO_PARAMETRO;
    }
    return dialogo_adicionar_fala(dialogo, NOME_APRESENTADOR, fala, IMG_RETRATO_ALDEAO, SOM_NENHUM);
}

Status dialogo_anexar_reacao(Dialogo *dialogo, bool acertou, const char *resposta_certa,
                             const char *nome_boss, int esmeraldas, int variante)
{
    /* Cada formato tem exatamente um %s: a 1ª fala recebe a resposta certa, a 2ª o nome do boss.
     * "" = sem 2ª fala. A variação 1 do acerto troca a fala do boss pela das esmeraldas. */
    static const char *const acerto_resposta[REACOES_POR_RESULTADO] = {
        "Isso! \"%s\" está certíssimo!",
        "Na mosca! Era \"%s\" mesmo.",
        "Hmmm! \"%s\"... resposta certa!"
    };
    static const char *const acerto_boss[REACOES_POR_RESULTADO] = {
        "O %s sentiu esse golpe!",
        "O %s está ficando nervoso...",
        "Toma essa, %s!"
    };
    static const char *const erro_resposta[REACOES_POR_RESULTADO] = {
        "Ihh... a resposta certa era \"%s\".",
        "Não foi dessa vez. Era \"%s\".",
        "Hmmm... errado! O certo era \"%s\"."
    };
    static const char *const erro_boss[REACOES_POR_RESULTADO] = {
        "O %s nem sentiu nada.",
        "",
        "O %s está rindo da sua cara!"
    };
    Status status;
    int indice;

    if (dialogo == NULL || resposta_certa == NULL || nome_boss == NULL) {
        return STATUS_ERRO_PARAMETRO;
    }
    indice = (variante < 0 ? -variante : variante) % REACOES_POR_RESULTADO;

    status = anexar_fala_formatada(dialogo, acertou ? acerto_resposta[indice] : erro_resposta[indice],
                                   resposta_certa);
    if (status != STATUS_OK) {
        return status;
    }

    if (acertou && indice == 1) {
        char fala[TAM_FALA];

        snprintf(fala, sizeof(fala), "Agora você tem %d esmeraldas!", esmeraldas);
        status = dialogo_adicionar_fala(dialogo, NOME_APRESENTADOR, fala, IMG_RETRATO_ALDEAO, SOM_NENHUM);
    } else {
        const char *segunda = acertou ? acerto_boss[indice] : erro_boss[indice];

        if (segunda[0] != '\0') {
            status = anexar_fala_formatada(dialogo, segunda, nome_boss);
        }
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
    dialogo->terminou = true;          /* vazio = nada para mostrar */
    dialogo->indice_fala_anunciada = -1;  /* depois do memset: 0 diria "a 1ª fala já foi entregue" */
}

Status dialogo_adicionar_fala(Dialogo *dialogo, const char *personagem, const char *texto,
                              ImagemId retrato, SomId som)
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
    nova_fala->som = som;
    nova_fala->fundo = IMG_NENHUMA;     /* FALA/FALA_SOM trocam pelo CENARIO atual */
    nova_fala->musica = MUSICA_MANTER;  /* FALA/FALA_SOM trocam se houver MUSICA(...) antes */
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
