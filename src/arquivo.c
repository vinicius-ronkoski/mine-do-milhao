/**
 * @file arquivo.c
 * @brief Implementação da persistência (CSV e binário).
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

/**
 * @brief Confere se todos os textos de uma Pergunta terminam com '\0'.
 *
 * Um .bin corrompido pode trazer textos sem terminador. Sem essa conferência,
 * printf("%s") e strlen() leriam além do vetor.
 *
 * @param p Pergunta lida do arquivo binário.
 * @return 1 se todos os textos têm '\0' dentro do próprio vetor; 0 caso contrário.
 */
static int textos_terminados(const Pergunta *p)
{
    int i;

    // memchr procura o '\0' dentro do vetor inteiro; NULL significa que não achou
    if (memchr(p->enunciado, '\0', sizeof p->enunciado) == NULL ||
        memchr(p->dica, '\0', sizeof p->dica) == NULL) {
        return 0;
    }
    for (i = 0; i < NUM_ALTERNATIVAS; i++) {
        if (memchr(p->alternativas[i], '\0', sizeof p->alternativas[i]) == NULL) {
            return 0;
        }
    }
    return 1;
}

Status bin_carregar(const char *caminho, Banco *banco)
{
    FILE *arquivo;
    Banco carregado;    // Banco temporário: só substitui o original se o arquivo estiver íntegro
    Pergunta pergunta;
    size_t quantidade, bytes_de_perguntas, i;
    long tamanho_arquivo;
    Status status = STATUS_OK;

    // Confere parametros
    if (caminho == NULL || banco == NULL) {
        return STATUS_ERRO_PARAMETRO;
    }

    // Abre o arquivo
    arquivo = fopen(caminho, "rb");
    if (arquivo == NULL) {
        perror(caminho);
        return STATUS_ERRO_ARQUIVO;
    }

    // Descobre o tamanho do arquivo: vai até o fim, lê a posição e volta ao início
    if (fseek(arquivo, 0, SEEK_END) != 0) {
        fclose(arquivo);
        return STATUS_ERRO_ARQUIVO;
    }
    tamanho_arquivo = ftell(arquivo);
    if (tamanho_arquivo < 0 || fseek(arquivo, 0, SEEK_SET) != 0) {
        fclose(arquivo);
        return STATUS_ERRO_ARQUIVO;
    }

    // Lê a quantidade de perguntas que o arquivo diz ter
    if (fread(&quantidade, sizeof quantidade, 1, arquivo) != 1) {
        fclose(arquivo);
        return STATUS_ERRO_FORMATO;
    }

    /*
     * O tamanho do arquivo precisa ser exatamente: quantidade + N structs Pergunta.
     * Isso evita alocar memória com base em um número absurdo lido de um arquivo
     * corrompido, e também detecta um .bin gerado antes de a struct Pergunta mudar
     * (a divisão é feita assim para não correr risco de overflow na multiplicação).
     */
    bytes_de_perguntas = (size_t)tamanho_arquivo - sizeof quantidade;
    if (bytes_de_perguntas % sizeof(Pergunta) != 0 ||
        bytes_de_perguntas / sizeof(Pergunta) != quantidade) {
        fclose(arquivo);
        return STATUS_ERRO_FORMATO;
    }

    // Lê as perguntas uma a uma, validando cada uma antes de inserir
    banco_iniciar(&carregado);
    for (i = 0; i < quantidade && status == STATUS_OK; i++) {
        if (fread(&pergunta, sizeof pergunta, 1, arquivo) != 1) {
            status = STATUS_ERRO_ARQUIVO;
        } else if (!textos_terminados(&pergunta)) {
            status = STATUS_ERRO_FORMATO;
        } else {
            status = banco_inserir(&carregado, &pergunta);
            if (status == STATUS_ERRO_PARAMETRO) {
                // banco_inserir recusou os dados: a pergunta gravada no arquivo era inválida
                status = STATUS_ERRO_FORMATO;
            }
        }
    }

    // Fecha o arquivo
    if (fclose(arquivo) != 0 && status == STATUS_OK) {
        status = STATUS_ERRO_ARQUIVO;
    }

    if (status != STATUS_OK) {
        banco_liberar(&carregado);  // Descarta o que foi lido e deixa o banco original intacto
        return status;
    }

    // Deu tudo certo: troca o conteúdo do banco pelo que foi lido
    banco_liberar(banco);
    *banco = carregado;

    return STATUS_OK;
}

Status bin_salvar(const char *caminho, const Banco *banco)
{
    FILE *arquivo;
    Status status = STATUS_OK;

    // Confere parametros
    if (caminho == NULL || banco == NULL) {
        return STATUS_ERRO_PARAMETRO;
    }

    // Abre o arquivo
    arquivo = fopen(caminho, "wb");
    if (arquivo == NULL) {
        perror(caminho);
        return STATUS_ERRO_ARQUIVO;
    }

    // Grava a quantidade primeiro (não dá para gravar o Banco inteiro: itens é ponteiro)
    if (fwrite(&banco->quantidade, sizeof banco->quantidade, 1, arquivo) != 1) {
        status = STATUS_ERRO_ARQUIVO;
    }

    // Grava o bloco de perguntas de uma vez (só se existir: banco vazio tem itens == NULL)
    if (status == STATUS_OK && banco->quantidade > 0 &&
        fwrite(banco->itens, sizeof(Pergunta), banco->quantidade, arquivo) != banco->quantidade) {
        status = STATUS_ERRO_ARQUIVO;
    }

    // Fecha o arquivo (o fclose descarrega o buffer, então ele também pode falhar)
    if (fclose(arquivo) != 0) {
        status = STATUS_ERRO_ARQUIVO;
    }

    return status;
}