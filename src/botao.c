/** @file botao.c @brief Esqueleto dos botões (ver botao.h). Compila só com USE_RAYLIB. */
#ifdef USE_RAYLIB
#include <string.h>
#include "botao.h"

Status botao_criar(Botao *botao, Rectangle area, const char *rotulo, int tecla_atalho)
{
    /* TODO: memset; copiar campos; habilitado = visivel = true; botao_definir_rotulo(). */
    (void)botao; (void)area; (void)rotulo; (void)tecla_atalho;
    return STATUS_NAO_IMPLEMENTADO;
}

Status botao_definir_rotulo(Botao *botao, const char *rotulo)
{
    /* TODO: strlen < sizeof botao->rotulo, senão STATUS_ERRO_PARAMETRO; strcpy. */
    (void)botao; (void)rotulo;
    return STATUS_NAO_IMPLEMENTADO;
}

void botao_layout_alternativas(Botao botoes[NUM_ALTERNATIVAS])
{
    /* TODO: usar LARGURA_JANELA/ALTURA_JANELA (config.h): 2 colunas x 2 linhas. */
    (void)botoes;
}

bool botao_atualizar(Botao *botao)
{
    /* TODO: ver passos em botao.h. */
    (void)botao;
    return false;
}

void botao_desenhar(const Botao *botao, const Assets *assets)
{
    /* TODO: ver botao.h. Texto: DrawTextEx(assets->fonte, ...) + MeasureTextEx para centralizar. */
    (void)botao; (void)assets;
}
#endif /* USE_RAYLIB */
