/**
 * @file jogo.c
 * @brief Laço principal visual (ver jogo.h). Compila só com USE_RAYLIB.
 *
 * Organização: 1) layout dos botões, 2) funções internas, 3) ciclo de vida
 * (executar/iniciar/encerrar), 4) laço por quadro, 5) telas, cenas e botões.
 */
#ifdef USE_RAYLIB
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "jogo.h"
#include "tela.h"
#include "utils.h"

/* ========================================================================= */
/* 1. Layout dos botões (em pixels, janela LARGURA_JANELA x ALTURA_JANELA)   */
/*    Para mudar a posição dos botões, altere só estes valores.              */
/* ========================================================================= */

#define MARGEM_ESQUERDA        30.0f   /**< Começo da coluna do jogo. */
#define LARGURA_COLUNA_JOGO   920.0f   /**< Largura da coluna do jogo (a escada fica à direita). */
#define ESPACO_ENTRE_BOTOES    20.0f   /**< Espaço entre botões vizinhos. */

#define TOPO_ALTERNATIVAS     435.0f   /**< y da primeira linha de alternativas. */
#define ALTURA_ALTERNATIVA     90.0f   /**< Altura de cada alternativa. */

#define TOPO_AJUDAS           645.0f   /**< y da linha PARAR / DICA / ELIMINAR. */
#define ALTURA_AJUDA           50.0f   /**< Altura dos botões de ajuda. */

/* ========================================================================= */
/* 2. Funções internas                                                       */
/* ========================================================================= */

/** @brief Retângulo da alternativa `posicao` numa grade 2x2 (A B / C D). */
static Rectangle area_da_alternativa(int posicao)
{
    Rectangle area;
    float largura = (LARGURA_COLUNA_JOGO - ESPACO_ENTRE_BOTOES) / 2.0f;

    area.x = MARGEM_ESQUERDA + (float)(posicao % 2) * (largura + ESPACO_ENTRE_BOTOES);
    area.y = TOPO_ALTERNATIVAS + (float)(posicao / 2) * (ALTURA_ALTERNATIVA + ESPACO_ENTRE_BOTOES / 2.0f);
    area.width = largura;
    area.height = ALTURA_ALTERNATIVA;
    return area;
}

/** @brief Retângulo do botão `indice` (0, 1 ou 2) da linha de ajudas. */
static Rectangle area_da_ajuda(int indice)
{
    Rectangle area;
    float largura = (LARGURA_COLUNA_JOGO - 2.0f * ESPACO_ENTRE_BOTOES) / 3.0f;

    area.x = MARGEM_ESQUERDA + (float)indice * (largura + ESPACO_ENTRE_BOTOES);
    area.y = TOPO_AJUDAS;
    area.width = largura;
    area.height = ALTURA_AJUDA;
    return area;
}

/** @brief Cria todos os botões nas posições do layout. */
static Status criar_botoes(Jogo *jogo)
{
    static const int teclas_alternativas[NUM_ALTERNATIVAS] = { KEY_A, KEY_B, KEY_C, KEY_D };
    Status status = STATUS_OK;
    int posicao;

    for (posicao = 0; posicao < NUM_ALTERNATIVAS && status == STATUS_OK; posicao++) {
        status = botao_criar(&jogo->alternativas[posicao], area_da_alternativa(posicao), "",
                             teclas_alternativas[posicao]);
    }
    if (status == STATUS_OK) {
        status = botao_criar(&jogo->botao_parar, area_da_ajuda(0), "Parar (P)", KEY_P);
    }
    if (status == STATUS_OK) {
        status = botao_criar(&jogo->botao_dica, area_da_ajuda(1), "Dica (H)", KEY_H);
    }
    if (status == STATUS_OK) {
        status = botao_criar(&jogo->botao_eliminar, area_da_ajuda(2), "Eliminar 2 (E)", KEY_E);
    }
    if (status == STATUS_OK) {
        /* Continuar ocupa o lugar do botão do meio (as ajudas não aparecem na revelação) */
        status = botao_criar(&jogo->botao_continuar, area_da_ajuda(1), "Continuar (ENTER)", KEY_ENTER);
    }
    return status;
}

/** @brief Aplica `funcao` a todos os botões do jogo (evita repetir a lista de botões). */
static void para_cada_botao(Jogo *jogo, void (*funcao)(Botao *botao))
{
    int posicao;

    for (posicao = 0; posicao < NUM_ALTERNATIVAS; posicao++) {
        funcao(&jogo->alternativas[posicao]);
    }
    funcao(&jogo->botao_parar);
    funcao(&jogo->botao_dica);
    funcao(&jogo->botao_eliminar);
    funcao(&jogo->botao_continuar);
}

/** @brief Solta um botão (estado normal): usado ao trocar de tela. */
static void soltar_botao(Botao *botao)
{
    botao->estado = BOTAO_NORMAL;
}

/** @brief true se algum botão está com o mouse em cima. */
static bool algum_botao_sob_mouse(const Jogo *jogo)
{
    int posicao;

    for (posicao = 0; posicao < NUM_ALTERNATIVAS; posicao++) {
        if (botao_sob_mouse(&jogo->alternativas[posicao])) {
            return true;
        }
    }
    return botao_sob_mouse(&jogo->botao_parar) || botao_sob_mouse(&jogo->botao_dica)
        || botao_sob_mouse(&jogo->botao_eliminar) || botao_sob_mouse(&jogo->botao_continuar);
}

/** @brief Define o cursor UMA vez por quadro, e só chama a raylib quando ele muda. */
static void atualizar_cursor(Jogo *jogo)
{
    int cursor_desejado = algum_botao_sob_mouse(jogo) ? MOUSE_CURSOR_POINTING_HAND : MOUSE_CURSOR_DEFAULT;

    if (cursor_desejado != jogo->cursor_atual) {
        SetMouseCursor(cursor_desejado);
        jogo->cursor_atual = cursor_desejado;
    }
}

/* ========================================================================= */
/* 3. Ciclo de vida                                                          */
/* ========================================================================= */

Status jogo_executar(const Banco *banco)
{
    Jogo jogo;
    Partida verificacao;
    Status status_inicio;

    if (banco == NULL) {
        return STATUS_ERRO_PARAMETRO;
    }

    /* Confere o banco ANTES de abrir a janela: com perguntas faltando o erro aparece no menu */
    status_inicio = partida_iniciar(&verificacao, banco);
    if (status_inicio != STATUS_OK) {
        return status_inicio;
    }

    /* Janela */
    SetTraceLogLevel(LOG_WARNING);  /* só avisos e erros da raylib no terminal */
    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_MSAA_4X_HINT);
    InitWindow(LARGURA_JANELA, ALTURA_JANELA, TITULO_JANELA);
    if (!IsWindowReady()) {
        return STATUS_ERRO_GRAFICO;
    }
    SetTargetFPS(FPS_ALVO);
    sorteio_semear((unsigned int)time(NULL));

    status_inicio = jogo_iniciar(&jogo, banco);
    if (status_inicio != STATUS_OK && status_inicio != STATUS_ASSET_FALLBACK) {
        jogo_encerrar(&jogo);
        CloseWindow();
        return status_inicio;
    }

    /* Laço principal: ESC ou o X da janela encerram */
    while (!WindowShouldClose() && jogo.tela != TELA_SAIR) {
        jogo_atualizar(&jogo, GetFrameTime());

        BeginDrawing();
        jogo_desenhar(&jogo);
        EndDrawing();
    }

    jogo_encerrar(&jogo);
    CloseWindow();
    return status_inicio;  /* STATUS_OK ou STATUS_ASSET_FALLBACK (o menu mostra o aviso) */
}

Status jogo_iniciar(Jogo *jogo, const Banco *banco)
{
    Status status_assets, status;

    if (jogo == NULL || banco == NULL) {
        return STATUS_ERRO_PARAMETRO;
    }

    memset(jogo, 0, sizeof(*jogo));
    jogo->banco = banco;
    jogo->ultimo_erro = STATUS_OK;
    jogo->cursor_atual = MOUSE_CURSOR_DEFAULT;
    dialogo_limpar(&jogo->dialogo);

    status_assets = assets_carregar(&jogo->assets);
    if (status_assets != STATUS_OK && status_assets != STATUS_ASSET_FALLBACK) {
        return status_assets;
    }

    status = criar_botoes(jogo);
    if (status != STATUS_OK) {
        return status;
    }

    assets_tocar_musica(&jogo->assets);
    status = jogo_nova_partida(jogo);
    if (status != STATUS_OK) {
        return status;
    }
    return status_assets;
}

void jogo_encerrar(Jogo *jogo)
{
    if (jogo == NULL) {
        return;
    }
    SetMouseCursor(MOUSE_CURSOR_DEFAULT);
    assets_descarregar(&jogo->assets);
}

Status jogo_nova_partida(Jogo *jogo)
{
    Status status;

    if (jogo == NULL) {
        return STATUS_ERRO_PARAMETRO;
    }

    status = partida_iniciar(&jogo->partida, jogo->banco);
    if (status != STATUS_OK) {
        return status;
    }
    status = jogo_preparar_botoes_pergunta(jogo);
    if (status != STATUS_OK) {
        return status;
    }

    /* Introdução + entrada do primeiro boss, depois a primeira pergunta */
    jogo_tocar_cena(jogo, CENA_INTRO, TELA_PERGUNTA);
    jogo_anexar_cena(jogo, dialogo_cena_do_boss(partida_boss_atual(&jogo->partida)->id));
    return STATUS_OK;
}

/* ========================================================================= */
/* 4. Laço por quadro                                                        */
/* ========================================================================= */

void jogo_atualizar(Jogo *jogo, float dt)
{
    if (jogo == NULL) {
        return;
    }

    assets_atualizar_musica(&jogo->assets);
    jogo->tempo_na_tela += dt;
    jogo->tempo_total += dt;

    switch (jogo->tela) {
    case TELA_DIALOGO:   tela_dialogo_atualizar(jogo, dt);   break;
    case TELA_PERGUNTA:  tela_pergunta_atualizar(jogo, dt);  break;
    case TELA_REVELACAO: tela_revelacao_atualizar(jogo, dt); break;
    case TELA_RESULTADO: tela_resultado_atualizar(jogo, dt); break;
    default: break;
    }

    atualizar_cursor(jogo);
}

void jogo_desenhar(const Jogo *jogo)
{
    if (jogo == NULL) {
        return;
    }

    ClearBackground(PALETA_FUNDO);
    switch (jogo->tela) {
    case TELA_DIALOGO:   tela_dialogo_desenhar(jogo);   break;
    case TELA_PERGUNTA:  tela_pergunta_desenhar(jogo);  break;
    case TELA_REVELACAO: tela_revelacao_desenhar(jogo); break;
    case TELA_RESULTADO: tela_resultado_desenhar(jogo); break;
    default: break;
    }

    if (jogo->ultimo_erro != STATUS_OK) {
        tela_desenhar_banner_erro(jogo);
    }
}

/* ========================================================================= */
/* 5. Telas, cenas e botões                                                  */
/* ========================================================================= */

void jogo_trocar_tela(Jogo *jogo, TelaJogo nova_tela)
{
    if (jogo == NULL) {
        return;
    }
    jogo->tela = nova_tela;
    jogo->tempo_na_tela = 0.0f;
    jogo->efeito_da_tela_tocado = false;
    para_cada_botao(jogo, soltar_botao);
}

Status jogo_tocar_cena(Jogo *jogo, CenaId cena, TelaJogo depois)
{
    Status status;

    if (jogo == NULL) {
        return STATUS_ERRO_PARAMETRO;
    }

    status = dialogo_carregar_cena(&jogo->dialogo, cena);
    if (status != STATUS_OK) {
        jogo_registrar_erro(jogo, status, "carregar cena");
        jogo_trocar_tela(jogo, depois);  /* pula a cena em vez de travar */
        return status;
    }

    jogo->tela_apos_dialogo = depois;
    jogo_trocar_tela(jogo, TELA_DIALOGO);
    return STATUS_OK;
}

Status jogo_anexar_cena(Jogo *jogo, CenaId cena)
{
    Status status;

    if (jogo == NULL) {
        return STATUS_ERRO_PARAMETRO;
    }
    status = dialogo_anexar_cena(&jogo->dialogo, cena);
    if (status != STATUS_OK) {
        jogo_registrar_erro(jogo, status, "anexar cena");
    }
    return status;
}

Status jogo_preparar_botoes_pergunta(Jogo *jogo)
{
    const PerguntaSorteada *pergunta;
    char rotulo[TAM_ROTULO_BOTAO];
    int posicao;
    Status status;

    if (jogo == NULL) {
        return STATUS_ERRO_PARAMETRO;
    }
    pergunta = partida_pergunta_atual(&jogo->partida);
    if (pergunta == NULL) {
        return STATUS_ERRO_PARAMETRO;
    }

    for (posicao = 0; posicao < NUM_ALTERNATIVAS; posicao++) {
        Botao *botao = &jogo->alternativas[posicao];

        snprintf(rotulo, sizeof(rotulo), "%c) %s", 'A' + posicao,
                 sorteio_texto_alternativa(pergunta, posicao));
        status = botao_definir_rotulo(botao, rotulo);
        if (status != STATUS_OK) {
            return status;
        }
        botao->visivel = true;
        botao->habilitado = !jogo->partida.alternativa_eliminada[posicao];
        botao->destaque = DESTAQUE_NENHUM;
    }

    jogo->botao_parar.habilitado = true;

    /* Dica paga: o rótulo mostra o preço da rodada; desabilitada sem saldo ou já comprada */
    snprintf(rotulo, sizeof(rotulo), "Dica: %d esm. (H)", partida_custo_dica(&jogo->partida));
    status = botao_definir_rotulo(&jogo->botao_dica, rotulo);
    if (status != STATUS_OK) {
        return status;
    }
    jogo->botao_dica.habilitado = partida_pode_comprar_dica(&jogo->partida);
    jogo->botao_eliminar.habilitado = !jogo->partida.eliminacao_usada;
    return STATUS_OK;
}

void jogo_registrar_erro(Jogo *jogo, Status status, const char *contexto)
{
    fprintf(stderr, "Erro no jogo (%s): %s\n", contexto != NULL ? contexto : "?",
            utils_mensagem_status(status));
    if (jogo != NULL) {
        jogo->ultimo_erro = status;
    }
}

bool jogo_botao_acionado(Jogo *jogo, Botao *botao)
{
    bool acionado;

    if (jogo == NULL || botao == NULL) {
        return false;
    }

    acionado = botao_atualizar(botao);  /* sempre atualiza: o hover aparece desde o 1º quadro */
    if (!acionado || jogo->tempo_na_tela < ATRASO_ENTRADA_TELA) {
        return false;
    }

    assets_tocar_som(&jogo->assets, SOM_CLIQUE);
    return true;
}

bool jogo_jogador_quer_avancar(void)
{
    /* Clique conta ao SOLTAR, igual aos botões: o mesmo clique não aciona a próxima tela */
    return IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER) || IsMouseButtonReleased(MOUSE_BUTTON_LEFT);
}

#endif /* USE_RAYLIB */
