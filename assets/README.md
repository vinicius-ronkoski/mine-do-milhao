# Assets do Mine do Milhão

Coloque os arquivos com **exatamente** estes nomes. A **extensão é livre** dentro dos
formatos aceitos: o jogo procura cada nome com as extensões da lista, nesta ordem.
Qualquer arquivo que faltar (ou que a raylib não reconheça) é trocado por um substituto
e o jogo continua; o terminal mostra um aviso dizendo se o arquivo **não foi encontrado**
ou se existe mas é **inválido**.

| Tipo | Extensões aceitas (ordem de procura) |
|---|---|
| Imagem e frames | `.png`, `.jpg`, `.jpeg`, `.bmp` |
| Som e música | `.wav`, `.ogg`, `.mp3` |
| Fonte | `.ttf`, `.otf` |

Para trocar um nome, edite a tabela correspondente em `src/assets.c`
(`nome_arquivo_imagem`, `nome_arquivo_som`, `prefixo_do_boss`) ou, para fonte e música,
`ARQUIVO_FONTE` e `ARQUIVO_MUSICA` em `include/config.h`. Os nomes nunca levam extensão.

## Imagens — `assets/sprites/`

| Nome | Uso | Se faltar |
|---|---|---|
| `fundo_pergunta` | Fundo da tela de perguntas (1280x720) | degradê escuro |
| `fundo_dialogo` | Fundo das cenas de diálogo | degradê escuro |
| `fundo_resultado` | Fundo da tela final | degradê escuro |
| `painel_enunciado` | Moldura do enunciado | painel cinza |
| `botao` | Skin única dos botões (o estado muda só a cor) | retângulo cinza |
| `caixa_dialogo` | Caixa de fala | painel cinza |
| `esmeralda` | Ícone de esmeralda | xadrez magenta |
| `retrato_aldeao` | Retrato do apresentador | xadrez magenta |

Use sempre **letras minúsculas** nos nomes: no Windows `Minecraft.ttf` e `minecraft.ttf` são o mesmo
arquivo, mas no Linux não — o jogo só acharia o primeiro no Windows.

## Fundo de cada fase (opcional) — `assets/sprites/fundos/`

Um fundo por boss, que troca o cenário sozinho a cada fase (1280x720):

| Fase | Nome |
|---|---|
| Creeper | `creeper` |
| Wither | `wither` |
| Warden | `warden` |
| Ender Dragon | `ender_dragon` |
| Herobrine | `herobrine` |

Se o fundo de uma fase faltar, a tela usa o fundo padrão dela (`fundo_pergunta`, `fundo_dialogo`); se esse
também faltar, um degradê escuro na cor do boss (as cores ficam em `cor_tema_boss()`, em `src/assets.c`).
Fundos de fase ausentes geram uma linha só no terminal ("N de 5 fundos de fase"), sem contar como erro.

## Frames dos bosses — `assets/sprites/frames/`

Sequência numerada a partir de `00`, até 8 quadros. Um único arquivo `_00` = imagem parada.

Tamanho recomendado: **64x64** (pixel art), PNG com fundo transparente, todos os quadros do mesmo
tamanho. O boss aparece num quadrado de 384 pixels (6 x 64, ver `include/layout.h`),
então cada pixel da arte vira um bloco de 6x6, sempre do mesmo tamanho. 32x32 e 16x16 também
encaixam. Imagem que não for quadrada não é esticada: aparece inteira, centralizada, sem deformar.
Cada quadro fica 0,15 s na tela.

| Boss | Nomes |
|---|---|
| Creeper | `creeper_00`, `creeper_01`, ... |
| Wither | `wither_00`, ... |
| Warden | `warden_00`, ... |
| Ender Dragon | `ender_dragon_00`, ... |
| Herobrine | `herobrine_00`, ... |

Se nenhum quadro existir, aparece um xadrez magenta (a "textura faltando" do Minecraft).

## Sons — `assets/sounds/`

| Nome | Quando toca |
|---|---|
| `clique` | Clique em botão |
| `acerto` | Resposta certa |
| `erro` | Resposta errada |
| `dano` | Boss perde vida (tela de revelação, depois da fala do apresentador) |
| `boss_derrotado` | Boss derrotado |
| `vitoria` | Prêmio máximo |
| `entrada_creeper`, `entrada_wither`, `entrada_warden`, `entrada_ender_dragon`, `entrada_herobrine` | Entrada do boss: 1ª pergunta da fase dele (toca uma vez) |
| `musica` | Música de fundo (em loop) |

Se faltar: silêncio. O volume de cada grupo fica em `include/config.h` (`VOLUME_MUSICA`, `VOLUME_EFEITOS`);
o clique toca sozinho em todo botão lido com `jogo_botao_acionado()` e a cada avanço do diálogo.

## Fonte — `assets/fonts/`

`minecraft` (`.ttf` ou `.otf`), carregada com os acentos do português. Se faltar: fonte padrão da raylib.
