/**
 * @file pergunta.c
 * @brief Implementação das operações sobre uma única Pergunta.
 */

#include <stdio.h>

#include "pergunta.h"
#include "utils.h"


const char *pergunta_nome_nivel(int nivel)
{
    // Define os níveis de dificuldade
    static const char *const nomes[] = {
        "Muito fácil", "Fácil", "Médio", "Difícil", "Muito difícil"
    };

    // Compara o nível de entrada com o mínimo e o máximo (def: config.h)
    if (nivel < NIVEL_MIN || nivel > NIVEL_MAX) {
        return "Inválido";
    }
    return nomes[nivel - NIVEL_MIN];
}

Status pergunta_validar(const Pergunta *pergunta)
{
    int i;

    if (pergunta == NULL || pergunta->enunciado[0] == '\0') {
        return STATUS_ERRO_PARAMETRO;
    }
    for (i = 0; i < NUM_ALTERNATIVAS; i++) {
        if (pergunta->alternativas[i][0] == '\0') {
            return STATUS_ERRO_PARAMETRO;
        }
    }
    if (pergunta->correta < 'A' || pergunta->correta > 'A' + NUM_ALTERNATIVAS - 1) {
        return STATUS_ERRO_PARAMETRO;
    }
    if (pergunta->nivel < NIVEL_MIN || pergunta->nivel > NIVEL_MAX) {
        return STATUS_ERRO_PARAMETRO;
    }
    return STATUS_OK;
}

void pergunta_imprimir(const Pergunta *pergunta)
{
    int i;

    if (pergunta == NULL) {
        return;
    }

    printf(COR_AMARELO "[Nível %d - %s]" COR_RESET "\n", pergunta->nivel, pergunta_nome_nivel(pergunta->nivel));
    printf(COR_CIANO "%s" COR_RESET "\n", pergunta->enunciado);
    for (i = 0; i < NUM_ALTERNATIVAS; i++) {
        printf("  %c) %s\n", 'A' + i, pergunta->alternativas[i]);
    }
    printf(COR_VERDE "Correta: %c" COR_RESET "\n", pergunta->correta);
    printf("Dica: %s\n", pergunta->dica);
}
