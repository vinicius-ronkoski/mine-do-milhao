/**
 * @file arquivo.c
 * @brief Implementação da persistência (CSV e binário).
 *
 * TODO (equipe): implementar todas as funções abaixo.
 */

#include "arquivo.h"

Status dados_carregar(Banco *banco)
{
    /* TODO: tentar fopen(CAMINHO_BIN, "rb"); se existir -> bin_carregar();
     *       senão -> csv_carregar(CAMINHO_CSV). */
    (void)banco;
    return STATUS_NAO_IMPLEMENTADO;
}

Status dados_salvar(const Banco *banco)
{
    /* TODO: chamar bin_salvar(CAMINHO_BIN, banco). */
    (void)banco;
    return STATUS_NAO_IMPLEMENTADO;
}

Status csv_carregar(const char *caminho, Banco *banco)
{
    /* TODO: abrir com "r", pular o cabeçalho, ler linha a linha (fgets),
     *       separar os campos tratando aspas, converter nivel (strtol),
     *       montar a Pergunta e chamar banco_inserir(). */
    (void)caminho;
    (void)banco;
    return STATUS_NAO_IMPLEMENTADO;
}

Status csv_exportar(const char *caminho, const Banco *banco)
{
    /* TODO: abrir com "w", gravar o cabeçalho e uma linha por pergunta,
     *       colocando entre aspas os campos de texto. */
    (void)caminho;
    (void)banco;
    return STATUS_NAO_IMPLEMENTADO;
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
