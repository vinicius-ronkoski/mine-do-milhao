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
    SomId som_proximo = SOM_NENHUM;          /* SOM(...) marca o som da próxima fala */
    MusicaId musica_proxima = MUSICA_MANTER; /* MUSICA(...) marca a troca na próxima fala */

    if (dialogo == NULL) {
        return STATUS_ERRO_PARAMETRO;
    }

/* Atalhos só deste switch. Regra única: tudo que modifica uma fala vem ANTES dela.
 *   FALA(personagem, texto, retrato)  acrescenta uma fala (guarda o primeiro erro em `status`)
 *   CENARIO(imagem)  fundo (IMG_*) desta fala E DAS SEGUINTES, até o próximo CENARIO;
 *                    CENARIO(IMG_NENHUMA) volta ao fundo da fase
 *   SOM(som)         som (SOM_*) tocado quando a PRÓXIMA fala aparecer; só nela
 *   MUSICA(musica)   música (MUSICA_*) que começa na PRÓXIMA fala; continua depois da cena
 * Exemplo:
 *   CENARIO(IMG_FUNDO_PALCO);
 *   SOM(SOM_PLATEIA);
 *   MUSICA(MUSICA_PRINCIPAL);
 *   FALA(NOME_APRESENTADOR, "Boa noite!", IMG_ICONE_ALDEAO);   <- palco + plateia + música
 *   FALA(NOME_APRESENTADOR, "Vamos começar.", IMG_ICONE_ALDEAO); <- só o palco continua */
#define FALA(personagem, texto, retrato)                                                   \
    do {                                                                                   \
        if (status == STATUS_OK) {                                                         \
            status = dialogo_adicionar_fala(dialogo, personagem, texto, retrato);          \
            if (status == STATUS_OK) {                                                     \
                Fala *fala_criada = &dialogo->falas[dialogo->quantidade - 1];              \
                fala_criada->fundo = cenario;                                              \
                fala_criada->som = som_proximo;                                            \
                fala_criada->musica = musica_proxima;                                      \
                som_proximo = SOM_NENHUM;        /* som e música valem só para esta fala */ \
                musica_proxima = MUSICA_MANTER;                                            \
            }                                                                              \
        }                                                                                  \
    } while (0)
#define CENARIO(imagem) (cenario = (imagem))
#define SOM(som) (som_proximo = (som))
#define MUSICA(musica) (musica_proxima = (musica))

    switch (cena) {
    case CENA_INTRO:
        MUSICA(MUSICA_SILENCIO);
        // Entrada no palco
        CENARIO(IMG_FUNDO_PALCO);
        SOM(SOM_PLATEIA);
        FALA(NOME_APRESENTADOR, "Muito obrigado, muito obrigado! Hmm!", IMG_ICONE_ALDEAO);
        FALA(
            NOME_APRESENTADOR, 
            "Boa noite senhoras e senhores! Boa noite moças e rapazes! Boa noite meninas e meninos!", 
            IMG_ICONE_ALDEAO
        );
        FALA(NOME_APRESENTADOR, "Auditório, boa noite!", IMG_ICONE_ALDEAO);

        // Auditorio
        CENARIO(IMG_FUNDO_AUDITORIO);
        FALA("Plateia", "BOA NOITE!", IMG_NENHUMA);

        // Apresentador
        CENARIO(IMG_FUNDO_PALCO);
        FALA(NOME_APRESENTADOR, "Bem vindos ao Mine do Milhão! Vamos conhecer as regras...", IMG_ICONE_ALDEAO);
        FALA(NOME_APRESENTADOR, "São 15 perguntas. Cada acerto vale esmeraldas, quem quer ficar rico?",
             IMG_ICONE_ALDEAO);

        // Auditório
        CENARIO(IMG_FUNDO_AUDITORIO);
        FALA("Plateia", "EU!", IMG_NENHUMA);

        // Foreshadowing
        CENARIO(IMG_FUNDO_PRETO);
        FALA("???", "E quem ganha algo com isso?", IMG_ICONE_DESCONHECIDO_2);

        // Apresentador
        CENARIO(IMG_FUNDO_PALCO);
        FALA(NOME_APRESENTADOR, "Não sei se comento... mas devem ter sido vozes da minha cabeça.", IMG_ICONE_ALDEAO);
        FALA(NOME_APRESENTADOR, "Bem, nas perguntas 5 e 10 suas esmeraldas ficam garantidas. "
             "Você pode parar a qualquer momento e levar o que juntou!", IMG_ICONE_ALDEAO);
        FALA(NOME_APRESENTADOR, "Então vamos para o Mine do-", IMG_ICONE_ALDEAO);
        
        // Comissao técnica
        FALA("Comissão Técnica", "EI, VOCÊ NÃO PODE ENTRAR AQUI!", IMG_NENHUMA);
        break;
    case CENA_BOSS_CREEPER:
        break;
    case CENA_BOSS_WITHER:
        MUSICA(MUSICA_WITHER);
        FALA(NOME_APRESENTADOR, "O que é o que é? Tem três cabeças e vai te explodir?!", IMG_ICONE_ALDEAO);
        break;
    case CENA_BOSS_WARDEN:
        MUSICA(MUSICA_WARDEN);
        FALA(NOME_APRESENTADOR, "Você fez barulho demais! Lá vem ele!",
             IMG_ICONE_ALDEAO);
        break;
    case CENA_BOSS_ENDER_DRAGON:
        MUSICA(MUSICA_ENDER_DRAGON);
        FALA(NOME_APRESENTADOR, "Até agora foi tudo fácil demais...", IMG_ICONE_ALDEAO);
        FALA(NOME_APRESENTADOR, "Você consegue derrotar o seu próprio fim?", IMG_ICONE_ALDEAO);
        FALA(NOME_APRESENTADOR, "ou melhor... o dragão do fim.", IMG_ICONE_ALDEAO);
        FALA(NOME_APRESENTADOR, "ha.", IMG_ICONE_ALDEAO);
        FALA(NOME_APRESENTADOR, "ha. ha.", IMG_ICONE_ALDEAO);
        FALA(NOME_APRESENTADOR, "ha. ha. ha.", IMG_ICONE_ALDEAO);
        FALA(NOME_APRESENTADOR, "Deveriam me pagar mais por fazer essas piadas horríveis", IMG_ICONE_ALDEAO);
        break;
    case CENA_BOSS_HEROBRINE:
        MUSICA(MUSICA_HEROBRINE);
        FALA(NOME_APRESENTADOR, "...",IMG_ICONE_ALDEAO);
        FALA(NOME_APRESENTADOR, "...",IMG_ICONE_ALDEAO);
        FALA(NOME_APRESENTADOR, "...boa sorte.",IMG_ICONE_ALDEAO);
        break;
    case CENA_BOSS_DERROTADO:
        FALA(NOME_APRESENTADOR, "ISSO! Que bom que conseguiu derrotar ele, vai ganhar algumas esmeraldas.", 
            IMG_ICONE_ALDEAO);
        break;
    case CENA_DERROTA:
        FALA(NOME_APRESENTADOR, "Droga! O Jogador! Ele caiu! O que farei agora?", IMG_ICONE_ALDEAO);
        FALA(NOME_APRESENTADOR, "Ei cara, levante, precisamos de você!", IMG_ICONE_ALDEAO_ASSUSTADO);
        break;
    case CENA_PARADA:
        FALA(NOME_APRESENTADOR, "Você não vai desistir né?", IMG_ICONE_ALDEAO);
        FALA(NOME_APRESENTADOR, "...né?", IMG_ICONE_ALDEAO_ASSUSTADO);
        break;

    /* --- História (quem chama cada uma: jogo_nova_partida em jogo.c e a revelação em tela.c) ---
     * Exemplo com cenário e som:
     *     CENARIO(IMG_CENARIO_VILA);
     *     FALA(NOME_APRESENTADOR, "...", IMG_ICONE_ALDEAO);
     *     CENARIO(IMG_CENARIO_NETHER);                   (o fundo muda a partir daqui)
     *     SOM(SOM_PORTAL);                               (só a fala de baixo toca o som)
     *     FALA(NOME_APRESENTADOR, "...", IMG_ICONE_ALDEAO);                             */
    case CENA_HISTORIA_1:
        MUSICA(MUSICA_PRINCIPAL);
        CENARIO(IMG_FUNDO_PALCO);
        FALA(NOME_APRESENTADOR, "Quem é você? POR QUE VOCÊ TA SUBINDO NO-", IMG_ICONE_ALDEAO);
        break;
    case CENA_HISTORIA_2:
        FALA(NOME_APRESENTADOR, "[História 2: escrever as falas em dialogo.c]", IMG_ICONE_ALDEAO);
        break;
    case CENA_HISTORIA_3:
        FALA(NOME_APRESENTADOR, "[História 3: escrever as falas em dialogo.c]", IMG_ICONE_ALDEAO);
        break;
    case CENA_HISTORIA_4:
        FALA(NOME_APRESENTADOR, "[História 4: escrever as falas em dialogo.c]", IMG_ICONE_ALDEAO);
        break;
    case CENA_HISTORIA_5:
        FALA(NOME_APRESENTADOR, "[História 5: escrever as falas em dialogo.c]", IMG_ICONE_ALDEAO);
        break;
    case CENA_HISTORIA_FIM:
        FALA(NOME_APRESENTADOR, "[História final: escrever as falas em dialogo.c]", IMG_ICONE_ALDEAO);
        break;
    default:
        status = STATUS_ERRO_PARAMETRO;  /* cena inexistente */
        break;
    }

#undef FALA
#undef CENARIO
#undef SOM
#undef MUSICA

    /* SOM(...) ou MUSICA(...) no fim do `case`, sem FALA depois, seria perdido sem aviso.
     * Só avisa no terminal: a cena continua tocando normalmente. */
    if (status == STATUS_OK && (som_proximo != SOM_NENHUM || musica_proxima != MUSICA_MANTER)) {
        TraceLog(LOG_WARNING, "DIALOGO: cena %d tem SOM/MUSICA sem FALA depois (ignorado)", (int)cena);
    }

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
    return dialogo_adicionar_fala(dialogo, NOME_APRESENTADOR, fala, IMG_ICONE_ALDEAO);
}

Status dialogo_anexar_reacao(Dialogo *dialogo, bool acertou, const char *resposta_certa,
                             BossId boss_id, int esmeraldas, int variante)
{
    /* 
     * - Cada texto pode ter NO MÁXIMO um %s (ou um %d, na tabela das esmeraldas). Sem nenhum também funciona.
     * - Para mostrar o símbolo de porcentagem, escreva %%.
     * - "" = essa fala não aparece.
     * - Cada fala tem REACOES_POR_RESULTADO (4) variações; uma delas é sorteada por reação. */

    /* 1ª fala: a resposta certa (%s = texto da alternativa certa). */
    static const char *const acerto_resposta[REACOES_POR_RESULTADO] = {
        "Isso! \"%s\" está certíssimo! Ainda bem que acertou!",
        "Exato! Era \"%s\" mesmo.",
        "Dessa vez você acertou essa questão!",
        "Isso! Isso! É isso mesmo!"
    };
    static const char *const erro_resposta[REACOES_POR_RESULTADO] = {
        "Droga! A resposta certa era \"%s\".",
        "Não foi dessa vez. Era \"%s\".",
        "Não, não, não, não, não!",
        "Nos vemos na próxima tentativa..."
    };

    /* 2ª fala: depende do boss (%s = nome do boss).
     * UMA LINHA POR BOSS, NA ORDEM DO ENUM BossId (boss.h). */
    static const char *const acerto_boss[BOSS_QUANTIDADE][REACOES_POR_RESULTADO] = {
        /* BOSS_CREEPER      */
        { 
            "Parece que suas respostas dão dano, continue!", 

            "Um acerto, um golpe? Não é hora de parar!", 

            "Você dá dano quando acerta! Como eu sei? Logo respondo", 

            "Que incrível! É como se acertar machucasse o %s"
        },
        /* BOSS_WITHER       */
        { 
            "Que medo! Ele é muito forte! Continue acertando", 

            "Que chamado nos chama em meio às chamas o que... "
            "Olha só para esse espírito de três cabeças", 

            "Ma oê, quem quer dinheiro (me ajude)",

            "Que calor! Como queria estar no palco com meu ar condicionado..." 
        },   
        /* BOSS_WARDEN*/
        { 
            "Acertou, mas fique em silêncio, ele nos ouve...",

            "Shii... não faça tanto barulho!",

            "fala baixo",

            "...",
        },
        /* BOSS_ENDER_DRAGON */          
        { 
            "Sei que está lidando com um dragão, mas gostou das roupas novas?",

            "O bafo dele é roxo! Eu nem sei o cheiro de roxo!",

            "E se fizermos o \"dragãozinho de papel\"?",

            "Se roubarmos o ovo dele, quantas esmeraldas dá?" 
        },
        /* BOSS_HEROBRINE   */
        { "", "", "", "" }    
    };
    static const char *const erro_boss[BOSS_QUANTIDADE][REACOES_POR_RESULTADO] = {
        /* BOSS_CREEPER      */
        { 
            "Fujam! Ele vai explodir!", 

            "Tragam um gato! Repito, tragam um gato!", 

            "Perdeu logo nas perguntas fáceis?! Que decepção!", 

            "Cadê a segurança?"
        },
        /* BOSS_WITHER       */
        { 
            "Por que eu fui invocar isso?", 

            "De fato ele se opõe aos céus!", 

            "Alô? Produção? Conseguem me buscar no Nether eu estou nas coordenadas...",

            "Eu não quero morrer aqui!" 
        },   
        /* BOSS_WARDEN*/
        { 
            "VOCÊ NÃO PODE ERRAR ASSIM!",

            "AGORA VOU FAZER TODO BARULHO POSSÍVEL! VÊ SE ACERTA NAS PRÓXIMAS",

            "FALEI PARA FALAR BAIXO, apesar de só eu estar falando...",

            "...putz",
        },
        /* BOSS_ENDER_DRAGON */          
        { 
            "Vou lá roubar o ovo dele! Depois ligo para a produção vir me buscar",

            "Não tem como eu te reviver? Nem com aquelas magias dos pillagers?",

            "Seremos cozidos pelo bafo roxo...",

            "Por favor dragão do fim! Tenha misericórdia!" 
        },
        /* BOSS_HEROBRINE   */
        { "", "", "", "" }    
    };
    

    /* 3ª fala, só no acerto: saldo de esmeraldas (%d = saldo atual). */
    static const char *const acerto_esmeraldas[REACOES_POR_RESULTADO] = {
        "Vou guardar %d esmeraldas para você!",
        "Me lembre de te dar %d depois!",
        "Você vai querer usar essas %d esmeraldas! Guarde elas!",
        "Esmeraldas, esmeraldas e agora tens %d esmeraldas!"
    };

    static const char *const acerto_esmeraldas_herobrine[REACOES_POR_RESULTADO] = {
        "Por que eu quero ter %d esmeraldas?",
        "Pra que tantas esmeraldas?",
        "Tudo isso para... %d esmeraldas?",
        ""
    };

    Status status = STATUS_OK;
    const char *texto;
    char fala[TAM_FALA];
    int indice;

    if (dialogo == NULL || resposta_certa == NULL || (int)boss_id < 0 || boss_id >= BOSS_QUANTIDADE) {
        return STATUS_ERRO_PARAMETRO;
    }
    indice = (variante < 0 ? -variante : variante) % REACOES_POR_RESULTADO;

    /* 1ª fala: resposta certa */
    /* Remove a primeira parte caso o boss seja o Herobrine*/
    if (boss_id == BOSS_HEROBRINE ) {
        texto = "";
    } else if (boss_id == BOSS_WARDEN){
        // Diz "..." caso seja o warden
        texto = "...";
    } else {
        texto = acertou ? acerto_resposta[indice] : erro_resposta[indice];
    }

    if (texto != NULL && texto[0] != '\0') {
        status = anexar_fala_formatada(dialogo, texto, resposta_certa);
    }

    /* 2ª fala: reação do boss */
    texto = acertou ? acerto_boss[boss_id][indice] : erro_boss[boss_id][indice];
    if (status == STATUS_OK && texto != NULL && texto[0] != '\0') {
        status = anexar_fala_formatada(dialogo, texto, boss_nome(boss_id));
    }

    /* 3ª fala (só no acerto): esmeraldas */
    if (boss_id == BOSS_HEROBRINE ) {
        texto = acerto_esmeraldas_herobrine[indice];
    } else if (boss_id == BOSS_WARDEN){
        // Diz "..." caso seja o warden
        texto = "...%d esmeraldas";
    } else {
        texto = acerto_esmeraldas[indice];
    }

    if (status == STATUS_OK && acertou && texto != NULL && texto[0] != '\0') {
        snprintf(fala, sizeof(fala), texto, esmeraldas);
        if (boss_id == BOSS_HEROBRINE) {
            status = dialogo_adicionar_fala(dialogo, "Você", fala, IMG_ICONE_STEVE);
        } else {
            status = dialogo_adicionar_fala(dialogo, NOME_APRESENTADOR, fala, IMG_ICONE_ALDEAO);
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