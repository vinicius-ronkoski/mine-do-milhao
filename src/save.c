/**
 * @file save.c
 * @brief Save do jogo em arquivo de texto (ver save.h). Sem raylib.
 */

#include <stdio.h>
#include <errno.h>
#include "save.h"
#include "arquivo.h"
#include "partida.h"

/** Tamanho do buffer do caminho do temporário (CAMINHO_SAVE + ".tmp"). */
#define TAM_CAMINHO_SAVE 256

/** Primeira linha do arquivo: explica os números para quem abrir o save. */
#define CABECALHO_SAVE "# Mine do Milhao - save: boss esmeraldas eliminar_usada"

/** @brief true se o conteúdo cabe nas regras do jogo (o saldo não passa do prêmio até o boss). */
static bool save_valido(const Save *save)
{
    int primeira_rodada, premio_ate_boss;

    if (save->boss < BOSS_WITHER || save->boss > BOSS_HEROBRINE) {
        return false;   /* o Creeper não é salvo: é o começo do jogo */
    }
    primeira_rodada = partida_primeira_rodada_do_nivel((int)save->boss + NIVEL_MIN);
    premio_ate_boss = partida_premio_da_rodada(primeira_rodada - 1);
    return save->esmeraldas >= 0 && save->esmeraldas <= premio_ate_boss;
}

Status save_gravar(const char *caminho, const Save *save)
{
    char temporario[TAM_CAMINHO_SAVE];
    FILE *arquivo;
    Status status = STATUS_OK;

    if (caminho == NULL || save == NULL || !save_valido(save)) {
        return STATUS_ERRO_PARAMETRO;
    }
    if (snprintf(temporario, sizeof(temporario), "%s.tmp", caminho) >= (int)sizeof(temporario)) {
        return STATUS_ERRO_PARAMETRO;
    }

    arquivo = fopen(temporario, "w");
    if (arquivo == NULL) {
        perror(temporario);
        return STATUS_ERRO_ARQUIVO;
    }
    if (fprintf(arquivo, "%s\n%d %d %d\n", CABECALHO_SAVE, (int)save->boss, save->esmeraldas,
                save->eliminacao_usada ? 1 : 0) < 0) {
        status = STATUS_ERRO_ARQUIVO;
    }
    if (fclose(arquivo) != 0) {
        status = STATUS_ERRO_ARQUIVO;
    }

    /* Só troca o save antigo se o novo foi gravado inteiro */
    if (status == STATUS_OK && !arquivo_substituir(temporario, caminho)) {
        perror(caminho);
        status = STATUS_ERRO_ARQUIVO;
    }
    if (status != STATUS_OK) {
        remove(temporario);
    }
    return status;
}

Status save_ler(const char *caminho, Save *save)
{
    char linha[128];
    FILE *arquivo;
    int boss, esmeraldas, eliminacao, lidos = 0;
    Save lido;

    if (caminho == NULL || save == NULL) {
        return STATUS_ERRO_PARAMETRO;
    }
    arquivo = fopen(caminho, "r");
    if (arquivo == NULL) {
        return STATUS_ERRO_ARQUIVO;   /* sem save: normal na primeira vez */
    }

    /* Pula comentários (#) e linhas vazias; a primeira linha de dados tem os 3 números */
    while (fgets(linha, sizeof(linha), arquivo) != NULL) {
        if (linha[0] == '#' || linha[0] == '\n' || linha[0] == '\r') {
            continue;
        }
        lidos = sscanf(linha, "%d %d %d", &boss, &esmeraldas, &eliminacao);
        break;
    }
    fclose(arquivo);

    if (lidos != 3 || (eliminacao != 0 && eliminacao != 1)) {
        return STATUS_ERRO_FORMATO;
    }
    lido.boss = (BossId)boss;
    lido.esmeraldas = esmeraldas;
    lido.eliminacao_usada = eliminacao == 1;
    if (!save_valido(&lido)) {
        return STATUS_ERRO_FORMATO;   /* editado à mão ou corrompido: ignorado */
    }
    *save = lido;
    return STATUS_OK;
}

Status save_apagar(const char *caminho)
{
    if (caminho == NULL) {
        return STATUS_ERRO_PARAMETRO;
    }
    if (remove(caminho) != 0 && errno != ENOENT) {
        perror(caminho);
        return STATUS_ERRO_ARQUIVO;
    }
    return STATUS_OK;
}
