/**
 * @file jogo.c
 * @brief Laço principal visual (ver jogo.h). Compila só com USE_RAYLIB.
 *
 * Organização: 1) funções internas, 2) ciclo de vida (executar/iniciar/encerrar),
 * 3) laço por quadro, 4) telas, cenas e botões. As posições dos botões estão em layout.h.
 */
#ifdef USE_RAYLIB
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "jogo.h"
#include "layout.h"
#include "tela.h"
#include "sorteio.h"
#include "utils.h"

/* ---- 1. Funções internas ---- */

/** @brief Retângulo da alternativa `posicao`: uma embaixo da outra (A, B, C, D). */
static Rectangle area_da_alternativa(int posicao)
{
    Rectangle area;

    area.x = MARGEM_ESQUERDA;
    area.y = TOPO_ALTERNATIVAS + (float)posicao * (ALTURA_ALTERNATIVA + ESPACO_ENTRE_BOTOES);
    area.width = LARGURA_COLUNA_JOGO;
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

/**
 * @brief Botões do menu principal, como no menu do Minecraft: duas linhas largas centralizadas,
 *        a 3ª linha vazia e, embaixo, Créditos e Sair lado a lado na mesma largura.
 */
static Status criar_botoes_menu(Jogo *jogo)
{
    float esquerda = ((float)LARGURA_JANELA - LARGURA_BOTAO_MENU) / 2.0f;
    float largura_metade = (LARGURA_BOTAO_MENU - ESPACO_ENTRE_BOTOES) / 2.0f;
    Rectangle area_jogar = { esquerda, TOPO_BOTOES_MENU, LARGURA_BOTAO_MENU, ALTURA_BOTAO_MENU };
    Rectangle area_carregar = { esquerda, TOPO_BOTOES_MENU + PASSO_BOTOES_MENU,
                                LARGURA_BOTAO_MENU, ALTURA_BOTAO_MENU };
    Rectangle area_creditos = { esquerda, TOPO_LINHA_INFERIOR_MENU, largura_metade, ALTURA_BOTAO_MENU };
    Rectangle area_sair = { esquerda + largura_metade + ESPACO_ENTRE_BOTOES, TOPO_LINHA_INFERIOR_MENU,
                            largura_metade, ALTURA_BOTAO_MENU };
    Status status;

    status = botao_criar(&jogo->botoes[BOTAO_JOGAR], area_jogar, "Novo jogo", KEY_ENTER);
    if (status == STATUS_OK) {
        status = botao_criar(&jogo->botoes[BOTAO_CARREGAR], area_carregar, "Carregar jogo", KEY_NULL);
    }
    if (status == STATUS_OK) {
        status = botao_criar(&jogo->botoes[BOTAO_CREDITOS], area_creditos, "Créditos", KEY_NULL);
    }
    if (status == STATUS_OK) {
        status = botao_criar(&jogo->botoes[BOTAO_SAIR], area_sair, "Sair do jogo", KEY_NULL);
    }

    /* Créditos: apagado até existir a tela. "Carregar jogo" é habilitado ao entrar no menu,
     * se houver save válido (tela_menu_atualizar). */
    jogo->botoes[BOTAO_CARREGAR].habilitado = false;
    jogo->botoes[BOTAO_CREDITOS].habilitado = false;
    return status;
}

/** @brief Cria todos os botões nas posições do layout. */
static Status criar_botoes(Jogo *jogo)
{
    static const int teclas_alternativas[NUM_ALTERNATIVAS] = { KEY_A, KEY_B, KEY_C, KEY_D };
    Status status = STATUS_OK;
    int posicao;

    for (posicao = 0; posicao < NUM_ALTERNATIVAS && status == STATUS_OK; posicao++) {
        status = botao_criar(&jogo->botoes[BOTAO_ALTERNATIVA_A + posicao], area_da_alternativa(posicao), "",
                             teclas_alternativas[posicao]);
    }
    if (status == STATUS_OK) {
        status = botao_criar(&jogo->botoes[BOTAO_PARAR], area_da_ajuda(0), "Parar (P)", KEY_P);
    }
    if (status == STATUS_OK) {
        status = botao_criar(&jogo->botoes[BOTAO_DICA], area_da_ajuda(1), "Dica (H)", KEY_H);
    }
    if (status == STATUS_OK) {
        status = botao_criar(&jogo->botoes[BOTAO_ELIMINAR], area_da_ajuda(2), "Eliminar 2 (E)", KEY_E);
    }
    if (status == STATUS_OK) {
        /* Continuar fica centralizado na janela (revelação e resultado não mostram as ajudas) */
        Rectangle area_continuar = { ((float)LARGURA_JANELA - LARGURA_CONTINUAR) / 2.0f, TOPO_AJUDAS,
                                     LARGURA_CONTINUAR, ALTURA_AJUDA };

        status = botao_criar(&jogo->botoes[BOTAO_CONTINUAR], area_continuar, "Continuar (ENTER)", KEY_ENTER);
    }
    if (status == STATUS_OK) {
        status = criar_botoes_menu(jogo);
    }
    return status;
}

/** @brief Solta todos os botões (estado normal): usado ao trocar de tela. */
static void soltar_botoes(Jogo *jogo)
{
    int id;

    for (id = 0; id < BOTAO_QUANTIDADE; id++) {
        jogo->botoes[id].estado = BOTAO_NORMAL;
    }
}

/** @brief true se o mouse está sobre algum botão (só os da tela ativa são atualizados). */
static bool algum_botao_sob_mouse(const Jogo *jogo)
{
    int id;

    for (id = 0; id < BOTAO_QUANTIDADE; id++) {
        if (botao_sob_mouse(&jogo->botoes[id])) {
            return true;
        }
    }
    return false;
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

/* ---- 2. Ciclo de vida ---- */

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

    SetTraceLogLevel(LOG_WARNING);  /* só avisos e erros da raylib no terminal */
    /* FLAG_WINDOW_RESIZABLE: o jogador pode redimensionar a janela; F11 alterna a tela cheia.
     * O layout continua em 1280x720 e escala.c ajusta tudo ao tamanho real. */
    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE);
    InitWindow(LARGURA_JANELA, ALTURA_JANELA, TITULO_JANELA);
    if (!IsWindowReady()) {
        return STATUS_ERRO_GRAFICO;
    }
    SetWindowMinSize(LARGURA_MINIMA_JANELA, ALTURA_MINIMA_JANELA);
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
        escala_comecar_desenho(&jogo.assets.escala);   /* daqui em diante: coordenadas 1280x720 */
        jogo_desenhar(&jogo);
        escala_terminar_desenho();
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

    /* O jogo abre no menu; a partida só é sorteada em "Novo jogo" (tela de carregamento) */
    jogo_trocar_tela(jogo, TELA_MENU);
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

    /* Partida nova começa sem o banner de erro da anterior */
    jogo->ultimo_erro = STATUS_OK;
    jogo->tempo_do_erro = 0.0f;
    jogo->ultima_variante_reacao = -1;  /* 1ª reação da partida pode ser qualquer variação */

    status = partida_iniciar(&jogo->partida, jogo->banco);
    if (status != STATUS_OK) {
        return status;
    }
    status = jogo_preparar_botoes_pergunta(jogo);
    if (status != STATUS_OK) {
        return status;
    }

    /* Introdução -> história antes do 1º boss -> entrada do 1º boss -> pergunta */
    jogo_tocar_cena(jogo, CENA_INTRO, TELA_PERGUNTA);
    jogo_anexar_cena(jogo, CENA_HISTORIA_1);
    jogo_anexar_cena(jogo, historia_cena_do_boss(partida_boss_atual(&jogo->partida)->id));
    return STATUS_OK;
}

Status jogo_continuar_save(Jogo *jogo)
{
    Save save;
    Status status;

    if (jogo == NULL) {
        return STATUS_ERRO_PARAMETRO;
    }
    jogo->ultimo_erro = STATUS_OK;
    jogo->tempo_do_erro = 0.0f;
    jogo->ultima_variante_reacao = -1;

    status = save_ler(CAMINHO_SAVE, &save);
    if (status != STATUS_OK) {
        return status;
    }
    status = partida_iniciar_no_boss(&jogo->partida, jogo->banco, save.boss, save.esmeraldas,
                                     save.eliminacao_usada);
    if (status != STATUS_OK) {
        return status;
    }
    status = jogo_preparar_botoes_pergunta(jogo);
    if (status != STATUS_OK) {
        return status;
    }

    /* Começa onde a luta começa: a entrada do boss (que também troca a música da seção) */
    jogo_tocar_cena(jogo, historia_cena_do_boss(save.boss), TELA_PERGUNTA);
    return STATUS_OK;
}

/* ---- 3. Laço por quadro ---- */

void jogo_atualizar(Jogo *jogo, float dt)
{
    if (jogo == NULL) {
        return;
    }

    /* Tela cheia: F11 alterna entre janela e "janela sem borda do tamanho do monitor".
     * Borderless em vez de ToggleFullscreen(): não troca a resolução do monitor e alterna
     * sem tela preta nem perda do contexto OpenGL, principalmente no Windows. */
    if (IsKeyPressed(TECLA_TELA_CHEIA)) {
        ToggleBorderlessWindowed();
    }
    /* Antes de ler os botões: converte o mouse para as coordenadas 1280x720 */
    escala_atualizar(&jogo->assets.escala, GetScreenWidth(), GetScreenHeight());
    assets_ajustar_fonte(&jogo->assets, jogo->assets.escala.camera.zoom);

    assets_atualizar_musica(&jogo->assets);
    jogo->tempo_na_tela += dt;
    jogo->tempo_total += dt;

    /* O banner de erro some sozinho depois de DURACAO_BANNER_ERRO segundos */
    if (jogo->ultimo_erro != STATUS_OK) {
        jogo->tempo_do_erro -= dt;
        if (jogo->tempo_do_erro <= 0.0f) {
            jogo->ultimo_erro = STATUS_OK;
        }
    }

    switch (jogo->tela) {
    case TELA_MENU:       tela_menu_atualizar(jogo, dt);       break;
    case TELA_CARREGANDO: tela_carregando_atualizar(jogo, dt); break;
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

    /* Fundo só na área do jogo (1280x720 virtuais); as faixas pretas fora dela vêm de escala.c */
    DrawRectangle(0, 0, LARGURA_JANELA, ALTURA_JANELA, PALETA_FUNDO);
    switch (jogo->tela) {
    case TELA_MENU:       tela_menu_desenhar(jogo);       break;
    case TELA_CARREGANDO: tela_carregando_desenhar(jogo); break;
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

/* ---- 4. Telas, cenas e botões ---- */

void jogo_trocar_tela(Jogo *jogo, TelaJogo nova_tela)
{
    if (jogo == NULL) {
        return;
    }
    jogo->tela = nova_tela;
    jogo->tempo_na_tela = 0.0f;
    jogo->efeito_da_tela_tocado = false;
    soltar_botoes(jogo);
}

Status jogo_tocar_cena(Jogo *jogo, CenaId cena, TelaJogo depois)
{
    Status status;

    if (jogo == NULL) {
        return STATUS_ERRO_PARAMETRO;
    }

    status = historia_carregar_cena(&jogo->dialogo, cena);
    if (status != STATUS_OK) {
        jogo_registrar_erro(jogo, status, "carregar cena");
        jogo_trocar_tela(jogo, depois);  /* pula a cena em vez de travar */
        return status;
    }

    jogo->tela_apos_dialogo = depois;
    jogo_trocar_tela(jogo, TELA_DIALOGO);
    return STATUS_OK;
}

Status jogo_tocar_reacao(Jogo *jogo)
{
    const PerguntaSorteada *pergunta;
    const Boss *boss;
    Status status;
    int variante;

    if (jogo == NULL) {
        return STATUS_ERRO_PARAMETRO;
    }
    pergunta = partida_pergunta_atual(&jogo->partida);
    boss = partida_boss_atual(&jogo->partida);
    if (pergunta == NULL || boss == NULL) {
        return STATUS_ERRO_PARAMETRO;
    }

    /* Sorteia a variação da reação sem repetir a anterior: sorteia entre uma a menos e,
     * se cair na anterior ou depois dela, pula uma. Assim as outras têm a mesma chance. */
    if (jogo->ultima_variante_reacao < 0 || REACOES_POR_RESULTADO < 2) {
        variante = sorteio_inteiro(0, REACOES_POR_RESULTADO - 1);
    } else {
        variante = sorteio_inteiro(0, REACOES_POR_RESULTADO - 2);
        if (variante >= jogo->ultima_variante_reacao) {
            variante++;
        }
    }
    jogo->ultima_variante_reacao = variante;

    dialogo_limpar(&jogo->dialogo);
    status = historia_anexar_reacao(&jogo->dialogo, jogo->partida.ultima_acertou,
                                   sorteio_texto_alternativa(pergunta, pergunta->posicao_correta),
                                   boss->id, partida_saldo(&jogo->partida), variante);  /* saldo = o que o HUD mostra */
    if (status != STATUS_OK) {
        jogo_registrar_erro(jogo, status, "reação");
        jogo_trocar_tela(jogo, TELA_REVELACAO);  /* pula a fala em vez de travar */
        return status;
    }
    jogo->tela_apos_dialogo = TELA_REVELACAO;
    jogo_trocar_tela(jogo, TELA_DIALOGO);
    return STATUS_OK;
}

Status jogo_anexar_cena(Jogo *jogo, CenaId cena)
{
    Status status;

    if (jogo == NULL) {
        return STATUS_ERRO_PARAMETRO;
    }
    status = historia_anexar_cena(&jogo->dialogo, cena);
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
        Botao *botao = &jogo->botoes[BOTAO_ALTERNATIVA_A + posicao];

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

    jogo->botoes[BOTAO_PARAR].habilitado = true;

    /* Dica paga: o rótulo mostra o preço da rodada; desabilitada sem saldo ou já comprada */
    snprintf(rotulo, sizeof(rotulo), "Dica (H)  %d", partida_custo_dica(&jogo->partida));
    status = botao_definir_rotulo(&jogo->botoes[BOTAO_DICA], rotulo);
    if (status != STATUS_OK) {
        return status;
    }
    jogo->botoes[BOTAO_DICA].icone = IMG_ESMERALDA;  /* o preço aparece como número + ícone */
    jogo->botoes[BOTAO_DICA].habilitado = partida_pode_comprar_dica(&jogo->partida);
    /* Eliminar duas também é pago: mesmo esquema da dica */
    snprintf(rotulo, sizeof(rotulo), "Eliminar (E)  %d", partida_custo_eliminar(&jogo->partida));
    status = botao_definir_rotulo(&jogo->botoes[BOTAO_ELIMINAR], rotulo);
    if (status != STATUS_OK) {
        return status;
    }
    jogo->botoes[BOTAO_ELIMINAR].icone = IMG_ESMERALDA;
    jogo->botoes[BOTAO_ELIMINAR].habilitado = partida_pode_eliminar(&jogo->partida);
    return STATUS_OK;
}

void jogo_registrar_erro(Jogo *jogo, Status status, const char *contexto)
{
    fprintf(stderr, "Erro no jogo (%s): %s\n", contexto != NULL ? contexto : "?",
            utils_mensagem_status(status));
    if (jogo != NULL) {
        jogo->ultimo_erro = status;
        jogo->tempo_do_erro = DURACAO_BANNER_ERRO;  /* erro repetido reinicia a contagem */
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
