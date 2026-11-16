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

| Comando        | O que faz                                              |
|----------------|--------------------------------------------------------|
| `make`         | Compila (objetos em `build/`, executável `milhao`)     |
| `make run`     | Compila (se preciso) e executa                         |
| `make clean`   | Remove objetos e executável                            |
| `make valgrind`| Executa sob o Valgrind (checa vazamentos de memória)   |
| `make debug`   | Abre o executável no `gdb`                             |

Na 2ª entrega (raylib): `make USE_RAYLIB=1`, com `raylib.h` em `include/` e a biblioteca em `lib/` (Linux) ou `libwin/` (Windows).

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
│   ├── perguntas.bin     # gerado na 1ª execução (fora do Git)
│   ├── ranking.bin       # criativo (fora do Git)
│   └── exportados/       # saída de "exportar CSV"
├── docs/                 # slides e outros documentos
├── include/              # só .h
├── src/                  # só .c
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

## Formato do `perguntas.csv`

```text
nivel,enunciado,alternativa_a,alternativa_b,alternativa_c,alternativa_d,correta,dica
```

- `nivel`: 1 (muito fácil) a 5 (muito difícil)
- `correta`: letra `A`, `B`, `C` ou `D`
- campos de enunciado, alterntivas e dicas devem estar entre aspas
- mínimo exigido: 20 muito fácil, 20 fácil, 15 médio, 15 difícil, 10 muito difícil (80 no total)

## Plano do jogo

**1ª entrega (parte básica):** cadastro completo (inserir, listar, pesquisar, alterar, excluir), navegação por nível, CSV inicial → binário, exportação CSV. Sem parte visual.

**2ª entrega (parte criativa, raylib):**

1. **Partida** no estilo SBT: 15 perguntas, perguntas e alternativas sorteadas por nível, marcos de segurança nas perguntas 5 e 10.
2. **Bosses** (um por nível de dificuldade) que perdem vida a cada acerto: Creeper, Wither, Warden, Ender Dragon e Herobrine.
3. **Esmeraldas e loja:** esmeraldas ganhas a cada acerto; a loja abre a cada marco de segurança, com buffs de tempo extra e de reduzir alternativas. A dica da pergunta também pode ser recuperada com esmeraldas.
4. **Minigame das dicas:** 3 baús, cada um com um tipo de bloco; o jogador recebe 10 blocos aleatórios e aperta 1, 2 ou 3 para guardá-los. Errar diminui o tempo restante; se falhar, perde a dica.
5. **Polimento:** tela inicial no estilo do menu do Minecraft, animações, músicas e arte feita à mão.


## Observações
- Os comentários feitos no arquivo.c tem uma representação educativa também, servindo como base de explicação para a compreensão do código de funções e técnicas não utilizadas regularmente na disciplina de Fundamentos de Programação.