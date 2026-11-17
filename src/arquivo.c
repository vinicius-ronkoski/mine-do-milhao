/**
 * @file arquivo.c
 * @brief Implementação da persistência (CSV e binário).
 *
 * TODO (equipe): implementar todas as funções abaixo.
 */

#include "arquivo.h"
#include "utils.h"
#include <string.h>
#include <stdio.h>

#define STR_(x) #x 
#define STR(x) STR_(x)

// Não verificada
Status dados_carregar(Banco *banco)
{
    // Abre o arquivo
    FILE *arquivo; 
    Status status;
    
    // Confere erro de parametro
    if (banco == NULL) {
        return STATUS_ERRO_PARAMETRO;
    }
    
    arquivo = fopen(CAMINHO_BIN, "rb");
    if (arquivo != NULL) {
        if (fclose(arquivo) != 0) {
            return STATUS_ERRO_ARQUIVO;
        }
        return bin_carregar(CAMINHO_BIN, banco);
    } 

    /*Primeira execução na qual não existe bin e confere o CSV*/
    status = csv_carregar(CAMINHO_CSV, banco);
    if (status != STATUS_OK) {
        return status;
    }

    /*Gera o bin para as próximas execuções*/
    if (dados_salvar(banco) != STATUS_OK) {
        fprintf(stderr, "Aviso: não foi possível gravar o arquivo binário\n");
    }

    return STATUS_OK;
}

Status dados_salvar(const Banco *banco)
{
    return bin_salvar(CAMINHO_BIN, banco);
}

// Não verificada
Status csv_carregar(const char *caminho, Banco *banco)
{
    // Declaração de variáveis
    FILE *arquivo;
    char linha[TAM_LINHA_CSV];
    Pergunta pergunta;
    Status status = STATUS_OK;

    int numero_linha = 1;

    // Confere parametros
    if (caminho == NULL || banco == NULL) {
        return STATUS_ERRO_PARAMETRO;
    }
    
    // Abre o arquivo
    arquivo = fopen(caminho, "r");
    if (arquivo == NULL) {
        perror(caminho);
        return STATUS_ERRO_ARQUIVO;
    }

    // Ignorando o cabeçalho
    if (fgets(linha, sizeof(linha), arquivo) == NULL) {
        fclose(arquivo);
        return STATUS_ERRO_FORMATO;
    }

    while (status == STATUS_OK && fgets(linha, sizeof(linha), arquivo) != NULL) {
        int lidos, consumidos = 0;

        numero_linha++;
        /* Remove \r pois é um caracter que fica como resquício junto ao \n no windows */
        linha[strcspn(linha, "\r\n")] = '\0';
        if (linha[0] == '\0') {
            continue;
        }

        memset(&pergunta, 0, sizeof(pergunta));

        /*
         *  Este trecho basicamente salva todas as informações do CSV em Pergunta
         * - Seu funcionamento está baseado em alguns conceitos, utilizando-se de:
         * - #define STR_(x) #x, no qual converte um valor x na formatação "x"
         * - [^\"] -> captura todos os valores em um range x com exceção de aspas dupla
         * - [] -> define um espaço de valores a serem lidos até encontrar algo diferente, e ^ inverte a lógica
         * - %n registra o numero de caracteres lidos até então
         */
        lidos = sscanf
        (
            linha,
            "%d,\"%" STR(MAX_ENUNCIADO) "[^\"]\","
            "\"%" STR(MAX_ALTERNATIVA) "[^\"]\",\"%" STR(MAX_ALTERNATIVA) "[^\"]\","
            "\"%" STR(MAX_ALTERNATIVA) "[^\"]\",\"%" STR(MAX_ALTERNATIVA) "[^\"]\","
            "%c,\"%" STR(MAX_DICA) "[^\"]\"%n",
            &pergunta.nivel, pergunta.enunciado,
            pergunta.alternativas[0], pergunta.alternativas[1],
            pergunta.alternativas[2], pergunta.alternativas[3],
            &pergunta.correta, pergunta.dica, &consumidos
        );

        // Confere se todo o sscanf funcionou corretamente ou se teve falhas
        if (
            lidos != 8 || consumidos == 0 ||
            linha[consumidos] != '\0' || pergunta_validar(&pergunta) != STATUS_OK
        ) { // Caso com erro
            status = STATUS_ERRO_FORMATO;
        } else {
            status = banco_inserir(banco, &pergunta);
        }

        // Printa mensagem de erro
        if (status != STATUS_OK) {
            fprintf(
                stderr, "%s, linha %d: %s\n", 
                caminho, numero_linha, utils_mensagem_status(status)
            );
        }

    }
    
    // Fecha o arquivo
    if (fclose(arquivo) != 0) {
        return STATUS_ERRO_ARQUIVO;
    }

    return status;
}

Status csv_exportar(const char *caminho, const Banco *banco)
{
    FILE *arquivo;
    size_t i;
    int j;

    // Confere parametros
    if (caminho == NULL || banco == NULL) {
        return STATUS_ERRO_PARAMETRO;
    }

    // Abre o arquivo
    arquivo = fopen(caminho, "w");
    if (arquivo == NULL) {
        perror(caminho);
        return STATUS_ERRO_ARQUIVO;
    }

    // Salva o cabeçalho
    fprintf(arquivo, "%s\n", CSV_CABECALHO);

    for (i = 0; i < banco->quantidade; i++) {
        const Pergunta *pergunta = &banco->itens[i]; // Pergunta imutavel, por isso const

        // Salva o enunciado
        fprintf(arquivo, "%d,\"%s\"", pergunta->nivel, pergunta->enunciado);

        // Salva as alternativas
        for (j = 0; j < NUM_ALTERNATIVAS; j++) {
            fprintf(arquivo, ",\"%s\"", pergunta->alternativas[j]);
        }
        // Salva resposta correta e a dica
        fprintf(arquivo, ",%c,\"%s\"\n", pergunta->correta, pergunta->dica);
    }

    // Fecha o arquivo
    if (fclose(arquivo) != 0) {
        return STATUS_ERRO_ARQUIVO;
    }

    return STATUS_OK;
}

Status bin_carregar(const char *caminho, Banco *banco)
{
    /* TODO: abrir com "rb", fread da quantidade (size_t), alocar o vetor
     *       e fread do bloco de Pergunta. */
    (void)caminho;
    (void)banco;
    return STATUS_NAO_IMPLEMENTADO;
}

Status bin_salvar(const char *caminho, const Banco *banco)
{
    /* TODO: abrir com "wb", fwrite da quantidade e depois do vetor. */
    (void)caminho;
    (void)banco;
    return STATUS_NAO_IMPLEMENTADO;
}
