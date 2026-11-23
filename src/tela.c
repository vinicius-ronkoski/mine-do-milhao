/**
 * @file tela.c
 * @brief Telas do jogo (ver tela.h). Compila só com USE_RAYLIB.
 *
 * Organização: 1) desenhos compartilhados, 2) diálogo, 3) pergunta, 4) revelação, 5) resultado.
 *
 * Fluxo de uma resposta: pergunta (clique) -> reação do apresentador (diálogo) -> revelação
 * (som de dano e a vida do boss caindo) -> próxima pergunta, boss derrotado, derrota ou vitória.
 */
#ifdef USE_RAYLIB
#include "tela.h"
#include "layout.h"
#include "utils.h"

#include <math.h>
#include <stdio.h>

/* Posições e tamanhos de cada tela: include/layout.h */

/** Duração da animação de dano na revelação: a barra desce e o boss treme/pisca em vermelho. */
#define DURACAO_DANO 0.6f

/* ========================================================================= */
/* 1. Desenhos compartilhados                                                */
/* ========================================================================= */

BossId tela_boss_da_fase(const Jogo *jogo)
{
    const Boss *boss;

    // Verifica parametros
    if (jogo == NULL) {
        return BOSS_QUANTIDADE;
    }

    // Recebe o Boss Atual e retorna o ID 
    boss = partida_boss_atual(&jogo->partida);
    return boss != NULL ? boss->id : BOSS_QUANTIDADE;
}

void tela_desenhar_barra_vida(Rectangle area, float fracao, Color cor)
{
    Rectangle preenchimento = area;

    if (fracao < 0.0f) {
        fracao = 0.0f;
    } else if (fracao > 1.0f) {
        fracao = 1.0f;
    }
    preenchimento.width = area.width * fracao;


    // Desenha um retangulo preenchido
    DrawRectangleRec(area, Fade(BLACK, 0.6f));
    DrawRectangleRec(preenchimento, cor);
    DrawRectangleLinesEx(area, 2.0f, PALETA_BORDA);
}

void tela_desenhar_banner_erro(const Jogo *jogo)
{
    Rectangle faixa = { 0.0f, (float)ALTURA_JANELA - ALTURA_BANNER_ERRO,
                        (float)LARGURA_JANELA, ALTURA_BANNER_ERRO };

    // Parametros
    if (jogo == NULL || jogo->ultimo_erro == STATUS_OK) {
        return;
    }

    // Desenha o retangulo preenchido com efeito de fade
    DrawRectangleRec(faixa, Fade(PALETA_ERRO, 0.9f));
    assets_desenhar_texto_centralizado(&jogo->assets, utils_mensagem_status(jogo->ultimo_erro),
                                       faixa, 20.0f, PALETA_TEXTO);
}

void tela_desenhar_hud(const Jogo *jogo)
{
    Rectangle area_saldo = AREA_ESMERALDAS, area_rodada = AREA_RODADA;
    Rectangle icone = { area_saldo.x + 12.0f, area_saldo.y + (area_saldo.height - LADO_ICONE_ESMERALDA) / 2.0f,
                        LADO_ICONE_ESMERALDA, LADO_ICONE_ESMERALDA };
    char texto[48];
    int rodada;

    if (jogo == NULL) {
        return;
    }

    /* Saldo: ícone da esmeralda + quanto o jogador pode gastar */
    assets_desenhar_painel(&jogo->assets, IMG_PAINEL_ENUNCIADO, area_saldo, PALETA_PAINEL, WHITE);
    assets_desenhar_imagem(&jogo->assets, IMG_ESMERALDA, icone, WHITE);
    snprintf(texto, sizeof(texto), "%d", partida_saldo(&jogo->partida));
    assets_desenhar_texto(&jogo->assets, texto, (Vector2){ icone.x + LADO_ICONE_ESMERALDA + 12.0f, area_saldo.y + 12.0f },
                          32.0f, PALETA_ESMERALDA);

    /* "Pergunta N/15" e o nome do nível */
    rodada = partida_numero_da_rodada(&jogo->partida);
    snprintf(texto, sizeof(texto), "Pergunta %d/%d", rodada, PERGUNTAS_POR_PARTIDA);
    assets_desenhar_texto(&jogo->assets, texto, (Vector2){ area_rodada.x, area_rodada.y + 2.0f },
                          24.0f, PALETA_TEXTO);
    assets_desenhar_texto(&jogo->assets, pergunta_nome_nivel(partida_nivel_da_rodada(jogo->partida.rodada)),
                          (Vector2){ area_rodada.x, area_rodada.y + 30.0f }, 20.0f, PALETA_TEXTO_SUAVE);
}

void tela_desenhar_boss(const Jogo *jogo, Rectangle area)
{
    const Boss *boss;
    Rectangle area_nome, area_sprite, area_vida, area_texto;
    Color cor_boss;
    float fracao;
    char texto[32];

    if (jogo == NULL) {
        return;
    }
    boss = partida_boss_atual(&jogo->partida);
    if (boss == NULL) {
        return;
    }

    /* `area` é o bloco inteiro: nome (36) | sprite quadrado (largura x largura) | barra (24) | "Vida x/y" */
    area_nome = (Rectangle){ area.x, area.y, area.width, 36.0f };
    area_sprite = (Rectangle){ area.x, area.y + 40.0f, area.width, area.width };
    area_vida = (Rectangle){ area.x, area_sprite.y + area_sprite.height + 15.0f, area.width, 24.0f };
    area_texto = (Rectangle){ area.x, area_vida.y + 28.0f, area.width, 30.0f };

    /* Na revelação de um acerto, a vida desce de (vida + 1) até a vida atual em DURACAO_DANO
     * segundos, e o boss treme e pisca em vermelho. A regra já tirou a vida em partida_responder;
     * aqui é só a animação. */
    fracao = boss_calculo_vida(boss);
    cor_boss = WHITE;
    if (jogo->tela == TELA_REVELACAO && jogo->partida.ultima_acertou
        && jogo->tempo_na_tela < DURACAO_DANO && boss->vida_maxima > 0) {
        float progresso = jogo->tempo_na_tela / DURACAO_DANO;  /* 0 -> 1 */
        float fracao_antes = (float)(boss->vida_atual + 1) / (float)boss->vida_maxima;

        fracao = fracao_antes + (fracao - fracao_antes) * progresso;
        cor_boss = (Color){ 255, (unsigned char)(255.0f * progresso), (unsigned char)(255.0f * progresso), 255 };
        area_sprite.x += sinf(jogo->tempo_na_tela * 60.0f) * 8.0f * (1.0f - progresso);
    }

    assets_desenhar_texto_centralizado(&jogo->assets, boss_nome(boss->id), area_nome, 30.0f, PALETA_DESTAQUE);
    assets_desenhar_boss(&jogo->assets, boss->id, jogo->tempo_total, area_sprite, cor_boss);
    tela_desenhar_barra_vida(area_vida, fracao, PALETA_ERRO);
    snprintf(texto, sizeof(texto), "Vida %d/%d", boss->vida_atual, boss->vida_maxima);
    assets_desenhar_texto_centralizado(&jogo->assets, texto, area_texto, 20.0f, PALETA_TEXTO);
}

void tela_desenhar_escada_premios(const Jogo *jogo, Rectangle area)
{
    /* TODO (equipe): ver tela.h. */
    (void)jogo; (void)area;
}

/* ========================================================================= */
/* 2. Tela de diálogo (pronta)                                               */
/* ========================================================================= */

void tela_dialogo_atualizar(Jogo *jogo, float dt)
{
    const Fala *fala_nova;
    bool avancou;

    if (jogo == NULL) {
        return;
    }
    avancou = jogo->tempo_na_tela >= ATRASO_ENTRADA_TELA && jogo_jogador_quer_avancar();
    if (avancou) {
        assets_tocar_som(&jogo->assets, SOM_CLIQUE);
    }
    if (dialogo_atualizar(&jogo->dialogo, dt, avancou)) {
        jogo_trocar_tela(jogo, jogo->tela_apos_dialogo);
        return;
    }

    /* Fala que acabou de aparecer (uma vez por fala): toca o som e troca a música dela.
     * SOM_NENHUM e MUSICA_MANTER não fazem nada. */
    fala_nova = dialogo_fala_nova(&jogo->dialogo);
    if (fala_nova != NULL) {
        assets_tocar_som(&jogo->assets, fala_nova->som);
        assets_trocar_musica(&jogo->assets, fala_nova->musica);
    }
}

void tela_dialogo_desenhar(const Jogo *jogo)
{
    const Boss *boss;
    const Fala *fala;
    Rectangle area_boss;

    if (jogo == NULL) {
        return;
    }

    /* Fala com cenário próprio (CENARIO numa cena de história): só o cenário, sem o boss */
    fala = dialogo_fala_atual(&jogo->dialogo);
    if (fala != NULL && fala->fundo != IMG_NENHUMA) {
        assets_desenhar_fundo(&jogo->assets, fala->fundo, BOSS_QUANTIDADE);
        dialogo_desenhar(&jogo->dialogo, &jogo->assets);
        return;
    }

    assets_desenhar_fundo(&jogo->assets, IMG_FUNDO_DIALOGO, tela_boss_da_fase(jogo));

    /* Boss da rodada no centro, acima da caixa de fala. Durante a reação a uma resposta
     * (ESTADO_REVELANDO) ele aparece mesmo derrotado: a queda é mostrada depois, na revelação. */
    boss = partida_boss_atual(&jogo->partida);
    if (boss != NULL && (!boss_derrotado(boss) || jogo->partida.estado == ESTADO_REVELANDO)) {
        area_boss.width = LADO_BOSS_DIALOGO;
        area_boss.height = LADO_BOSS_DIALOGO;
        area_boss.x = ((float)LARGURA_JANELA - LADO_BOSS_DIALOGO) / 2.0f;
        area_boss.y = TOPO_BOSS_DIALOGO;
        assets_desenhar_boss(&jogo->assets, boss->id, jogo->tempo_total, area_boss, WHITE);
    }

    dialogo_desenhar(&jogo->dialogo, &jogo->assets);
}

/* ========================================================================= */
/* 3. Tela de pergunta (pronta)                                              */
/*    Esquerda: enunciado, alternativas A-D uma embaixo da outra, ajudas.    */
/*    Direita: esmeraldas (canto superior), boss, barra de vida e dica.      */
/* ========================================================================= */

void tela_pergunta_atualizar(Jogo *jogo, float dt)
{
    const Boss *boss;
    Status status = STATUS_OK;
    int posicao;

    (void)dt;
    if (jogo == NULL) {
        return;
    }

    /* Som de entrada: só na 1ª pergunta do boss (vida cheia) e uma vez por tela */
    boss = partida_boss_atual(&jogo->partida);
    if (!jogo->efeito_da_tela_tocado && boss != NULL && boss->vida_atual == boss->vida_maxima) {
        assets_tocar_som(&jogo->assets, (SomId)(SOM_ENTRADA_CREEPER + boss->id));
    }
    jogo->efeito_da_tela_tocado = true;

    for (posicao = 0; posicao < NUM_ALTERNATIVAS; posicao++) {
        if (jogo_botao_acionado(jogo, &jogo->alternativas[posicao])) {
            status = partida_responder(&jogo->partida, posicao);
            if (status == STATUS_OK) {
                /* Respondeu: som de acerto/erro e a reação do apresentador; a revelação vem depois */
                assets_tocar_som(&jogo->assets, jogo->partida.ultima_acertou ? SOM_ACERTO : SOM_ERRO);
                jogo_tocar_reacao(jogo);
                return;  /* a tela mudou: não lê os outros botões neste quadro */
            }
        }
    }
    if (jogo_botao_acionado(jogo, &jogo->botao_dica)) {
        status = partida_comprar_dica(&jogo->partida);
        jogo_preparar_botoes_pergunta(jogo);
    }
    if (jogo_botao_acionado(jogo, &jogo->botao_eliminar)) {
        status = partida_eliminar_duas(&jogo->partida);
        jogo_preparar_botoes_pergunta(jogo);
    }
    if (jogo_botao_acionado(jogo, &jogo->botao_parar)) {
        status = partida_parar(&jogo->partida);
        if (status == STATUS_OK) {
            jogo_tocar_cena(jogo, CENA_PARADA, TELA_RESULTADO);
        }
    }
    if (status != STATUS_OK) {
        jogo_registrar_erro(jogo, status, "pergunta");
    }
}

void tela_pergunta_desenhar(const Jogo *jogo)
{
    const PerguntaSorteada *pergunta;
    const char *dica;
    Rectangle enunciado = AREA_ENUNCIADO;
    Rectangle texto_enunciado = { enunciado.x + 20.0f, enunciado.y + 16.0f,
                                  enunciado.width - 40.0f, enunciado.height - 32.0f };
    int posicao;

    if (jogo == NULL) {
        return;
    }
    pergunta = partida_pergunta_atual(&jogo->partida);
    dica = partida_dica_atual(&jogo->partida);
    assets_desenhar_fundo(&jogo->assets, IMG_FUNDO_PERGUNTA, tela_boss_da_fase(jogo));

    /* Coluna da esquerda: enunciado (com a dica embaixo, se comprada), alternativas e ajudas */
    assets_desenhar_painel(&jogo->assets, IMG_PAINEL_ENUNCIADO, enunciado, PALETA_PAINEL, WHITE);
    if (dica != NULL) {
        Rectangle texto_dica = { texto_enunciado.x, enunciado.y + enunciado.height - ALTURA_DICA_ENUNCIADO,
                                 texto_enunciado.width, ALTURA_DICA_ENUNCIADO - 12.0f };
        char linha_dica[TAM_DICA + 8];

        texto_enunciado.height -= ALTURA_DICA_ENUNCIADO;  /* a pergunta encolhe para dar lugar à dica */
        DrawLineEx((Vector2){ texto_dica.x, texto_dica.y - 4.0f },
                   (Vector2){ texto_dica.x + texto_dica.width, texto_dica.y - 4.0f }, 2.0f, PALETA_BORDA);
        snprintf(linha_dica, sizeof(linha_dica), "Dica: %s", dica);
        assets_desenhar_texto_ajustado(&jogo->assets, linha_dica, texto_dica, TAMANHO_DICA, 14.0f,
                                       PALETA_DESTAQUE);
    }
    if (pergunta != NULL) {
        assets_desenhar_texto_ajustado(&jogo->assets, pergunta->origem->enunciado, texto_enunciado,
                                       TAMANHO_ENUNCIADO, 18.0f, PALETA_TEXTO);
    }
    for (posicao = 0; posicao < NUM_ALTERNATIVAS; posicao++) {
        botao_desenhar(&jogo->alternativas[posicao], &jogo->assets, TAMANHO_BOTAO);
    }
    botao_desenhar(&jogo->botao_parar, &jogo->assets, TAMANHO_BOTAO_AJUDA);
    botao_desenhar(&jogo->botao_dica, &jogo->assets, TAMANHO_BOTAO_AJUDA);
    botao_desenhar(&jogo->botao_eliminar, &jogo->assets, TAMANHO_BOTAO_AJUDA);

    /* Coluna da direita */
    tela_desenhar_hud(jogo);
    tela_desenhar_boss(jogo, AREA_BOSS);
}

/* ========================================================================= */
/* 4. Tela de revelação (depois da reação do apresentador)                   */
/*    Sem pergunta nem alternativas: só o boss, a vida e o resultado.        */
/* ========================================================================= */

void tela_revelacao_atualizar(Jogo *jogo, float dt)
{
    const Boss *proximo_boss;
    Status status;

    (void)dt;
    if (jogo == NULL) {
        return;
    }

    /* Ao entrar: som de dano se acertou (a barra cai em tela_desenhar_boss) */
    if (!jogo->efeito_da_tela_tocado) {
        if (jogo->partida.ultima_acertou) {
            assets_tocar_som(&jogo->assets, SOM_DANO_BOSS);
        }
        botao_definir_rotulo(&jogo->botao_continuar, "Continuar (ENTER)");
        jogo->efeito_da_tela_tocado = true;
    }

    if (!jogo_botao_acionado(jogo, &jogo->botao_continuar)) {
        return;
    }
    status = partida_confirmar_revelacao(&jogo->partida);
    if (status != STATUS_OK) {
        jogo_registrar_erro(jogo, status, "revelação");
        return;
    }

    /* Para onde ir depois da resposta */
    switch (jogo->partida.estado) {
    case ESTADO_FIM_ERRO:        /* errou: agradecimento e tela de derrota */
        jogo_tocar_cena(jogo, CENA_DERROTA, TELA_RESULTADO);
        break;
    case ESTADO_FIM_VITORIA:     /* acertou a 15ª: o último boss caiu */
        jogo_tocar_cena(jogo, CENA_BOSS_DERROTADO, TELA_RESULTADO);
        jogo_anexar_cena(jogo, CENA_HISTORIA_FIM);
        jogo_anexar_cena(jogo, CENA_VITORIA);
        break;
    default:                     /* a partida continua */
        jogo_preparar_botoes_pergunta(jogo);
        proximo_boss = partida_boss_atual(&jogo->partida);
        if (jogo->partida.boss_derrotado_agora && proximo_boss != NULL) {
            /* Boss caiu: comemoração, história e entrada do boss do próximo nível */
            jogo_tocar_cena(jogo, CENA_BOSS_DERROTADO, TELA_PERGUNTA);
            switch (proximo_boss->id) {
            case BOSS_WITHER:       jogo_anexar_cena(jogo, CENA_HISTORIA_2); break;  /* indo para o Nether */
            case BOSS_WARDEN:       jogo_anexar_cena(jogo, CENA_HISTORIA_3); break;
            case BOSS_ENDER_DRAGON: jogo_anexar_cena(jogo, CENA_HISTORIA_4); break;  /* indo para o Fim */
            case BOSS_HEROBRINE:    jogo_anexar_cena(jogo, CENA_HISTORIA_5); break;  /* voltando do Fim */
            default:                break;                                           /* sem história */
            }
            jogo_anexar_cena(jogo, dialogo_cena_do_boss(proximo_boss->id));
        } else {
            jogo_trocar_tela(jogo, TELA_PERGUNTA);
        }
        break;
    }
}

/** @brief Número em verde com o ícone da esmeralda ao lado, os dois centralizados em `area`. */
static void desenhar_numero_com_esmeralda(const Jogo *jogo, int numero, Rectangle area, float tamanho)
{
    char texto[16];
    float largura_texto, x;
    Rectangle icone;

    snprintf(texto, sizeof(texto), "%d", numero);
    largura_texto = assets_largura_texto(&jogo->assets, texto, tamanho);
    x = area.x + (area.width - (largura_texto + 8.0f + LADO_ICONE_ESMERALDA)) / 2.0f;
    assets_desenhar_texto(&jogo->assets, texto, (Vector2){ x, area.y + (area.height - tamanho) / 2.0f },
                          tamanho, PALETA_ESMERALDA);
    icone = (Rectangle){ x + largura_texto + 8.0f, area.y + (area.height - LADO_ICONE_ESMERALDA) / 2.0f,
                         LADO_ICONE_ESMERALDA, LADO_ICONE_ESMERALDA };
    assets_desenhar_imagem(&jogo->assets, IMG_ESMERALDA, icone, WHITE);
}

void tela_revelacao_desenhar(const Jogo *jogo)
{
    const Boss *boss;
    Rectangle area_boss = { ((float)LARGURA_JANELA - LADO_BOSS_REVELACAO) / 2.0f, TOPO_BOSS_REVELACAO,
                            LADO_BOSS_REVELACAO, LADO_BOSS_REVELACAO + 110.0f };
    Rectangle linha1 = { 0.0f, TOPO_MENSAGEM_REVELACAO, (float)LARGURA_JANELA, 50.0f };
    Rectangle linha2 = { 0.0f, TOPO_DETALHE_REVELACAO, (float)LARGURA_JANELA, 40.0f };
    char texto[96];

    if (jogo == NULL) {
        return;
    }
    assets_desenhar_fundo(&jogo->assets, IMG_FUNDO_PERGUNTA, tela_boss_da_fase(jogo));
    tela_desenhar_boss(jogo, area_boss);

    boss = partida_boss_atual(&jogo->partida);
    if (jogo->partida.ultima_acertou) {
        if (jogo->partida.boss_derrotado_agora && boss != NULL) {
            snprintf(texto, sizeof(texto), "%s derrotado!", boss_nome(boss->id));
        } else {
            snprintf(texto, sizeof(texto), "Acertou! -1 de vida");
        }
        assets_desenhar_texto_centralizado(&jogo->assets, texto, linha1, 40.0f, PALETA_DESTAQUE);
        desenhar_numero_com_esmeralda(jogo, partida_saldo(&jogo->partida), linha2, 32.0f);
    } else {
        assets_desenhar_texto_centralizado(&jogo->assets, "Errou! Nenhum dano.", linha1, 40.0f, PALETA_ERRO);
    }

    botao_desenhar(&jogo->botao_continuar, &jogo->assets, TAMANHO_BOTAO_AJUDA);
}

/* ========================================================================= */
/* 5. Tela de resultado (fim da partida: derrota, parada ou vitória)         */
/* ========================================================================= */

void tela_resultado_atualizar(Jogo *jogo, float dt)
{
    Status status;

    (void)dt;
    if (jogo == NULL) {
        return;
    }
    if (!jogo->efeito_da_tela_tocado) {
        botao_definir_rotulo(&jogo->botao_continuar, "Reiniciar (ENTER)");
        jogo->efeito_da_tela_tocado = true;
    }

    /* Reiniciar: partida nova desde a introdução */
    if (jogo_botao_acionado(jogo, &jogo->botao_continuar)) {
        status = jogo_nova_partida(jogo);
        if (status != STATUS_OK) {
            jogo_registrar_erro(jogo, status, "reiniciar");
        }
    }
}

void tela_resultado_desenhar(const Jogo *jogo)
{
    const Partida *partida;
    const char *titulo;
    Rectangle janela_titulo = { 0.0f, TOPO_TITULO_RESULTADO, (float)LARGURA_JANELA, 70.0f };
    Rectangle retrato = { ((float)LARGURA_JANELA - LADO_RETRATO_RESULTADO) / 2.0f, TOPO_RETRATO_RESULTADO,
                          LADO_RETRATO_RESULTADO, LADO_RETRATO_RESULTADO };
    Rectangle linha_obrigado = { 0.0f, TOPO_AGRADECIMENTO, (float)LARGURA_JANELA, 40.0f };
    Rectangle linha_premio = { 0.0f, TOPO_PREMIO_RESULTADO, (float)LARGURA_JANELA, 56.0f };
    Rectangle linha_detalhe1 = { 0.0f, TOPO_DETALHE1_RESULTADO, (float)LARGURA_JANELA, 32.0f };
    Rectangle linha_detalhe2 = { 0.0f, TOPO_DETALHE2_RESULTADO, (float)LARGURA_JANELA, 32.0f };
    char texto[96];

    if (jogo == NULL) {
        return;
    }
    partida = &jogo->partida;
    assets_desenhar_fundo(&jogo->assets, IMG_FUNDO_RESULTADO, BOSS_QUANTIDADE);

    switch (partida->estado) {
    case ESTADO_FIM_VITORIA: titulo = "Vitória!";      break;
    case ESTADO_FIM_PARADA:  titulo = "Você parou";    break;
    default:                 titulo = "Fim de jogo";   break;
    }
    assets_desenhar_texto_centralizado(&jogo->assets, titulo, janela_titulo, 56.0f, PALETA_DESTAQUE);

    /* O apresentador agradece */
    assets_desenhar_imagem(&jogo->assets, IMG_RETRATO_ALDEAO, retrato, WHITE);
    assets_desenhar_texto_centralizado(&jogo->assets, "Obrigado por jogar o Mine do Milhão!",
                                       linha_obrigado, 28.0f, PALETA_TEXTO);

    /* Prêmio final (já com marco de segurança e dicas descontadas) */
    snprintf(texto, sizeof(texto), "Você leva %d esmeralda%s", partida_premio_final(partida),
             partida_premio_final(partida) == 1 ? "" : "s");
    assets_desenhar_texto_centralizado(&jogo->assets, texto, linha_premio, 40.0f, PALETA_ESMERALDA);

    if (partida->estado == ESTADO_FIM_ERRO) {
        if (partida->esmeraldas_garantidas > 0) {
            snprintf(texto, sizeof(texto), "O marco de segurança garantiu %d esmeraldas.",
                     partida->esmeraldas_garantidas);
        } else {
            snprintf(texto, sizeof(texto), "Você errou antes do primeiro marco (pergunta 5).");
        }
    } else {
        snprintf(texto, sizeof(texto), "Você chegou até a pergunta %d de %d.",
                 partida_numero_da_rodada(partida), PERGUNTAS_POR_PARTIDA);
    }
    assets_desenhar_texto_centralizado(&jogo->assets, texto, linha_detalhe1, 22.0f, PALETA_TEXTO_SUAVE);
    if (partida->esmeraldas_gastas > 0) {
        snprintf(texto, sizeof(texto), "(%d esmeralda%s gasta%s em ajudas já descontada%s)",
                 partida->esmeraldas_gastas, partida->esmeraldas_gastas == 1 ? "" : "s",
                 partida->esmeraldas_gastas == 1 ? "" : "s", partida->esmeraldas_gastas == 1 ? "" : "s");
        assets_desenhar_texto_centralizado(&jogo->assets, texto, linha_detalhe2, 22.0f, PALETA_TEXTO_SUAVE);
    }

    botao_desenhar(&jogo->botao_continuar, &jogo->assets, TAMANHO_BOTAO_AJUDA);
}

#endif /* USE_RAYLIB */
