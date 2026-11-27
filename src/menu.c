/**
 * @file menu.c
 * @brief Implementação do menu principal em modo texto.
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "arquivo.h"
#include "menu.h"
#include "utils.h"
#ifdef USE_RAYLIB
    #include "jogo.h"
#endif

/** Opção "sair" do menu. */
#define OPCAO_SAIR 0

/** Opção que abre o jogo visual (raylib). */
#define OPCAO_JOGAR 8

/** Maior número de opção do menu. */
#define OPCAO_MAX OPCAO_JOGAR

/** Quantas perguntas aparecem por tela ao listar todas (cabem em um terminal de 30 linhas). */
#define PERGUNTAS_POR_PAGINA 2

/** Tamanho do buffer com o nome do arquivo exportado (inclui o ".csv"). */
#define TAM_NOME_EXPORTACAO 64

/** Nome sugerido quando o usuário só aperta ENTER ao exportar. */
#define NOME_EXPORTACAO_PADRAO "perguntas.csv"

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
    printf(COR_AMARELO " 8 - JOGAR (Mine do Milhão)\n" COR_RESET);
    printf(" 0 - Sair\n\n");
}

/**
 * @brief Pergunta ao usuário um número de resultado (1 a total).
 *
 * @return true se escolheu; false se cancelou (digitou 0) ou a entrada acabou.
 */
static bool escolher_resultado(const char *prompt, size_t total, size_t *escolhido)
{
    int numero;

    if (utils_ler_int(prompt, 0, (int)total, &numero) != STATUS_OK || numero == 0) {
        return false;
    }
    *escolhido = (size_t)(numero - 1);
    return true;
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
        const Pergunta *pergunta = banco_obter(banco, indices[i]);

        printf(COR_AMARELO "%3zu" COR_RESET " - [N%d] %.70s%s\n",
               i + 1, pergunta->nivel, pergunta->enunciado, strlen(pergunta->enunciado) > 70 ? "..." : "");
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
 * @return true se o usuário escolheu uma pergunta; false se nada foi escolhido.
 */
static bool buscar_e_escolher(const Banco *banco, const char *acao,
                             size_t *indices, size_t *escolhido)
{
    char trecho[TAM_ENUNCIADO];
    char prompt[64];
    size_t total, pos;

    if (utils_ler_linha("Digite um trecho do enunciado: ", trecho, sizeof(trecho)) != STATUS_OK) {
        return false;
    }
    if (trecho[0] == '\0') {
        utils_beep();
        printf(COR_VERMELHO "O trecho não pode ser vazio.\n" COR_RESET);
        return false;
    }

    total = banco_buscar_texto(banco, trecho, indices, banco->quantidade);
    if (total == 0) {
        printf(COR_VERMELHO "\n%s\n" COR_RESET, utils_mensagem_status(STATUS_NAO_ENCONTRADO));
        return false;
    }

    printf("\n%zu pergunta(s) encontrada(s):\n\n", total);
    listar_resultados(banco, indices, total);

    snprintf(prompt, sizeof(prompt), "\nNúmero da pergunta a %s (0 cancela): ", acao);
    if (!escolher_resultado(prompt, total, &pos)) {
        return false;
    }
    *escolhido = indices[pos];
    return true;
}

/**
 * @brief Lê um campo de texto obrigatório; ao alterar, ENTER mantém o valor atual.
 *
 * @param rotulo   Nome do campo.
 * @param destino  Buffer que recebe o texto.
 * @param tamanho  Tamanho do buffer.
 * @param atual    Valor atual (NULL ao cadastrar: então o campo é obrigatório).
 * @return true se o campo ficou preenchido; false se a entrada terminou.
 */
static bool ler_campo(const char *rotulo, char *destino, size_t tamanho, const char *atual)
{
    char prompt[96];
    char buffer[TAM_ENUNCIADO];

    if (atual != NULL) {
        snprintf(prompt, sizeof(prompt), "%s (ENTER mantém): ", rotulo);
    } else {
        snprintf(prompt, sizeof(prompt), "%s: ", rotulo);
    }

    for (;;) {
        if (utils_ler_linha(prompt, buffer, tamanho < sizeof(buffer) ? tamanho : sizeof(buffer)) != STATUS_OK) {
            return false;
        }
        if (buffer[0] == '\0' && atual != NULL) {
            return true; /* mantém o que já estava em destino */
        }
        /* O CSV usa aspas como delimitador, então elas não podem aparecer no texto. */
        if (buffer[0] != '\0' && strchr(buffer, '"') == NULL) {
            strcpy(destino, buffer);
            return true;
        }
        utils_beep();
        printf(COR_VERMELHO "Campo obrigatório e sem aspas duplas (\").\n" COR_RESET);
    }
}

/**
 * @brief Lê a letra da alternativa correta (A a D).
 *
 * Se *correta já contém uma letra válida (alterar), ENTER mantém o valor atual.
 * Se não (cadastrar, com a Pergunta zerada), o campo é obrigatório.
 *
 * @param correta Letra atual (entrada) e letra lida (saída).
 * @return true se leu; false se a entrada terminou.
 */
static bool ler_correta(char *correta)
{
    char buffer[16];
    bool tem_valor_atual = (*correta >= 'A' && *correta < 'A' + NUM_ALTERNATIVAS);
    const char *prompt = tem_valor_atual ? "Alternativa correta A-D (ENTER mantém): "
                                         : "Alternativa correta A-D: ";

    for (;;) {
        if (utils_ler_linha(prompt, buffer, sizeof(buffer)) != STATUS_OK) {
            return false;
        }
        if (buffer[0] == '\0' && tem_valor_atual) {
            return true;
        }
        if (buffer[0] != '\0' && buffer[1] == '\0' && toupper((unsigned char)buffer[0]) >= 'A'
            && toupper((unsigned char)buffer[0]) < 'A' + NUM_ALTERNATIVAS) {
            *correta = (char)toupper((unsigned char)buffer[0]);
            return true;
        }
        utils_beep();
        printf(COR_VERMELHO "Digite uma letra de A a %c.\n" COR_RESET, 'A' + NUM_ALTERNATIVAS - 1);
    }
}

/**
 * @brief Lê o nível de dificuldade.
 *
 * Se *nivel já contém um nível válido (alterar), ENTER mantém o valor atual.
 * Se não (cadastrar, com a Pergunta zerada), o campo é obrigatório.
 *
 * @param nivel Nível atual (entrada) e nível lido (saída).
 * @return true se leu; false se a entrada terminou.
 */
static bool ler_nivel(int *nivel)
{
    char buffer[16];
    char prompt[64];
    char *fim;
    long numero;
    bool tem_valor_atual = (*nivel >= NIVEL_MIN && *nivel <= NIVEL_MAX);

    snprintf(prompt, sizeof(prompt), "Nível %d-%d%s: ", NIVEL_MIN, NIVEL_MAX,
             tem_valor_atual ? " (ENTER mantém)" : "");

    for (;;) {
        if (utils_ler_linha(prompt, buffer, sizeof(buffer)) != STATUS_OK) {
            return false;
        }
        if (buffer[0] == '\0' && tem_valor_atual) {
            return true;
        }
        numero = strtol(buffer, &fim, 10);
        if (buffer[0] != '\0' && *fim == '\0' && numero >= NIVEL_MIN && numero <= NIVEL_MAX) {
            *nivel = (int)numero;
            return true;
        }
        utils_beep();
        printf(COR_VERMELHO "Digite um número de %d a %d.\n" COR_RESET, NIVEL_MIN, NIVEL_MAX);
    }
}

/**
 * @brief Mostra perguntas página a página e deixa o usuário navegar entre elas.
 *
 * Usada por "Listar todas" (várias perguntas por tela) e por "Navegar por nível"
 * (uma pergunta por tela). Comandos: P (ou ENTER) próxima, A anterior, S sai e um
 * número pula direto para aquela pergunta.
 *
 * @param banco      Banco consultado.
 * @param indices    Posições, dentro do banco, das perguntas a mostrar.
 * @param total      Quantidade de posições em indices (maior que zero).
 * @param titulo     Título exibido no topo da tela.
 * @param por_pagina Quantas perguntas aparecem por tela (maior que zero).
 */
static void exibir_paginas(const Banco *banco, const size_t *indices, size_t total,
                           const char *titulo, size_t por_pagina)
{
    size_t primeira = 0;        /* Posição, em indices, da primeira pergunta da tela */
    size_t ultima, i;           /* ultima = uma posição depois da última exibida */
    const char *aviso = NULL;   /* Mensagem de erro do comando anterior */
    char comando[8];
    char *fim;
    long numero;

    for (;;) {
        utils_limpar_tela();
        printf(COR_AMARELO "== %s ==\n\n" COR_RESET, titulo);

        ultima = primeira + por_pagina;
        if (ultima > total) {
            ultima = total;
        }

        for (i = primeira; i < ultima; i++) {
            printf(COR_AZUL "Pergunta %zu de %zu" COR_RESET "\n", i + 1, total);
            pergunta_imprimir(banco_obter(banco, indices[i]));
            printf("\n");
        }

        if (aviso != NULL) {
            printf(COR_VERMELHO "%s\n" COR_RESET, aviso);
            aviso = NULL;
        }
        printf("[P] próxima  [A] anterior  [S] sair  ou digite o nº da pergunta\n");

        if (utils_ler_linha("Comando: ", comando, sizeof(comando)) != STATUS_OK) {
            return; /* fim da entrada (EOF) */
        }

        switch (toupper((unsigned char)comando[0])) {
        case '\0':  /* ENTER sozinho também avança */
        case 'P':
            if (ultima < total) {
                primeira = ultima;
            } else {
                aviso = "Esta é a última página.";
                utils_beep();
            }
            break;
        case 'A':
            if (primeira == 0) {
                aviso = "Esta é a primeira página.";
                utils_beep();
            } else if (primeira >= por_pagina) {
                primeira -= por_pagina;
            } else {
                primeira = 0;
            }
            break;
        case 'S':
            return;
        default:
            numero = strtol(comando, &fim, 10);
            if (isdigit((unsigned char)comando[0]) && *fim == '\0'
                && numero >= 1 && (size_t)numero <= total) {
                /* Arredonda para o começo da página que contém a pergunta pedida */
                primeira = ((size_t)numero - 1) / por_pagina * por_pagina;
            } else {
                aviso = "Comando inválido. Use P, A, S ou o número de uma pergunta.";
                utils_beep();
            }
            break;
        }
    }
}

/**
 * @brief Tela "Cadastrar pergunta": lê todos os campos e insere no banco.
 *
 * A Pergunta começa zerada, então todos os campos são obrigatórios.
 */
static void tela_cadastrar(Banco *banco)
{
    Pergunta nova;
    Status status;
    int i, ok;

    utils_limpar_tela();
    printf(COR_AMARELO "== Cadastrar pergunta ==\n\n" COR_RESET);

    printf("Níveis: ");
    for (i = NIVEL_MIN; i <= NIVEL_MAX; i++) {
        printf("%d=%s%s", i, pergunta_nome_nivel(i), i < NIVEL_MAX ? ", " : "\n\n");
    }

    /* Zera tudo: os campos vazios são o sinal de "ainda não preenchido" */
    memset(&nova, 0, sizeof(nova));

    ok = ler_campo("Enunciado", nova.enunciado, TAM_ENUNCIADO, NULL);
    for (i = 0; ok && i < NUM_ALTERNATIVAS; i++) {
        char rotulo[32];

        snprintf(rotulo, sizeof(rotulo), "Alternativa %c", 'A' + i);
        ok = ler_campo(rotulo, nova.alternativas[i], TAM_ALTERNATIVA, NULL);
    }
    ok = ok && ler_correta(&nova.correta);
    ok = ok && ler_nivel(&nova.nivel);
    ok = ok && ler_campo("Dica", nova.dica, TAM_DICA, NULL);

    if (ok) {
        status = banco_inserir(banco, &nova);
        if (status == STATUS_OK) {
            printf(COR_VERDE "\nPergunta cadastrada com sucesso! Total no banco: %zu\n\n" COR_RESET,
                   banco->quantidade);
            pergunta_imprimir(&nova);
        } else {
            utils_beep();
            printf(COR_VERMELHO "\nNão foi possível cadastrar: %s\n" COR_RESET,
                   utils_mensagem_status(status));
        }
    }

    utils_pausar();
}

/**
 * @brief Tela "Listar todas as perguntas": mostra o banco inteiro, página a página.
 */
static void tela_listar(const Banco *banco)
{
    size_t *indices, i;

    if (banco->quantidade == 0) {
        utils_limpar_tela();
        printf(COR_AMARELO "== Listar perguntas ==\n\n" COR_RESET);
        printf("O banco está vazio.\n");
        utils_pausar();
        return;
    }

    /* Todas as posições, na ordem: reaproveita o mesmo visualizador da navegação por nível */
    indices = malloc(banco->quantidade * sizeof(*indices));
    if (indices == NULL) {
        printf(COR_VERMELHO "%s\n" COR_RESET, utils_mensagem_status(STATUS_ERRO_MEMORIA));
        utils_pausar();
        return;
    }
    for (i = 0; i < banco->quantidade; i++) {
        indices[i] = i;
    }

    exibir_paginas(banco, indices, banco->quantidade, "Todas as perguntas", PERGUNTAS_POR_PAGINA);

    free(indices);
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

    indices = malloc(banco->quantidade * sizeof(*indices));
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

    indices = malloc(banco->quantidade * sizeof(*indices));
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

            snprintf(rotulo, sizeof(rotulo), "Alternativa %c", 'A' + i);
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

    indices = malloc(banco->quantidade * sizeof(*indices));
    if (indices == NULL) {
        printf(COR_VERMELHO "%s\n" COR_RESET, utils_mensagem_status(STATUS_ERRO_MEMORIA));
        utils_pausar();
        return;
    }

    if (buscar_e_escolher(banco, "excluir", indices, &escolhido)) {
        printf("\n");
        pergunta_imprimir(banco_obter(banco, escolhido));

        if (utils_ler_linha(COR_VERMELHO "\nConfirma a exclusão? (s/N): " COR_RESET,
                            resposta, sizeof(resposta)) == STATUS_OK
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

/**
 * @brief Tela "Navegar por nível": escolhe um nível e percorre suas perguntas uma a uma.
 */
static void tela_navegar_nivel(const Banco *banco)
{
    size_t *indices, total;
    char titulo[64];
    int nivel, opcao = 0;

    if (banco->quantidade == 0) {
        utils_limpar_tela();
        printf(COR_AMARELO "== Navegar por nível ==\n\n" COR_RESET);
        printf("O banco está vazio.\n");
        utils_pausar();
        return;
    }

    indices = malloc(banco->quantidade * sizeof(*indices));
    if (indices == NULL) {
        printf(COR_VERMELHO "%s\n" COR_RESET, utils_mensagem_status(STATUS_ERRO_MEMORIA));
        utils_pausar();
        return;
    }

    do {
        utils_limpar_tela();
        printf(COR_AMARELO "== Navegar por nível de dificuldade ==\n\n" COR_RESET);

        /* Mostra quantas perguntas existem em cada nível */
        for (nivel = NIVEL_MIN; nivel <= NIVEL_MAX; nivel++) {
            total = banco_filtrar_nivel(banco, nivel, indices, banco->quantidade);
            printf(" %d - %s (%zu pergunta(s))\n", nivel, pergunta_nome_nivel(nivel), total);
        }
        printf(" 0 - Voltar\n\n");

        if (utils_ler_int("Escolha o nível: ", 0, NIVEL_MAX, &opcao) != STATUS_OK) {
            break; /* fim da entrada (EOF) */
        }

        if (opcao != 0) {
            total = banco_filtrar_nivel(banco, opcao, indices, banco->quantidade);
            if (total == 0) {
                utils_beep();
                printf(COR_VERMELHO "\nNenhuma pergunta neste nível.\n" COR_RESET);
                utils_pausar();
            } else {
                snprintf(titulo, sizeof(titulo), "Nível %d - %s", opcao, pergunta_nome_nivel(opcao));
                exibir_paginas(banco, indices, total, titulo, 1);
            }
        }
    } while (opcao != 0);

    free(indices);
}

/**
 * @brief Confere se o nome do arquivo termina com ".csv".
 *
 * @param nome Nome do arquivo.
 * @return true se termina com ".csv"; false caso contrário.
 */
static bool termina_com_csv(const char *nome)
{
    size_t tamanho = strlen(nome);

    return tamanho >= 4 && strcmp(nome + tamanho - 4, ".csv") == 0;
}

/**
 * @brief Tela "Exportar CSV": grava o banco atual em CAMINHO_EXPORTADOS.
 *
 * O arquivo gerado tem o mesmo formato do CSV inicial, então pode ser lido de volta.
 */
static void tela_exportar(const Banco *banco)
{
    char nome[TAM_NOME_EXPORTACAO];
    char caminho[sizeof(CAMINHO_EXPORTADOS) + TAM_NOME_EXPORTACAO];
    char resposta[8];
    FILE *existente;
    Status status;

    utils_limpar_tela();
    printf(COR_AMARELO "== Exportar perguntas para CSV ==\n\n" COR_RESET);

    if (banco->quantidade == 0) {
        printf("O banco está vazio, não há o que exportar.\n");
        utils_pausar();
        return;
    }

    printf("Os arquivos são gravados na pasta %s\n\n", CAMINHO_EXPORTADOS);
    if (utils_ler_linha("Nome do arquivo (ENTER = " NOME_EXPORTACAO_PADRAO "): ", nome, sizeof(nome)) != STATUS_OK) {
        return; /* fim da entrada (EOF) */
    }
    if (nome[0] == '\0') {
        strcpy(nome, NOME_EXPORTACAO_PADRAO);
    }

    /* Barras e outros símbolos permitiriam gravar fora da pasta ou gerar nome inválido no Windows */
    if (strpbrk(nome, "/\\:*?\"<>|") != NULL) {
        utils_beep();
        printf(COR_VERMELHO "\nNome inválido: não use / \\ : * ? \" < > |\n" COR_RESET);
        utils_pausar();
        return;
    }
    if (!termina_com_csv(nome)) {
        if (strlen(nome) + strlen(".csv") >= sizeof(nome)) {
            utils_beep();
            printf(COR_VERMELHO "\nNome muito longo.\n" COR_RESET);
            utils_pausar();
            return;
        }
        strcat(nome, ".csv");
    }
    snprintf(caminho, sizeof(caminho), "%s%s", CAMINHO_EXPORTADOS, nome);

    /* Se o arquivo já existe (conseguiu abrir para leitura), pede confirmação antes de sobrescrever */
    existente = fopen(caminho, "r");
    if (existente != NULL) {
        fclose(existente);
        if (utils_ler_linha(COR_AMARELO "\nO arquivo já existe. Sobrescrever? (s/N): " COR_RESET,
                            resposta, sizeof(resposta)) != STATUS_OK
            || (resposta[0] != 's' && resposta[0] != 'S')) {
            printf("\nExportação cancelada.\n");
            utils_pausar();
            return;
        }
    }

    status = csv_exportar(caminho, banco);
    if (status == STATUS_OK) {
        printf(COR_VERDE "\n%zu pergunta(s) exportada(s) para %s\n" COR_RESET,
               banco->quantidade, caminho);
    } else {
        utils_beep();
        printf(COR_VERMELHO "\nNão foi possível exportar: %s\n" COR_RESET,
               utils_mensagem_status(status));
    }

    utils_pausar();
}

/**
 * @brief Tela "Jogar": abre a janela do jogo (raylib) e volta ao menu quando ela fecha.
 *
 * Sem USE_RAYLIB o jogo não existe no executável; avisa como compilar.
 * Assets que faltarem não impedem o jogo (fallback), só geram um aviso.
 */
static void tela_jogar(const Banco *banco)
{
#ifdef USE_RAYLIB
    Status status;

    utils_limpar_tela();
    printf(COR_AMARELO "== Jogar ==\n\n" COR_RESET);
    printf("Abrindo a janela do jogo...\n");

    status = jogo_executar(banco);
    if (status == STATUS_ASSET_FALLBACK) {
        printf(COR_AMARELO "\nAviso: %s\n" COR_RESET, utils_mensagem_status(status));
    } else if (status == STATUS_NAO_ENCONTRADO) {
        utils_beep();
        printf(COR_VERMELHO "\nNão há perguntas suficientes para uma partida.\n" COR_RESET);
        printf("Cada partida precisa de 2 perguntas muito fáceis, 2 fáceis, 4 médias,\n"
               "4 difíceis e 3 muito difíceis (veja PERGUNTAS_POR_NIVEL no config.h).\n");
    } else if (status != STATUS_OK) {
        utils_beep();
        printf(COR_VERMELHO "\nNão foi possível jogar: %s\n" COR_RESET,
               utils_mensagem_status(status));
    }
#else
    (void)banco;
    utils_limpar_tela();
    printf(COR_AMARELO "== Jogar ==\n\n" COR_RESET);
    utils_beep();
    printf(COR_VERMELHO "Este executável foi compilado/rodade sem a raylib.\n" COR_RESET);
    printf("Compile com: make USE_RAYLIB=1\nRode com: make USE_RAYLIB=1");
#endif
    utils_pausar();
}

void menu_principal(Banco *banco)
{
    int opcao = OPCAO_SAIR;

    do {
        utils_limpar_tela();
        exibir_menu();

        if (utils_ler_int("Escolha uma opção: ", OPCAO_SAIR, OPCAO_MAX, &opcao) != STATUS_OK) {
            break; /* fim da entrada (EOF) */
        }

        switch (opcao) {
        case 1: tela_cadastrar(banco); break;
        case 2: tela_listar(banco); break;
        case 3: tela_pesquisar(banco); break;
        case 4: tela_alterar(banco); break;
        case 5: tela_excluir(banco); break;
        case 6: tela_navegar_nivel(banco); break;
        case 7: tela_exportar(banco); break;
        case OPCAO_JOGAR: tela_jogar(banco); break;
        default: break;
        }
    } while (opcao != OPCAO_SAIR);
}
