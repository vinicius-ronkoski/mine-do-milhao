/**
 * @file dialogo.c
 * @brief Implementação dos diálogos (ver dialogo.h). Compila só com USE_RAYLIB.
 *
 * Organização: 1) falas de cada cena (PERSONALIZE AQUI), 2) controle do diálogo, 3) desenho.
 */
#ifdef USE_RAYLIB
#include <stdio.h>
#include <string.h>
#include "dialogo.h"
#include "layout.h"

/** Nome do apresentador nas falas. */
#define NOME_APRESENTADOR "Silvio Aldeão"

/** Número grande de letras: usado para "completar" a fala de uma vez. */
#define LETRAS_FALA_COMPLETA 100000

/* ---- 1. Falas de cada cena ---- */

Status dialogo_anexar_cena(Dialogo *dialogo, CenaId cena)
{
    Status status = STATUS_OK;
    ImagemId cenario = IMG_NENHUMA;          /* toda cena começa com o fundo da fase */
    ImagemId retrato_atual = IMG_NENHUMA;    /* toda cena começa sem retrato; RETRATO(...) troca */
    SomId som_proximo = SOM_NENHUM;          /* SOM(...) marca o som da próxima fala */
    MusicaId musica_proxima = MUSICA_MANTER; /* MUSICA(...) marca a troca na próxima fala */

    if (dialogo == NULL) {
        return STATUS_ERRO_PARAMETRO;
    }

/* Atalhos só deste switch. Regra única: tudo que modifica uma fala vem ANTES dela.
 *   FALA(personagem, texto)  acrescenta uma fala (guarda o primeiro erro em `status`)
 *   CENARIO(imagem)  fundo (IMG_*) desta fala E DAS SEGUINTES, até o próximo CENARIO;
 *                    CENARIO(IMG_NENHUMA) volta ao fundo da fase
 *   RETRATO(imagem)  retrato (IMG_*) desta fala E DAS SEGUINTES, até o próximo RETRATO;
 *                    RETRATO(IMG_NENHUMA) = sem retrato. Troque junto quando mudar o personagem.
 *   SOM(som)         som (SOM_*) tocado quando a PRÓXIMA fala aparecer; só nela
 *   MUSICA(musica)   música (MUSICA_*) que começa na PRÓXIMA fala; continua depois da cena
 * Exemplo:
 *   CENARIO(IMG_FUNDO_PALCO);
 *   RETRATO(IMG_ICONE_ALDEAO);
 *   SOM(SOM_PLATEIA);
 *   MUSICA(MUSICA_PRINCIPAL);
 *   FALA(NOME_APRESENTADOR, "Boa noite!");      <- palco + aldeão + plateia + música
 *   FALA(NOME_APRESENTADOR, "Vamos começar.");  <- palco e aldeão continuam
 *   RETRATO(IMG_NENHUMA);
 *   FALA("Plateia", "EBA!");                    <- sem retrato */
#define FALA(personagem, texto)                                                            \
    do {                                                                                   \
        if (status == STATUS_OK) {                                                         \
            status = dialogo_adicionar_fala(dialogo, personagem, texto, retrato_atual);    \
            if (status == STATUS_OK) {                                                     \
                Fala *fala_criada = &dialogo->falas[dialogo->quantidade - 1];              \
                fala_criada->fundo = cenario;                                              \
                fala_criada->som = som_proximo;                                            \
                fala_criada->musica = musica_proxima;                                      \
                som_proximo = SOM_NENHUM;        /* som e música valem só para esta fala */ \
                musica_proxima = MUSICA_MANTER;                                            \
            }                                                                              \
        }                                                                                  \
    } while (0)
#define CENARIO(imagem) (cenario = (imagem))
#define RETRATO(imagem) (retrato_atual = (imagem))
#define SOM(som) (som_proximo = (som))
#define MUSICA(musica) (musica_proxima = (musica))

    switch (cena) {
    case CENA_INTRO:
        MUSICA(MUSICA_SILENCIO);
        // Entrada no palco
        CENARIO(IMG_FUNDO_PALCO);
        SOM(SOM_PLATEIA);
        RETRATO(IMG_ICONE_ALDEAO);
        FALA(NOME_APRESENTADOR, "Muito obrigado, muito obrigado! Hmm!");
        FALA(
            NOME_APRESENTADOR,
            "Boa noite senhoras e senhores! Boa noite moças e rapazes! Boa noite meninas e meninos!"
        );
        FALA(NOME_APRESENTADOR, "Auditório, boa noite!");

        // Auditorio
        CENARIO(IMG_FUNDO_AUDITORIO);
        RETRATO(IMG_NENHUMA);
        FALA("Plateia", "BOA NOITE!");

        // Apresentador
        CENARIO(IMG_FUNDO_PALCO);
        RETRATO(IMG_ICONE_ALDEAO);
        FALA(NOME_APRESENTADOR, "Bem vindos ao Mine do Milhão! Vamos conhecer as regras...");
        FALA(NOME_APRESENTADOR, "São 15 perguntas. Cada acerto vale esmeraldas, quem quer ficar rico?");

        // Auditório
        CENARIO(IMG_FUNDO_AUDITORIO);
        RETRATO(IMG_NENHUMA);
        FALA("Plateia", "EU!");

        // Foreshadowing
        RETRATO(IMG_ICONE_DESCONHECIDO_2);
        FALA("???", "E quem ganha algo com isso?");

        // Apresentador
        CENARIO(IMG_FUNDO_PALCO);
        RETRATO(IMG_ICONE_ALDEAO);
        FALA(NOME_APRESENTADOR, "Não sei se comento... mas devem ter sido vozes da minha cabeça.");
        FALA(NOME_APRESENTADOR, "Bem, nas perguntas 5 e 10 suas esmeraldas ficam garantidas. "
             "Você pode parar a qualquer momento e levar o que juntou!");
        FALA(NOME_APRESENTADOR, "Então vamos para o Mine do-");
        break;
    case CENA_BOSS_CREEPER:
        break;
    case CENA_BOSS_WITHER:
        MUSICA(MUSICA_WITHER);
        RETRATO(IMG_ICONE_ALDEAO);
        FALA(NOME_APRESENTADOR, "Prepare-se, ele é muito mais forte que o creeper!");
        break;
    case CENA_BOSS_WARDEN:
        MUSICA(MUSICA_WARDEN);
        RETRATO(IMG_ICONE_ALDEAO_ASSUSTADO);
        FALA(NOME_APRESENTADOR, "...shh, não deixe ele te ouvir!");
        break;
    case CENA_BOSS_ENDER_DRAGON:
        MUSICA(MUSICA_END);
        RETRATO(IMG_ICONE_ALDEAO);
        FALA(NOME_APRESENTADOR, "Até agora foi tudo fácil demais...");
        FALA(NOME_APRESENTADOR, "Você consegue derrotar o seu próprio fim?");
        FALA(NOME_APRESENTADOR, "ou melhor... o dragão do fim.");
        FALA(NOME_APRESENTADOR, "ha.");
        FALA(NOME_APRESENTADOR, "ha. ha.");
        FALA(NOME_APRESENTADOR, "ha. ha. ha.");
        FALA(NOME_APRESENTADOR, "Deveriam me pagar mais por fazer essas piadas horríveis");
        break;
    case CENA_BOSS_HEROBRINE:
        MUSICA(MUSICA_HEROBRINE);
        RETRATO(IMG_ICONE_HEROBRINE);
        FALA("\"Você\"", "O que estou fazendo aqui?");
        break;
    case CENA_DERROTA:
        RETRATO(IMG_ICONE_ALDEAO);
        FALA(NOME_APRESENTADOR, "Droga! O(A) Jogador(a)! Ele caiu! O que farei agora?");
        RETRATO(IMG_ICONE_ALDEAO_ASSUSTADO);
        FALA(NOME_APRESENTADOR, "Ei cara, levante, precisamos de você!");
        break;
    case CENA_PARADA:
        RETRATO(IMG_ICONE_ALDEAO);
        FALA(NOME_APRESENTADOR, "Você não vai desistir né?");
        RETRATO(IMG_ICONE_ALDEAO_ASSUSTADO);
        FALA(NOME_APRESENTADOR, "...né?");
        break;

    /* --- História (quem chama cada uma: jogo_nova_partida em jogo.c e a revelação em tela.c) ---
     * Exemplo com cenário e som:
     *     CENARIO(IMG_CENARIO_PALCO);
     *     FALA(NOME_APRESENTADOR, "...");
     *     CENARIO(IMG_CENARIO_NETHER);                   (o fundo muda a partir daqui)
     *     SOM(SOM_PORTAL);                               (só a fala de baixo toca o som)
     *     FALA(NOME_APRESENTADOR, "...");                             */
    case CENA_HISTORIA_1:
        // Comissao técnica
        RETRATO(IMG_NENHUMA);
        CENARIO(IMG_H1_CENA_1);
        FALA("Comissão Técnica", "EI, VOCÊ NÃO PODE ENTRAR AQUI!");

        // Creeper
        CENARIO(IMG_H1_CENA_2);
        SOM(SOM_CREEPER);
        RETRATO(IMG_ICONE_CREEPER);
        FALA("Creeper", "*sons insanos*");

        // Volta ao palco
        CENARIO(IMG_H1_CENA_3);
        MUSICA(MUSICA_PRINCIPAL);
        RETRATO(IMG_ICONE_ALDEAO_ASSUSTADO);
        FALA(NOME_APRESENTADOR, "Ei cara, por que você está vindo até o pal-");
        break;
    case CENA_HISTORIA_2:
        // Inicio Cena Historia 1
        CENARIO(IMG_FUNDO_PALCO);


        RETRATO(IMG_ICONE_ALDEAO_ASSUSTADO);
        FALA(NOME_APRESENTADOR, "Ainda bem que nada aconteceu, obrigado por nos salvar!");
        RETRATO(IMG_ICONE_ALDEAO);
        FALA(
            NOME_APRESENTADOR,
            "Bem, pelo o que eu entendi, quando você responde uma pergunta que eu faço "
            "você consegue atacar os inimigos e causar dano!"
        );
        FALA(NOME_APRESENTADOR, "Parece que teremos que nos unir, caso venha algum outro inimigo");

        CENARIO(IMG_H2_CENA_1);
        FALA(NOME_APRESENTADOR, "O que é isso? Parece que o Creeper deixou um livro...");

        // Leitura do livro
        CENARIO(IMG_H2_CENA_2);
        RETRATO(IMG_NENHUMA);
        FALA(" ", "\"O chamado o chama em meio a chamas de um chamado sem fim.\"");
        FALA(" ",
            "\"Aquele que se opõe aos céus e dos ossos carbonizados nasce em meio as chamas.\""
            "                                                                            -H."
        );

        // Vilager volta, meio revoltado
        CENARIO(IMG_H2_CENA_3);
        RETRATO(IMG_ICONE_ALDEAO);
        FALA(NOME_APRESENTADOR, "Como assim? Isso nem faz sentido");
        FALA(NOME_APRESENTADOR, "Quem escreveu isso com certeza é um péssimo escritor...");
        FALA(NOME_APRESENTADOR, "Mas, me deu uma ideia muito interessante!");

        MUSICA(MUSICA_SILENCIO);
        FALA(NOME_APRESENTADOR, "Chamas, ossos carbonizados, oposição aos céus... É óbvio que é");

        SOM(SOM_THE_NETHER);
        FALA(NOME_APRESENTADOR, "O NETHER");

        // O chamado
        RETRATO(IMG_NENHUMA);
        FALA("Você", "Mas, como iremos para");

        SOM(SOM_THE_NETHER);
        FALA("Você", "O NETHER");

        RETRATO(IMG_ICONE_ALDEAO);
        FALA(NOME_APRESENTADOR, "Venha comigo... e plateia, boa noite!");

        CENARIO(IMG_FUNDO_AUDITORIO);
        RETRATO(IMG_NENHUMA);
        SOM(SOM_PLATEIA);
        FALA("Plateia", "Boa noite!");

        // Indo para o Nether
        CENARIO(IMG_H2_CENA_4);
        RETRATO(IMG_ICONE_ALDEAO);
        FALA(NOME_APRESENTADOR, "Olha só, um portal do nether que por algum motivo a nossa equipe da...");
        FALA(NOME_APRESENTADOR, "Muriel Eixo Games, ou MEG, deixou para trás.");
        FALA(NOME_APRESENTADOR, "Vou usar meus poderes de apresentador para acender o portal!");

        SOM(SOM_FLINT_AND_STEEL);
        FALA(NOME_APRESENTADOR, "FLINT AND STEEL");

        CENARIO(IMG_H2_CENA_5);
        SOM(SOM_RECEBA);
        FALA(NOME_APRESENTADOR, "Receba. Vamos logo");

        // No portal
        CENARIO(IMG_FUNDO_PORTAL);
        SOM(SOM_PORTAL);
        FALA(NOME_APRESENTADOR, "Ei cara, o que te motivou a vir no programa? Está com alguma dívida?");

        // Já no nether
        CENARIO(IMG_H2_CENA_6);
        FALA(NOME_APRESENTADOR, "Que calor!");
        FALA(NOME_APRESENTADOR, "Bem agora que estamos no Nether, você sabe o que fazer.");
        FALA(NOME_APRESENTADOR, "...");
        FALA(NOME_APRESENTADOR, "...");
        FALA(NOME_APRESENTADOR, "...você não sabe, então?");
        FALA(NOME_APRESENTADOR, "...vou procurar algo.");

        // Villager olha para o lado
        CENARIO(IMG_H2_CENA_7);
        RETRATO(IMG_ICONE_ALDEAO_ASSUSTADO);
        FALA(NOME_APRESENTADOR, "Meu Deus. Olha só isso!");

        CENARIO(IMG_H2_CENA_8);
        FALA(NOME_APRESENTADOR, "Três Soul Sands e Três cabeças de esqueleto wither...");
        FALA(NOME_APRESENTADOR, "Você sabe o que isso significa?");


        RETRATO(IMG_ICONE_ALDEAO);
        FALA(NOME_APRESENTADOR, "Que tem três blocos e três cabeças");
        FALA(NOME_APRESENTADOR, "Ha ha ha ha...");
        FALA(NOME_APRESENTADOR, "Essa piada nem fez sentiddo... Enfim, provavelmente é para invocarmos uma entidade.");
        FALA(NOME_APRESENTADOR, "Vou usar os poderes novamente e invocar o próprio Wither!");
        break;
    case CENA_HISTORIA_3:
        // Inicio Historia parte 3
        CENARIO(IMG_H2_CENA_9);
        RETRATO(IMG_ICONE_ALDEAO);
        FALA(NOME_APRESENTADOR, "Ufa, conseguimos vencer ele, mesmo com perguntas bem mais difíceis!");

        // Villager em frente ao portal
        CENARIO(IMG_H3_CENA_1);
        // Jogador
        RETRATO(IMG_NENHUMA);
        FALA("Você", "Foi bem fácil, na verdade.");

        // Villager
        MUSICA(MUSICA_SILENCIO);
        RETRATO(IMG_ICONE_ALDEAO);
        FALA(NOME_APRESENTADOR, "Como assim?! Você deveria ver as perguntas do Mine do Milhão para conseguir o prêmio final!");
        FALA(NOME_APRESENTADOR, "Bem, vamos voltar logo.");

        // No portal
        CENARIO(IMG_FUNDO_PORTAL);
        SOM(SOM_PORTAL);
        FALA(NOME_APRESENTADOR, "Está demorando mais que o normal para voltarmos, o que será que está acontecendo?");

        // Saindo
        CENARIO(IMG_H3_CENA_2);
        SOM(SOM_ALDEAO);
        FALA(NOME_APRESENTADOR, "Hmm...");
        FALA(NOME_APRESENTADOR, "Paramos em algum lugar estranho, vamos voltar.");

        // Chegando no local do warden
        CENARIO(IMG_H3_CENA_3);
        FALA(NOME_APRESENTADOR, "Vamos logo! Por que está parado?");
        FALA(NOME_APRESENTADOR, "Atrás de mim? O que tem atrás de mim?");

        // Sculk ativando
        CENARIO(IMG_H3_CENA_4);
        SOM(SOM_SCULK);
        RETRATO(IMG_ICONE_ALDEAO_ASSUSTADO);
        FALA(NOME_APRESENTADOR, "Mas que-");
        break;
    case CENA_HISTORIA_4:
        // Inicio parte 4 - Historia
        CENARIO(IMG_H3_CENA_5);
        SOM(SOM_ALDEAO);
        RETRATO(IMG_ICONE_ALDEAO);
        FALA(NOME_APRESENTADOR, "Hmm...");
        FALA(NOME_APRESENTADOR, "Parabéns... eu acho.");
        FALA(NOME_APRESENTADOR, "Bem enquanto você lutava com suas respostas, eu reparei num quarto no final da sala.");

        // Apresentador aponta sala do fim do corredor
        CENARIO(IMG_H4_CENA_1);
        FALA(NOME_APRESENTADOR, "Caso não tenha visto, olhe a literal seta mágica que eu fiz com poderes de apresentador.");
        FALA(NOME_APRESENTADOR, "Vamos lá!");

        // Perto do portal
        CENARIO(IMG_H4_CENA_2);
        FALA(NOME_APRESENTADOR,
            "Olha só! Um portal do fim convenientemente colocada como recurso para cortar roteiro! Que incrível!");
        FALA(NOME_APRESENTADOR, "Deve ser coisa da MEG");
        FALA(NOME_APRESENTADOR, "Ah! Se nós poderiamos voltar pelo portal do nether? Claro!");
        FALA(NOME_APRESENTADOR,
            "Mas será mais divertido! "
            "Não é como se eu estivesse com preguiça de andar alguns blocos para trás");

        RETRATO(IMG_ICONE_DESCONHECIDO_1);
        FALA("???", "Cuidado com o te aguarda. A curiosidade sempre mata o gato.");

        RETRATO(IMG_ICONE_ALDEAO_ASSUSTADO);
        FALA(NOME_APRESENTADOR, "Q-quem está falando isso? Foi você Jogador(a)?");

        RETRATO(IMG_NENHUMA);
        FALA(" ", "*Você nega balançando a cabeça*");

        RETRATO(IMG_ICONE_ALDEAO);
        FALA(NOME_APRESENTADOR, "Então vamos logo!");

        // Entrando no end
        MUSICA(MUSICA_SILENCIO);
        RETRATO(IMG_NENHUMA);
        CENARIO(IMG_H4_CENA_3);
        FALA(" ", "*Você avista uma fenda em meio de uma tempestade*");

        RETRATO(IMG_ICONE_ALDEAO);
        CENARIO(IMG_H4_CENA_4);
        FALA(NOME_APRESENTADOR, "Olha só, até ganhei roupas novas! Que lugar maravilhoso!");

        // Ender Dragon
        RETRATO(IMG_ICONE_ALDEAO_ASSUSTADO);
        SOM(SOM_ENDER_DRAGON_VOO);
        FALA(NOME_APRESENTADOR, "...");

        // Ender Dragon
        CENARIO(IMG_H4_CENA_5);
        RETRATO(IMG_ICONE_ENDER_DRAGON);
        SOM(SOM_ENDER_DRAGON_IDLE);
        FALA("Dragão do Fim", "*Sons absurdamente insanos*");

        RETRATO(IMG_ICONE_ALDEAO_ASSUSTADO);
        FALA(NOME_APRESENTADOR, "CUIDADO!");
        break;
    case CENA_HISTORIA_5:
        CENARIO(IMG_FUNDO_FIM);
        RETRATO(IMG_NENHUMA);
        FALA(
            " ",
            "*Você derrotou o Dragão do fim. Um grande portal se abre no centro da ilha.*"
        );

        CENARIO(IMG_FUNDO_PRETO);
        FALA(" ", "*Pulando dentro dele, sua visão é acobertado por uma interpretação abstrata da realidade.*");

        // Baseado em "End Poem" com tradução não oficial
        CENARIO(IMG_H5_CENA_1);
        MUSICA(MUSICA_MOOG_CITY);
        FALA(" ", "Quando tomar coragem para finalizar sua exploração pelo vasto universo:");
        FALA(" ", "Sentirá maior remorso do que foi ou não foi feito?");
        FALA(" ", "Se a dúvida o tem; \"Navegue para longe do porto seguro.");
        FALA(" ", "Pegue os ventos alísios em suas velas. Explore. Sonhe. Descubra\"");
        FALA(" ", "A única estrela que guia o norte da sua jornada, foi amarguramente devastada pela bússuola da vida,");
        FALA(" ", "encontrando-se entre devaneios de um mundo que sequer já existiu.");
        FALA(" ", "Em seu leito, carregara a resposta que determinava o mapa dos mares que já navegou.");
        FALA(" ", "Mapa esse que, oculto pelos pensamentos da estrela, na determinação de sua jornada encontrou todas as respostas.");
        FALA(" ", "E mesmo sem poder interpretar o próprio achado, descobriu o responsável da indescoberta aventura...");
        FALA(" ", "...que nunca acontecera devido aos anseios e temores de quem estava no controle do leme.");

        CENARIO(IMG_H5_CENA_2);
        MUSICA(MUSICA_SILENCIO);

        // Villager e Player acordando
        RETRATO(IMG_ICONE_ALDEAO);
        FALA(NOME_APRESENTADOR, "Ei... você está bem?");
        FALA(NOME_APRESENTADOR, "Eu tive um sonho...");
        FALA(NOME_APRESENTADOR, "peculiar.");
        FALA(NOME_APRESENTADOR, "O que aconteceu com seus olhos?");

        RETRATO(IMG_ICONE_STEVE);
        FALA("Você", "Meus...");

        RETRATO(IMG_ICONE_HEROBRINE);
        FALA("\"Você\"", "...olhos?");
        break;
    case CENA_HISTORIA_FIM:
        // Fim da historia
        RETRATO(IMG_ICONE_ALDEAO);
        MUSICA(MUSICA_SILENCIO);
        CENARIO(IMG_H5_CENA_3);

        FALA(NOME_APRESENTADOR, "...");
        FALA(NOME_APRESENTADOR, "...");

        RETRATO(IMG_ICONE_STEVE);
        FALA("Você", "O-o que aconteceu?");

        RETRATO(IMG_ICONE_ALDEAO);
        FALA(NOME_APRESENTADOR, "Venha comigo.");

        CENARIO(IMG_FUNDO_PRETO);
        FALA(NOME_APRESENTADOR, "...");

        MUSICA(MUSICA_EQUINOXE);
        CENARIO(IMG_H6_CENA_1);
        FALA(NOME_APRESENTADOR, "...");
        FALA(NOME_APRESENTADOR, "Você caiu, com olhos brancos e começou a se debater.");
        FALA(NOME_APRESENTADOR, "Estava dizendo sobre esmeraldas, do porquê ter buscado os prêmios");
        FALA(NOME_APRESENTADOR,
            "Eu não sei o que te motivou a chegar aqui, mas não deveria se culpar por conquistar algo");

        // Villager hipotético
        CENARIO(IMG_H6_CENA_2);
        FALA(NOME_APRESENTADOR, "Você poderia ter parado na primeira pergunta, ignorando a aventura.");

        CENARIO(IMG_H6_CENA_3);
        FALA(NOME_APRESENTADOR, "Mas, você decidiu vir até aqui, e não desistiu em qualquer momento.");
        FALA(NOME_APRESENTADOR, "Mesmo quando o seu maior inimigo era...");

        CENARIO(IMG_H6_CENA_4);
        FALA(NOME_APRESENTADOR, "Você.");
        FALA(NOME_APRESENTADOR, "E só você teve a coragem de enfrentar");
        FALA(NOME_APRESENTADOR,
            "Uma entidade explosiva, um espirito voador de três cabeças, um gigante feito de almas,"
            "e um Dragão gigante");
        FALA(NOME_APRESENTADOR, "Mas, se não tivesse a coragem de vencer a si mesmo, jamais teria chegado tão longe.");
        FALA(NOME_APRESENTADOR, "Vamos voltar para casa, meu amigo.");

        RETRATO(IMG_ICONE_STEVE);
        FALA("Você", "...");
        FALA("Você", "Obrigado.");

        CENARIO(IMG_H6_CENA_5);
        RETRATO(IMG_ICONE_ALDEAO);
        FALA(NOME_APRESENTADOR, "...");

        CENARIO(IMG_H6_CENA_6);
        SOM(SOM_PLATEIA);
        FALA(NOME_APRESENTADOR, "Muito obrigado, muito obrigado! Hmm!");
        FALA(
            NOME_APRESENTADOR,
            "Boa noite senhoras e senhores! Boa noite moças e rapazes! Boa noite meninas e meninos!"
        );
        FALA(NOME_APRESENTADOR, "Auditório, boa noite!");

        // Auditorio
        CENARIO(IMG_FUNDO_AUDITORIO);
        RETRATO(IMG_NENHUMA);
        SOM(SOM_PLATEIA);
        FALA("Plateia", "BOA NOITE!");

        // Apresentador
        CENARIO(IMG_H6_CENA_6);
        RETRATO(IMG_ICONE_ALDEAO);
        FALA(NOME_APRESENTADOR, "Voltamos com a programação normal. E vim anunciar o vencedor do...");
        FALA(NOME_APRESENTADOR, "MINE DO MILHÃO");

        CENARIO(IMG_H6_CENA_7);

        FALA(NOME_APRESENTADOR, "Uma salva de palmas para o nosso vencendor!");

        CENARIO(IMG_H6_CENA_8);
        RETRATO(IMG_NENHUMA);
        SOM(SOM_PLATEIA);
        FALA("Plateia", "WOAH!");

        CENARIO(IMG_H6_CENA_6);
        RETRATO(IMG_ICONE_ALDEAO);
        FALA(NOME_APRESENTADOR, "E lembrem-se crianças:");
        FALA(NOME_APRESENTADOR, "O verdadeiro Mine do Milhão são os amigos que fazemos pelo caminho");

        SOM(SOM_ALDEAO);
        FALA(NOME_APRESENTADOR, "Muito obrigado, muito obrigado! E Boa noite!");
        break;
    default:
        status = STATUS_ERRO_PARAMETRO;  /* cena inexistente */
        break;
    }

#undef FALA
#undef CENARIO
#undef RETRATO
#undef SOM
#undef MUSICA

    /* SOM(...) ou MUSICA(...) no fim do `case`, sem FALA depois, seria perdido sem aviso.
     * Só avisa no terminal: a cena continua tocando normalmente. */
    if (status == STATUS_OK && (som_proximo != SOM_NENHUM || musica_proxima != MUSICA_MANTER)) {
        TraceLog(LOG_WARNING, "DIALOGO: cena %d tem SOM/MUSICA sem FALA depois (ignorado)", (int)cena);
    }

    if (status == STATUS_OK && dialogo->quantidade > 0 && dialogo->indice < dialogo->quantidade) {
        dialogo->terminou = false;  /* havia terminado e ganhou falas novas: volta a tocar */
    }
    return status;
}

/**
 * @brief Monta uma fala do apresentador trocando o único %s de `formato` por `texto` e a acrescenta.
 *        Se não couber em TAM_FALA, o texto é cortado (snprintf nunca passa do buffer).
 */
static Status anexar_fala_formatada(Dialogo *dialogo, const char *formato, const char *texto)
{
    char fala[TAM_FALA];
    int escritos = snprintf(fala, sizeof(fala), formato, texto);

    if (escritos < 0) {
        return STATUS_ERRO_PARAMETRO;
    }
    return dialogo_adicionar_fala(dialogo, NOME_APRESENTADOR, fala, IMG_ICONE_ALDEAO);
}

Status dialogo_anexar_reacao(Dialogo *dialogo, bool acertou, const char *resposta_certa,
                             BossId boss_id, int esmeraldas, int variante)
{
    /*
     * - Cada texto pode ter NO MÁXIMO um %s (ou um %d, na tabela das esmeraldas). Sem nenhum também funciona.
     * - Para mostrar o símbolo de porcentagem, escreva %%.
     * - "" = essa fala não aparece.
     * - Cada fala tem REACOES_POR_RESULTADO (4) variações; uma delas é sorteada por reação. */

    /* 1ª fala: a resposta certa (%s = texto da alternativa certa). */

    static const char *const acerto_resposta[REACOES_POR_RESULTADO] = {
        "Isso! \"%s\" está certíssimo! Ainda bem que acertou!",
        "Exato! Era \"%s\" mesmo.",
        "Dessa vez você acertou essa questão!",
        "Isso! Isso! É isso mesmo!"
    };
    static const char *const erro_resposta[REACOES_POR_RESULTADO] = {
        "Droga! A resposta certa era \"%s\".",
        "Não foi dessa vez. Era \"%s\".",
        "Não, não, não, não, não!",
        "Nos vemos na próxima tentativa..."
    };

    /* 2ª fala: depende do boss (%s = nome do boss).
     * UMA LINHA POR BOSS, NA ORDEM DO ENUM BossId (boss.h). */
    static const char *const acerto_boss[BOSS_QUANTIDADE][REACOES_POR_RESULTADO] = {
        /* BOSS_CREEPER      */
        {
            "Parece que suas respostas dão dano, continue!",

            "Um acerto, um golpe? Não é hora de parar!",

            "Você dá dano quando acerta! Como eu sei? Logo respondo",

            "Que incrível! É como se acertar machucasse o %s"
        },
        /* BOSS_WITHER       */
        {
            "Que medo! Ele é muito forte! Continue acertando",

            "Que chamado nos chama em meio às chamas o que... "
            "Olha só para esse espírito de três cabeças",

            "Ma oê, quem quer dinheiro (me ajude)",

            "Que calor! Como queria estar no palco com meu ar condicionado..."
        },
        /* BOSS_WARDEN*/
        {
            "Acertou, mas fique em silêncio, ele nos ouve...",

            "Shii... não faça tanto barulho!",

            "fala baixo",

            "...",
        },
        /* BOSS_ENDER_DRAGON */
        {
            "Sei que está lidando com um dragão, mas gostou das roupas novas?",

            "O bafo dele é roxo! Eu nem sei o cheiro de roxo!",

            "E se fizermos o \"dragãozinho de papel\"?",

            "Se roubarmos o ovo dele, quantas esmeraldas dá?"
        },
        /* BOSS_HEROBRINE   */
        { "", "", "", "" }
    };
    static const char *const erro_boss[BOSS_QUANTIDADE][REACOES_POR_RESULTADO] = {
        /* BOSS_CREEPER      */
        {
            "Fujam! Ele vai explodir!",

            "Tragam um gato! Repito, tragam um gato!",

            "Perdeu logo nas perguntas fáceis?! Que decepção!",

            "Cadê a segurança?"
        },
        /* BOSS_WITHER       */
        {
            "Por que eu fui invocar isso?",

            "De fato ele se opõe aos céus!",

            "Alô? Produção? Conseguem me buscar no Nether eu estou nas coordenadas...",

            "Eu não quero morrer aqui!"
        },
        /* BOSS_WARDEN*/
        {
            "VOCÊ NÃO PODE ERRAR ASSIM!",

            "AGORA VOU FAZER TODO BARULHO POSSÍVEL! VÊ SE ACERTA NAS PRÓXIMAS",

            "FALEI PARA FALAR BAIXO, apesar de só eu estar falando...",

            "...putz",
        },
        /* BOSS_ENDER_DRAGON */
        {
            "Vou lá roubar o ovo dele! Depois ligo para a produção vir me buscar",

            "Não tem como eu te reviver? Nem com aquelas magias dos pillagers?",

            "Seremos cozidos pelo bafo roxo...",

            "Por favor dragão do fim! Tenha misericórdia!"
        },
        /* BOSS_HEROBRINE   */
        { "", "", "", "" }
    };


    /* 3ª fala, só no acerto: saldo de esmeraldas (%d = saldo atual). */
    static const char *const acerto_esmeraldas[REACOES_POR_RESULTADO] = {
        "",
        "",
        "Você vai querer usar essas %d esmeraldas! Guarde elas!",
        "Esmeraldas, esmeraldas e agora tens %d esmeraldas!"
    };

    static const char *const acerto_esmeraldas_herobrine[REACOES_POR_RESULTADO] = {
        "Por que eu quero ter mais %d esmeraldas?",
        "Pra que tantas esmeraldas?",
        "Tudo isso para... %d esmeraldas?",
        "",
    };

    Status status = STATUS_OK;
    const char *texto;
    char fala[TAM_FALA];
    int indice;

    if (dialogo == NULL || resposta_certa == NULL || (int)boss_id < 0 || boss_id >= BOSS_QUANTIDADE) {
        return STATUS_ERRO_PARAMETRO;
    }
    indice = (variante < 0 ? -variante : variante) % REACOES_POR_RESULTADO;

    /* 1ª fala: resposta certa */
    /* Remove a primeira parte caso o boss seja o Herobrine*/
    if (boss_id == BOSS_HEROBRINE ) {
        texto = "";
    } else if (boss_id == BOSS_WARDEN){
        // Diz "..." caso seja o warden
        texto = "...";
    } else {
        texto = acertou ? acerto_resposta[indice] : erro_resposta[indice];
    }

    if (texto != NULL && texto[0] != '\0') {
        status = anexar_fala_formatada(dialogo, texto, resposta_certa);
    }

    /* 2ª fala: reação do boss */
    texto = acertou ? acerto_boss[boss_id][indice] : erro_boss[boss_id][indice];
    if (status == STATUS_OK && texto != NULL && texto[0] != '\0') {
        status = anexar_fala_formatada(dialogo, texto, boss_nome(boss_id));
    }

    /* 3ª fala (só no acerto): esmeraldas */
    if (boss_id == BOSS_HEROBRINE ) {
        texto = acerto_esmeraldas_herobrine[indice];
    } else if (boss_id == BOSS_WARDEN){
        // Diz "..." caso seja o warden
        texto = "...%d esmeraldas";
    } else {
        texto = acerto_esmeraldas[indice];
    }

    if (status == STATUS_OK && acertou && texto != NULL && texto[0] != '\0') {
        snprintf(fala, sizeof(fala), texto, esmeraldas);
        if (boss_id == BOSS_HEROBRINE) {
            status = dialogo_adicionar_fala(dialogo, "Você", fala, IMG_ICONE_STEVE);
        } else {
            status = dialogo_adicionar_fala(dialogo, NOME_APRESENTADOR, fala, IMG_ICONE_ALDEAO);
        }

    }
    return status;
}

/* ---- 2. Controle do diálogo ---- */

void dialogo_limpar(Dialogo *dialogo)
{
    if (dialogo == NULL) {
        return;
    }
    memset(dialogo, 0, sizeof(*dialogo));
    dialogo->terminou = true;          /* vazio = nada para mostrar */
    dialogo->indice_fala_anunciada = -1;  /* depois do memset: 0 diria "a 1ª fala já foi entregue" */
}

Status dialogo_adicionar_fala(Dialogo *dialogo, const char *personagem, const char *texto,
                              ImagemId retrato)
{
    Fala *nova_fala;

    if (dialogo == NULL || personagem == NULL || texto == NULL
        || dialogo->quantidade >= DIALOGO_MAX_FALAS
        || strlen(personagem) >= TAM_NOME_PERSONAGEM || strlen(texto) >= TAM_FALA) {
        return STATUS_ERRO_PARAMETRO;
    }

    nova_fala = &dialogo->falas[dialogo->quantidade];
    strcpy(nova_fala->personagem, personagem);
    strcpy(nova_fala->texto, texto);
    nova_fala->retrato = retrato;
    nova_fala->som = SOM_NENHUM;        /* a macro FALA troca se houver SOM(...) antes */
    nova_fala->fundo = IMG_NENHUMA;     /* a macro FALA troca pelo CENARIO atual */
    nova_fala->musica = MUSICA_MANTER;  /* a macro FALA troca se houver MUSICA(...) antes */
    dialogo->quantidade++;
    dialogo->terminou = false;
    return STATUS_OK;
}

const Fala *dialogo_fala_nova(Dialogo *dialogo)
{
    /* Nada novo: diálogo vazio/acabado ou esta fala já foi entregue */
    if (dialogo == NULL || dialogo->terminou || dialogo->indice >= dialogo->quantidade
        || dialogo->indice == dialogo->indice_fala_anunciada) {
        return NULL;
    }
    dialogo->indice_fala_anunciada = dialogo->indice;
    return &dialogo->falas[dialogo->indice];
}

Status dialogo_carregar_cena(Dialogo *dialogo, CenaId cena)
{
    if (dialogo == NULL) {
        return STATUS_ERRO_PARAMETRO;
    }
    dialogo_limpar(dialogo);
    return dialogo_anexar_cena(dialogo, cena);
}

CenaId dialogo_cena_do_boss(BossId id)
{
    if ((int)id < 0 || id >= BOSS_QUANTIDADE) {
        return CENA_QUANTIDADE;
    }
    return (CenaId)(CENA_BOSS_CREEPER + (int)id);
}

const Fala *dialogo_fala_atual(const Dialogo *dialogo)
{
    if (dialogo == NULL || dialogo->terminou || dialogo->indice >= dialogo->quantidade) {
        return NULL;
    }
    return &dialogo->falas[dialogo->indice];
}

int dialogo_letras_visiveis(const Dialogo *dialogo)
{
    float letras = 0.0f;

    if (dialogo == NULL) {
        return 0;
    }
    letras = dialogo->tempo_na_fala * (float)DIALOGO_LETRAS_POR_SEGUNDO;
    return letras > (float)LETRAS_FALA_COMPLETA ? LETRAS_FALA_COMPLETA : (int)letras;
}

bool dialogo_atualizar(Dialogo *dialogo, float dt, bool jogador_avancou)
{
    const Fala *fala;

    if (dialogo == NULL) {
        return true;
    }
    fala = dialogo_fala_atual(dialogo);
    if (fala == NULL) {
        dialogo->terminou = true;
        return true;
    }

    dialogo->tempo_na_fala += dt;
    if (!jogador_avancou) {
        return false;
    }

    if (dialogo_letras_visiveis(dialogo) < assets_contar_letras(fala->texto)) {
        /* 1º toque: mostra a fala inteira de uma vez */
        dialogo->tempo_na_fala = (float)LETRAS_FALA_COMPLETA / (float)DIALOGO_LETRAS_POR_SEGUNDO;
        return false;
    }

    /* 2º toque: próxima fala (ou fim) */
    dialogo->indice++;
    dialogo->tempo_na_fala = 0.0f;
    if (dialogo->indice >= dialogo->quantidade) {
        dialogo->terminou = true;
    }
    return dialogo->terminou;
}

/* ---- 3. Desenho ---- */

void dialogo_desenhar(const Dialogo *dialogo, const Assets *assets)
{
    const Fala *fala = dialogo_fala_atual(dialogo);
    Rectangle caixa, area_retrato, area_texto, area_nome;
    bool tem_retrato;

    if (fala == NULL || assets == NULL) {
        return;
    }

    /* Caixa no rodapé da janela */
    caixa.x = MARGEM_CAIXA_DIALOGO;
    caixa.width = LARGURA_JANELA - 2.0f * MARGEM_CAIXA_DIALOGO;
    caixa.height = ALTURA_CAIXA_DIALOGO;
    caixa.y = ALTURA_JANELA - ALTURA_CAIXA_DIALOGO - MARGEM_CAIXA_DIALOGO;
    assets_desenhar_painel(assets, IMG_CAIXA_DIALOGO, caixa, PALETA_PAINEL, WHITE);

    /* Retrato à esquerda (se a fala tiver um) */
    tem_retrato = (fala->retrato != IMG_NENHUMA);
    area_retrato.x = caixa.x + MARGEM_CAIXA_DIALOGO;
    area_retrato.y = caixa.y + (caixa.height - LADO_RETRATO_DIALOGO) / 2.0f;
    area_retrato.width = LADO_RETRATO_DIALOGO;
    area_retrato.height = LADO_RETRATO_DIALOGO;
    if (tem_retrato) {
        assets_desenhar_imagem(assets, fala->retrato, area_retrato, WHITE);
    }

    /* Nome e texto à direita do retrato */
    area_nome.x = tem_retrato ? area_retrato.x + LADO_RETRATO_DIALOGO + MARGEM_CAIXA_DIALOGO
                              : caixa.x + MARGEM_CAIXA_DIALOGO;
    area_nome.y = caixa.y + MARGEM_CAIXA_DIALOGO * 0.75f;
    area_nome.width = caixa.x + caixa.width - MARGEM_CAIXA_DIALOGO - area_nome.x;
    area_nome.height = FONTE_NOME_DIALOGO;
    assets_desenhar_texto(assets, fala->personagem, (Vector2){ area_nome.x, area_nome.y },
                          FONTE_NOME_DIALOGO, PALETA_DESTAQUE);

    area_texto.x = area_nome.x;
    area_texto.y = area_nome.y + FONTE_NOME_DIALOGO * 1.5f;
    area_texto.width = area_nome.width;
    area_texto.height = caixa.y + caixa.height - MARGEM_CAIXA_DIALOGO * 1.5f - area_texto.y;
    assets_desenhar_texto_quebrado(assets, fala->texto, area_texto, FONTE_TEXTO_DIALOGO,
                                   PALETA_TEXTO, dialogo_letras_visiveis(dialogo));

    /* Aviso para continuar, só com a fala completa */
    if (dialogo_letras_visiveis(dialogo) >= assets_contar_letras(fala->texto)) {
        const char *aviso = "ESPAÇO ou clique para continuar";
        Vector2 posicao_aviso;

        posicao_aviso.x = caixa.x + caixa.width - MARGEM_CAIXA_DIALOGO
                        - assets_largura_texto(assets, aviso, FONTE_AVISO_DIALOGO);
        posicao_aviso.y = caixa.y + caixa.height - MARGEM_CAIXA_DIALOGO - FONTE_AVISO_DIALOGO * 0.5f;
        assets_desenhar_texto(assets, aviso, posicao_aviso, FONTE_AVISO_DIALOGO, PALETA_TEXTO_SUAVE);
    }
}

#endif /* USE_RAYLIB */