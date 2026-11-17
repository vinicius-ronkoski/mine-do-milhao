/**
 * @file menu.c
 * @brief Implementação do menu principal em modo texto.
 *
 * TODO (equipe): trocar as chamadas restantes a tela_em_construcao() pela
 * função real da opção (cadastrar, listar, navegar por nível, exportar).
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "menu.h"
#include "utils.h"

/** Opção "sair" do menu. */
#define OPCAO_SAIR 0

/** Maior número de opção do menu. */
#define OPCAO_MAX 7

/**
 * @brief Desenha as opções do menu principal.
 */
static void exibir_menu(void)
{
    printf(COR_VERDE "=====================================\n");
    printf("        MINE DO MILHÃO - MENU\n");
    printf("=====================================\n" COR_RESET);
    printf(" 1 - Cadastrar pergunta\n");
    printf(" 2 - Listar todas as perguntas\n");
    printf(" 3 - Pesquisar pergunta\n");
    printf(" 4 - Alterar pergunta\n");
    printf(" 5 - Excluir pergunta\n");
    printf(" 6 - Navegar por nível de dificuldade\n");
    printf(" 7 - Exportar perguntas para CSV\n");
    printf(" 0 - Sair\n\n");
}

/**
 * @brief Tela provisória de uma opção ainda não implementada.
 *
 * @param titulo Nome da opção.
 * @param banco  Banco de perguntas (usado só para mostrar o total).
 */
static void tela_em_construcao(const char *titulo, const Banco *banco)
{
    utils_limpar_tela();
    printf(COR_AMARELO "== %s ==\n" COR_RESET, titulo);
    printf("Perguntas no banco: %zu\n", banco->quantidade);
    printf("Em construção...\n");
    utils_pausar();
}

/**
 * @brief Pergunta ao usuário um número de resultado (1 a total).
 *
 * @return 1 se escolheu; 0 se cancelou (digitou 0) ou a entrada acabou.
 */
static int escolher_resultado(const char *prompt, size_t total, size_t *escolhido)
{
    int numero;

    if (!utils_ler_inteiro(prompt, 0, (int)total, &numero) || numero == 0) {
        return 0;
    }
    *escolhido = (size_t)(numero - 1);
    return 1;
}

/**
 * @brief Lista resumida dos resultados (posição, nível e enunciado).
 *
 * @param banco   Banco consultado.
 * @param indices Posições dos resultados dentro do banco.
 * @param total   Quantidade de resultados.
 */
static void listar_resultados(const Banco *banco, const size_t *indices, size_t total)
{
    size_t i;

    for (i = 0; i < total; i++) {
        const Pergunta *p = banco_obter(banco, indices[i]);

        printf(COR_AMARELO "%3zu" COR_RESET " - [N%d] %.70s%s\n",
               i + 1, p->nivel, p->enunciado, strlen(p->enunciado) > 70 ? "..." : "");
    }
}

/**
 * @brief Pede um trecho do enunciado, busca no banco e deixa o usuário escolher um resultado.
 *
 * Usada por pesquisar, alterar e excluir, pois as três começam do mesmo jeito.
 *
 * @param banco     Banco consultado.
 * @param acao      Verbo mostrado no prompt (ex.: "alterar").
 * @param indices   Vetor de saída (precisa ter banco->quantidade posições).
 * @param escolhido Recebe a posição, DENTRO DO BANCO, da pergunta escolhida.
 * @return 1 se o usuário escolheu uma pergunta; 0 se nada foi escolhido.
 */
static int buscar_e_escolher(const Banco *banco, const char *acao,
                             size_t *indices, size_t *escolhido)
{
    char trecho[TAM_ENUNCIADO];
    char prompt[64];
    size_t total, pos;

    if (!utils_ler_linha("Digite um trecho do enunciado: ", trecho, sizeof trecho)) {
        return 0;
    }
    if (trecho[0] == '\0') {
        utils_beep();
        printf(COR_VERMELHO "O trecho não pode ser vazio.\n" COR_RESET);
        return 0;
    }

    total = banco_buscar_texto(banco, trecho, indices, banco->quantidade);
    if (total == 0) {
        printf(COR_VERMELHO "\n%s\n" COR_RESET, utils_mensagem_status(STATUS_NAO_ENCONTRADO));
        return 0;
    }

    printf("\n%zu pergunta(s) encontrada(s):\n\n", total);
    listar_resultados(banco, indices, total);

    snprintf(prompt, sizeof prompt, "\nNúmero da pergunta a %s (0 cancela): ", acao);
    if (!escolher_resultado(prompt, total, &pos)) {
        return 0;
    }
    *escolhido = indices[pos];
    return 1;
}

/**
 * @brief Lê um campo de texto obrigatório; ao alterar, ENTER mantém o valor atual.
 *
 * @param rotulo   Nome do campo.
 * @param destino  Buffer que recebe o texto.
 * @param tamanho  Tamanho do buffer.
 * @param atual    Valor atual (NULL ao cadastrar: então o campo é obrigatório).
 * @return 1 se o campo ficou preenchido; 0 se a entrada terminou.
 */
static int ler_campo(const char *rotulo, char *destino, size_t tamanho, const char *atual)
{
    char prompt[96];
    char buffer[TAM_ENUNCIADO];

    if (atual != NULL) {
        snprintf(prompt, sizeof prompt, "%s (ENTER mantém): ", rotulo);
    } else {
        snprintf(prompt, sizeof prompt, "%s: ", rotulo);
    }

    for (;;) {
        if (!utils_ler_linha(prompt, buffer, tamanho < sizeof buffer ? tamanho : sizeof buffer)) {
            return 0;
        }
        if (buffer[0] == '\0' && atual != NULL) {
            return 1; /* mantém o que já estava em destino */
        }
        /* O CSV usa aspas como delimitador, então elas não podem aparecer no texto. */
        if (buffer[0] != '\0' && strchr(buffer, '"') == NULL) {
            strcpy(destino, buffer);
            return 1;
        }
        utils_beep();
        printf(COR_VERMELHO "Campo obrigatório e sem aspas duplas (\").\n" COR_RESET);
    }
}

/**
 * @brief Lê a letra da alternativa correta (A a D); ENTER mantém a atual.
 *
 * @return 1 se leu; 0 se a entrada terminou.
 */
static int ler_correta(char *correta)
{
    char buffer[16];

    for (;;) {
        if (!utils_ler_linha("Alternativa correta A-D (ENTER mantém): ", buffer, sizeof buffer)) {
            return 0;
        }
        if (buffer[0] == '\0') {
            return 1;
        }
        if (buffer[1] == '\0' && toupper((unsigned char)buffer[0]) >= 'A'
            && toupper((unsigned char)buffer[0]) < 'A' + NUM_ALTERNATIVAS) {
            *correta = (char)toupper((unsigned char)buffer[0]);
            return 1;
        }
        utils_beep();
        printf(COR_VERMELHO "Digite uma letra de A a %c.\n" COR_RESET, 'A' + NUM_ALTERNATIVAS - 1);
    }
}

/**
 * @brief Lê o nível de dificuldade; ENTER mantém o atual.
 *
 * @return 1 se leu; 0 se a entrada terminou.
 */
static int ler_nivel(int *nivel)
{
    char buffer[16];
    char *fim;
    long n;

    for (;;) {
        if (!utils_ler_linha("Nível 1-5 (ENTER mantém): ", buffer, sizeof buffer)) {
            return 0;
        }
        if (buffer[0] == '\0') {
            return 1;
        }
        n = strtol(buffer, &fim, 10);
        if (*fim == '\0' && n >= NIVEL_MIN && n <= NIVEL_MAX) {
            *nivel = (int)n;
            return 1;
        }
        utils_beep();
        printf(COR_VERMELHO "Digite um número de %d a %d.\n" COR_RESET, NIVEL_MIN, NIVEL_MAX);
    }
}

/**
 * @brief Tela "Pesquisar pergunta": busca por trecho do enunciado e mostra a escolhida.
 */
static void tela_pesquisar(const Banco *banco)
{
    size_t *indices, escolhido;

    utils_limpar_tela();
    printf(COR_AMARELO "== Pesquisar pergunta ==\n\n" COR_RESET);

    if (banco->quantidade == 0) {
        printf("O banco está vazio.\n");
        utils_pausar();
        return;
    }

    indices = malloc(banco->quantidade * sizeof *indices);
    if (indices == NULL) {
        printf(COR_VERMELHO "%s\n" COR_RESET, utils_mensagem_status(STATUS_ERRO_MEMORIA));
        utils_pausar();
        return;
    }

    if (buscar_e_escolher(banco, "exibir", indices, &escolhido)) {
        printf("\n");
        pergunta_imprimir(banco_obter(banco, escolhido));
    }

    free(indices);
    utils_pausar();
}

/**
 * @brief Tela "Alterar pergunta": busca, mostra a atual e pede os novos dados.
 *
 * Trabalha numa CÓPIA da pergunta: só grava no banco se tudo for válido.
 */
static void tela_alterar(Banco *banco)
{
    size_t *indices, escolhido;
    Pergunta copia;
    Status status;
    int i, ok;

    utils_limpar_tela();
    printf(COR_AMARELO "== Alterar pergunta ==\n\n" COR_RESET);

    if (banco->quantidade == 0) {
        printf("O banco está vazio.\n");
        utils_pausar();
        return;
    }

    indices = malloc(banco->quantidade * sizeof *indices);
    if (indices == NULL) {
        printf(COR_VERMELHO "%s\n" COR_RESET, utils_mensagem_status(STATUS_ERRO_MEMORIA));
        utils_pausar();
        return;
    }

    if (buscar_e_escolher(banco, "alterar", indices, &escolhido)) {
        copia = *banco_obter(banco, escolhido);

        printf("\nPergunta atual:\n");
        pergunta_imprimir(&copia);
        printf("\nDigite os novos dados (ENTER mantém o valor atual):\n\n");

        ok = ler_campo("Enunciado", copia.enunciado, TAM_ENUNCIADO, copia.enunciado);
        for (i = 0; ok && i < NUM_ALTERNATIVAS; i++) {
            char rotulo[32];

            snprintf(rotulo, sizeof rotulo, "Alternativa %c", 'A' + i);
            ok = ler_campo(rotulo, copia.alternativas[i], TAM_ALTERNATIVA, copia.alternativas[i]);
        }
        ok = ok && ler_correta(&copia.correta);
        ok = ok && ler_nivel(&copia.nivel);
        ok = ok && ler_campo("Dica", copia.dica, TAM_DICA, copia.dica);

        if (ok) {
            status = banco_alterar(banco, escolhido, &copia);
            if (status == STATUS_OK) {
                printf(COR_VERDE "\nPergunta alterada com sucesso!\n" COR_RESET);
            } else {
                utils_beep();
                printf(COR_VERMELHO "\nNão foi possível alterar: %s\n" COR_RESET,
                       utils_mensagem_status(status));
            }
        }
    }

    free(indices);
    utils_pausar();
}

/**
 * @brief Tela "Excluir pergunta": busca, mostra a escolhida e pede confirmação.
 */
static void tela_excluir(Banco *banco)
{
    size_t *indices, escolhido;
    Status status;
    char resposta[8];

    utils_limpar_tela();
    printf(COR_AMARELO "== Excluir pergunta ==\n\n" COR_RESET);

    if (banco->quantidade == 0) {
        printf("O banco está vazio.\n");
        utils_pausar();
        return;
    }

    indices = malloc(banco->quantidade * sizeof *indices);
    if (indices == NULL) {
        printf(COR_VERMELHO "%s\n" COR_RESET, utils_mensagem_status(STATUS_ERRO_MEMORIA));
        utils_pausar();
        return;
    }

    if (buscar_e_escolher(banco, "excluir", indices, &escolhido)) {
        printf("\n");
        pergunta_imprimir(banco_obter(banco, escolhido));

        if (utils_ler_linha(COR_VERMELHO "\nConfirma a exclusão? (s/N): " COR_RESET,
                            resposta, sizeof resposta)
            && (resposta[0] == 's' || resposta[0] == 'S')) {
            status = banco_remover(banco, escolhido);
            if (status == STATUS_OK) {
                printf(COR_VERDE "\nPergunta excluída com sucesso!\n" COR_RESET);
            } else {
                utils_beep();
                printf(COR_VERMELHO "\nNão foi possível excluir: %s\n" COR_RESET,
                       utils_mensagem_status(status));
            }
        } else {
            printf("\nExclusão cancelada.\n");
        }
    }

    free(indices);
    utils_pausar();
}

void menu_principal(Banco *banco)
{
    int opcao = OPCAO_SAIR;

    do {
        utils_limpar_tela();
        exibir_menu();

        if (!utils_ler_inteiro("Escolha uma opção: ", OPCAO_SAIR, OPCAO_MAX, &opcao)) {
            break; /* fim da entrada (EOF) */
        }

        switch (opcao) {
        case 1: tela_em_construcao("Cadastrar pergunta", banco); break;
        case 2: tela_em_construcao("Listar perguntas", banco); break;
        case 3: tela_pesquisar(banco); break;
        case 4: tela_alterar(banco); break;
        case 5: tela_excluir(banco); break;
        case 6: tela_em_construcao("Navegar por nível", banco); break;
        case 7: tela_em_construcao("Exportar CSV", banco); break;
        default: break;
        }
    } while (opcao != OPCAO_SAIR);
}
