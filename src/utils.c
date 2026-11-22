/**
 * @file utils.c
 * @brief Implementação das funções auxiliares de terminal.
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"

void utils_limpar_buffer(void)
{
    int ch;

    /* Descarta o resto da linha digitada até o ENTER (ou o fim da entrada) */
    while ((ch = getchar()) != '\n' && ch != EOF) {
    }
}

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

    utils_ler_linha("\nPressione ENTER para continuar...", descarte, sizeof(descarte));
}

Status utils_ler_linha(const char *prompt, char *destino, size_t tamanho)
{
    size_t tam_lido;

    // Verifica parametros
    if (destino == NULL || tamanho == 0) {
        return STATUS_ERRO_PARAMETRO;
    }

    // Verifica a entrada de texto, se é valido, printa ele
    if (prompt != NULL) {
        printf("%s", prompt);
        fflush(stdout);
    }

    // Le a linha e trata erro
    if (fgets(destino, (int)tamanho, stdin) == NULL) {
        destino[0] = '\0';
        return STATUS_ERRO_ARQUIVO;
    }

    // Registra a quantidade lida e substitui o '\n' por '\0
    tam_lido = strlen(destino);
    if (tam_lido > 0 && destino[tam_lido - 1] == '\n') {
        destino[tam_lido - 1] = '\0';
    } else {
        /* Linha maior que o buffer: descarta o resto até o fim da linha. */
        utils_limpar_buffer();
    }
    
    return STATUS_OK;
}

Status utils_ler_int(const char *prompt, int minimo, int maximo, int *valor)
{
    char buffer[32];
    char *fim;
    long numero;
    Status status;

    if (valor == NULL) {
        return STATUS_ERRO_PARAMETRO;
    }

    /* Repete até ler um número válido ou a leitura falhar (EOF) */
    do {
        status = utils_ler_linha(prompt, buffer, sizeof(buffer));
        if (status != STATUS_OK) {
            break;   /* a entrada acabou: devolve o erro da própria leitura */
        }

        errno = 0;
        numero = strtol(buffer, &fim, 10);
        if (fim != buffer && *fim == '\0' && errno == 0
            && numero >= minimo && numero <= maximo) {
            *valor = (int)numero;   /* status já é STATUS_OK: o laço termina */
        } else {
            utils_beep();
            printf(
                COR_VERMELHO "Entrada inválida! Digite um número de %d a %d.\n" COR_RESET, 
                minimo, maximo
            );
            status = STATUS_ERRO_PARAMETRO;   /* entrada inválida: pede de novo */
        }
    } while (status != STATUS_OK);

    return status;
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
    case STATUS_SALDO_INSUFICIENTE: return "Esmeraldas insuficientes.";
    case STATUS_ASSET_FALLBACK:  return "Assets não foram encontrados (usando substitutos).";
    }
    return "Erro desconhecido.";
}
