# Mine do Milhão

Jogo do Milhão com temática **Minecraft**, em linguagem C.
Projeto prático de Fundamentos de Programação 2 (FDCO2B) — UTFPR, campus Apucarana, prof. Muriel de Souza Godoi.

> Projeto **exclusivamente acadêmico**, sem fins lucrativos e **não afiliado à Mojang/Microsoft**. Minecraft é marca de seus respectivos donos.

## Integrantes

- Gabriel Yuji Aoki
- Paulo André Bueno Valadares
- Vinícius Ronkoski Ferreira

## Como compilar e executar

Requer `gcc` e `make`. Execute sempre a partir da **raiz do projeto** (os caminhos `data/...` são relativos a ela).

| Comando           | O que faz                                              |
|-------------------|--------------------------------------------------------|
| `make`            | Compila (objetos em `build/texto-<compilador>/`, executável `milhao`) |
| `make run`        | Compila (se preciso) e executa                         |
| `make clean`      | Remove objetos e executáveis (dos dois modos)          |
| `make valgrind`   | Executa sob o Valgrind (checa vazamentos de memória)   |
| `make debug`      | Abre o executável no `gdb`                             |
| `make clear_data` | Apaga os `.dat` de `data/` (o jogo volta a ler o CSV)  |
| `make teste`      | Testa a lógica da partida (sorteio, boss, regras) sem abrir janela |

Na 2ª entrega (raylib): `make USE_RAYLIB=1` (objetos em `build/raylib-<compilador>/`). O `raylib.h` e as bibliotecas ficam em `third_party/raylib/` (Linux em `lib/linux/`, Windows em `lib/windows/`). O jogo é aberto pela opção **8 - JOGAR** do menu.

**Windows:** o jogo (`USE_RAYLIB=1`) precisa de um **gcc de 64 bits** (MinGW-w64, ex.: w64devkit); o MinGW clássico de 32 bits só compila o modo texto. Confira com `gcc -dumpmachine` (deve mostrar `x86_64-w64-mingw32`). Passo a passo em [`docs/ARQUITETURA.md`](docs/ARQUITETURA.md#windows-o-jogo-precisa-de-um-gcc-de-64-bits). No Linux, se o link falhar com `cannot find -lGL`: `make USE_RAYLIB=1 GL_LIB=-l:libGL.so.1`. Detalhes em [`docs/ARQUITETURA.md`](docs/ARQUITETURA.md#9-compilar-no-windows-e-no-linux).

**Documentação:** [`docs/ARQUITETURA.md`](docs/ARQUITETURA.md) explica cada arquivo e como eles se ligam; [`docs/MUDANCAS.md`](docs/MUDANCAS.md) registra cada mudança. Os nomes dos arquivos de imagem e som estão em [`assets/README.md`](assets/README.md).

## Estrutura

```text
jogo-milhao-minecraft/
├── assets/               # parte criativa
│   ├── sprites/          # mobs, villager, blocos, esmeralda
│   ├── sounds/
│   └── fonts/
├── build/                # .o gerados (fora do Git)
├── data/
│   ├── perguntas.csv     # dados iniciais (vai para o Git)
│   ├── perguntas.dat     # gerado na 1ª execução (fora do Git)
│   ├── ranking.dat       # criativo (fora do Git)
│   └── exportados/       # saída de "exportar CSV"
├── docs/                 # ARQUITETURA.md, MUDANCAS.md, slides
├── include/              # só .h
├── src/                  # só .c
├── tests/                # teste_partida.c (make teste)
├── third_party/raylib/   # raylib 5.5 (licença zlib), não escrita pela equipe
├── Makefile
└── README.md
```

### Módulos (1ª entrega)

| Arquivo               | Responsabilidade                                                  |
|-----------------------|-------------------------------------------------------------------|
| `config.h`            | Constantes, caminhos de arquivo e códigos de retorno (`Status`)   |
| `pergunta.h/.c`       | Struct `Pergunta`, validação e impressão de uma pergunta          |
| `banco.h/.c`          | Vetor dinâmico de perguntas e CRUD (inserir, alterar, remover, buscar, filtrar por nível) |
| `arquivo.h/.c`        | Leitura do CSV, arquivo binário e exportação CSV                  |
| `menu.h/.c`           | Menu principal em modo texto                                      |
| `utils.h/.c`          | Leitura segura de entrada, cores ANSI, beep, mensagens de erro    |
| `main.c`              | Carrega os dados, roda o menu e salva ao sair                     |

### Módulos (2ª entrega)

| Arquivo               | Responsabilidade                                                  | raylib? |
|-----------------------|-------------------------------------------------------------------|---------|
| `sorteio.h/.c`        | Sorteia as perguntas por nível e embaralha as alternativas        | não |
| `boss.h/.c`           | Um boss por nível, que perde vida a cada acerto                   | não |
| `partida.h/.c`        | Regras do Jogo do Milhão: rodadas, prêmios, marcos, parar/errar, ajudas | não |
| `assets.h/.c`         | Imagens, sons, música, fonte e texto, com fallback                | sim |
| `botao.h/.c`          | Botões clicáveis com atalho de teclado                            | sim |
| `dialogo.h/.c`        | Falas dos personagens com efeito de máquina de escrever           | sim |
| `tela.h/.c`           | Atualização e desenho de cada tela                                | sim |
| `jogo.h/.c`           | Janela, laço principal e troca de telas                           | sim |

## Formato do `perguntas.csv`

```text
nivel,enunciado,alternativa_a,alternativa_b,alternativa_c,alternativa_d,correta,dica
```

- `nivel`: 1 (muito fácil) a 5 (muito difícil)
- `correta`: letra `A`, `B`, `C` ou `D`
- campos de enunciado, alterntivas e dicas devem estar entre aspas
- mínimo exigido: 20 muito fácil, 20 fácil, 15 médio, 15 difícil, 10 muito difícil (80 no total)

## Plano do jogo

**1ª entrega (parte básica):** cadastro completo (inserir, listar, pesquisar, alterar, excluir), navegação por nível, CSV inicial → binário (.dat), exportação CSV. Sem parte visual.

**2ª entrega (parte criativa, raylib):**

1. **Partida** no estilo SBT: 15 perguntas, perguntas e alternativas sorteadas por nível, marcos de segurança nas perguntas 5 e 10.
2. **Bosses** (um por nível de dificuldade) que perdem vida a cada acerto: Creeper, Wither, Warden, Ender Dragon e Herobrine.
3. **Esmeraldas como moeda:** as esmeraldas acumuladas são o prêmio e também o saldo para comprar a **dica** da pergunta. O preço sobe com o nível (1, 2, 5, 10 e 20 esmeraldas); pode-se comprar uma vez por pergunta, e o que foi gasto é descontado do prêmio final.
4. **Eliminar duas:** ajuda grátis, uma vez por partida, que desabilita duas alternativas erradas.
5. **Polimento:** tela inicial no estilo do menu do Minecraft, animações, músicas e arte feita à mão.


## Observações
- Os comentários feitos no arquivo.c tem uma representação educativa também, servindo como base de explicação para a compreensão do código de funções e técnicas não utilizadas regularmente na disciplina de Fundamentos de Programação.
- O minigame das dicas (3 baús) e a loja nos marcos ficaram fora por escopo: a dica passou a ser comprada com esmeraldas, e sem cronômetro o buff de tempo extra não teria efeito.
