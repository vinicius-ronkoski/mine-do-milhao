# Registro de mudanças

Cada mudança feita no projeto, com o arquivo, o que mudou e por quê.
A explicação de como o código funciona e como os arquivos se ligam está em
[`ARQUITETURA.md`](ARQUITETURA.md).

---

## 22/11/2026 (noite) — coluna da pergunta mais estreita, boss maior e preço com ícone

### Pedido
Blocos do enunciado e das alternativas mais estreitos, boss maior, e o preço das ajudas como
número + ícone da esmeralda (em vez de "1 esm.").

### Mudanças
| Arquivo | Mudança | Motivo |
|---|---|---|
| `include/layout.h` | Coluna da pergunta 760 → 640 px; boss 384x384 (6 x 64) na pergunta, na revelação e no diálogo; HUD e boss recentralizados na coluna da direita (x 710 a 1260); revelação reposicionada (nome em y 20, mensagens em 525 e 575); `LADO_ICONE_ESMERALDA` = 32; `AREA_DICA` saiu | Boss maior com escala inteira para 64x64; ícone múltiplo de 16 (a esmeralda é 16x16 ampliada). |
| `src/tela.c` | A dica comprada aparece numa faixa amarela embaixo do enunciado (a pergunta encolhe para caber) | Com o boss em 384 px não sobra espaço para o quadro da dica à direita; perto da pergunta ela também fica mais fácil de ler. |
| `include/botao.h`, `src/botao.c` | Campo `icone` no `Botao` (IMG_NENHUMA = sem ícone); com ícone, texto e ícone ficam numa linha, centralizados, diminuindo a fonte se precisar; desabilitado, o ícone fica cinza | Preço = número + esmeralda. |
| `src/jogo.c` | Rótulos "Dica (H)  N" e "Eliminar (E)  N" com `icone = IMG_ESMERALDA` | "Eliminar 2 (E)" encolhia demais na coluna mais estreita. |
| `src/tela.c` | HUD com o ícone em 32 px; revelação mostra o saldo como número + ícone (`desenhar_numero_com_esmeralda`) | Mesmo padrão em todo lugar. |
| `include/tela.h`, `assets/README.md`, `docs/ARQUITETURA.md` | Atualizados | Coerência. |

### Como foi testado
- Prints das telas (diálogo, pergunta com e sem dica, revelação, resultado) conferidos com os
  frames 64x64 do Creeper e o `esmeralda.png` da equipe.
- Compila sem avisos (`-Werror`) no Linux e no Windows 64, também com `-O2`; `make teste` passa.

---

## 22/11/2026 (noite) — bosses em pixel art 64x64

### Pedido
Poder desenhar os bosses em pixel art de 64x64.

### Mudanças
| Arquivo | Mudança | Motivo |
|---|---|---|
| `include/layout.h` | Quadrados do boss com lado múltiplo de 64: diálogo 260 → 256, pergunta 280 → 256 (`AREA_BOSS` recentralizada em x 917), revelação 300 → 320 | Um frame de 64x64 aparece em escala inteira (4x e 5x): todos os pixels da arte viram blocos do mesmo tamanho. Com escala quebrada (280 ÷ 64 = 4,375) alguns pixels ficariam maiores que outros. |
| `src/assets.c` | `assets_desenhar_boss` usa `desenhar_textura_encaixada()`: o maior tamanho possível sem deformar, centralizado | Antes a imagem era esticada até virar quadrada; o `creeper_00.png` atual (497x922) ficaria achatado. |
| `assets/README.md` | Tamanho recomendado dos frames (64x64, PNG transparente) | Documentação. |

### Como foi testado
- Frame de teste 64x64 (xadrez de 1 pixel) desenhado no diálogo, na pergunta e na revelação: cada pixel
  virou exatamente um bloco 4x4, 4x4 e 5x5, todos iguais e no lugar certo.
- Com o `creeper_00.png` da equipe: aparece inteiro e sem deformar nas três telas.
- Compila sem avisos (`-Werror`) no Linux e no Windows 64, também com `-O2`; `make teste` passa.

---

## 22/11/2026 (noite) — layout num arquivo só (`include/layout.h`)

### Pedido
Separar o layout num arquivo próprio, para mexer no visual sem procurar em vários arquivos.

### Mudanças
| Arquivo | Mudança |
|---|---|
| `include/layout.h` (novo) | Todas as posições e tamanhos: coluna da pergunta, coluna da direita (HUD, boss, dica), revelação, resultado, botão Continuar, caixa de diálogo e faixa de erro; mapa da tela de pergunta no comentário do topo |
| `src/jogo.c` | Saíram as macros de posição dos botões (seção 1 removida, seções renumeradas); inclui `layout.h` |
| `src/tela.c` | Saíram `AREA_*`, `LADO_*`, `ALTURA_BANNER_ERRO`, `TAMANHO_*`; os números soltos das telas de revelação e resultado viraram `TOPO_*`; o tamanho 22 dos botões virou `TAMANHO_BOTAO_AJUDA` |
| `src/dialogo.c` | Saíram as macros da caixa de fala; `LADO_RETRATO` virou `LADO_RETRATO_DIALOGO` (para não confundir com o retrato do resultado) |
| `include/tela.h`, `docs/ARQUITETURA.md` | Apontam para `layout.h` |

Ficaram onde estavam, porque não são layout de tela: `DURACAO_DANO` (animação, `tela.c`),
margens internas do botão (`botao.c`) e constantes de carregamento de assets (`assets.c`).

### Como foi testado
- Cada tela (diálogo, pergunta, pergunta com dica, revelação no meio da animação e no fim,
  resultado, faixa de erro) foi desenhada num estado fixo antes e depois da mudança:
  **os 7 prints são idênticos pixel a pixel**, ou seja, nada mudou na tela.
- Compila sem avisos (`-Werror`) no Linux e no Windows 64, também com `-O2`; `make teste` passa.

---

## 22/11/2026 (noite) — "eliminar duas" passa a custar esmeraldas

### Pedido
Faltava verificar se o jogador tem esmeraldas suficientes para eliminar alternativas.
Até aqui o eliminar era grátis (uma vez por partida), por isso não havia verificação.

### Regra
- Preço por nível: 2, 4, 10, 20, 40 (`CUSTO_ELIMINAR_POR_NIVEL`, o dobro da dica).
- Continua uma vez por partida. Sem saldo: `STATUS_SALDO_INSUFICIENTE`, nada é cobrado nem eliminado.
- O gasto entra em `esmeraldas_gastas` e sai do prêmio final, como a dica.
- Sem comprar dicas, o saldo só alcança o preço a partir da 6ª pergunta (10 esmeraldas).

### Mudanças
| Arquivo | Mudança |
|---|---|
| `include/config.h` | `CUSTO_ELIMINAR_POR_NIVEL` |
| `include/partida.h`, `src/partida.c` | `partida_custo_eliminar`, `partida_pode_eliminar`; `partida_eliminar_duas` confere o saldo e cobra; tabela por nível em `custo_por_nivel()` (usada pela dica e pelo eliminar) |
| `src/jogo.c` | Botão "Eliminar 2: N esm. (E)", desabilitado sem saldo ou depois de usado |
| `src/tela.c` | Resultado: "gastas em ajudas" (antes "em dicas") |
| `tests/teste_partida.c` | Recusa sem saldo (nada eliminado nem cobrado), preço por rodada, cobrança, compras aleatórias de eliminar no teste de partidas: 5968+ verificações, 0 falhas |
| `include/tela.h`, `README.md`, `docs/ARQUITETURA.md` | Regra documentada |

---

## 22/11/2026 (tarde) — reação do apresentador, revelação com dano e tela de resultado

### Pedido
Depois de responder: sem a pergunta na tela, o apresentador comenta (falas diferentes para acerto
e erro); ao fim da fala, som "dano" e o boss perde uma vida se acertou (nada se errou). Errou:
fala de agradecimento, tela de derrota e botão Reiniciar. Marcos de segurança como o PDF pede.

### Decisões (perguntadas à equipe)
- **Errar encerra a partida na hora** (regra do SBT). Como o boss tem uma vida por pergunta da seção,
  "não matar o boss até a última pergunta da seção" só acontece errando, e errar já é derrota.
- **Marco = prêmio garantido** (o que o PDF pede; já existia). Não há save de progresso;
  a tela de derrota mostra quanto o marco garantiu.

### Mudanças
| Arquivo | Mudança | Motivo |
|---|---|---|
| `include/dialogo.h`, `src/dialogo.c` | `dialogo_anexar_reacao()`: 3 variações para acerto e 3 para erro, com estruturas diferentes (citam a resposta certa, o boss ou as esmeraldas; uma de erro tem só uma fala) | Falas montadas na hora com `snprintf`; as tabelas ficam junto das outras falas para a equipe editar. |
| `src/dialogo.c` | `CENA_DERROTA` agora agradece ("Obrigado por jogar...") | A reação já diz que errou e qual era a certa. |
| `include/jogo.h`, `src/jogo.c` | `jogo_tocar_reacao()`: limpa o diálogo, anexa a reação (variação sorteada) e vai para TELA_REVELACAO quando acabar | Mesmo padrão de `jogo_tocar_cena`. |
| `src/jogo.c` | Botão Continuar centralizado (`LARGURA_CONTINUAR`) | Revelação e resultado não têm a coluna da esquerda. |
| `src/assets.c`, `assets/README.md` | `SOM_DANO_BOSS` passa a ler `assets/sounds/dano` | Nome pedido pela equipe (`dano.ogg`). |
| `src/tela.c` | Pergunta: ao responder toca acerto/erro e chama `jogo_tocar_reacao` | A revelação vem depois da fala. |
| `src/tela.c` | Diálogo: durante a reação (ESTADO_REVELANDO) o boss aparece mesmo já derrotado | Senão ele sumiria antes da animação de queda. |
| `src/tela.c` | `tela_desenhar_boss`: na revelação de um acerto a barra desce em `DURACAO_DANO` (0,6 s) e o boss treme e pisca em vermelho | A regra já tirou a vida em `partida_responder`; aqui é só o efeito. |
| `src/tela.c` | `tela_revelacao_*`: boss no centro, mensagem (acertou / derrotado / errou), som `dano`, Continuar com o roteamento (derrota, boss derrotado + próximo boss, vitória, próxima pergunta) | Tela pronta. |
| `src/tela.c` | `tela_resultado_*`: título por estado, retrato e agradecimento, prêmio final, marco garantido, dicas descontadas; "Reiniciar (ENTER)" chama `jogo_nova_partida` | Tela pronta. O aviso provisório foi removido. |
| `include/tela.h`, `docs/ARQUITETURA.md` | Documentação e fluxo atualizados | Coerência. |

### Como foi testado
- Jogo real jogado de ponta a ponta por um "jogador" automático (teclas enviadas à janela):
  derrota na pergunta 7 (depois do marco: leva 10 − 1 de dica = 9), reinício, e duas vitórias
  completas (15 perguntas, 5 bosses, 299 esmeraldas com 1 de dica). Prints de cada tela conferidos.
- As vitórias rodaram com AddressSanitizer/UBSan: nenhum erro.
- Compila sem avisos (`-Werror`) no Linux e no Windows 64, também com `-O2`; `make teste` passa.
- Não testado: som (o ambiente não tem placa de som) e o clique do mouse (o teste usou o teclado).

---

## 22/11/2026 (tarde) — tela de pergunta com o boss à direita e som de entrada

### Pedido
Tela de pergunta no formato do Jogo do Milhão: enunciado e alternativas alinhados à esquerda,
uma embaixo da outra; o boss à direita com a barra de vida embaixo; esmeraldas no canto superior
direito; um som na entrada de cada boss (arquivo colocado pela equipe).

### Mudanças
| Arquivo | Mudança | Motivo |
|---|---|---|
| `src/jogo.c` | Macros de layout e `area_da_alternativa`: alternativas A-D uma embaixo da outra numa coluna de 760 px | O lado direito fica livre para o boss. |
| `include/assets.h`, `src/assets.c` | `SOM_ENTRADA_CREEPER` ... `SOM_ENTRADA_HEROBRINE` (arquivos `entrada_<boss>`) | Som próprio de cada boss; `SOM_ENTRADA_CREEPER + boss->id` segue a ordem do enum `BossId`, como `CENA_BOSS_CREEPER + BossId`. |
| `src/tela.c` | `tela_pergunta_atualizar`: respostas, dica paga, eliminar, parar e o som de entrada (só na 1ª pergunta do boss, vida cheia) | Tela de pergunta pronta. |
| `src/tela.c` | `tela_pergunta_desenhar`: enunciado, botões e quadro da dica; usa as funções já previstas `tela_desenhar_hud` (saldo, "Pergunta N/15", nível) e `tela_desenhar_boss` (nome, sprite, barra e "Vida x/y"), agora implementadas | Idem. A primeira versão criou funções `static` paralelas a essas; corrigido para reaproveitar as do `tela.h`, que a revelação também pode usar. |
| `src/tela.c` | Seções reorganizadas: 3) pergunta (pronta), 4) revelação/resultado (a implementar) | O aviso provisório agora só serve às duas telas que faltam. |
| `include/tela.h`, `assets/README.md` | Layout e passos atualizados; nomes dos sons de entrada | Coerência. |

### Como foi testado
- Jogo aberto direto na tela de pergunta (1ª pergunta e 2ª com dica comprada): print conferido.
- Compila sem avisos (`-Werror`) no Linux e no Windows 64; `make teste`: 5943 verificações, 0 falhas.
- Não testado aqui: clique real nessa tela e o som (o ambiente de teste não tem placa de som).

---

## 22/11/2026 (madrugada) — dica comprada com esmeraldas (minigame e loja cortados)

### Pedido
Desistir do minigame por escopo e fazer a dica ser paga com esmeraldas.

### Regra
- Preço por nível: 1, 2, 5, 10, 20 esmeraldas (`CUSTO_DICA_POR_NIVEL`).
- Saldo = esmeraldas acumuladas − esmeraldas gastas. Na 1ª pergunta o saldo é 0.
- Uma compra por pergunta, em quantas perguntas o saldo permitir.
- O gasto sai do prêmio final em qualquer fim (parar, vencer, errar), sem ficar negativo.
- "Eliminar duas" continua grátis, uma vez por partida.

### Mudanças
| Arquivo | Mudança | Motivo |
|---|---|---|
| `include/config.h` | `CUSTO_DICA_POR_NIVEL { 1, 2, 5, 10, 20 }`; novo `STATUS_SALDO_INSUFICIENTE` | Preço num lugar só; a tela distingue "sem saldo" de "já comprou". |
| `src/utils.c` | Mensagem "Esmeraldas insuficientes." | Novo status. |
| `include/partida.h`, `src/partida.c` | `dica_usada` → `esmeraldas_gastas`; `partida_usar_dica` → `partida_comprar_dica`; novas `partida_saldo`, `partida_custo_dica`, `partida_pode_comprar_dica`; `partida_premio_final` desconta o gasto | `esmeraldas` é sobrescrita pela escada de prêmios a cada acerto, então o gasto precisa de um campo próprio. |
| `src/jogo.c` | Botão da dica mostra o preço ("Dica: 5 esm. (H)") e fica desabilitado sem saldo ou depois da compra | A tela não precisa calcular nada. |
| `include/tela.h`, `include/jogo.h` | Passo a passo e comentários com `partida_comprar_dica` | Coerência. |
| `tests/teste_partida.c` | Compra recusada sem saldo, compra aceita, dica certa, sem compra dupla, preço por rodada, desconto ao parar e ao errar (inclusive ficando em 0), compras aleatórias no teste de partidas | 5943 verificações, 0 falhas (também com ASan/UBSan). |
| `README.md`, `docs/ARQUITETURA.md` | Plano e regras atualizados; minigame e loja marcados como cortados | Coerência entre o que foi feito e o que é apresentado. |

---

## 22/11/2026 (madrugada) — quebra de linha simplificada e mais rápida

### Pedido
Reduzir a complexidade de `montar_linha` (quebra automática de texto) sem mudar o resultado,
acelerar o corte de palavras longas e corrigir o que não compilava no `assets.c`.

### Mudanças
| Arquivo | Mudança | Motivo |
|---|---|---|
| `src/assets.c` | Removida a linha solta `indice_quadro;` em `carregar_animacao_boss` | Não compilava (identificador não declarado). |
| `src/assets.c` | `achar_arquivo`: comentários atualizados (sem achar, devolve só a `base`) | A montagem de `base(.png/.jpg/...)` foi removida pela equipe; os comentários ainda a descreviam. |
| `src/assets.c` | `montar_linha` escreve direto em `linha` e desfaz (`linha[usado] = '\0'`) se a palavra não couber | Elimina o buffer `candidata`, dois `memcpy` e a checagem dupla de capacidade. |
| `src/assets.c` | Novas `pular_espacos`, `contar_letras_ate` e `cortar_palavra` | Cada função faz uma coisa; o laço de espaços não se repete. |
| `src/assets.c` | `cortar_palavra` conta só as letras da palavra e cresce a partir da primeira letra | Antes contava o resto do texto inteiro e diminuía letra por letra: com área estreita e texto longo, segundos por quadro. |

### Como foi testado
- As duas versões rodaram lado a lado com a fonte `minecraft.ttf`, a fonte padrão da raylib e o
  `MeasureTextEx` real: 10 mil textos aleatórios por fonte + 16 casos fixos (vazio, só espaços,
  `\n` seguidos, palavra gigante, acentos), variando largura, tamanho e buffer.
  **909.526 linhas comparadas, 0 diferenças** (texto da linha e início da próxima).
- AddressSanitizer/UBSan: 142 mil linhas, sem acesso fora dos buffers.
- O comparador foi validado estragando a versão nova de propósito: acusou a diferença.
- Diálogo longo, palavra gigante, efeito de digitação e botões com fonte ajustada: prints das duas
  versões idênticos pixel a pixel.
- Velocidade (20 textos de até 2 KB, fonte 24): 1 px de largura 5,9 s → 0,006 s;
  120 px 0,37 s → 0,002 s; 600 px 0,027 s → 0,003 s.
- Compila sem avisos (`-Werror`) no Linux e no Windows 64, também com `-O2`; `make teste` passa.

---

## 22/11/2026 (madrugada) — fundo dinâmico por fase (boss)

### Pedido
Poder trocar o fundo das telas conforme o boss/fase, chamando uma função do `assets`.

### Assets adicionados pela equipe (conferidos)
| Arquivo | Conferido |
|---|---|
| `assets/fonts/Minecraft.ttf` | Fonte Minecraftia 2.0; tem todos os acentos do português e o ç. **Nome com M maiúsculo**: o código procura `minecraft`; no Windows funciona, no Linux não seria encontrada. |
| `assets/sprites/retrato_aldeao.png` | Na verdade é um **JPEG** (290x290) com extensão `.png`. A raylib abre mesmo assim (reconhece pelo conteúdo) e as bibliotecas de Linux e Windows têm suporte a JPEG. |

Também conferidas as edições manuais em `dialogo.c` (falas novas, apresentador "Silvio Aldeão"),
`arquivo.c` (`for (size_t i ...)` no `csv_exportar`) e `assets.c` (xadrez de fallback 16x16): coerentes.
A cena do Ender Dragon tem 7 falas; somada a "boss derrotado" fica em 8, dentro do limite de 16.

### Mudanças
| Arquivo | Mudança | Motivo |
|---|---|---|
| `include/config.h` | `CAMINHO_FUNDOS "assets/sprites/fundos/"` | Pasta dos fundos de fase. |
| `include/assets.h`, `src/assets.c` | Nova `assets_desenhar_fundo(assets, fundo_padrao, boss)`: usa o fundo da fase (`fundos/<boss>`), senão o fundo padrão da tela, senão um degradê na cor do boss | O cenário muda a cada boss com uma chamada, e já funciona antes de existir arte. |
| `src/assets.c` | `Assets.fundos_boss[]` carregados ao iniciar (opcionais: só arquivo corrompido gera aviso; ausentes geram uma linha "N de 5") e liberados ao sair | Fundos de fase não poluem o terminal nem contam como erro. |
| `src/assets.c` | `cor_tema_boss()`: verde (Creeper), cinza (Wither), azul-escuro (Warden), roxo (Ender Dragon), vermelho-escuro (Herobrine) | Clima de cada fase sem arte. |
| `src/assets.c` | `prefixo_frames_boss` → `prefixo_do_boss` | Agora serve para frames e fundos. |
| `include/tela.h`, `src/tela.c` | Nova `tela_boss_da_fase(jogo)`; removida `tela_desenhar_fundo` (a lógica foi para `assets_desenhar_fundo`) | Uma função de fundo só; as telas pegam o boss da rodada com uma chamada. |
| `src/tela.c` | Diálogo e telas provisórias usam o fundo da fase; resultado usa o fundo sem boss | Exemplo do padrão para as telas da equipe. |
| `assets/sprites/fundos/` | Pasta nova (com `.gitkeep`) | Lugar dos fundos de fase. |
| `assets/README.md`, `docs/ARQUITETURA.md` | Seção de fundos de fase; aviso para usar nomes em minúsculas | Documentação. |

### Como usar nas telas
```c
/* Pergunta, revelação, diálogo: cenário da fase atual */
assets_desenhar_fundo(&jogo->assets, IMG_FUNDO_PERGUNTA, tela_boss_da_fase(jogo));

/* Resultado: sem fundo de fase */
assets_desenhar_fundo(&jogo->assets, IMG_FUNDO_RESULTADO, BOSS_QUANTIDADE);
```
Para outros critérios além do boss (por exemplo, um fundo especial nos marcos 5 e 10), o caminho é o
mesmo: uma tabela de imagens em `Assets` carregada em `assets_carregar` e um parâmetro a mais nesta função.

### Verificação
- Linux e Windows 64 bits, com e sem raylib, também com `gcc -O2`: sem avisos. `make teste`: 5885 verificações, 0 falhas.
- Teste visual das 5 fases + resultado: Creeper com `fundos/creeper.png` e Warden com `fundos/warden.jpg` (arte de
  teste, não incluída), os outros três no degradê da cor do boss, resultado no degradê neutro.
- Jogo real com a fonte Minecraftia, o retrato (JPEG) e as falas novas: acentos corretos, fundo da fase no diálogo, saída 0.

---

## 22/11/2026 (madrugada) — música e som de clique

### Arquivos adicionados pela equipe
| Arquivo | Conferido |
|---|---|
| `assets/sounds/musica.ogg` | Ogg Vorbis, estéreo, 44,1 kHz, 5 min 14 s; abre com a raylib do projeto. |
| `assets/sounds/clique.ogg` | Ogg Vorbis, estéreo, 44,1 kHz, 0,6 s; abre com a raylib do projeto. |

Os nomes já batem com a tabela do `assets.c` e a extensão `.ogg` é aceita, então os dois
são carregados sem mudar a tabela.

### Inconsistências encontradas
| Problema | Efeito |
|---|---|
| A música tocava em volume máximo, sem controle | Abafaria falas e efeitos; mudar o volume exigiria mexer no código da raylib. |
| Nenhum lugar tocava `SOM_CLIQUE` | O clique carregava, mas nunca soava. |
| Cada tela teria que lembrar de tocar o clique **e** de ignorar cliques nos primeiros `ATRASO_ENTRADA_TELA` segundos | Fácil de esquecer em uma das telas e ficar diferente das outras. |

### Mudanças
| Arquivo | Mudança | Motivo |
|---|---|---|
| `include/config.h` | Novas `VOLUME_MUSICA` (0.35) e `VOLUME_EFEITOS` (0.80) | Volume ajustável num lugar só. |
| `src/assets.c` | Aplica `VOLUME_EFEITOS` em cada som e `VOLUME_MUSICA` na música ao carregar | Idem. |
| `include/jogo.h`, `src/jogo.c` | Nova `jogo_botao_acionado(jogo, botao)`: atualiza o botão, ignora cliques no início da tela e toca `SOM_CLIQUE` quando ele é acionado | Todo botão do jogo soa igual; regra em um lugar só. As telas usam esta função em vez de `botao_atualizar`. |
| `src/tela.c` | O diálogo toca o clique quando o jogador avança (ESPAÇO, ENTER ou clique) | Retorno sonoro também nas falas. |
| `include/tela.h` | Passos das telas de pergunta, revelação e resultado usam `jogo_botao_acionado` | Documentação igual ao padrão novo. |

### Verificação
- Linux e Windows 64 bits, com e sem raylib, também com `gcc -O2`: sem avisos. `make teste`: 5885 verificações, 0 falhas.
- Jogo executado com AddressSanitizer, com os dois arquivos na pasta, avançando o diálogo por clique e tecla: sem erros, saída 0.
- **Não testado:** ouvir o som. O ambiente de teste não tem placa de som (e o emulador de áudio travou a raylib).
  Testem no Windows: a música deve começar ao abrir o jogo e o clique soar a cada avanço do diálogo.

---

## 22/11/2026 — revisão geral e padronização

### Estado verificado antes das mudanças
A versão da equipe (com as edições manuais de 21/11) já compilava e passava em tudo:
Linux com e sem raylib, Windows 64 bits (compilação cruzada), `make teste` (5885 verificações,
0 falhas, também com AddressSanitizer/UBSan) e o menu de texto.

### Mudanças feitas pela equipe (registradas aqui)
| Arquivo | Mudança |
|---|---|
| `include/utils.h`, `src/utils.c` | `utils_ler_linha` e `utils_ler_inteiro` passaram a devolver `Status` (`STATUS_OK` em vez de 1); `utils_ler_inteiro` virou `utils_ler_int`, com saída única num `do-while`; nova `utils_limpar_buffer` pública; mensagens sem o prefixo "Erro:". |
| `src/menu.c` | As 11 chamadas de leitura passaram a comparar com `STATUS_OK`. |
| `include/config.h` | `ARQUIVO_MUSICA` ao lado de `ARQUIVO_FONTE`. |
| `src/arquivo.c` | `fclose` conferido nos caminhos de erro de `dat_carregar`; `sizeof(x)` com parênteses; comentários. |
| `assets/` | `LEIA-ME.md` renomeado para `README.md`; novo `sounds/clique.ogg`. |

### Problemas encontrados na revisão
| Problema | Efeito |
|---|---|
| `clique.ogg` no disco, mas o código procurava `clique.wav` | O som nunca carregava (o arquivo é válido: Vorbis 44,1 kHz, testado com a raylib do projeto). |
| `ARQUIVO_MUSICA` definida no `config.h` **e** no `assets.c` | Duas fontes da verdade; mudar uma não mudava a outra. |
| `arquivo.c` tratava o `fclose` de erro de três jeitos | Em alguns casos um erro de formato virava erro de arquivo; em outros o `if` devolvia o mesmo valor nos dois caminhos; no `csv_carregar` o retorno nem era conferido. |
| Perguntas de sim/não devolvendo `int` 1/0 | `boss_derrotado`, `partida_eh_marco`, `partida_encerrada`, funções internas de `banco.c` e `menu.c`, enquanto botões, assets e diálogo já usavam `bool`. |
| Parâmetros de uma letra (`p`, `ps`) e `indices_out` em inglês | Contra a regra de nomes legíveis do enunciado. |
| `sizeof x` (54 vezes) misturado com `sizeof(x)` (13) | Dois estilos no mesmo arquivo. |
| Comentários desatualizados | `textos_terminados` dizia "1 ou 0" mas devolve `Status`; `assets.h` citava `assets_imagem_fallback` e `ImagemID`, que não existem; README e ARQUITETURA apontavam para `assets/LEIA-ME.md`. |
| `partida_responder` calculava o ponteiro do boss antes de validar a rodada | Com rodada inválida o índice seria -1 (comportamento indefinido). Só aparecia com `gcc -O2`; gcc mais novos podem acusar. |

### Mudanças
| Arquivo | Mudança | Motivo |
|---|---|---|
| `include/config.h` | Bloco **"Convenções do projeto"** no topo: nomes, quando usar `Status`/`bool`/`int`, `sizeof(x)`, `CAMINHO_*`/`ARQUIVO_*` | Regras escritas num lugar só, para todo código novo seguir o mesmo padrão. |
| `include/config.h` | `#include <stdbool.h>` aqui (removido de `assets.h`, `botao.h`, `dialogo.h`, `jogo.h`, `partida.h`) | `bool` disponível em todo módulo pelo mesmo caminho. |
| `include/config.h` | `ARQUIVO_FONTE` e `ARQUIVO_MUSICA` sem extensão; `STATUS_ERRO_ARQUIVO` documenta que inclui o EOF do teclado | O assets procura as extensões aceitas; o uso do `utils` fica documentado no tipo. |
| `src/assets.c` | Nomes das tabelas sem extensão + `achar_arquivo()`: tenta `.png/.jpg/.jpeg/.bmp` (imagens e frames), `.wav/.ogg/.mp3` (sons e música), `.ttf/.otf` (fonte) | Resolve o `clique.ogg` e qualquer próximo arquivo em outro formato, sem mexer no código. |
| `src/assets.c` | Aviso diz se o arquivo **não foi encontrado** (mostra as extensões tentadas) ou é **inválido** | Diagnóstico direto no terminal. |
| `src/assets.c` | Removida a `ARQUIVO_MUSICA` duplicada; macro `QUANTIDADE_DE(vetor)` | Uma fonte da verdade. |
| `include/assets.h`, `assets/README.md` | Comentários corrigidos; README com tabela de extensões e nomes sem extensão | Documentação igual ao código. |
| `src/arquivo.c` | Nova `fechar_com_erro(arquivo, erro)` em todos os caminhos de erro | Um padrão só: no erro, devolve o erro original; no sucesso, o `fclose` continua conferido. |
| `src/arquivo.c` | Comentário de `textos_terminados` corrigido; removidos "// Não verificada" e comentários soltos | Comentário igual ao código. |
| `boss`, `partida` | `boss_derrotado`, `partida_eh_marco`, `partida_encerrada` devolvem `bool` | Convenção. Quem compara com 1/0 continua funcionando (`true` vale 1). |
| `src/banco.c`, `src/menu.c` | Funções internas de sim/não devolvem `bool` (`caracteres_iguais`, `texto_contem_trecho`, `escolher_resultado`, `buscar_e_escolher`, `ler_campo`, `ler_correta`, `ler_nivel`, `termina_com_csv`) | Convenção. |
| `pergunta`, `sorteio`, `arquivo.c`, `menu.c` | `p` → `pergunta`, `ps` → `sorteada`, `indices_out` → `indices_saida`, `n` → `numero` | Nomes legíveis. |
| todos os `.c` e `tests/` | `sizeof x` → `sizeof(x)` (51 trocas) | Um estilo só (o que a equipe escolheu no `arquivo.c`). |
| `include/utils.h` | Documentação real de `utils_limpar_buffer`, com o aviso de que ela espera uma linha se for chamada sem sobra | Evitar travar o menu por uso errado. |
| `src/partida.c` | `partida_responder` valida a rodada antes de calcular o índice do boss | Corrige o comportamento indefinido apontado pelo `-O2`. |
| `README.md`, `docs/ARQUITETURA.md` | Links para `assets/README.md`; seção de convenções; assets com extensões | Documentação em dia. |

### Verificação depois das mudanças
- `make` e `make USE_RAYLIB=1` no Linux; `make` e `make USE_RAYLIB=1` para Windows 64 bits: sem avisos.
- Todos os `.c` também com `gcc -O2` (Linux e MinGW), nos dois modos: sem avisos.
- `make teste`: 5885 verificações, 0 falhas, também com AddressSanitizer/UBSan.
- Menu: entrada inválida pede de novo; exclusão com "n" cancela e com "s" exclui (80 → 79); exportação cria,
  "n" cancela e "s" sobrescreve.
- Jogo na tela virtual: fundo `.jpg`, Creeper com quadro `_00.png` e `_01.bmp`, retrato `.png` carregados;
  arquivo corrompido avisado como "inválido"; saída com código 0.
- **Não testado:** som tocando (o ambiente de teste não tem placa de som; o `clique.ogg` foi validado
  abrindo com a raylib) e execução no Windows real.

---

## 21/11/2026 (noite, teste com o w64devkit) — objetos velhos de outro compilador

### Problema encontrado
Com o w64devkit (gcc de 64 bits), `make USE_RAYLIB=1` só fez o link e falhou com
`i386 architecture of input file 'build/raylib/arquivo.o' is incompatible with i386:x86-64 output`,
seguido de `undefined reference` para tudo (inclusive `WinMain`).

### Causa
Os `.o` em `build/raylib/` tinham sido gerados pela tentativa anterior com o gcc de **32 bits**.
O `make` decide o que recompilar pelas datas dos arquivos, não pelo compilador: como os `.o` eram
mais novos que os `.c`, ele não recompilou e tentou ligar objetos de 32 bits num programa de 64 bits.
Não era problema da pasta da raylib.

### Mudança
| Arquivo | Mudança | Motivo |
|---|---|---|
| `Makefile` | Pasta de objetos por modo **e por compilador**: `build/<modo>-<saída de gcc -dumpmachine>/` | Trocar de gcc nunca mais reaproveita objetos do outro compilador. |
| `Makefile` | `ALVO_GCC` calculado uma vez, logo após `CC` | Usado na pasta de build e na checagem de 32 bits. |
| `README.md`, `docs/ARQUITETURA.md` | Nomes das pastas de build | Documentação em dia. |

Testado: objetos de 32 bits na pasta + gcc de 64 bits → recompila tudo na pasta nova e gera `milhao.exe` de 64 bits.
`make clean` apaga os objetos de todas as pastas (inclusive o `build/raylib/` antigo).

---

## 21/11/2026 (noite, depois do teste no Windows) — gcc de 32 bits

### Problema encontrado
Depois de um `make clean`, `mingw32-make USE_RAYLIB=1` no Windows falhou no link com centenas de
`undefined reference` (`InitWindow`, `DrawTextEx`...). A biblioteca foi encontrada, mas nenhuma função.

### Causa
- Os objetos gerados pelo gcc instalado são de **32 bits** (`Intel 80386 COFF`), provavelmente o MinGW clássico.
- A `third_party/raylib/lib/windows/libraylib.a` é de **64 bits** (`Intel amd64 COFF`).
- O linker não mistura os dois. Antes não aparecia porque o esqueleto não chamava nenhuma função da raylib.
- Não é erro do código: compilando para Windows com um gcc de 64 bits (MinGW-w64), o link funciona.

### Mudanças
| Arquivo | Mudança | Motivo |
|---|---|---|
| `Makefile` | Com `USE_RAYLIB=1` no Windows, confere `gcc -dumpmachine` e para com uma mensagem clara se o gcc for de 32 bits | Trocar 100 linhas de `undefined reference` por uma instrução do que fazer. |
| `Makefile` | No Windows, detecta se o make usa `cmd` ou `sh` (w64devkit, MSYS2, Git Bash) e escolhe `del`/`mkdir` ou `rm`/`mkdir -p` | O gcc de 64 bits recomendado (w64devkit) roda o make com `sh`, onde `del` e `mkdir build\raylib` não funcionam. |
| `docs/ARQUITETURA.md` | Seção 9: como conferir o gcc e instalar o w64devkit | Passo a passo para quem tem o MinGW de 32 bits. |
| `README.md` | Aviso do gcc de 64 bits | Idem. |

Testado: Linux; Windows 64 bits via `sh` (MinGW cruzado); gcc de 32 bits simulado (mostra a mensagem);
modo texto com gcc de 32 bits continua compilando. Não testado: o `cmd` real do Windows com o w64devkit.

---

## 21/11/2026 (noite) — infraestrutura visual, `.dat` e Windows/Linux

### Verificação feita
- `make` e `make USE_RAYLIB=1` compilam no Linux (gcc) e no Windows (MinGW gcc 13, compilação cruzada), com `-Wall -Wextra -Werror`.
- `make teste`: 5885 verificações, 0 falhas, também com AddressSanitizer e UBSan.
- Jogo executado numa tela virtual (Linux): o menu abre a janela, os fallbacks aparecem, o diálogo avança por tecla e clique, ESC volta ao menu, **o jogo abre uma segunda vez sem problemas**, e o programa sai com código 0. Sem erros de memória (AddressSanitizer).
- **Não testado:** executar o `.exe` no Windows (só compilação e link). Testem `make USE_RAYLIB=1` e `make run` no Windows.

### Arquivo binário: `.bin` → `.dat`
| Arquivo | Mudança | Motivo |
|---|---|---|
| `include/config.h` | `CAMINHO_BIN "data/perguntas.bin"` → `CAMINHO_DAT "data/perguntas.dat"` | Decisão da equipe: o binário passa a ser `.dat`. |
| `include/arquivo.h`, `src/arquivo.c` | `bin_carregar`/`bin_salvar` → `dat_carregar`/`dat_salvar`; comentários atualizados; aviso de falha mostra o caminho | Nome coerente com a extensão. |
| `src/main.c` | Comentário "preserva o .bin" → ".dat" | Coerência. |
| `.gitignore` | `data/perguntas.bin`/`ranking.bin` → `.dat` | O `.dat` é gerado em cada computador. |
| `Makefile` | `make clear_data` agora funciona | Ele já procurava `*.dat`; antes não apagava nada porque o arquivo era `.bin`. Depois de editar o CSV, rode `make clear_data` para o jogo reler o CSV. |
| `README.md` | Árvore e tabela de comandos com `.dat` | Documentação em dia. |

O `data/perguntas.bin` antigo na pasta de vocês não é mais lido e pode ser apagado.

### Makefile (Windows e Linux)
| Mudança | Motivo |
|---|---|
| Bibliotecas na ordem `-lraylib ... -lm` (antes `-lm` vinha primeiro) | **Bug no Linux:** com biblioteca estática, `-lm` antes da raylib dá `undefined reference to sqrtf`. Só não aparecia porque nenhum código chamava a raylib de verdade. |
| Objetos em `build/texto/` e `build/raylib/` (depois: `build/<modo>-<compilador>/`, ver entrada mais recente) | Trocar entre `make` e `make USE_RAYLIB=1` misturava objetos dos dois modos e quebrava o link. Agora cada modo tem sua pasta. |
| Link sempre refeito (alvo `FORCE`) | Depois de trocar de modo, o `milhao` antigo era mais novo que os objetos e não era religado. |
| Variável `GL_LIB` (padrão `-lGL`) | Linux sem o pacote `libgl-dev`: `make USE_RAYLIB=1 GL_LIB=-l:libGL.so.1`. |
| `clean` apaga os objetos dos dois modos; `clear_data` no `.PHONY` | Limpeza completa. |
| Arquivo reorganizado em seções comentadas | Legibilidade. |

### Novo `.gitattributes`
Normaliza o fim de linha: o Git guarda com LF e entrega CRLF no Windows e LF no Linux. Arquivos binários (`.a`, `.dat`, `.png`, `.wav`...) nunca são convertidos. Antes, copiar arquivos entre os dois sistemas fazia o Git mostrar arquivos inteiros como "modificados" sem mudança real (aconteceu com `perguntas.csv` e `arquivo.c`).

### `assets` (reescrito)
| Mudança | Motivo |
|---|---|
| Implementado: carga de imagens, sons, música, fonte e frames, todos com fallback | Passo 2 do plano: o jogo roda mesmo com a pasta `assets/` vazia. |
| Funções de texto vieram de `tela` para cá: `assets_desenhar_texto`, `_centralizado`, `_quebrado` (com máquina de escrever), `_ajustado` (reduz a fonte e centraliza), `assets_largura_texto`, `assets_contar_letras` | A fonte mora em `Assets`; `dialogo` e `botao` precisavam de texto e dependeriam de `tela` (módulo de cima). |
| Nova `assets_desenhar_painel(id, destino, cor_se_faltar, tint)` | Botões, painéis e caixas sem imagem viram retângulos com borda, não o xadrez magenta ilegível. |
| Nova `assets_imagem_eh_fallback` | Fundos sem imagem viram degradê escuro. |
| Removidos `IMG_BOSS_*`, `ANIM_ESMERALDA`, `AnimacaoId`, `assets_imagem_do_boss`, `assets_animacao_do_boss` | Eram duas formas de fazer a mesma coisa. Agora o boss é só animação (um quadro = imagem parada), indexada por `BossId`. |
| `IMG_BOTAO_NORMAL/HOVER/PRESSIONADO/DESABILITADO` → `IMG_BOTAO` | Uma imagem só; o estado muda a cor. Três desenhos a menos para fazer. |
| `IMG_NENHUMA` | Fala sem retrato. |
| Funções de carga internas viraram `static` em `assets.c` | Não eram usadas fora do módulo; o `.h` fica com só o que as telas usam. |
| Removida `assets_imagem` | Sem uso (as telas desenham por `assets_desenhar_imagem`). |
| Frames num vetor fixo (`MAX_QUADROS_ANIMACAO` = 8) em vez de `malloc`/`realloc` | Mais simples de explicar e sem risco de vazamento. |
| Paleta de cores `PALETA_*` | Cores num lugar só, também usadas como fallback. |
| Novo `assets/LEIA-ME.md` e pasta `assets/sprites/frames/` | Lista exata dos nomes de arquivo esperados. |

### `botao` (implementado)
| Mudança | Motivo |
|---|---|
| Implementado: criar, rótulo, atualizar (clique ao soltar ou tecla), desenhar | Passo 3 do plano. |
| Novo campo `destaque` (`DESTAQUE_CORRETO`/`DESTAQUE_ERRADO`) | Pintar certo/errado na revelação. |
| Nova `botao_sob_mouse` | O cursor é decidido uma vez por quadro em `jogo.c` (o botão não mexe mais no cursor). |
| `botao_desenhar` recebe o tamanho da fonte e reduz o texto até caber | Alternativas longas cabem no botão. |
| `botao_layout_alternativas` saiu do módulo | A posição dos botões é decisão da tela do jogo (agora em `jogo.c`). |

### `dialogo` (implementado)
| Mudança | Motivo |
|---|---|
| Nova `dialogo_anexar_cena` | **Bug de projeto:** não dava para tocar duas cenas seguidas (boss derrotado + próximo boss; introdução + Creeper). |
| `dialogo_atualizar(dialogo, dt, jogador_avancou)` recebe a entrada por parâmetro | O módulo não lê teclado; o mesmo clique não "vaza" para outra tela. |
| Removida `dialogo_terminou` | Repetia o campo `terminou` e o retorno de `dialogo_atualizar`. |
| Novas `dialogo_fala_atual` e `dialogo_letras_visiveis` | Usadas no desenho e úteis para as telas. |
| `dialogo_cena_do_boss` com id inválido devolve `CENA_QUANTIDADE` | Erro aparece em vez de virar Creeper. |
| Falas provisórias em todas as cenas (marcadas "TEXTO PROVISÓRIO") | O fluxo funciona; a equipe personaliza o texto. |

### `jogo` (implementado)
| Mudança | Motivo |
|---|---|
| Implementado: `jogo_executar` (janela e laço), `jogo_iniciar`, `jogo_encerrar`, `jogo_nova_partida`, `jogo_atualizar`, `jogo_desenhar`, `jogo_trocar_tela`, `jogo_tocar_cena`, `jogo_preparar_botoes_pergunta`, `jogo_registrar_erro` | Passo 1 do plano. |
| `jogo_executar` confere o banco **antes** de abrir a janela | Com perguntas faltando, o erro aparece no menu (com explicação) em vez de uma janela vazia. |
| Novas `jogo_anexar_cena` e `jogo_jogador_quer_avancar` | Encadear cenas; entrada de "avançar" igual em todas as telas. |
| Novos botões `botao_dica` (H) e `botao_eliminar` (E) | Ajudas da parte criativa. |
| Campos `tempo_total`, `efeito_da_tela_tocado`, `cursor_atual`; macro `ATRASO_ENTRADA_TELA` | Animação contínua do boss; tocar um som uma vez por tela; cursor sem chamadas repetidas; evitar o clique da tela anterior. |
| Layout dos botões em macros no topo de `jogo.c` | Mudar posição sem caçar números no código. |
| `SetTraceLogLevel(LOG_WARNING)` | O terminal mostra só avisos, não o log inteiro da raylib. |

### `tela`
| Mudança | Motivo |
|---|---|
| Implementadas: tela de diálogo, `tela_desenhar_fundo`, `tela_desenhar_barra_vida`, `tela_desenhar_banner_erro` | Infraestrutura comum. |
| Pergunta, revelação, resultado, HUD, boss e escada: **continuam TODO**, com passo a passo em `tela.h` e um aviso provisório na janela | É a parte do jogo que a equipe vai personalizar e explicar na arguição. |
| Removidas `tela_desenhar_texto_*` | Foram para `assets`. |

### `partida` — ajudas
| Mudança | Motivo |
|---|---|
| Novas `partida_usar_dica`, `partida_dica_atual`, `partida_eliminar_duas` | O enunciado pede ajudas na parte criativa. Uma vez cada por partida; valem só na rodada em que foram usadas. |
| Novos campos `dica_usada`, `dica_visivel`, `eliminacao_usada`, `alternativa_eliminada[4]` | Estado das ajudas. |
| `partida_responder` recusa alternativa eliminada | Regra coerente com a ajuda. |
| `tests/teste_partida.c`: novo `teste_ajudas` | Dica correta, só uma vez, some na rodada seguinte; elimina sempre 2 erradas e nunca a certa (500 sorteios). |

### `menu`
Mensagem específica quando faltam perguntas para uma partida (quantas de cada nível são necessárias).

### Achado: caracteres fora do Latin-1
A fonte carrega os caracteres 32 a 255. Acentos e `ç` funcionam (testado, inclusive com a fonte padrão da raylib), mas travessão (—), aspas curvas (“ ”) e reticências (…) aparecem como `?`. O CSV e as falas atuais não têm nenhum desses. Aviso adicionado em `assets.h`.

---

## 21/11/2026 (tarde) — correção do build e padronização (commit `b63682b`)

| Arquivo | Mudança | Motivo |
|---|---|---|
| `include/sorteio.h` | `sorteio_embaralhar_vetor(size_t *, size_t)` | O `.c` já usava `size_t*`; tipos diferentes no `.h` e no `.c` quebravam o `make`. |
| `src/main.c` | Sempre chama `menu_principal` | Com raylib o menu era pulado e a opção de jogar nunca aparecia. |
| `src/menu.c` | Opção `8 - JOGAR`, `OPCAO_MAX` = 8, `jogo_executar(banco)`, pausa após erro, aviso sem raylib | O 8 era recusado; `&banco` era `Banco**`; a mensagem sumia. |
| `include/config.h` | `CAMINHO_*` para tudo; `PERGUNTAS_POR_NIVEL`; comentários Doxygen | Padronização pedida pela equipe; distribuição por nível num lugar só. |
| `include/boss.h`, `src/boss.c` | Implementado; `float boss_calculo_vida`; nível inválido → `BOSS_QUANTIDADE` | Barra de vida precisa de fração; erro não pode virar Creeper. |
| `include/sorteio.h`, `src/sorteio.c` | Implementado; contagens por parâmetro | Removida a dependência circular com `partida`. |
| `include/partida.h`, `src/partida.c` | Implementado; `boss_derrotado_agora`; `partida_numero_da_rodada`; `partida_iniciar` não altera a struct se falhar | Detectar o boss derrotado depois de confirmar; HUD 1 a 15; não começar partida inválida. |
| `include/tela.h`, `botao.h`, `jogo.h` | Documentação corrigida | Nome da função de vida, boss derrotado, cursor. |
| `third_party/raylib/` | raylib movida para cá com `LICENSE`; `raygui.h`, `rlgl.h`, `raymath.h` removidos | Separar código de terceiros; os três headers não eram usados. |
| `tests/teste_partida.c`, `make teste` | Teste sem janela | Validar as regras sem abrir o jogo. |
