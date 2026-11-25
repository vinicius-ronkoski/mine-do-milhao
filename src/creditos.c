/**
 * @file creditos.c
 * @brief Texto dos créditos (ver creditos.h). A rolagem fica em tela.c (seção 7).
 */
#include "creditos.h"

/**
 * Texto dos créditos, uma linha por item, de cima para baixo. Editar os créditos é só mexer aqui
 * (velocidade, espaçamento e tamanhos ficam em layout.h, seção 8).
 * Primeiro caractere: '#' = título (amarelo, maior); '~' = rótulo (cinza, menor); "" = linha vazia.
 * A fonte só tem Latin-1 (assets.c, NUM_CODEPOINTS): nada de travessão (—) ou aspas curvas.
 */
static const char *const LINHAS_CREDITOS[] = {
    "#Créditos",
    "",
    "~Uma realização",
    "#Eixo da Programação",
    "",
    "",
    "#Equipe",
    "Gabriel Yuji Aoki",
    "Paulo Bueno Valadares",
    "Vinícius Ferreira Ronkoski",
    "",
    "",
    "#Composição Artística",
    "~Arte, remixagem e roteiro",
    "Gabriel Yuji Aoki",
    "",
    "",
    "~Agradecimentos especiais",
    "#Ao grande Mestre Muriel,",
    "que nos acompanhou na jornada",
    "e nos deu ensinamentos para que",
    "pudéssemos embarcar nesta aventura.",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "#Agradecimentos a MEG (Muriel Eixo Games)"
    "",
    "~Apresentador, Diretor, Roteiro, Ilustração, Sonoplastia...",
    "Silvio Villager",
    "",
    "Em memória de Silvio Santos, lendário apresentador."
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "#Agradecimentos a terceiros",
    "",
    "",
    "~Texturas, efeitos sonoros e jogo original",
    "Mojang Studios / Microsoft",
    "",
    "",
    "~C418",
    "\"Concrete Halls\"", 
    "\"Moog City\"", 
    "\"Équinoxe\"",
    "\"The End\"", 
    "\"Blind Spots\"",
    "\"Alpha\"",
    "",
    "",
    "~Lena Raine",
    "\"Pigstep\"",
    "",
    "",
    "~Antimo & Welles",
    "\"Wither Storm Theme\"",
    "",
    "",
    "~Equipe do SBT",
    "\"Abertura do Show do Milhão\"",
    "",
    "",
    "~Merlin",
    "\"Eclipse - A Bliss Edit\"",
    "(edição dos Bliss Shaders, de X0nk)",
    "",
    "",
    "~Ramon Santamaria",
    "raylib",
    "",
    "",
    "~Muse Group",
    "Audacity",
    "~Controle de Volume",
    "",
    "",
    "~Image-Line",
    "FL Studio 21",
    "~Produção Musical e Remixagem",
    "",
    "",
    "~Igara Studio S.A.",
    "Aseprite",
    "~Produção Visual e Pixel Art",
    "",
    "",
    "",
    "",
    "~Minecraft é marca da Mojang Studios / Microsoft.",
    "~Projeto acadêmico, sem fins comerciais.",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "\"Se não tivesse a coragem de vencer a si mesmo,",
    "jamais teria chegado tão longe.\"",
    "~- Um Apresentador que nunca existiu",
};

#define QUANTIDADE_LINHAS ((int)(sizeof(LINHAS_CREDITOS) / sizeof(LINHAS_CREDITOS[0])))

int creditos_quantidade_linhas(void)
{
    return QUANTIDADE_LINHAS;
}

const char *creditos_linha(int indice)
{
    if (indice < 0 || indice >= QUANTIDADE_LINHAS) {
        return "";
    }
    return LINHAS_CREDITOS[indice];
}
