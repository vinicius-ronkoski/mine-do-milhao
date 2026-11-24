# Guia rápido — cenas, cenários, sons e imagens

Receitas curtas para mexer no jogo sem procurar no código. Nomes e tamanhos de arquivo:
[`assets/README.md`](../assets/README.md).

Regras que valem para tudo:
- Arquivos em `assets/` são citados **sem extensão** no código (o jogo testa `.png`, `.jpg`, `.ogg`...).
- Nomes de arquivo em **minúsculas** (no Linux, `Creeper.png` ≠ `creeper.png`).
- Arquivo faltando **não trava** o jogo: imagem vira xadrez, som vira silêncio e o terminal avisa.
- Depois de trocar um arquivo, **feche e abra o jogo** (tudo é carregado na abertura).
- Texto na tela: use `-`, `"` e `...` (travessão, aspas curvas e reticências viram `?` na fonte).

---

## 1. Funções de cena (diálogo)

| Função | Onde | O que faz |
|---|---|---|
| `jogo_tocar_cena(jogo, CENA_X, TELA_Y)` | `jogo.h` | Limpa o diálogo, toca a cena e depois vai para a tela `TELA_Y` |
| `jogo_anexar_cena(jogo, CENA_X)` | `jogo.h` | Encadeia mais uma cena no diálogo que já está tocando |
| `jogo_tocar_reacao(jogo)` | `jogo.h` | Fala de acerto/erro depois de responder (já usada na tela de pergunta) |
| `dialogo_cena_do_boss(boss_id)` | `dialogo.h` | Cena de entrada do boss (`CENA_BOSS_CREEPER + id`) |

Dentro do `case` de uma cena, em `dialogo_anexar_cena()` (`dialogo.c`):

| Atalho | O que faz |
|---|---|
| `FALA(personagem, texto)` | Uma fala |
| `CENARIO(IMG_X)` | A próxima fala **e as seguintes** usam esse fundo (sem o boss). `CENARIO(IMG_NENHUMA)` volta ao fundo da fase |
| `RETRATO(IMG_X)` | A próxima fala **e as seguintes** usam esse retrato. `RETRATO(IMG_NENHUMA)` = sem retrato. Toda cena começa sem retrato |
| `SOM(SOM_X)` | A **próxima** fala toca `SOM_X` quando aparece (só ela) |
| `MUSICA(MUSICA_X)` | A música troca quando a **próxima** fala aparecer e continua depois da cena (ver seção 5b) |

**Regra única: tudo que modifica uma fala vem antes dela.** `SOM` ou `MUSICA` no fim do `case`, sem
`FALA` depois, é ignorado (o terminal avisa).

`personagem` é um texto (`NOME_APRESENTADOR` = "Silvio Aldeão"). Ao trocar de personagem, troque o `RETRATO` junto.

Limites: cada fala até 255 caracteres (`TAM_FALA`); um diálogo até 32 falas, **somando as cenas
encadeadas** (`DIALOGO_MAX_FALAS`, em `config.h`).

---

## 2. Criar uma cena nova

1. **`dialogo.h`** — nome novo no enum `CenaId`, antes de `CENA_QUANTIDADE`:
   ```c
   CENA_HISTORIA_6,        /**< O que acontece nesta cena. */
   ```
2. **`dialogo.c`** — um `case` em `dialogo_anexar_cena()`:
   ```c
   case CENA_HISTORIA_6:
       CENARIO(IMG_CENARIO_NETHER);
       RETRATO(IMG_ICONE_ALDEAO);
       FALA(NOME_APRESENTADOR, "Chegamos ao Nether.");
       SOM(SOM_TROVAO);
       FALA(NOME_APRESENTADOR, "Ouviu isso?");
       break;
   ```
3. **Chamar** no ponto do fluxo onde ela deve tocar (próxima seção).

Cena sem `case` = faixa de erro "cena inexistente" e nada toca.

---

## 3. Onde as cenas são chamadas

| Momento | Arquivo / função | Cenas hoje |
|---|---|---|
| Início da partida | `jogo.c`, `jogo_nova_partida()` | `CENA_INTRO` → `CENA_HISTORIA_1` → entrada do Creeper |
| Depois de responder | `tela.c`, `tela_pergunta_atualizar()` | reação (acerto/erro) |
| Parar | `tela.c`, `tela_pergunta_atualizar()` | `CENA_PARADA` |
| Errou | `tela.c`, `tela_revelacao_atualizar()` | `CENA_DERROTA` |
| Boss caiu | `tela.c`, `tela_revelacao_atualizar()` (`switch` pelo próximo boss) | `CENA_BOSS_DERROTADO` → `CENA_HISTORIA_2..5` → entrada do próximo boss |
| Venceu | `tela.c`, `tela_revelacao_atualizar()` | `CENA_BOSS_DERROTADO` → `CENA_HISTORIA_FIM` → `CENA_VITORIA` |

**Várias cenas seguidas:** mais de um `jogo_anexar_cena` no mesmo lugar, na ordem desejada.

**Cena entre duas perguntas** (fora da troca de boss): no ramo `default` do `switch` de
`tela_revelacao_atualizar()`, com um `if` pela rodada:
```c
} else if (partida_numero_da_rodada(&jogo->partida) == 8) {      /* acabou de acertar a 7ª */
    jogo_tocar_cena(jogo, CENA_HISTORIA_EXTRA, TELA_PERGUNTA);
}
```
Ali é `tocar` (ainda não há diálogo tocando); para encadear mais, `anexar` logo depois.

**Falas de acerto/erro:** tabelas em `dialogo_anexar_reacao()` (`dialogo.c`). Cada texto tem
exatamente um `%s` (resposta certa ou nome do boss).

---

## 4. Adicionar uma imagem (cenário, retrato, ícone)

1. Arquivo em **`assets/sprites/`** (ex.: `cenario_nether.png`).
2. **`assets.h`** — nome no enum `ImagemId`, antes de `IMG_QUANTIDADE`:
   ```c
   IMG_CENARIO_NETHER,      /**< Cenário das cenas no Nether. */
   ```
3. **`assets.c`** — nome do arquivo, sem extensão, na tabela de `nome_arquivo_imagem()`,
   **na mesma posição** do enum:
   ```c
   "cenario_nether",        /* IMG_CENARIO_NETHER */
   ```

Usos:
- **Cenário de cena:** `CENARIO(IMG_CENARIO_NETHER);` no `case`. Proporção 16:9 (pixel art: 320x180 ou 640x360).
- **Retrato de personagem:** `RETRATO(IMG_X);` antes da `FALA`. Quadrado.
- **Ícone num botão:** `botao.icone = IMG_X;` (ao lado do texto; ex.: preço da dica). Ícone de pixel art 16x16
  aparece em 32 px (`LADO_ICONE_ESMERALDA`, em `layout.h`).
- **Desenhar em qualquer tela:** `assets_desenhar_imagem(&jogo->assets, IMG_X, retangulo, WHITE);`

A ordem do enum e a da tabela **têm de bater**: o enum é o índice da tabela.

---

## 5. Adicionar um som

1. Arquivo em **`assets/sounds/`** (`.ogg`, `.wav` ou `.mp3`).
2. **`assets.h`** — nome no enum `SomId`, antes de `SOM_QUANTIDADE`:
   ```c
   SOM_TROVAO,              /**< Trovão nas cenas de história. */
   ```
3. **`assets.c`** — nome do arquivo na tabela de `nome_arquivo_som()`, na mesma posição.

Usos:
- **Numa fala:** `SOM(SOM_TROVAO);` na linha antes da `FALA(...)`.
- **Em qualquer lugar:** `assets_tocar_som(&jogo->assets, SOM_TROVAO);` (`SOM_NENHUM` não toca nada).

Sons que o jogo já toca sozinho: `clique` (botões e diálogo), `acerto`, `erro`, `dano` (revelação),
`entrada_<boss>` (1ª pergunta de cada boss). Volume dos efeitos: `VOLUME_EFEITOS` (`config.h`).

---

## 5b. Músicas

Toca uma música por vez, em loop, e ela **não volta sozinha**: fica até outro `MUSICA(...)`.

| Música | Arquivo em `assets/sounds/` | Onde começa hoje |
|---|---|---|
| `MUSICA_PRINCIPAL` | `musica` | 1ª fala da `CENA_INTRO` (também ao reiniciar) |
| `MUSICA_CREEPER` ... `MUSICA_HEROBRINE` | `musica_creeper`, `musica_wither`, `musica_warden`, `musica_ender_dragon`, `musica_herobrine` | 1ª fala da cena de entrada de cada boss |

- **Trocar numa cena:** `MUSICA(MUSICA_X);` antes da fala em que ela deve começar.
- **Voltar:** outro `MUSICA(MUSICA_PRINCIPAL);` (ou a do boss) onde quiser.
- **Arquivo faltando:** a troca é ignorada e a música atual continua (o terminal diz quantas faltam).
- **Fora de cena:** `assets_trocar_musica(&jogo->assets, MUSICA_X);`
- **Música nova:** id no enum `MusicaId` (`assets.h`, antes de `MUSICA_QUANTIDADE`) e nome do arquivo
  na tabela `nome_arquivo_musica()` (`assets.c`), na mesma posição.
- Volume de todas: `VOLUME_MUSICA` (`config.h`). Prefira `.ogg` (arquivos menores no Git).

---

## 6. Bosses e fundos de fase (só arquivos, sem código)

| O quê | Pasta e nome | Tamanho |
|---|---|---|
| Frames do boss | `assets/sprites/frames/<boss>_00.png`, `_01`... (até 8) | 64x64, fundo transparente |
| Fundo da fase | `assets/sprites/fundos/<boss>.png` | 16:9 (pixel art 320x180) |
| Fundo padrão das telas | `assets/sprites/fundo_pergunta`, `fundo_dialogo`, `fundo_resultado` | 16:9 |

`<boss>` = `creeper`, `wither`, `warden`, `ender_dragon`, `herobrine`. Note o `_` antes do número
(`ender_dragon_00`, não `ender_dragon00`).

---

## 7. Posições e tamanhos na tela

Tudo em **`include/layout.h`** (coluna da pergunta, boss, HUD, revelação, resultado, caixa de diálogo).
Mudar o visual = mudar números ali. Tamanhos de boss: múltiplos de 64; de ícone: múltiplos de 16.

---

## 8. Conferir depois de mexer

```
make USE_RAYLIB=1      compila com a janela (erros de digitação no enum/tabela aparecem aqui)
make run USE_RAYLIB=1  joga (opção 8 no menu)
make teste             testa as regras (não cobre telas nem sons)
```
O terminal lista cada arquivo de `assets/` não encontrado ao abrir o jogo.
