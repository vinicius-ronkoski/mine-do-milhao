# Esturutra atual

├── Makefile              # all, clean, run (obrigatórios)
├── README.md             # tema, integrantes, como compilar, ideias criativas
├── .gitignore            # build/, *.bin, docs/html/
│
├── include/              # só .h
├── src/                  # só .c (um par .c/.h por estrutura)
│
├── data/
│   ├── perguntas.csv     # dados iniciais (vai pro Git, o professor pede)
│   ├── perguntas.bin     # gerado na 1ª execução (fora do Git)
│   ├── ranking.bin       # criativo (fora do Git)
│   └── exportados/       # saída da função "exportar CSV"
│
├── assets/               # só na parte criativa
│   ├── sprites/          # mobs, villager, blocos, esmeralda
│   ├── sounds/
│   └── fonts/
│
├── build/                # .o gerados (fora do Git)
└── docs/                 # saída do Doxygen + slides da apresentação
