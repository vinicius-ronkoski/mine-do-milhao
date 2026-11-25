/**
 * @file layout.h
 * @brief Posições e tamanhos (pixels virtuais de 1280x720) de tudo na janela do jogo.
 *
 * Usado por jogo.c (botões), tela.c (telas) e dialogo.c (caixa de fala). AREA_* = Rectangle.
 * Boss em quadrados de 384 = 6 x 64 e ícone em 32 = 2 x 16: a pixel art fica em escala inteira
 * (ao mudar, mantenha múltiplos de 64 e 16).
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

/* ---- 1. Tela de pergunta: coluna da esquerda ---- */

#define MARGEM_ESQUERDA        30.0f   /**< x da coluna da pergunta. */
#define LARGURA_COLUNA_JOGO   640.0f   /**< Largura da coluna da pergunta. */
#define ESPACO_ENTRE_BOTOES    14.0f   /**< Espaço entre botões vizinhos. */

/** Painel do enunciado e o tamanho máximo do texto (reduz se não couber). */
#define AREA_ENUNCIADO     (Rectangle){ MARGEM_ESQUERDA, 30.0f, LARGURA_COLUNA_JOGO, 200.0f }
#define TAMANHO_ENUNCIADO  34.0f

/** Faixa da dica comprada, embaixo do enunciado. */
#define ALTURA_DICA_ENUNCIADO  64.0f
#define TAMANHO_DICA           22.0f

#define TOPO_ALTERNATIVAS     250.0f   /**< y da alternativa A (B-D embaixo). */
#define ALTURA_ALTERNATIVA     70.0f   /**< Altura de cada alternativa. */
#define TAMANHO_BOTAO          26.0f   /**< Fonte das alternativas. */

#define TOPO_AJUDAS           624.0f   /**< y de Parar/Dica/Eliminar e do Continuar. */
#define ALTURA_AJUDA           56.0f
#define TAMANHO_BOTAO_AJUDA    22.0f   /**< Fonte das ajudas e do Continuar. */

/** Ícone da esmeralda (HUD, preços, revelação). */
#define LADO_ICONE_ESMERALDA   32.0f

/* ---- 2. Tela de pergunta: coluna da direita ---- */

/** HUD: "Pergunta N/15" e saldo de esmeraldas. */
#define AREA_RODADA        (Rectangle){  710.0f,  20.0f, 380.0f,  56.0f }
#define AREA_ESMERALDAS    (Rectangle){ 1110.0f,  20.0f, 150.0f,  56.0f }

/** Bloco do boss (nome, sprite 384x384, barra e "Vida x/y"), centralizado na coluna da direita. */
#define AREA_BOSS          (Rectangle){  793.0f,  80.0f, 384.0f, 493.0f }

/* ---- 3. Tela de revelação (boss no centro e o resultado da resposta) ---- */

#define LADO_BOSS_REVELACAO       384.0f
#define TOPO_BOSS_REVELACAO        20.0f   /**< y do nome do boss. */
#define TOPO_MENSAGEM_REVELACAO   525.0f   /**< y de "Acertou! -1 de vida" / "Errou!". */
#define TOPO_DETALHE_REVELACAO    575.0f   /**< y do saldo de esmeraldas (número + ícone). */

/** Botão Continuar/Menu (centralizado, em TOPO_AJUDAS). */
#define LARGURA_CONTINUAR         320.0f

/* ---- 4. Tela de resultado (fim da partida). Os textos são centralizados. ---- */

#define TOPO_TITULO_RESULTADO      40.0f   /**< "Fim de jogo" / "Você parou" / "Vitória!". */
#define TOPO_RETRATO_RESULTADO    130.0f
#define LADO_RETRATO_RESULTADO    180.0f
#define TOPO_AGRADECIMENTO        330.0f   /**< "Obrigado por jogar...". */
#define TOPO_PREMIO_RESULTADO     390.0f   /**< "Você leva N esmeraldas". */
#define TOPO_DETALHE1_RESULTADO   460.0f   /**< Marco de segurança / até qual pergunta chegou. */
#define TOPO_DETALHE2_RESULTADO   500.0f   /**< Ajudas descontadas. */

/* ---- 5. Diálogo ---- */

#define LADO_BOSS_DIALOGO         384.0f   /**< Boss acima da caixa de fala. */
#define TOPO_BOSS_DIALOGO          80.0f

#define ALTURA_CAIXA_DIALOGO      200.0f   /**< Caixa de fala, encostada embaixo. */
#define MARGEM_CAIXA_DIALOGO       24.0f
#define LADO_RETRATO_DIALOGO      150.0f
#define FONTE_NOME_DIALOGO         26.0f
#define FONTE_TEXTO_DIALOGO        24.0f
#define FONTE_AVISO_DIALOGO        18.0f   /**< "ESPAÇO ou clique para continuar". */

/* ---- 6. Menu principal (no estilo do menu clássico do Minecraft) ---- */

/** Logo no tamanho do titulo.png (918x100), centralizado; o texto de fallback usa TAMANHO_TITULO_MENU. */
#define AREA_TITULO_MENU          (Rectangle){ 181.0f, 110.0f, 918.0f, 100.0f }
#define TAMANHO_TITULO_MENU        64.0f

#define LARGURA_BOTAO_MENU        530.0f   /**< Botões largos; a linha de baixo divide esta largura em dois. */
#define ALTURA_BOTAO_MENU          50.0f
#define TOPO_BOTOES_MENU          308.0f   /**< y de "Novo jogo"; "Carregar jogo" vem PASSO_BOTOES_MENU abaixo. */
#define PASSO_BOTOES_MENU          64.0f   /**< Distância entre as linhas (a 3ª linha fica vazia). */
#define TOPO_LINHA_INFERIOR_MENU  531.0f   /**< y de "Créditos" e "Sair do jogo". */

#define MARGEM_VERSAO_MENU          8.0f   /**< TEXTO_VERSAO no canto inferior esquerdo. */
#define TAMANHO_VERSAO_MENU        22.0f

/* ---- 7. Carregamento ---- */

#define TOPO_TEXTO_CARREGANDO     300.0f
#define TAMANHO_TEXTO_CARREGANDO   34.0f
#define TOPO_BARRA_CARREGANDO     370.0f
#define LARGURA_BARRA_CARREGANDO  400.0f
#define ALTURA_BARRA_CARREGANDO    20.0f

/* ---- 8. Créditos (texto em LINHAS_CREDITOS, tela.c) ---- */

#define VELOCIDADE_CREDITOS        60.0f   /**< Pixels por segundo que o texto sobe. */
#define ESPACO_LINHA_CREDITOS      40.0f   /**< Distância vertical entre duas linhas. */
#define TAMANHO_TITULO_CREDITOS    36.0f   /**< Linhas com '#': títulos, em amarelo. */
#define TAMANHO_SUBTITULO_CREDITOS 22.0f   /**< Linhas com '~': rótulos, em cinza. */
#define TAMANHO_TEXTO_CREDITOS     26.0f   /**< Linhas comuns. */
#define PAUSA_FINAL_CREDITOS        4.0f   /**< Segundos parada na última linha antes de voltar ao menu. */
#define TAMANHO_AVISO_CREDITOS     18.0f   /**< "ESPAÇO ou clique para pular", no canto inferior direito. */
#define MARGEM_AVISO_CREDITOS       8.0f

/* ---- 9. Faixa de erro (rodapé, todas as telas) ---- */

#define ALTURA_BANNER_ERRO         34.0f

#endif /* LAYOUT_H */
