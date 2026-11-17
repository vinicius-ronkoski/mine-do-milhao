/**
 * @file test_banco.c
 * @author Paulo Valadares (paulovaladares@alunos.utfpr.edu.br)
 * @brief testes automáticos das funções de banco.c
 * @version 0.1
 * @date 2026-11-17
 *
 * @copyright Copyright (c) 2026
 *
 */
#include <stdio.h>
#include <string.h>
#include "banco.h"

#define TOTAL_DE_PERGUNTAS_DO_TESTE 100
#define TAMANHO_DO_VETOR_DE_INDICES 200

/**
 * @brief confere uma condição e printa se passou ou falhou
 *
 * @param condicao resultado da conferência (diferente de zero é sucesso)
 * @param descricao texto que explica o que está sendo testado
 * @param testes_feitos contador de testes feitos
 * @param testes_falhos contador de testes que falharam
 */
static void verificar(int condicao, const char *descricao, int *testes_feitos, int *testes_falhos) {
    //contando o teste
    (*testes_feitos)++;

    //printando o resultado
    if (condicao) {
        printf("[OK]    %s\n", descricao);
    } else {
        printf("[FALHA] %s\n", descricao);
        (*testes_falhos)++;
    }
}

/**
 * @brief monta uma pergunta válida só para os testes
 *
 * @param enunciado texto do enunciado
 * @param nivel nível de dificuldade (1 a 5)
 * @return a pergunta montada
 */
static Pergunta criar_pergunta_de_teste(const char *enunciado, int nivel) {
    Pergunta pergunta_criada;

    //zerando tudo antes de preencher
    memset(&pergunta_criada, 0, sizeof(pergunta_criada));

    //preenchendo os campos
    snprintf(pergunta_criada.enunciado, sizeof(pergunta_criada.enunciado), "%s", enunciado);
    for (int i = 0; i < NUM_ALTERNATIVAS; i++) {
        strcpy(pergunta_criada.alternativas[i], "alternativa");
    }
    strcpy(pergunta_criada.dica, "dica");
    pergunta_criada.correta = 'A';
    pergunta_criada.nivel = nivel;

    return pergunta_criada;
}

int main(){
    //declarando nossas variáveis
    Banco banco;
    size_t indices_encontrados[TAMANHO_DO_VETOR_DE_INDICES];
    int testes_feitos = 0;
    int testes_falhos = 0;

    //testando o banco recém iniciado
    banco_iniciar(&banco);
    verificar(banco.itens == NULL && banco.quantidade == 0 && banco.capacidade == 0,
              "banco_iniciar deixa o banco vazio", &testes_feitos, &testes_falhos);
    verificar(banco_obter(&banco, 0) == NULL,
              "banco_obter em banco vazio devolve NULL", &testes_feitos, &testes_falhos);
    verificar(banco_remover(&banco, 0) == STATUS_ERRO_PARAMETRO,
              "banco_remover em banco vazio devolve erro", &testes_feitos, &testes_falhos);

    //inserindo as perguntas (isso força vários realloc)
    int todas_foram_inseridas = 1;
    for (int i = 0; i < TOTAL_DE_PERGUNTAS_DO_TESTE; i++) {
        char texto_do_enunciado[64];
        snprintf(texto_do_enunciado, sizeof(texto_do_enunciado), "Pergunta numero %d sobre Creeper", i);
        Pergunta pergunta_atual = criar_pergunta_de_teste(texto_do_enunciado, i % 5 + 1);
        if (banco_inserir(&banco, &pergunta_atual) != STATUS_OK) {
            todas_foram_inseridas = 0;
        }
    }
    verificar(todas_foram_inseridas && banco.quantidade == TOTAL_DE_PERGUNTAS_DO_TESTE &&
              banco.capacidade >= banco.quantidade,
              "banco_inserir guarda 100 perguntas e cresce o vetor", &testes_feitos, &testes_falhos);

    //testando entradas inválidas na inserção
    Pergunta pergunta_invalida = criar_pergunta_de_teste("", 1);
    verificar(banco_inserir(&banco, &pergunta_invalida) == STATUS_ERRO_PARAMETRO,
              "banco_inserir recusa pergunta inválida", &testes_feitos, &testes_falhos);
    verificar(banco_inserir(NULL, &pergunta_invalida) == STATUS_ERRO_PARAMETRO,
              "banco_inserir recusa banco NULL", &testes_feitos, &testes_falhos);
    verificar(banco_inserir(&banco, NULL) == STATUS_ERRO_PARAMETRO,
              "banco_inserir recusa pergunta NULL", &testes_feitos, &testes_falhos);
    verificar(banco.quantidade == TOTAL_DE_PERGUNTAS_DO_TESTE,
              "as inserções inválidas não mudaram a quantidade", &testes_feitos, &testes_falhos);

    //testando o banco_obter
    const Pergunta *pergunta_obtida = banco_obter(&banco, 7);
    verificar(pergunta_obtida != NULL && strcmp(pergunta_obtida->enunciado, "Pergunta numero 7 sobre Creeper") == 0,
              "banco_obter devolve a pergunta da posição 7", &testes_feitos, &testes_falhos);
    verificar(banco_obter(&banco, TOTAL_DE_PERGUNTAS_DO_TESTE) == NULL,
              "banco_obter com índice fora devolve NULL", &testes_feitos, &testes_falhos);

    //testando o filtro por nível (são 20 perguntas de cada nível)
    verificar(banco_filtrar_nivel(&banco, 3, indices_encontrados, TAMANHO_DO_VETOR_DE_INDICES) == 20,
              "banco_filtrar_nivel acha 20 perguntas do nível 3", &testes_feitos, &testes_falhos);
    verificar(banco_obter(&banco, indices_encontrados[0])->nivel == 3,
              "a pergunta filtrada realmente é do nível 3", &testes_feitos, &testes_falhos);
    verificar(banco_filtrar_nivel(&banco, 3, indices_encontrados, 5) == 5,
              "banco_filtrar_nivel respeita o limite do vetor de saída", &testes_feitos, &testes_falhos);
    verificar(banco_filtrar_nivel(&banco, 9, indices_encontrados, TAMANHO_DO_VETOR_DE_INDICES) == 0,
              "banco_filtrar_nivel com nível inexistente acha 0", &testes_feitos, &testes_falhos);

    //testando a busca por texto (sem diferenciar maiúsculas)
    verificar(banco_buscar_texto(&banco, "creeper", indices_encontrados, TAMANHO_DO_VETOR_DE_INDICES) == 100,
              "banco_buscar_texto ignora maiúsculas e minúsculas", &testes_feitos, &testes_falhos);
    verificar(banco_buscar_texto(&banco, "NUMERO 42 ", indices_encontrados, TAMANHO_DO_VETOR_DE_INDICES) == 1 &&
              indices_encontrados[0] == 42,
              "banco_buscar_texto acha só a pergunta 42", &testes_feitos, &testes_falhos);
    verificar(banco_buscar_texto(&banco, "wither", indices_encontrados, TAMANHO_DO_VETOR_DE_INDICES) == 0,
              "banco_buscar_texto sem resultado devolve 0", &testes_feitos, &testes_falhos);

    //testando o banco_alterar
    Pergunta pergunta_nova = criar_pergunta_de_teste("Texto alterado", 5);
    verificar(banco_alterar(&banco, 10, &pergunta_nova) == STATUS_OK &&
              strcmp(banco_obter(&banco, 10)->enunciado, "Texto alterado") == 0,
              "banco_alterar troca a pergunta da posição 10", &testes_feitos, &testes_falhos);
    verificar(banco_alterar(&banco, TOTAL_DE_PERGUNTAS_DO_TESTE, &pergunta_nova) == STATUS_ERRO_PARAMETRO,
              "banco_alterar recusa índice fora", &testes_feitos, &testes_falhos);
    verificar(banco_alterar(&banco, 10, &pergunta_invalida) == STATUS_ERRO_PARAMETRO,
              "banco_alterar recusa pergunta inválida", &testes_feitos, &testes_falhos);

    //testando o banco_remover (a ordem das perguntas deve ser mantida)
    verificar(banco_remover(&banco, 0) == STATUS_OK &&
              strcmp(banco_obter(&banco, 0)->enunciado, "Pergunta numero 1 sobre Creeper") == 0,
              "banco_remover tira a primeira e mantém a ordem", &testes_feitos, &testes_falhos);
    verificar(banco_remover(&banco, banco.quantidade - 1) == STATUS_OK && banco.quantidade == 98,
              "banco_remover tira a última", &testes_feitos, &testes_falhos);
    verificar(banco_remover(&banco, 98) == STATUS_ERRO_PARAMETRO,
              "banco_remover recusa índice fora", &testes_feitos, &testes_falhos);

    //removendo quase tudo (o vetor deve encolher sem perder as perguntas que sobraram)
    size_t capacidade_antes_de_remover = banco.capacidade;
    int todas_foram_removidas = 1;
    while (banco.quantidade > 3) {
        if (banco_remover(&banco, 0) != STATUS_OK) {
            todas_foram_removidas = 0;
        }
    }
    verificar(todas_foram_removidas && banco.capacidade < capacidade_antes_de_remover &&
              banco.capacidade >= banco.quantidade,
              "o vetor encolhe quando sobra muito espaço", &testes_feitos, &testes_falhos);

    //esvaziando o banco de vez e usando de novo
    while (banco.quantidade > 0) {
        banco_remover(&banco, 0);
    }
    Pergunta pergunta_depois_de_esvaziar = criar_pergunta_de_teste("Depois de esvaziar", 2);
    verificar(banco_inserir(&banco, &pergunta_depois_de_esvaziar) == STATUS_OK && banco.quantidade == 1,
              "o banco continua funcionando depois de esvaziar", &testes_feitos, &testes_falhos);

    //liberando a memória (chamar duas vezes não pode quebrar)
    banco_liberar(&banco);
    banco_liberar(&banco);
    verificar(banco.itens == NULL && banco.quantidade == 0 && banco.capacidade == 0,
              "banco_liberar zera o banco", &testes_feitos, &testes_falhos);

    //printando o resumo
    printf("\n%d testes feitos, %d falharam\n", testes_feitos, testes_falhos);

    if (testes_falhos == 0) {
        return 0;
    }
    return 1;
}
