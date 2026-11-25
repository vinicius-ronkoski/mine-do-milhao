/**
 * @file tela.c
 * @brief Telas do jogo (ver tela.h). Compila só com USE_RAYLIB.
 *
 * Organização: 1) desenhos compartilhados, 2) menu e carregamento, 3) diálogo, 4) pergunta,
 *              5) revelação, 6) resultado, 7) créditos (texto em creditos.c).
 *
 * Fluxo de uma resposta: pergunta (clique) -> reação do apresentador (diálogo) -> revelação
 * (som de dano e a vida do boss caindo) -> próxima pergunta, boss derrotado, derrota ou vitória.
 */
#ifdef USE_RAYLIB
#include "tela.h"
#include "creditos.h"
#include "layout.h"
#include "utils.h"

#include <math.h>
#include <stdio.h>

/* Posições e tamanhos de cada tela: include/layout.h */

/** Duração da animação de dano na revelação: a barra desce e o boss treme/pisca em vermelho. */
#define DURACAO_DANO 0.6f

/* ---- 1. Desenhos compartilhados ---- */

/** @brief Desenha os botões de `primeiro` a `ultimo` (ids consecutivos em BotaoId). */
static void desenhar_botoes(const Jogo *jogo, BotaoId primeiro, BotaoId ultimo, float tamanho_fonte)
{
    int id;

    for (id = (int)primeiro; id <= (int)ultimo; id++) {
        botao_desenhar(&jogo->botoes[id], &jogo->assets, tamanho_fonte);
    }
}

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

/* ---- 2. Menu principal e carregamento ---- */

void tela_menu_atualizar(Jogo *jogo, float dt)
{
    (void)dt;
    if (jogo == NULL) {
        return;
    }
    if (!jogo->efeito_da_tela_tocado) {
        Save save;

        assets_trocar_musica(&jogo->assets, MUSICA_PRINCIPAL);
        /* "Carregar jogo" só com um save válido (o arquivo pode ter mudado desde a última vez) */
        jogo->botoes[BOTAO_CARREGAR].habilitado = save_ler(CAMINHO_SAVE, &save) == STATUS_OK;
        jogo->efeito_da_tela_tocado = true;
    }

    if (jogo_botao_acionado(jogo, &jogo->botoes[BOTAO_JOGAR])) {
        jogo->acao_apos_carregar = ACAO_NOVA_PARTIDA;
        jogo_trocar_tela(jogo, TELA_CARREGANDO);
    } else if (jogo_botao_acionado(jogo, &jogo->botoes[BOTAO_CARREGAR])) {
        jogo->acao_apos_carregar = ACAO_CONTINUAR_SAVE;
        jogo_trocar_tela(jogo, TELA_CARREGANDO);
    } else if (jogo_botao_acionado(jogo, &jogo->botoes[BOTAO_CREDITOS])) {
        jogo_trocar_tela(jogo, TELA_CREDITOS);   /* direto: não há nada para carregar */
    } else if (jogo_botao_acionado(jogo, &jogo->botoes[BOTAO_SAIR])) {
        jogo_trocar_tela(jogo, TELA_SAIR);
    }
}

void tela_menu_desenhar(const Jogo *jogo)
{
    Vector2 posicao_versao = { MARGEM_VERSAO_MENU,
                               (float)ALTURA_JANELA - MARGEM_VERSAO_MENU - TAMANHO_VERSAO_MENU };

    if (jogo == NULL) {
        return;
    }
    assets_desenhar_fundo(&jogo->assets, IMG_FUNDO_MENU, BOSS_QUANTIDADE);

    /* Logo; sem o titulo.png, o nome do jogo em texto */
    if (assets_imagem_fallback(&jogo->assets, IMG_TITULO)) {
        assets_desenhar_texto_centralizado(&jogo->assets, TITULO_JANELA, AREA_TITULO_MENU,
                                           TAMANHO_TITULO_MENU, PALETA_DESTAQUE);
    } else {
        assets_desenhar_imagem(&jogo->assets, IMG_TITULO, AREA_TITULO_MENU, WHITE);
    }

    desenhar_botoes(jogo, BOTAO_JOGAR, BOTAO_SAIR, TAMANHO_BOTAO);

    assets_desenhar_texto(&jogo->assets, TEXTO_VERSAO, posicao_versao, TAMANHO_VERSAO_MENU, PALETA_TEXTO);
}

void tela_carregando_atualizar(Jogo *jogo, float dt)
{
    Status status;

    (void)dt;
    if (jogo == NULL || jogo->tempo_na_tela < DURACAO_CARREGANDO) {
        return;
    }
    switch (jogo->acao_apos_carregar) {
    case ACAO_NOVA_PARTIDA:      /* sorteio, botões e CENA_INTRO */
        status = jogo_nova_partida(jogo);
        break;
    case ACAO_CONTINUAR_SAVE:    /* partida a partir do boss salvo */
        status = jogo_continuar_save(jogo);
        break;
    default:
        jogo_trocar_tela(jogo, TELA_MENU);
        return;
    }
    if (status != STATUS_OK) {
        jogo_registrar_erro(jogo, status, "carregar partida");
        jogo_trocar_tela(jogo, TELA_MENU);
    }
}

void tela_carregando_desenhar(const Jogo *jogo)
{
    Rectangle area_texto = { 0.0f, TOPO_TEXTO_CARREGANDO, (float)LARGURA_JANELA, 50.0f };
    Rectangle area_barra = { ((float)LARGURA_JANELA - LARGURA_BARRA_CARREGANDO) / 2.0f, TOPO_BARRA_CARREGANDO,
                             LARGURA_BARRA_CARREGANDO, ALTURA_BARRA_CARREGANDO };

    if (jogo == NULL) {
        return;
    }
    assets_desenhar_fundo(&jogo->assets, IMG_FUNDO_MENU, BOSS_QUANTIDADE);
    DrawRectangle(0, 0, LARGURA_JANELA, ALTURA_JANELA, Fade(BLACK, 0.5f));
    assets_desenhar_texto_centralizado(&jogo->assets, "Carregando...", area_texto,
                                       TAMANHO_TEXTO_CARREGANDO, PALETA_TEXTO);
    tela_desenhar_barra_vida(area_barra, jogo->tempo_na_tela / DURACAO_CARREGANDO, PALETA_ESMERALDA);
}

/* ---- 3. Tela de diálogo ---- */

/**
 * @brief Save automático: quando começa uma cena que grava (historia_boss_da_cena_salva), guarda a
 *        cena, o saldo e a ajuda usada. Cada cena grava uma vez (jogo->cena_salva).
 */
static void gravar_save_da_cena(Jogo *jogo, int cena)
{
    Save save;
    Status status;

    if (cena == FALA_SEM_CENA || cena == jogo->cena_salva
        || historia_boss_da_cena_salva((CenaId)cena) == BOSS_QUANTIDADE) {
        return;
    }
    save.cena = (CenaId)cena;
    save.esmeraldas = partida_saldo(&jogo->partida);
    save.eliminacao_usada = jogo->partida.eliminacao_usada;
    status = save_gravar(CAMINHO_SAVE, &save);
    if (status != STATUS_OK) {
        jogo_registrar_erro(jogo, status, "salvar jogo");
        return;
    }
    jogo->cena_salva = cena;
}

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
        gravar_save_da_cena(jogo, fala_nova->cena);
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

/* ---- 4. Tela de pergunta ---- */
/* Esquerda: enunciado (e a dica comprada), alternativas A-D e ajudas. */
/* Direita: esmeraldas (canto superior), boss e barra de vida. */

void tela_pergunta_atualizar(Jogo *jogo, float dt)
{
    Status status = STATUS_OK;
    int posicao;

    (void)dt;
    if (jogo == NULL) {
        return;
    }

    for (posicao = 0; posicao < NUM_ALTERNATIVAS; posicao++) {
        if (jogo_botao_acionado(jogo, &jogo->botoes[BOTAO_ALTERNATIVA_A + posicao])) {
            status = partida_responder(&jogo->partida, posicao);
            if (status == STATUS_OK) {
                const Boss *boss = partida_boss_atual(&jogo->partida);

                /* Respondeu: som de acerto/erro (menos no Herobrine) e a reação do apresentador;
                 * a revelação vem depois */
                if (boss != NULL && boss->id != BOSS_HEROBRINE) {
                    assets_tocar_som(&jogo->assets, jogo->partida.ultima_acertou ? SOM_ACERTO : SOM_ERRO);
                }
                jogo_tocar_reacao(jogo);
                return;  /* a tela mudou: não lê os outros botões neste quadro */
            }
        }
    }
    if (jogo_botao_acionado(jogo, &jogo->botoes[BOTAO_DICA])) {
        status = partida_comprar_dica(&jogo->partida);
        jogo_preparar_botoes_pergunta(jogo);
    }
    if (jogo_botao_acionado(jogo, &jogo->botoes[BOTAO_ELIMINAR])) {
        status = partida_eliminar_duas(&jogo->partida);
        jogo_preparar_botoes_pergunta(jogo);
    }
    if (jogo_botao_acionado(jogo, &jogo->botoes[BOTAO_PARAR])) {
        status = partida_parar(&jogo->partida);
        if (status == STATUS_OK) {
            /* Levou o prêmio: a partida acabou e o save não vale mais */
            if (save_apagar(CAMINHO_SAVE) != STATUS_OK) {
                jogo_registrar_erro(jogo, STATUS_ERRO_ARQUIVO, "apagar save");
            }
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
    desenhar_botoes(jogo, BOTAO_ALTERNATIVA_A, BOTAO_ALTERNATIVA_D, TAMANHO_BOTAO);
    desenhar_botoes(jogo, BOTAO_PARAR, BOTAO_ELIMINAR, TAMANHO_BOTAO_AJUDA);

    /* Coluna da direita */
    tela_desenhar_hud(jogo);
    tela_desenhar_boss(jogo, AREA_BOSS);
}

/* ---- 5. Tela de revelação (depois da reação do apresentador) ---- */
/* Sem pergunta nem alternativas: só o boss, a vida e o resultado. */

void tela_revelacao_atualizar(Jogo *jogo, float dt)
{
    const Boss *proximo_boss;
    Status status;

    (void)dt;
    if (jogo == NULL) {
        return;
    }

    /* Ao entrar: som de boss derrotado se foi o golpe final, senão de dano se acertou
     * (a barra cai em tela_desenhar_boss) */
    if (!jogo->efeito_da_tela_tocado) {
        if (jogo->partida.boss_derrotado_agora) {
            assets_tocar_som(&jogo->assets, SOM_BOSS_DERROTADO);
        } else if (jogo->partida.ultima_acertou) {
            assets_tocar_som(&jogo->assets, SOM_DANO_BOSS);
        }
        botao_definir_rotulo(&jogo->botoes[BOTAO_CONTINUAR], "Continuar (ENTER)");
        jogo->efeito_da_tela_tocado = true;
    }

    if (!jogo_botao_acionado(jogo, &jogo->botoes[BOTAO_CONTINUAR])) {
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
    case ESTADO_FIM_VITORIA:     /* acertou a 15ª: o último boss caiu e o save não serve mais */
        if (save_apagar(CAMINHO_SAVE) != STATUS_OK) {
            jogo_registrar_erro(jogo, STATUS_ERRO_ARQUIVO, "apagar save");
        }
        jogo_tocar_cena(jogo, CENA_HISTORIA_FIM, TELA_RESULTADO);
        break;
    default:                     /* acertou e a partida continua */
        jogo_preparar_botoes_pergunta(jogo);
        proximo_boss = partida_boss_atual(&jogo->partida);
        if (jogo->partida.boss_derrotado_agora && proximo_boss != NULL) {
            /* Boss caiu: história da transição e entrada do boss do próximo nível.
             * A primeira cena começa o diálogo (tocar); a entrada do boss vem anexada. */
            CenaId historia = historia_cena_antes_do_boss(proximo_boss->id);

            if (historia != CENA_QUANTIDADE) {
                jogo_tocar_cena(jogo, historia, TELA_PERGUNTA);
                jogo_anexar_cena(jogo, historia_cena_do_boss(proximo_boss->id));
            } else {
                jogo_tocar_cena(jogo, historia_cena_do_boss(proximo_boss->id), TELA_PERGUNTA);
            }
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

    botao_desenhar(&jogo->botoes[BOTAO_CONTINUAR], &jogo->assets, TAMANHO_BOTAO_AJUDA);
}

/* ---- 6. Tela de resultado (fim da partida: derrota, parada ou vitória) ---- */

void tela_resultado_atualizar(Jogo *jogo, float dt)
{
    bool venceu;

    (void)dt;
    if (jogo == NULL) {
        return;
    }
    venceu = jogo->partida.estado == ESTADO_FIM_VITORIA;
    if (!jogo->efeito_da_tela_tocado) {
        botao_definir_rotulo(&jogo->botoes[BOTAO_CONTINUAR], venceu ? "Créditos (ENTER)" : "Menu (ENTER)");
        jogo->efeito_da_tela_tocado = true;
    }

    /* Vitória: créditos (que depois voltam ao menu). Derrota ou parada: menu, pelo carregamento. */
    if (jogo_botao_acionado(jogo, &jogo->botoes[BOTAO_CONTINUAR])) {
        if (venceu) {
            jogo_trocar_tela(jogo, TELA_CREDITOS);
        } else {
            jogo->acao_apos_carregar = ACAO_IR_AO_MENU;
            jogo_trocar_tela(jogo, TELA_CARREGANDO);
        }
    }
}

void tela_resultado_desenhar(const Jogo *jogo)
{
    const Partida *partida;
    const char *mensagem_resultado;
    const char *titulo;
    Rectangle janela_titulo = { 0.0f, TOPO_TITULO_RESULTADO, (float)LARGURA_JANELA, 70.0f };
    Rectangle retrato = { ((float)LARGURA_JANELA - LADO_RETRATO_RESULTADO) / 2.0f, TOPO_RETRATO_RESULTADO,
                          LADO_RETRATO_RESULTADO, LADO_RETRATO_RESULTADO };
    Rectangle linha_obrigado = { 0.0f, TOPO_AGRADECIMENTO, (float)LARGURA_JANELA, 40.0f };
    Rectangle linha_premio = { 0.0f, TOPO_PREMIO_RESULTADO, (float)LARGURA_JANELA, 56.0f };
    Rectangle linha_detalhe1 = { 0.0f, TOPO_DETALHE1_RESULTADO, (float)LARGURA_JANELA, 32.0f };
    Rectangle linha_detalhe2 = { 0.0f, TOPO_DETALHE2_RESULTADO, (float)LARGURA_JANELA, 32.0f };
    char texto[96];
    ImagemId fundo, icone;

    if (jogo == NULL) {
        return;
    }
    partida = &jogo->partida;

    switch (partida->estado) {
    case ESTADO_FIM_VITORIA:
        titulo = "Vitória!";
        icone = IMG_ICONE_ALDEAO;
        mensagem_resultado = "\"O Villager hipotético sente orgulho!\"";
        fundo = IMG_FUNDO_VITORIA;
        break;

    case ESTADO_FIM_PARADA:
        titulo = "Você parou";
        icone = IMG_ICONE_ALDEAO_ASSUSTADO;
        mensagem_resultado = "Vai me deixar lutar sozinho?";
        fundo = IMG_FUNDO_RESULTADO;
        break;

    default:
        titulo = "Fim de jogo";
        icone = IMG_ICONE_DESCONHECIDO_2;
        mensagem_resultado = "A maior virtude humana é a determinação.";
        fundo = IMG_FUNDO_DERROTA;
        break;
    }

    /* Mensagem Resultado */
    assets_desenhar_fundo(&jogo->assets, fundo, BOSS_QUANTIDADE);
    assets_desenhar_texto_centralizado(&jogo->assets, mensagem_resultado, linha_obrigado, 28.0f, PALETA_TEXTO);

    assets_desenhar_imagem(&jogo->assets, icone, retrato, WHITE);
    assets_desenhar_texto_centralizado(&jogo->assets, titulo, janela_titulo, 56.0f, PALETA_DESTAQUE);



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

    botao_desenhar(&jogo->botoes[BOTAO_CONTINUAR], &jogo->assets, TAMANHO_BOTAO_AJUDA);
}

/* ---- 7. Créditos ---- */

/** Aviso no rodapé dos créditos (as teclas são as de jogo_jogador_quer_avancar). */
#define AVISO_CREDITOS "ESPAÇO ou clique para pular"

/** @brief Quanto o texto sobe no total: até a última linha chegar ao centro da tela, onde para. */
static float subida_total_creditos(void)
{
    return (float)ALTURA_JANELA / 2.0f + (float)(creditos_quantidade_linhas() - 1) * ESPACO_LINHA_CREDITOS;
}

void tela_creditos_atualizar(Jogo *jogo, float dt)
{
    (void)dt;
    if (jogo == NULL) {
        return;
    }
    if (!jogo->efeito_da_tela_tocado) {
        assets_trocar_musica(&jogo->assets, MUSICA_CREDITOS);   /* o menu volta à principal ao entrar */
        jogo->efeito_da_tela_tocado = true;
    }
    /* Só sai quando o jogador pede. O atraso evita que o clique que abriu os créditos já os feche. */
    if (jogo->tempo_na_tela >= ATRASO_ENTRADA_TELA && jogo_jogador_quer_avancar()) {
        assets_tocar_som(&jogo->assets, SOM_CLIQUE);
        jogo_trocar_tela(jogo, TELA_MENU);
    }
}

void tela_creditos_desenhar(const Jogo *jogo)
{
    float deslocamento;
    Vector2 posicao_aviso;
    int indice;

    if (jogo == NULL) {
        return;
    }
    assets_desenhar_fundo(&jogo->assets, IMG_FUNDO_MENU, BOSS_QUANTIDADE);
    DrawRectangle(0, 0, LARGURA_JANELA, ALTURA_JANELA, Fade(BLACK, 0.6f));

    /* Quanto o texto já subiu; para na subida total e a frase final fica parada no centro */
    deslocamento = fminf(VELOCIDADE_CREDITOS * jogo->tempo_na_tela, subida_total_creditos());
    for (indice = 0; indice < creditos_quantidade_linhas(); indice++) {
        const char *texto = creditos_linha(indice);
        /* A 1ª linha nasce logo abaixo da tela e todas sobem juntas */
        float y = (float)ALTURA_JANELA + (float)indice * ESPACO_LINHA_CREDITOS - deslocamento;
        float tamanho = TAMANHO_TEXTO_CREDITOS;
        Color cor = PALETA_TEXTO;
        Rectangle area;

        if (texto[0] == '\0' || y < -ESPACO_LINHA_CREDITOS || y > (float)ALTURA_JANELA) {
            continue;   /* linha vazia ou fora da tela */
        }
        if (texto[0] == '#') {
            tamanho = TAMANHO_TITULO_CREDITOS;
            cor = PALETA_DESTAQUE;
            texto++;
        } else if (texto[0] == '~') {
            tamanho = TAMANHO_SUBTITULO_CREDITOS;
            cor = PALETA_TEXTO_SUAVE;
            texto++;
        }
        area = (Rectangle){ 0.0f, y, (float)LARGURA_JANELA, ESPACO_LINHA_CREDITOS };
        assets_desenhar_texto_centralizado(&jogo->assets, texto, area, tamanho, cor);
    }

    /* Aviso fixo no canto inferior direito (o canto oposto ao da versão no menu) */
    posicao_aviso.x = (float)LARGURA_JANELA - MARGEM_AVISO_CREDITOS
                      - assets_largura_texto(&jogo->assets, AVISO_CREDITOS, TAMANHO_AVISO_CREDITOS);
    posicao_aviso.y = (float)ALTURA_JANELA - MARGEM_AVISO_CREDITOS - TAMANHO_AVISO_CREDITOS;
    assets_desenhar_texto(&jogo->assets, AVISO_CREDITOS, posicao_aviso, TAMANHO_AVISO_CREDITOS, PALETA_TEXTO_SUAVE);
}

#endif /* USE_RAYLIB */
