/**
 * @file save.c
 * @brief Save do jogo em arquivo de texto (ver save.h). Só compila com USE_RAYLIB.
 */
#ifdef USE_RAYLIB

#include <stdio.h>
#include <errno.h>
#include "save.h"
#include "arquivo.h"
#include "partida.h"

/** Tamanho do buffer do caminho do temporário (CAMINHO_SAVE + ".tmp"). */
#define TAM_CAMINHO_SAVE 256

/** Primeira linha do arquivo: explica os números para quem abrir o save. */
#define CABECALHO_SAVE "# Mine do Milhao - save: cena esmeraldas eliminar_usada"

/** Maior nome de cena lido do arquivo (o sscanf usa %31s: TAM_NOME_CENA - 1). */
#define TAM_NOME_CENA 32

/** @brief true se a cena grava o save e o saldo não passa do prêmio acumulado até aquele boss. */
static bool save_valido(const Save *save)
{
    BossId boss = historia_boss_da_cena_salva(save->cena);
    int primeira_rodada;

    if (boss == BOSS_QUANTIDADE) {
        return false;   /* cena que não grava (intro, derrota...) ou nome desconhecido */
    }
    primeira_rodada = partida_primeira_rodada_do_nivel((int)boss + NIVEL_MIN);
    return save->esmeraldas >= 0 && save->esmeraldas <= partida_premio_da_rodada(primeira_rodada - 1);
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
    if (fprintf(arquivo, "%s\n%s %d %d\n", CABECALHO_SAVE, historia_nome_cena(save->cena), save->esmeraldas,
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
    char linha[128], nome_cena[TAM_NOME_CENA], sobra;
    FILE *arquivo;
    int esmeraldas, eliminacao, lidos = 0;
    Save lido;

    if (caminho == NULL || save == NULL) {
        return STATUS_ERRO_PARAMETRO;
    }
    arquivo = fopen(caminho, "r");
    if (arquivo == NULL) {
        return STATUS_ERRO_ARQUIVO;   /* sem save: normal na primeira vez */
    }

    /* Pula comentários (#) e linhas vazias; a primeira linha de dados tem os 3 números.
     * O 4º campo (%c) só é lido se sobrar texto: linhas com campos a mais são recusadas. */
    while (fgets(linha, sizeof(linha), arquivo) != NULL) {
        if (linha[0] == '#' || linha[0] == '\n' || linha[0] == '\r') {
            continue;
        }
        lidos = sscanf(linha, "%31s %d %d %c", nome_cena, &esmeraldas, &eliminacao, &sobra);
        break;
    }
    fclose(arquivo);

    if (lidos != 3 || (eliminacao != 0 && eliminacao != 1)) {
        return STATUS_ERRO_FORMATO;
    }
    lido.cena = historia_cena_pelo_nome(nome_cena);
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

#endif /* USE_RAYLIB */
