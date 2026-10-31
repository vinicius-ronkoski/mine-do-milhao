## Estrutura atual

```text
jogo-milhao-minecraft/
├── assets/               # só na parte criativa
│   ├── sprites/          # mobs, villager, blocos, esmeralda
│   ├── sounds/
│   └── fonts/
├── build/                # .o gerados (fora do Git)
├── data/
│   ├── perguntas.csv     # dados iniciais (vai pro Git, o professor pede)
│   ├── perguntas.bin     # gerado na 1ª execução (fora do Git)
│   ├── ranking.bin       # criativo (fora do Git)
│   └── exportados/       # saída da função "exportar CSV"
├── docs/                 # slides da apresentação e outros documentos
├── include/              # só .h
├── Makefile              # all, clean, run (obrigatórios)
├── src/                  # só .c (um par .c/.h por estrutura)
└──README.md             # tema, integrantes, como compilar, ideias criativas
```
