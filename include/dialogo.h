/**
 * @file dialogo.h
 * @brief Diálogos de personagens: falas em sequência com efeito de máquina de escrever.
 *
 * Uma CENA é uma lista de falas (ex.: CENA_INTRO). Um DIALOGO é o que está tocando agora:
 * pode juntar várias cenas em sequência com dialogo_anexar_cena() — por exemplo
 * "boss derrotado" seguida da entrada do próximo boss.
 *
 * COMO PERSONALIZAR AS FALAS: edite o `case` da cena em dialogo_anexar_cena() (dialogo.c);
 *   as reações a acerto/erro ficam nas tabelas de dialogo_anexar_reacao() (dialogo.c).
 * COMO ADICIONAR UMA CENA: (1) novo id em CenaId, antes de CENA_QUANTIDADE;
 *                          (2) novo `case` em dialogo_anexar_cena().
 * CENÁRIO: CENARIO(IMG_X) dentro do `case` troca o fundo das falas que vêm depois dele (até o
 *   próximo CENARIO); sem CENARIO a cena usa o fundo da fase. Com cenário próprio o boss não aparece.
 *   Imagem nova: id no enum ImagemId (assets.h), nome em nome_arquivo_imagem() (assets.c), arquivo
 *   em assets/sprites/ (16:9).
 * HISTÓRIA: as cenas CENA_HISTORIA_* são chamadas direto, com jogo_anexar_cena(), nos pontos do
 *   fluxo onde devem tocar: jogo_nova_partida() (jogo.c) e tela_revelacao_atualizar() (tela.c).
 * SOM EM UMA FALA: use FALA_SOM(personagem, texto, retrato, SOM_X) no lugar de FALA(...).
 *   O som toca uma vez, quando a fala aparece. Som novo: id no enum SomId (assets.h),
 *   nome do arquivo em nome_arquivo_som() (assets.c) e o arquivo em assets/sounds/.
 * MÚSICA: MUSICA(MUSICA_X) dentro do `case` troca a música quando a PRÓXIMA fala aparecer.
 *   A música continua depois da cena: para voltar, outro MUSICA(...) (não volta sozinha).
 *   Música nova: id no enum MusicaId (assets.h), nome em nome_arquivo_musica() (assets.c).
 *
 * O diálogo só guarda som, cenário e música; quem toca e desenha é tela.c (dialogo_fala_nova).
 *
 * O módulo não lê teclado/mouse: quem chama informa se o jogador pediu para avançar.
 * Só compila com USE_RAYLIB=1.
 */
#ifndef DIALOGO_H
#define DIALOGO_H

#include "raylib.h"
#include "config.h"
#include "assets.h"
#include "boss.h"

/** Quantas variações de reação existem para acerto e para erro (dialogo_anexar_reacao). */
#define REACOES_POR_RESULTADO 3

/** Uma fala. */
typedef struct {
    char personagem[TAM_NOME_PERSONAGEM];  /**< Nome mostrado acima do texto. */
    char texto[TAM_FALA];                  /**< Conteúdo (UTF-8, com acentos). */
    ImagemId retrato;                      /**< Retrato (IMG_*) ou IMG_NENHUMA. */
    SomId som;                             /**< Som que toca quando a fala aparece, ou SOM_NENHUM. */
    ImagemId fundo;                        /**< Cenário desta fala (CENARIO) ou IMG_NENHUMA = fundo da fase. */
    MusicaId musica;                       /**< Música que começa nesta fala (MUSICA) ou MUSICA_MANTER. */
} Fala;

/** Cenas disponíveis. A ordem dos bosses segue o enum BossId. */
typedef enum {
    CENA_INTRO = 0,         /**< Abertura da partida. */
    CENA_BOSS_CREEPER,      /**< Entrada de cada boss: CENA_BOSS_CREEPER + BossId. */
    CENA_BOSS_WITHER,
    CENA_BOSS_WARDEN,
    CENA_BOSS_ENDER_DRAGON,
    CENA_BOSS_HEROBRINE,
    CENA_BOSS_DERROTADO,    /**< Reação depois de derrotar um boss. */
    CENA_VITORIA,           /**< Acertou as 15. */
    CENA_DERROTA,           /**< Errou. */
    CENA_PARADA,            /**< Parou e levou as esmeraldas. */
    CENA_HISTORIA_1,        /**< Parte introdutória com o creeper. */
    CENA_HISTORIA_2,        /**< Indo para o Nether */
    CENA_HISTORIA_3,        /**< Indo para o warden */
    CENA_HISTORIA_4,        /**< Indo para a fim */
    CENA_HISTORIA_5,        /**< Voltando do fim e checando steve*/
    CENA_HISTORIA_FIM,       /**< Vítoria, paz e entrega dos prêmios*/
    CENA_QUANTIDADE         /**< Quantidade de cenas (também é o valor "cena inválida"). */
} CenaId;

/** Sequência de falas em reprodução. */
typedef struct {
    Fala falas[DIALOGO_MAX_FALAS];  /**< Falas, na ordem. */
    int quantidade;                 /**< Falas preenchidas. */
    int indice;                     /**< Fala atual (0 a quantidade-1). */
    float tempo_na_fala;            /**< Segundos desde o início da fala atual (máquina de escrever). */
    bool terminou;                  /**< true depois de avançar além da última fala (ou se vazio). */
    int indice_fala_anunciada;      /**< Última fala já entregue por dialogo_fala_nova (-1 = nenhuma). */
} Dialogo;

/** @brief Esvazia o diálogo (terminou = true, pois não há falas). */
void dialogo_limpar(Dialogo *dialogo);

/**
 * @brief Acrescenta uma fala ao fim.
 * @param som Som tocado quando a fala aparecer (ver dialogo_fala_nova), ou SOM_NENHUM.
 *            A música da fala começa como MUSICA_MANTER (a macro MUSICA troca).
 * @return STATUS_OK ou STATUS_ERRO_PARAMETRO (NULL, texto longo demais ou DIALOGO_MAX_FALAS excedido).
 */
Status dialogo_adicionar_fala(Dialogo *dialogo, const char *personagem, const char *texto,
                              ImagemId retrato, SomId som);

/**
 * @brief A fala que acabou de aparecer, entregue UMA vez por fala (nas outras chamadas, NULL).
 *
 * A tela (tela_dialogo_atualizar) chama isto a cada quadro; quando a fala muda, recebe a fala nova
 * uma vez e toca o som dela e troca a música, se houver. Assim cada efeito acontece uma vez só.
 */
const Fala *dialogo_fala_nova(Dialogo *dialogo);

/**
 * @brief Acrescenta as falas de uma cena ao fim do diálogo atual (sem apagar o que já havia).
 * @return STATUS_OK ou STATUS_ERRO_PARAMETRO (cena inválida ou falas demais).
 */
Status dialogo_anexar_cena(Dialogo *dialogo, CenaId cena);

/**
 * @brief Acrescenta a reação do apresentador a uma resposta (falas montadas na hora).
 *
 * Há REACOES_POR_RESULTADO variações para acerto e para erro, com estruturas diferentes
 * (citam a resposta, o boss ou as esmeraldas). `variante` escolhe qual (use um número sorteado).
 *
 * @param acertou         Resultado da resposta.
 * @param resposta_certa  Texto da alternativa correta.
 * @param nome_boss       Nome do boss do nível (boss_nome).
 * @param esmeraldas      Esmeraldas acumuladas depois da resposta.
 * @param variante        Qualquer inteiro; usa-se variante % REACOES_POR_RESULTADO.
 * @return STATUS_OK ou STATUS_ERRO_PARAMETRO.
 */
Status dialogo_anexar_reacao(Dialogo *dialogo, bool acertou, const char *resposta_certa,
                             const char *nome_boss, int esmeraldas, int variante);

/** @brief Limpa o diálogo e carrega uma cena (= dialogo_limpar + dialogo_anexar_cena). */
Status dialogo_carregar_cena(Dialogo *dialogo, CenaId cena);

/** @brief Cena de entrada de um boss (CENA_BOSS_CREEPER + id); CENA_QUANTIDADE se o id for inválido. */
CenaId dialogo_cena_do_boss(BossId id);

/**
 * @brief Avança o relógio da fala e trata o pedido de avançar.
 *
 * Avançar com a fala ainda sendo "digitada" completa a fala; com a fala completa, passa para a próxima.
 *
 * @param dt               Tempo do quadro (GetFrameTime()).
 * @param jogador_avancou  true se o jogador clicou / apertou ESPAÇO ou ENTER neste quadro.
 * @return true quando o diálogo acabou.
 */
bool dialogo_atualizar(Dialogo *dialogo, float dt, bool jogador_avancou);

/** @brief Fala sendo mostrada, ou NULL se o diálogo terminou. */
const Fala *dialogo_fala_atual(const Dialogo *dialogo);

/** @brief Quantas letras da fala atual já aparecem (efeito máquina de escrever). */
int dialogo_letras_visiveis(const Dialogo *dialogo);

/**
 * @brief Desenha a caixa de diálogo na parte de baixo da tela: caixa, retrato, nome e texto parcial,
 *        mais o aviso "ESPAÇO para continuar" quando a fala está completa.
 */
void dialogo_desenhar(const Dialogo *dialogo, const Assets *assets);

#endif /* DIALOGO_H */
