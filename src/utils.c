/**
 * @file utils.c
 * @brief Implementação das funções auxiliares de terminal.
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"

void utils_configurar_terminal(void)
{
#ifdef _WIN32
    /* Faz o prompt do Windows exibir acentos corretamente. */
    system("chcp 65001 > nul");
#endif
}

void utils_limpar_tela(void)
{
    printf("\033[2J\033[H");
    fflush(stdout);
}

void utils_beep(void)
{
    printf("\a");
    fflush(stdout);
}

void utils_pausar(void)
{
    char descarte[8];

    utils_ler_linha("\nPressione ENTER para continuar...", descarte, sizeof descarte);
}

int utils_ler_linha(const char *prompt, char *destino, size_t tamanho)
{
    size_t tam_lido;
    int c;

    if (destino == NULL || tamanho == 0) {
        return 0;
    }

    if (prompt != NULL) {
        printf("%s", prompt);
        fflush(stdout);
    }

    if (fgets(destino, (int)tamanho, stdin) == NULL) {
        destino[0] = '\0';
        return 0;
    }

    tam_lido = strlen(destino);
    if (tam_lido > 0 && destino[tam_lido - 1] == '\n') {
        destino[tam_lido - 1] = '\0';
    } else {
        /* Linha maior que o buffer: descarta o resto até o fim da linha. */
        while ((c = getchar()) != '\n' && c != EOF) {
        }
    }

    return 1;
}

int utils_ler_inteiro(const char *prompt, int minimo, int maximo, int *valor)
{
    char buffer[32];
    char *fim;
    long numero;

    if (valor == NULL) {
        return 0;
    }

    while (utils_ler_linha(prompt, buffer, sizeof buffer)) {
        errno = 0;
        numero = strtol(buffer, &fim, 10);

        if (fim != buffer && *fim == '\0' && errno == 0
            && numero >= minimo && numero <= maximo) {
            *valor = (int)numero;
            return 1;
        }

        utils_beep();
        printf(COR_VERMELHO "Entrada inválida! Digite um número de %d a %d.\n" COR_RESET,
               minimo, maximo);
    }

    return 0;
}

const char *utils_mensagem_status(Status status)
{
    switch (status) {
    case STATUS_OK:              return "Operação concluída com sucesso.";
    case STATUS_ERRO_ARQUIVO:    return "Não foi possível abrir, ler ou gravar o arquivo.";
    case STATUS_ERRO_MEMORIA:    return "Memória insuficiente.";
    case STATUS_ERRO_PARAMETRO:  return "Parâmetro inválido.";
    case STATUS_ERRO_FORMATO:    return "Arquivo fora do formato esperado.";
    case STATUS_NAO_ENCONTRADO:  return "Pergunta não encontrada.";
    case STATUS_NAO_IMPLEMENTADO:return "Função ainda não implementada.";
    case STATUS_ERRO_GRAFICO:    return "Não foi possível iniciar a janela gráfica.";
    case STATUS_ASSET_FALLBACK:  return "Alguns arquivos de assets não foram encontrados (usando substitutos).";
    }
    return "Erro desconhecido.";
}
