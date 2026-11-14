/**
 * @file pergunta.c
 * @brief Implementação das operações sobre uma única Pergunta.
 */

#include <stdio.h>

#include "pergunta.h"
#include "utils.h"

/**
 * @brief Recebe o número de dificuldade e devolve em texto
 * 
 * @param nivel número em entrada 
 * @return const char* nome do 
 */
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

int pergunta_valida(const Pergunta *p)
{
    int i;

    if (p == NULL || p->enunciado[0] == '\0') {
        return 0;
    }
    for (i = 0; i < NUM_ALTERNATIVAS; i++) {
        if (p->alternativas[i][0] == '\0') {
            return 0;
        }
    }
    if (p->correta < 'A' || p->correta > 'A' + NUM_ALTERNATIVAS - 1) {
        return 0;
    }
    return p->nivel >= NIVEL_MIN && p->nivel <= NIVEL_MAX;
}

void pergunta_imprimir(const Pergunta *p)
{
    int i;

    if (p == NULL) {
        return;
    }

    printf(COR_AMARELO "[Nível %d - %s]" COR_RESET "\n", p->nivel, pergunta_nome_nivel(p->nivel));
    printf(COR_CIANO "%s" COR_RESET "\n", p->enunciado);
    for (i = 0; i < NUM_ALTERNATIVAS; i++) {
        printf("  %c) %s\n", 'A' + i, p->alternativas[i]);
    }
    printf(COR_VERDE "Correta: %c" COR_RESET "\n", p->correta);
    printf("Dica: %s\n", p->dica);
}
