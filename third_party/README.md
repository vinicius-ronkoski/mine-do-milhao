# third_party

Código de terceiros, separado do código do projeto (`src/` e `include/`).

| Pasta | O que é | Licença |
|-------|---------|---------|
| `raylib/include/raylib.h` | Cabeçalho da raylib 5.5 | zlib/libpng (`raylib/LICENSE`) |
| `raylib/lib/linux/libraylib.a` | Biblioteca estática compilada para Linux | zlib/libpng |
| `raylib/lib/windows/libraylib.a` | Biblioteca estática compilada para Windows (MinGW) | zlib/libpng |

Nada aqui foi escrito pela equipe e nada foi modificado. Só é usado com `make USE_RAYLIB=1`.
