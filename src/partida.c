/**
 * @file partida.c
 * @brief Implementação das regras da partida (ver partida.h). Sem raylib.
 */
#include "partida.h"

/**
 * @brief Quantas perguntas de um nível entram na partida (tabela PERGUNTAS_POR_NIVEL do config.h).
 *
 * @param nivel Nível de NIVEL_MIN a NIVEL_MAX.
 * @return Quantidade; 0 se o nível for inválido.
 */
static int contagem_do_nivel(int nivel)
{
    /* Tabela local à função (sem variável global). */
    static const int tabela[NUM_NIVEIS] = PERGUNTAS_POR_NIVEL;

    if (nivel < NIVEL_MIN || nivel > NIVEL_MAX) {
        return 0;
    }
    return tabela[nivel - NIVEL_MIN];
}

Status partida_iniciar(Partida *partida, const Banco *banco)
{
    Partida nova = {0};
    int por_nivel[NUM_NIVEIS];
    int nivel;
    Status status;

    if (partida == NULL || banco == NULL) {
        return STATUS_ERRO_PARAMETRO;
    }

    for (nivel = NIVEL_MIN; nivel <= NIVEL_MAX; nivel++) {
        por_nivel[nivel - NIVEL_MIN] = partida_perguntas_no_nivel(nivel);
    }
    status = sorteio_montar_partida(banco, por_nivel, nova.perguntas);
    if (status != STATUS_OK) {
        return status;
    }

    for (nivel = NIVEL_MIN; nivel <= NIVEL_MAX; nivel++) {
        status = boss_iniciar(&nova.bosses[nivel - NIVEL_MIN], boss_id_do_nivel(nivel),
                              por_nivel[nivel - NIVEL_MIN]);
        if (status != STATUS_OK) {
            return status;
        }
    }

    nova.rodada = 0;
    nova.esmeraldas = 0;
    nova.esmeraldas_garantidas = 0;
    nova.escolha = -1;
    nova.ultima_acertou = false;
    nova.boss_derrotado_agora = false;
    nova.esmeraldas_gastas = 0;
    nova.dica_visivel = false;
    nova.eliminacao_usada = false;
    nova.estado = ESTADO_RESPONDENDO;

    *partida = nova;  /* só aqui a partida do chamador é alterada */
    return STATUS_OK;
}

int partida_nivel_da_rodada(int rodada)
{
    int nivel, acumulado = 0;

    if (rodada < 0 || rodada >= PERGUNTAS_POR_PARTIDA) {
        return 0;
    }

    /* Soma as faixas (2, 2, 4, 4, 3) até passar da rodada pedida */
    for (nivel = NIVEL_MIN; nivel <= NIVEL_MAX; nivel++) {
        acumulado += contagem_do_nivel(nivel);
        if (rodada < acumulado) {
            return nivel;
        }
    }
    return 0;  /* só acontece se PERGUNTAS_POR_NIVEL não somar PERGUNTAS_POR_PARTIDA */
}

int partida_perguntas_no_nivel(int nivel)
{
    return contagem_do_nivel(nivel);
}

int partida_numero_da_rodada(const Partida *partida)
{
    return partida == NULL ? 0 : partida->rodada + 1;
}

bool partida_eh_marco(int rodada)
{
    /* As macros dos marcos contam a partir de 1; rodada é índice a partir de 0. */
    return rodada + 1 == RODADA_MARCO_1 || rodada + 1 == RODADA_MARCO_2;
}

int partida_premio_da_rodada(int rodada)
{
    /* Tabela local à função (sem variável global). */
    static const int tabela[PERGUNTAS_POR_PARTIDA] = PREMIOS_ESMERALDAS;

    if (rodada < 0 || rodada >= PERGUNTAS_POR_PARTIDA) {
        return 0;
    }
    return tabela[rodada];
}

const PerguntaSorteada *partida_pergunta_atual(const Partida *partida)
{
    if (partida == NULL || partida_encerrada(partida)
        || partida->rodada < 0 || partida->rodada >= PERGUNTAS_POR_PARTIDA) {
        return NULL;
    }
    return &partida->perguntas[partida->rodada];
}

const Boss *partida_boss_atual(const Partida *partida)
{
    int nivel;

    if (partida == NULL) {
        return NULL;
    }
    nivel = partida_nivel_da_rodada(partida->rodada);
    if (nivel == 0) {
        return NULL;
    }
    return &partida->bosses[nivel - NIVEL_MIN];
}

Status partida_responder(Partida *partida, int posicao_escolhida)
{
    const PerguntaSorteada *pergunta;
    Boss *boss;
    Status status;
    int nivel;

    if (partida == NULL || partida->estado != ESTADO_RESPONDENDO
        || posicao_escolhida < 0 || posicao_escolhida >= NUM_ALTERNATIVAS
        || partida->alternativa_eliminada[posicao_escolhida]) {
        return STATUS_ERRO_PARAMETRO;
    }

    /* Confere a rodada ANTES de usar o nível como índice (nível inválido = 0) */
    pergunta = partida_pergunta_atual(partida);
    nivel = partida_nivel_da_rodada(partida->rodada);
    if (pergunta == NULL || nivel == 0) {
        return STATUS_ERRO_PARAMETRO;
    }
    boss = &partida->bosses[nivel - NIVEL_MIN];

    partida->escolha = posicao_escolhida;
    partida->boss_derrotado_agora = false;
    partida->ultima_acertou = (posicao_escolhida == pergunta->posicao_correta);

    if (partida->ultima_acertou) {
        partida->esmeraldas = partida_premio_da_rodada(partida->rodada);
        status = boss_receber_dano(boss, 1);
        if (status != STATUS_OK) {
            return status;
        }
        partida->boss_derrotado_agora = boss_derrotado(boss);
        if (partida_eh_marco(partida->rodada)) {
            partida->esmeraldas_garantidas = partida->esmeraldas;
        }
    }

    partida->estado = ESTADO_REVELANDO;
    return STATUS_OK;
}

Status partida_confirmar_revelacao(Partida *partida)
{
    if (partida == NULL || partida->estado != ESTADO_REVELANDO) {
        return STATUS_ERRO_PARAMETRO;
    }

    if (!partida->ultima_acertou) {
        partida->estado = ESTADO_FIM_ERRO;
    } else if (partida->rodada >= PERGUNTAS_POR_PARTIDA - 1) {
        partida->estado = ESTADO_FIM_VITORIA;  /* rodada fica em 14: o último boss ainda é consultável */
    } else {
        int posicao;

        partida->rodada++;
        partida->escolha = -1;
        /* Efeitos das ajudas valem só para a rodada em que foram usadas */
        partida->dica_visivel = false;
        for (posicao = 0; posicao < NUM_ALTERNATIVAS; posicao++) {
            partida->alternativa_eliminada[posicao] = false;
        }
        partida->estado = ESTADO_RESPONDENDO;
    }
    return STATUS_OK;
}

/* ---- Ajudas ---- */

int partida_saldo(const Partida *partida)
{
    if (partida == NULL) {
        return 0;
    }
    return partida->esmeraldas - partida->esmeraldas_gastas;
}

/** @brief Preço de uma ajuda na rodada atual, lido da tabela `custos` (um valor por nível). */
static int custo_por_nivel(const Partida *partida, const int custos[NUM_NIVEIS])
{
    int nivel;

    if (partida == NULL || partida_encerrada(partida)) {
        return 0;
    }
    nivel = partida_nivel_da_rodada(partida->rodada);
    if (nivel == 0) {
        return 0;
    }
    return custos[nivel - NIVEL_MIN];
}

int partida_custo_dica(const Partida *partida)
{
    /* Tabela local à função (sem variável global). */
    static const int custos[NUM_NIVEIS] = CUSTO_DICA_POR_NIVEL;

    return custo_por_nivel(partida, custos);
}

int partida_custo_eliminar(const Partida *partida)
{
    static const int custos[NUM_NIVEIS] = CUSTO_ELIMINAR_POR_NIVEL;

    return custo_por_nivel(partida, custos);
}

bool partida_pode_eliminar(const Partida *partida)
{
    return partida != NULL
        && partida->estado == ESTADO_RESPONDENDO
        && !partida->eliminacao_usada
        && partida_saldo(partida) >= partida_custo_eliminar(partida);
}

bool partida_pode_comprar_dica(const Partida *partida)
{
    return partida != NULL
        && partida->estado == ESTADO_RESPONDENDO
        && !partida->dica_visivel
        && partida_saldo(partida) >= partida_custo_dica(partida);
}

Status partida_comprar_dica(Partida *partida)
{
    if (partida == NULL || partida->estado != ESTADO_RESPONDENDO || partida->dica_visivel) {
        return STATUS_ERRO_PARAMETRO;
    }
    if (partida_saldo(partida) < partida_custo_dica(partida)) {
        return STATUS_SALDO_INSUFICIENTE;
    }
    partida->esmeraldas_gastas += partida_custo_dica(partida);
    partida->dica_visivel = true;
    return STATUS_OK;
}

const char *partida_dica_atual(const Partida *partida)
{
    const PerguntaSorteada *pergunta = partida_pergunta_atual(partida);

    if (pergunta == NULL || !partida->dica_visivel) {
        return NULL;
    }
    return pergunta->origem->dica;
}

Status partida_eliminar_duas(Partida *partida)
{
    const PerguntaSorteada *pergunta;
    size_t posicoes_erradas[NUM_ALTERNATIVAS];
    size_t total_erradas = 0;
    int posicao;

    if (partida == NULL || partida->estado != ESTADO_RESPONDENDO || partida->eliminacao_usada) {
        return STATUS_ERRO_PARAMETRO;
    }
    if (partida_saldo(partida) < partida_custo_eliminar(partida)) {
        return STATUS_SALDO_INSUFICIENTE;
    }
    pergunta = partida_pergunta_atual(partida);
    if (pergunta == NULL) {
        return STATUS_ERRO_PARAMETRO;
    }

    /* Junta as posições erradas, embaralha e elimina as duas primeiras */
    for (posicao = 0; posicao < NUM_ALTERNATIVAS; posicao++) {
        if (posicao != pergunta->posicao_correta) {
            posicoes_erradas[total_erradas++] = (size_t)posicao;
        }
    }
    sorteio_embaralhar_vetor(posicoes_erradas, total_erradas);
    partida->alternativa_eliminada[posicoes_erradas[0]] = true;
    partida->alternativa_eliminada[posicoes_erradas[1]] = true;

    partida->esmeraldas_gastas += partida_custo_eliminar(partida);
    partida->eliminacao_usada = true;
    return STATUS_OK;
}

Status partida_parar(Partida *partida)
{
    if (partida == NULL || partida->estado != ESTADO_RESPONDENDO) {
        return STATUS_ERRO_PARAMETRO;
    }
    partida->estado = ESTADO_FIM_PARADA;
    return STATUS_OK;
}

bool partida_encerrada(const Partida *partida)
{
    if (partida == NULL) {
        return false;
    }
    return partida->estado == ESTADO_FIM_PARADA
        || partida->estado == ESTADO_FIM_ERRO
        || partida->estado == ESTADO_FIM_VITORIA;
}

int partida_premio_final(const Partida *partida)
{
    int premio;

    if (partida == NULL) {
        return 0;
    }

    switch (partida->estado) {
    case ESTADO_FIM_PARADA:
    case ESTADO_FIM_VITORIA:
        premio = partida->esmeraldas;
        break;
    case ESTADO_FIM_ERRO:
        premio = REGRA_ERRO_VOLTA_AO_MARCO ? partida->esmeraldas_garantidas : 0;
        break;
    default:
        return 0;  /* ainda em andamento */
    }

    /* Desconta as dicas compradas; quem gastou mais do que levaria sai com 0 */
    premio -= partida->esmeraldas_gastas;
    return premio > 0 ? premio : 0;
}

int partida_primeira_rodada_do_nivel(int nivel)
{
    int anterior, rodada = 0;

    if (nivel < NIVEL_MIN || nivel > NIVEL_MAX) {
        return -1;
    }
    for (anterior = NIVEL_MIN; anterior < nivel; anterior++) {
        rodada += contagem_do_nivel(anterior);
    }
    return rodada;
}

Status partida_iniciar_no_boss(Partida *partida, const Banco *banco, BossId boss, int saldo,
                               bool eliminacao_usada)
{
    Partida nova;
    int nivel, nivel_boss = 0, rodada;
    Status status;

    if (partida == NULL) {
        return STATUS_ERRO_PARAMETRO;
    }
    for (nivel = NIVEL_MIN; nivel <= NIVEL_MAX; nivel++) {
        if (boss_id_do_nivel(nivel) == boss) {
            nivel_boss = nivel;
        }
    }
    if (nivel_boss == 0) {
        return STATUS_ERRO_PARAMETRO;   /* boss inválido */
    }

    status = partida_iniciar(&nova, banco);
    if (status != STATUS_OK) {
        return status;
    }

    /* Começa na 1ª pergunta do nível do boss; os bosses anteriores já caíram */
    nova.rodada = partida_primeira_rodada_do_nivel(nivel_boss);
    for (nivel = NIVEL_MIN; nivel < nivel_boss; nivel++) {
        nova.bosses[nivel - NIVEL_MIN].vida_atual = 0;
    }

    /* Prêmio = o da última rodada acertada; garantido = o do último marco já passado */
    nova.esmeraldas = nova.rodada > 0 ? partida_premio_da_rodada(nova.rodada - 1) : 0;
    for (rodada = 0; rodada < nova.rodada; rodada++) {
        if (partida_eh_marco(rodada)) {
            nova.esmeraldas_garantidas = partida_premio_da_rodada(rodada);
        }
    }

    /* O gasto em ajudas é o que falta para o saldo salvo */
    if (saldo < 0 || saldo > nova.esmeraldas) {
        return STATUS_ERRO_PARAMETRO;
    }
    nova.esmeraldas_gastas = nova.esmeraldas - saldo;
    nova.eliminacao_usada = eliminacao_usada;

    *partida = nova;
    return STATUS_OK;
}
