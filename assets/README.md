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

**Para adicionar ou trocar um arquivo:** o id fica no enum (`ImagemId`, `SomId` ou `MusicaId`, em
`include/assets.h`, antes do `*_QUANTIDADE`) e o nome, sem extensão, na tabela de `src/assets.c`
(`nome_arquivo_imagem`, `nome_arquivo_som`, `nome_arquivo_musica`), na mesma ordem do enum.
A fonte é `ARQUIVO_FONTE`, em `include/config.h`.

Use sempre **letras minúsculas** nos nomes: no Windows `Minecraft.ttf` e `minecraft.ttf` são o mesmo
arquivo, mas no Linux não.

## Imagens — `assets/sprites/`

Se faltar: painéis, botão e caixa de fala viram um retângulo colorido; os fundos de tela viram um degradê
na cor da fase (ver abaixo); as demais imagens viram o xadrez magenta ("textura faltando" do Minecraft).
Imagens com 800 px ou mais de largura são suavizadas ao ampliar (fundos); as menores ficam nítidas (pixel art).

| Arquivo | Id no código | Uso | Existe na pasta? |
|---|---|---|---|
| `painel_enunciado` | `IMG_PAINEL_ENUNCIADO` | Moldura atrás do enunciado (fallback: painel cinza) | **falta** |
| `botao` | `IMG_BOTAO` | Skin padrão dos botões; o estado só muda a cor (fallback: retângulo) | **falta** |
| `botao_menu` | `IMG_BOTAO_MENU` | "Novo jogo" e "Carregar jogo", 530x50 (fallback: `botao`) | **falta** |
| `botao_menu_pequeno` | `IMG_BOTAO_MENU_PEQUENO` | "Créditos" e "Sair do jogo", 258x50 (fallback: `botao`) | **falta** |
| `caixa_dialogo` | `IMG_CAIXA_DIALOGO` | Caixa de fala (fallback: painel cinza) | sim |
| `esmeralda` | `IMG_ESMERALDA` | Ícone da esmeralda (HUD e preços das ajudas) | sim |
| `titulo` | `IMG_TITULO` | Logo do menu (fallback: TITULO_JANELA em texto) | sim |
| `fundo_menu` | `IMG_FUNDO_MENU` | Fundo do menu e da tela de carregamento | sim |
| `fundo_pergunta` | `IMG_FUNDO_PERGUNTA` | Fundo da tela de perguntas (1280x720) | **falta** |
| `fundo_dialogo` | `IMG_FUNDO_DIALOGO` | Fundo das cenas de diálogo | **falta** |
| `palco` | `IMG_FUNDO_PALCO` | Imagem do palco (Minecraft) | sim |
| `fundo_portal` | `IMG_FUNDO_PORTAL` | Imagem do portal do Nether | sim |
| `fundo_fim` | `IMG_FUNDO_FIM` | Imagem de fundo do fim | sim |
| `auditorio` | `IMG_FUNDO_AUDITORIO` | Imagem do auditório (Minecraft) | sim |
| `fundo_preto` | `IMG_FUNDO_PRETO` | Fundo Preto | sim |
| `fundo_vitoria` | `IMG_FUNDO_VITORIA` | Fundo com vitória | **falta** |
| `fundo_derrota` | `IMG_FUNDO_DERROTA` | Fundo com derrota | sim |
| `fundo_resultado` | `IMG_FUNDO_RESULTADO` | Fundo da tela final. (Fundo de terra) | sim |
| `fundos/creeper` | `IMG_FUNDO_CREEPER` | Fundo da fase do Creeper (fundos/creeper) | sim |
| `fundos/wither` | `IMG_FUNDO_WITHER` | Fundo da fase do Wither (fundos/wither) | sim |
| `fundos/warden` | `IMG_FUNDO_WARDEN` | Fundo da fase do Warden (fundos/warden) | sim |
| `fundos/ender_dragon` | `IMG_FUNDO_ENDER_DRAGON` | Fundo da fase do Ender Dragon (fundos/ender_dragon) | sim |
| `fundos/herobrine` | `IMG_FUNDO_HEROBRINE` | Fundo da fase do Herobrine (fundos/herobrine) | sim |
| `icone_aldeao00` | `IMG_ICONE_ALDEAO` | Icone Villager | sim |
| `icone_aldeao01` | `IMG_ICONE_ALDEAO_ASSUSTADO` | Icone Villager asustado | sim |
| `icone_desconhecido00` | `IMG_ICONE_DESCONHECIDO_1` | Icone desconhecido (Herobrine) | sim |
| `icone_desconhecido01` | `IMG_ICONE_DESCONHECIDO_2` | Icone desconhecido (Sem interrogação) | sim |
| `retrato_creeper` | `IMG_ICONE_CREEPER` | Icone Creeper | sim |
| `retrato_wither` | `IMG_ICONE_WITHER` | Icone Wither | sim |
| `retrato_warden` | `IMG_ICONE_WARDEN` | Icone Warden (Herobrine) | sim |
| `retrato_ender_dragon` | `IMG_ICONE_ENDER_DRAGON` | Icone Ender Dragon | sim |
| `retrato_steve` | `IMG_ICONE_STEVE` | Icone Steve | sim |
| `retrato_herobrine` | `IMG_ICONE_HEROBRINE` | Icone Herobrine | sim |
| `h1_00` | `IMG_H1_CENA_1` | Frame 1 do Creeper auditorio | sim |
| `h1_01` | `IMG_H1_CENA_2` | Frame 2 do Creeper auditorio | sim |
| `h1_02` | `IMG_H1_CENA_3` | Apresentador olha para o Creeper | sim |
| `h2_00` | `IMG_H2_CENA_1` | Creeper dropa item | sim |
| `h2_01` | `IMG_H2_CENA_2` | Leitura do livro com enigma | sim |
| `h2_02` | `IMG_H2_CENA_3` | Villager olha para o jogador | sim |
| `h2_03` | `IMG_H2_CENA_4` | Portal do nether desativado | sim |
| `h2_04` | `IMG_H2_CENA_5` | Portal do nether ativado | sim |
| `h2_05` | `IMG_H2_CENA_6` | Entram no nether, com portal no fundo | sim |
| `h2_06` | `IMG_H2_CENA_7` | Villager olha para o lado | sim |
| `h2_07` | `IMG_H2_CENA_8` | Villager com Wither sem estar montado | sim |
| `h2_08` | `IMG_H2_CENA_9` | Villager olha pra cima, chamar img nenhuma | sim |
| `h3_00` | `IMG_H3_CENA_1` | Villager aliviado olha para o jogador | sim |
| `h3_01` | `IMG_H3_CENA_2` | Voltam, dentro de caverna, Villager olha para o portal | sim |
| `h3_02` | `IMG_H3_CENA_3` | Villager olha para o jogador na caverna | sim |
| `h3_03` | `IMG_H3_CENA_4` | Sensor de Sculk ativa, chamando o warden | sim |
| `h3_04` | `IMG_H3_CENA_5` | Mesmo fundo do warden | sim |
| `h4_00` | `IMG_H4_CENA_1` | Villager aponta para saída com sua seta mágica | sim |
| `h4_01` | `IMG_H4_CENA_2` | Villager encontra um portal convenientemente colocado ali | sim |
| `h4_02` | `IMG_H4_CENA_3` | Jogador olha para o redomoinho | sim |
| `h4_03` | `IMG_H4_CENA_4` | Villager mostra sua roupa nova e zomba | sim |
| `h4_04` | `IMG_H4_CENA_5` | Barulho do ender dragon | sim |
| `h5_00` | `IMG_H5_CENA_1` | Cena do fim do jogo | sim |
| `h5_01` | `IMG_H5_CENA_2` | Villager e Player no mapa do herobrine | sim |
| `h5_02` | `IMG_H5_CENA_3` | Fica escuro e villager pergunta se está tudo bem | sim |
| `h6_00` | `IMG_H6_CENA_1` | Frame 1 - Villager na paz | sim |
| `h6_01` | `IMG_H6_CENA_2` | Frame 2 - Villager na paz | sim |
| `h6_02` | `IMG_H6_CENA_3` | Frame 3 - Villager na paz | sim |
| `h6_03` | `IMG_H6_CENA_4` | Frame 4 - Villager na paz | sim |
| `h6_04` | `IMG_H6_CENA_5` | De volta ao palco, olhar baixo | sim |
| `h6_05` | `IMG_H6_CENA_6` | No palco, anunciando do vencedor | sim |
| `h6_06` | `IMG_H6_CENA_7` | Frame 1 - Steve pulando | sim |
| `h6_07` | `IMG_H6_CENA_8` | Frame 2 - Steve pulando | sim |
| `h6_08` | `IMG_H6_CENA_9` | Frame 1 - Steve pulando com esmeralda | sim |
| `h6_09` | `IMG_H6_CENA_10` | Frame 2 - Steve pulando com esmeralda | sim |

### Fundo de cada fase

Os fundos `fundos/<boss>` (`IMG_FUNDO_CREEPER` ... `IMG_FUNDO_HEROBRINE`) são escolhidos sozinhos pela fase
em `assets_desenhar_fundo` e também podem ser usados em `CENARIO(...)` nas cenas da história. Se o de
uma fase faltar, a tela usa o fundo padrão dela (`fundo_pergunta`, `fundo_dialogo`); se esse também faltar,
um degradê escuro na cor do boss (`cor_tema_boss()`, em `src/assets.c`).

## Frames dos bosses — `assets/sprites/frames/`

Sequência numerada a partir de `00`, até 8 quadros (`creeper_00`, `creeper_01`, ...; prefixos `creeper`,
`wither`, `warden`, `ender_dragon`, `herobrine`, em `prefixo_do_boss`). Um único `_00` = imagem parada.
Cada quadro fica 0,15 s na tela.

Tamanho recomendado: **64x64** (pixel art), fundo transparente, todos os quadros do mesmo tamanho. O boss
é desenhado em escala inteira (6x na janela, 9x em tela cheia 1920x1080), então cada pixel da arte vira um
bloco do mesmo tamanho. Imagem não quadrada aparece inteira, centralizada, sem deformar.
Se nenhum quadro existir, aparece o xadrez magenta.

## Sons — `assets/sounds/`

Tocados uma vez, por `assets_tocar_som` ou por `SOM(...)` numa cena. Se faltar: silêncio.

| Arquivo | Id no código | Uso | Existe na pasta? |
|---|---|---|---|
| `clique` | `SOM_CLIQUE` | Clique em botão | sim |
| `acerto` | `SOM_ACERTO` | Resposta certa | sim |
| `erro` | `SOM_ERRO` | Resposta errada | sim |
| `dano` | `SOM_DANO_BOSS` | Boss perdeu vida | sim |
| `boss_derrotado` | `SOM_BOSS_DERROTADO` | Boss caiu | sim |
| `vitoria` | `SOM_VITORIA` | Prêmio máximo | **falta** |
| `plateia` | `SOM_PLATEIA` | Som de entrada da plateia | sim |
| `entrada_creeper` | `SOM_CREEPER` | Som do creeper | sim |
| `portal` | `SOM_PORTAL` | Som do portal do Nether | sim |
| `aldeao` | `SOM_ALDEAO` | Som de aldeão | sim |
| `sculk` | `SOM_SCULK` | Som de Sculk | sim |
| `ender_dragon_00` | `SOM_ENDER_DRAGON_VOO` | Som do Ender Dragon Voando | sim |
| `ender_dragon_01` | `SOM_ENDER_DRAGON_IDLE` | Som do ender dragon | sim |
| `flint_and_steel` | `SOM_FLINT_AND_STEEL` | Som do Minecraft Movie | sim |
| `the_nether` | `SOM_THE_NETHER` | Som do Minecraft Movie | sim |
| `receba` | `SOM_RECEBA` | Receba (Luva de Pedreiro) | sim |
| `vine_boom` | `SOM_VINE_BOOM` | Som de Vine Boom | sim |

## Músicas — `assets/sounds/`

Toca uma por vez, em loop, até a próxima troca (`MUSICA(...)` numa cena da história). Música faltando:
a troca é ignorada e a que estava tocando continua. `MUSICA_SILENCIO` para a música.

| Arquivo | Id no código | Uso | Existe na pasta? |
|---|---|---|---|
| `musica` | `MUSICA_PRINCIPAL` | "musica": abertura e telas fora das seções | sim |
| `musica_creeper` | `MUSICA_CREEPER` | Seção do Creeper (sem uso: a fase do Creeper usa a principal) | **falta** |
| `musica_wither` | `MUSICA_WITHER` |  | sim |
| `musica_warden` | `MUSICA_WARDEN` |  | sim |
| `musica_end` | `MUSICA_END` | C418 original The End, placeholder | sim |
| `musica_moog_city` | `MUSICA_MOOG_CITY` | C418 original Moog City, placeholder | sim |
| `musica_herobrine` | `MUSICA_HEROBRINE` |  | sim |
| `musica_equinoxe` | `MUSICA_EQUINOXE` | C418 original Equinoxe, placeholder | sim |
| `musica_pigstep` | `MUSICA_PIGSTEP` | Lena Raine | sim |
| `musica_creditos` | `MUSICA_CREDITOS` | Tela de créditos | sim |

O volume de cada grupo fica em `include/config.h` (`VOLUME_MUSICA`, `VOLUME_EFEITOS`); o clique toca
sozinho em todo botão lido com `jogo_botao_acionado()` e a cada avanço do diálogo.

## Fonte — `assets/fonts/`

`minecraft` (`.ttf` ou `.otf`), carregada com os acentos do português. Se faltar: fonte padrão da raylib.
