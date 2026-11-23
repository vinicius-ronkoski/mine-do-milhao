/**
 * @file layout.h
 * @brief Posições e tamanhos de tudo que aparece na janela do jogo, num lugar só.
 *
 * Para mudar o visual, altere os valores aqui. Quem usa:
 *   - jogo.c    cria os botões (alternativas, ajudas, Continuar) nestas posições;
 *   - tela.c    desenha cada tela (pergunta, revelação, resultado, HUD, boss);
 *   - dialogo.c desenha a caixa de fala.
 *
 * Unidades: pixels na janela de LARGURA_JANELA x ALTURA_JANELA (1280x720, config.h),
 * com a origem (0, 0) no canto superior esquerdo. TAMANHO_* e FONTE_* são tamanhos de fonte.
 * As AREA_* são (Rectangle){ x, y, largura, altura } e só podem ser usadas onde raylib.h
 * está incluído (todos os arquivos acima incluem).
 *
 * TAMANHO DOS BOSSES: o quadrado onde o boss aparece tem lado 384 = 6 x 64 em todas as telas.
 * Assim um frame de pixel art de 64x64 aparece em escala inteira, com todos os pixels do mesmo
 * tamanho (32x32 e 16x16 também encaixam). Se mudar LADO_BOSS_* ou a largura de AREA_BOSS,
 * mantenha múltiplos de 64.
 *
 * ÍCONE DA ESMERALDA: LADO_ICONE_ESMERALDA = 32, múltiplo de 16 (a arte da esmeralda é 16x16
 * ampliada), para os pixels do ícone também ficarem iguais.
 *
 * Mapa da tela de pergunta:
 *
 *   x=30                   x=670   x=710                       x=1260
 *   +-----------------------+       Pergunta N/15      [esm 12]   y=20
 *   |  enunciado            |              Nome do boss           y=80
 *   |  (dica embaixo, se    |         +------------------+
 *   |   comprada)           |         |                  |
 *   +-----------------------+         |      sprite      |
 *   [ A) .................. ]  y=250  |     384x384      |
 *   [ B) .................. ]         |                  |
 *   [ C) .................. ]         +------------------+
 *   [ D) .................. ]         [===== vida =====]          y=519
 *   [Parar] [Dica 1] [Elim. 2] y=624       Vida x/y
 */
#ifndef LAYOUT_H
#define LAYOUT_H

#include "config.h"

/* ========================================================================= */
/* 1. Tela de pergunta: coluna da esquerda                                   */
/* ========================================================================= */

#define MARGEM_ESQUERDA        30.0f   /**< x da coluna da pergunta. */
#define LARGURA_COLUNA_JOGO   640.0f   /**< Largura da coluna da pergunta (o boss fica à direita). */
#define ESPACO_ENTRE_BOTOES    14.0f   /**< Espaço entre botões vizinhos. */

/** Painel do enunciado e o tamanho máximo do texto (diminui sozinho se não couber). */
#define AREA_ENUNCIADO     (Rectangle){ MARGEM_ESQUERDA, 30.0f, LARGURA_COLUNA_JOGO, 200.0f }
#define TAMANHO_ENUNCIADO  34.0f

/** Dica comprada: faixa na parte de baixo do painel do enunciado (a pergunta encolhe para caber). */
#define ALTURA_DICA_ENUNCIADO  64.0f
#define TAMANHO_DICA           22.0f

#define TOPO_ALTERNATIVAS     250.0f   /**< y da alternativa A (B, C e D ficam embaixo dela). */
#define ALTURA_ALTERNATIVA     70.0f   /**< Altura de cada alternativa. */
#define TAMANHO_BOTAO          26.0f   /**< Fonte das alternativas. */

#define TOPO_AJUDAS           624.0f   /**< y da linha Parar / Dica / Eliminar (e do Continuar). */
#define ALTURA_AJUDA           56.0f   /**< Altura dos botões de ajuda e do Continuar. */
#define TAMANHO_BOTAO_AJUDA    22.0f   /**< Fonte das ajudas e do Continuar/Reiniciar. */

/** Ícone da esmeralda no HUD, nos preços dos botões e na revelação (múltiplo de 16). */
#define LADO_ICONE_ESMERALDA   32.0f

/* ========================================================================= */
/* 2. Tela de pergunta: coluna da direita                                    */
/* ========================================================================= */

/** HUD: "Pergunta N/15" + nível, e o saldo de esmeraldas no canto superior direito. */
#define AREA_RODADA        (Rectangle){  710.0f,  20.0f, 380.0f,  56.0f }
#define AREA_ESMERALDAS    (Rectangle){ 1110.0f,  20.0f, 150.0f,  56.0f }

/** Bloco do boss: nome + sprite quadrado (lado = largura = 384 = 6 x 64) + barra de vida + "Vida x/y".
 *  Centralizado na coluna da direita (x 710 a 1260). */
#define AREA_BOSS          (Rectangle){  793.0f,  80.0f, 384.0f, 493.0f }

/* ========================================================================= */
/* 3. Tela de revelação (boss no centro e o resultado da resposta)           */
/* ========================================================================= */

#define LADO_BOSS_REVELACAO       384.0f   /**< Lado do sprite do boss (centralizado; 6 x 64). */
#define TOPO_BOSS_REVELACAO        20.0f   /**< y do nome do boss. */
#define TOPO_MENSAGEM_REVELACAO   525.0f   /**< y de "Acertou! -1 de vida" / "Errou!". */
#define TOPO_DETALHE_REVELACAO    575.0f   /**< y do saldo de esmeraldas (número + ícone). */

/** Botão Continuar/Reiniciar: centralizado na largura, em TOPO_AJUDAS, altura ALTURA_AJUDA. */
#define LARGURA_CONTINUAR         320.0f

/* ========================================================================= */
/* 4. Tela de resultado (fim da partida). Os textos são centralizados.       */
/* ========================================================================= */

#define TOPO_TITULO_RESULTADO      40.0f   /**< "Fim de jogo" / "Você parou" / "Vitória!". */
#define TOPO_RETRATO_RESULTADO    130.0f
#define LADO_RETRATO_RESULTADO    180.0f
#define TOPO_AGRADECIMENTO        330.0f   /**< "Obrigado por jogar...". */
#define TOPO_PREMIO_RESULTADO     390.0f   /**< "Você leva N esmeraldas". */
#define TOPO_DETALHE1_RESULTADO   460.0f   /**< Marco de segurança / até qual pergunta chegou. */
#define TOPO_DETALHE2_RESULTADO   500.0f   /**< Ajudas descontadas. */

/* ========================================================================= */
/* 5. Diálogo                                                                */
/* ========================================================================= */

#define LADO_BOSS_DIALOGO         384.0f   /**< Boss no centro, acima da caixa de fala (6 x 64). */
#define TOPO_BOSS_DIALOGO          80.0f

#define ALTURA_CAIXA_DIALOGO      200.0f   /**< Caixa de fala, encostada embaixo. */
#define MARGEM_CAIXA_DIALOGO       24.0f
#define LADO_RETRATO_DIALOGO      150.0f   /**< Retrato do apresentador dentro da caixa. */
#define FONTE_NOME_DIALOGO         26.0f
#define FONTE_TEXTO_DIALOGO        24.0f
#define FONTE_AVISO_DIALOGO        18.0f   /**< "ESPAÇO ou clique para continuar". */

/* ========================================================================= */
/* 6. Faixa de erro (rodapé, todas as telas)                                 */
/* ========================================================================= */

#define ALTURA_BANNER_ERRO         34.0f

#endif /* LAYOUT_H */
