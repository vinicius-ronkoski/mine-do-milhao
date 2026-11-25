/**
 * @file arquivo.c
 * @brief Implementação da persistência (CSV e binário).
 */

#include "arquivo.h"
#include "utils.h"
#include <string.h>
#include <stdio.h>

#ifdef _WIN32
#include <windows.h>   /* MoveFileExA: no Windows o rename() não substitui um arquivo que já existe */
#endif

/** Tamanho do buffer do nome do arquivo temporário (caminho do .dat + ".tmp"). */
#define TAM_CAMINHO_TEMPORARIO 256

/** Converte o VALOR de uma macro em texto (usado para montar o formato do sscanf). */
#define STR_(x) #x
#define STR(x) STR_(x)

/**
 * @brief Fecha um arquivo num caminho de erro e devolve o erro que já tinha acontecido.
 *
 * Quando a leitura já falhou, o erro que interessa ao chamador é o original
 * (formato, leitura...). Uma falha extra do fclose não muda o resultado, por isso
 * é ignorada de propósito. No caminho de sucesso, o fclose é sempre conferido.
 *
 * @param arquivo Arquivo aberto.
 * @param erro    Status que será devolvido.
 * @return O próprio `erro`.
 */
static Status fechar_com_erro(FILE *arquivo, Status erro)
{
    fclose(arquivo);
    return erro;
}

Status dados_carregar(Banco *banco)
{
    FILE *arquivo;
    Status status;

    // Confere erro de parametro
    if (banco == NULL) {
        return STATUS_ERRO_PARAMETRO;
    }

    // Se o .dat já existe, ele é a fonte dos dados (o fopen aqui só testa se existe)
    arquivo = fopen(CAMINHO_DAT, "rb");
    if (arquivo != NULL) {
        if (fclose(arquivo) != 0) {
            return STATUS_ERRO_ARQUIVO;
        }
        return dat_carregar(CAMINHO_DAT, banco);
    }

    /* Primeira execução: ainda não existe o .dat, então lê o CSV */
    status = csv_carregar(CAMINHO_CSV, banco);
    if (status != STATUS_OK) {
        return status;
    }

    /* Gera o .dat para as próximas execuções */
    if (dados_salvar(banco) != STATUS_OK) {
        fprintf(stderr, "Aviso: não foi possível gravar o arquivo " CAMINHO_DAT "\n");
    }

    return STATUS_OK;
}

Status dados_salvar(const Banco *banco)
{
    return dat_salvar(CAMINHO_DAT, banco);
}

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
        return fechar_com_erro(arquivo, STATUS_ERRO_FORMATO);
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

    for (size_t i = 0; i < banco->quantidade; i++) {
        const Pergunta *pergunta = &banco->itens[i]; // Pergunta imutavel, por isso const

        // Salva o enunciado
        fprintf(arquivo, "%d,\"%s\"", pergunta->nivel, pergunta->enunciado);

        // Salva as alternativas
        for (int j = 0; j < NUM_ALTERNATIVAS; j++) {
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
 * Essa função faz essa conferencia em cada pergunta caso exista alguma corrupção
 *
 * @param pergunta Pergunta lida do arquivo binário.
 * @return STATUS_OK se todos os textos têm '\0' dentro do próprio vetor;
 *         STATUS_ERRO_FORMATO se algum não tem; STATUS_ERRO_PARAMETRO se pergunta for NULL.
 */
static Status textos_terminados(const Pergunta *pergunta)
{
    if (pergunta == NULL) {
        return STATUS_ERRO_PARAMETRO;
    }

    // memchr procura o '\0' dentro do vetor inteiro; NULL significa que não achou
    // ENUNCIADO e DICA
    if (memchr(pergunta->enunciado, '\0', sizeof(pergunta->enunciado)) == NULL ||
        memchr(pergunta->dica, '\0', sizeof(pergunta->dica)) == NULL) {
        return STATUS_ERRO_FORMATO;
    }

    // ALTERNATIVAS
    for (int i = 0; i < NUM_ALTERNATIVAS; i++) {
        if (memchr(pergunta->alternativas[i], '\0', sizeof(pergunta->alternativas[i])) == NULL) {
            return STATUS_ERRO_FORMATO;
        }
    }
    return STATUS_OK;
}

Status dat_carregar(const char *caminho, Banco *banco)
{
    FILE *arquivo;
    Banco carregado;    // Banco temporário: só substitui o original se o arquivo estiver íntegro
    Pergunta pergunta;
    size_t quantidade, bytes_de_perguntas;
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
        return fechar_com_erro(arquivo, STATUS_ERRO_ARQUIVO);
    }
    tamanho_arquivo = ftell(arquivo);

    // ftell falhou (devolve -1) ou não foi possível voltar ao início
    if (tamanho_arquivo < 0 || fseek(arquivo, 0, SEEK_SET) != 0) {
        return fechar_com_erro(arquivo, STATUS_ERRO_ARQUIVO);
    }

    // Lê a quantidade de perguntas que o arquivo diz ter
    if (fread(&quantidade, sizeof(quantidade), 1, arquivo) != 1) {
        return fechar_com_erro(arquivo, STATUS_ERRO_FORMATO);
    }

    /*
     * O tamanho do arquivo precisa ser exatamente: quantidade + N structs Pergunta.
     */
    bytes_de_perguntas = (size_t)tamanho_arquivo - sizeof(quantidade);
    if (bytes_de_perguntas % sizeof(Pergunta) != 0 ||
        bytes_de_perguntas / sizeof(Pergunta) != quantidade) {
        return fechar_com_erro(arquivo, STATUS_ERRO_FORMATO);
    }

    // Lê as perguntas uma a uma, validando cada uma antes de inserir
    banco_iniciar(&carregado);
    for (size_t i = 0; i < quantidade && status == STATUS_OK; i++) {
        // Lê, confere os textos e insere no banco temporário
        if (fread(&pergunta, sizeof(pergunta), 1, arquivo) != 1) {
            status = STATUS_ERRO_ARQUIVO;
        } else if (textos_terminados(&pergunta) != STATUS_OK) {
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

    // Algum erro: descarta tudo o que foi lido
    if (status != STATUS_OK) {
        banco_liberar(&carregado);  // Descarta o que foi lido e deixa o banco original intacto
        return status;
    }

    // Deu tudo certo: troca o conteúdo do banco pelo que foi lido
    banco_liberar(banco);
    *banco = carregado;

    return STATUS_OK;
}

/**
 * @brief Troca `destino` por `origem` de uma vez (o destino antigo só some se a troca der certo).
 *
 * No Linux o rename() já substitui o arquivo existente de forma atômica. No Windows o rename()
 * falha se o destino existe, então usa MoveFileExA com MOVEFILE_REPLACE_EXISTING.
 *
 * @return true se a troca foi feita.
 */
static bool substituir_arquivo(const char *origem, const char *destino)
{
#ifdef _WIN32
    return MoveFileExA(origem, destino, MOVEFILE_REPLACE_EXISTING) != 0;
#else
    return rename(origem, destino) == 0;
#endif
}

Status dat_salvar(const char *caminho, const Banco *banco)
{
    FILE *arquivo;
    char temporario[TAM_CAMINHO_TEMPORARIO];
    Status status = STATUS_OK;

    // Confere parametros
    if (caminho == NULL || banco == NULL) {
        return STATUS_ERRO_PARAMETRO;
    }

    // Grava num arquivo temporário ao lado do original (caminho + ".tmp"). O .dat antigo só é
    // trocado no final: se o programa cair no meio da gravação, as perguntas antigas continuam
    // intactas (abrir direto o caminho com "wb" apagaria o conteúdo antes de gravar).
    if (snprintf(temporario, sizeof(temporario), "%s.tmp", caminho) >= (int)sizeof(temporario)) {
        return STATUS_ERRO_PARAMETRO;   // caminho longo demais para o buffer
    }

    // Abre o arquivo temporário
    arquivo = fopen(temporario, "wb");
    if (arquivo == NULL) {
        perror(temporario);
        return STATUS_ERRO_ARQUIVO;
    }

    // Grava a quantidade primeiro
    if (fwrite(&banco->quantidade, sizeof(banco->quantidade), 1, arquivo) != 1) {
        status = STATUS_ERRO_ARQUIVO;
    }

    // Grava o bloco de perguntas de uma vez (só se existir: banco vazio tem itens == NULL)
    if (status == STATUS_OK && banco->quantidade > 0 &&
        fwrite(banco->itens, sizeof(Pergunta), banco->quantidade, arquivo) != banco->quantidade) {
        status = STATUS_ERRO_ARQUIVO;
    }

    // Fecha o arquivo (o fclose descarrega o buffer: um erro aqui também é falha de gravação)
    if (fclose(arquivo) != 0) {
        status = STATUS_ERRO_ARQUIVO;
    }

    // Deu tudo certo: o temporário vira o .dat definitivo. Se falhou, apaga o temporário
    // e deixa o .dat antigo como estava.
    if (status == STATUS_OK && !substituir_arquivo(temporario, caminho)) {
        perror(caminho);
        status = STATUS_ERRO_ARQUIVO;
    }
    if (status != STATUS_OK) {
        remove(temporario);
    }

    return status;
}
