/**
 * @file arquivo.h
 * @brief Persistência das perguntas: CSV inicial, binário (.dat) e exportação CSV.
 *
 * Fluxo: main -> dados_carregar() (lê o .dat; na 1ª execução lê o CSV e cria o .dat)
 *        -> menu -> dados_salvar() ao sair.
 * CSV: cabeçalho CSV_CABECALHO; enunciado, alternativas e dica SEMPRE entre aspas.
 * .dat: um size_t com a quantidade + o bloco de structs Pergunta (fwrite). Fica fora do Git,
 * pois size_t e o alinhamento da struct mudam entre compiladores de 32 e 64 bits.
 */

#ifndef ARQUIVO_H
#define ARQUIVO_H

#include "config.h"
#include "banco.h"

/**
 * @brief Carrega o banco no início: CAMINHO_DAT se existir; senão csv_carregar() e cria o .dat.
 * @param banco Banco vazio que recebe as perguntas.
 * @return STATUS_OK ou código de erro (falha só ao criar o .dat é apenas um aviso).
 */
Status dados_carregar(Banco *banco);

/**
 * @brief Grava o banco em CAMINHO_DAT ao sair (dat_salvar). Não chamar se o carregamento falhou.
 * @return STATUS_OK ou código de erro.
 */
Status dados_salvar(const Banco *banco);

/**
 * @brief Lê um CSV e insere cada pergunta válida no banco (para na primeira linha inválida).
 * @return STATUS_OK, STATUS_ERRO_ARQUIVO ou STATUS_ERRO_FORMATO.
 */
Status csv_carregar(const char *caminho, Banco *banco);

/**
 * @brief Exporta o banco para um CSV no mesmo formato do inicial (opção "Exportar" do menu).
 * @param caminho Arquivo a criar (ex.: CAMINHO_EXPORTADOS "perguntas.csv").
 * @return STATUS_OK ou STATUS_ERRO_ARQUIVO.
 */
Status csv_exportar(const char *caminho, const Banco *banco);

/**
 * @brief Lê o .dat inteiro; só substitui o banco se o arquivo estiver íntegro.
 * @return STATUS_OK, STATUS_ERRO_ARQUIVO ou STATUS_ERRO_FORMATO.
 */
Status dat_carregar(const char *caminho, Banco *banco);

/**
 * @brief Grava o banco no .dat (via arquivo temporário: o .dat antigo só é trocado se der certo).
 * @return STATUS_OK ou STATUS_ERRO_ARQUIVO.
 */
Status dat_salvar(const char *caminho, const Banco *banco);

#endif /* ARQUIVO_H */
