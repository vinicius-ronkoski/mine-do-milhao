/**
 * @file arquivo.h
 * @brief Persistência: leitura do CSV inicial, arquivo binário e exportação CSV.
 *
 * Formato do CSV (primeira linha é o cabeçalho e deve ser ignorada):
 * @code
 * nivel,enunciado,alternativa_a,alternativa_b,alternativa_c,alternativa_d,correta,dica
 * 1,"Qual mob explode?",Zumbi,Creeper,Esqueleto,Aranha,B,"Ele chia antes."
 * @endcode
 * enunciado e alternativas SEMPRE devem possuir aspas
 *
 * Formato do binário: um size_t com a quantidade de perguntas, seguido do
 * bloco de structs Pergunta gravado com fwrite().
 */

#ifndef ARQUIVO_H
#define ARQUIVO_H

#include "config.h"
#include "banco.h"

/**
 * @brief Carrega os dados no início do programa.
 *
 * Se CAMINHO_BIN existir, lê dele. Caso contrário (primeira execução),
 * lê CAMINHO_CSV e já cria o binário para as próximas execuções. Falha
 * ao criar o binário gera apenas um aviso, sem impedir o carregamento.
 *
 * @param banco Banco (vazio) que receberá as perguntas.
 * @return STATUS_OK ou um código de erro.
 */
Status dados_carregar(Banco *banco);

/**
 * @brief Salva todos os dados no binário ao sair do programa.
 *
 * Atenção: não sobrescrever o binário se o carregamento inicial falhou,
 * senão as perguntas antigas se perdem.
 *
 * @param banco Banco a gravar.
 * @return STATUS_OK ou um código de erro.
 */
Status dados_salvar(const Banco *banco);

/**
 * @brief Lê perguntas de um CSV e as insere no banco.
 *
 * @param caminho Caminho do arquivo .csv.
 * @param banco   Banco que recebe as perguntas.
 * @return STATUS_OK, STATUS_ERRO_ARQUIVO ou STATUS_ERRO_FORMATO.
 */
Status csv_carregar(const char *caminho, Banco *banco);

/**
 * @brief Exporta o banco atual para um arquivo CSV.
 *
 * @param caminho Caminho do arquivo a criar (ex.: PASTA_EXPORTADOS "perguntas.csv").
 * @param banco   Banco exportado.
 * @return STATUS_OK ou STATUS_ERRO_ARQUIVO.
 */
Status csv_exportar(const char *caminho, const Banco *banco);

/**
 * @brief Lê o banco inteiro do arquivo binário.
 *
 * @param caminho Caminho do .bin.
 * @param banco   Banco que recebe as perguntas.
 * @return STATUS_OK, STATUS_ERRO_ARQUIVO ou STATUS_ERRO_FORMATO.
 */
Status bin_carregar(const char *caminho, Banco *banco);

/**
 * @brief Grava o banco inteiro no arquivo binário.
 *
 * @param caminho Caminho do .bin.
 * @param banco   Banco a gravar.
 * @return STATUS_OK ou STATUS_ERRO_ARQUIVO.
 */
Status bin_salvar(const char *caminho, const Banco *banco);

#endif /* ARQUIVO_H */
