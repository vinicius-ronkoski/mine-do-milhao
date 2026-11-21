/** @file tela.c @brief Esqueleto das telas (ver tela.h). Compila só com USE_RAYLIB. */
#ifdef USE_RAYLIB
#include "tela.h"
#include "utils.h"

void tela_dialogo_atualizar(Jogo *jogo, float dt)            { (void)jogo; (void)dt; /* TODO */ }
void tela_dialogo_desenhar(const Jogo *jogo)                 { (void)jogo;           /* TODO */ }
void tela_pergunta_atualizar(Jogo *jogo, float dt)           { (void)jogo; (void)dt; /* TODO */ }
void tela_pergunta_desenhar(const Jogo *jogo)                { (void)jogo;           /* TODO */ }
void tela_revelacao_atualizar(Jogo *jogo, float dt)          { (void)jogo; (void)dt; /* TODO */ }
void tela_revelacao_desenhar(const Jogo *jogo)               { (void)jogo;           /* TODO */ }
void tela_resultado_atualizar(Jogo *jogo, float dt)          { (void)jogo; (void)dt; /* TODO */ }
void tela_resultado_desenhar(const Jogo *jogo)               { (void)jogo;           /* TODO */ }

void tela_desenhar_fundo(const Assets *assets, ImagemId id)
{
    /* TODO: assets_desenhar_imagem(assets, id, {0,0,LARGURA_JANELA,ALTURA_JANELA}, WHITE). */
    (void)assets; (void)id;
}

void tela_desenhar_texto_centralizado(const Assets *assets, const char *texto, Rectangle area, float tamanho, Color cor)
{
    /* TODO: MeasureTextEx + DrawTextEx(assets->fonte, ...). */
    (void)assets; (void)texto; (void)area; (void)tamanho; (void)cor;
}

void tela_desenhar_texto_com_quebra(const Assets *assets, const char *texto, Rectangle area,
                                    float tamanho, Color cor, int limite_caracteres)
{
    /* TODO: acumular palavras até estourar area.width; nova linha += tamanho * 1.2f. */
    (void)assets; (void)texto; (void)area; (void)tamanho; (void)cor; (void)limite_caracteres;
}

void tela_desenhar_hud(const Jogo *jogo)                                   { (void)jogo; /* TODO */ }
void tela_desenhar_boss(const Jogo *jogo, Rectangle area)                  { (void)jogo; (void)area; /* TODO */ }
void tela_desenhar_barra_vida(Rectangle area, float fracao, Color cor)     { (void)area; (void)fracao; (void)cor; /* TODO */ }
void tela_desenhar_escada_premios(const Jogo *jogo, Rectangle area)        { (void)jogo; (void)area; /* TODO */ }
void tela_desenhar_banner_erro(const Jogo *jogo)                           { (void)jogo; /* TODO */ }
#endif /* USE_RAYLIB */
