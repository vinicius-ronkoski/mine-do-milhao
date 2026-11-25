/**
 * @file historia.c
 * @brief Conteúdo da história (ver historia.h): falas de cada cena e reações do apresentador.
 *
 * Organização: 1) cenas (um `case` por CenaId), 2) reações a acerto/erro, 3) utilidades.
 * O motor que mostra as falas fica em dialogo.c. Compila só com USE_RAYLIB.
 */
#ifdef USE_RAYLIB

#include <stdio.h>
#include <string.h>
#include "historia.h"

/** Nome do apresentador nas falas. */
#define NOME_APRESENTADOR "Silvio Aldeão"

/* ---- 1. Falas de cada cena  ---- */

Status historia_anexar_cena(Dialogo *dialogo, CenaId cena)
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
                fala_criada->cena = (int)cena;   /* para o save saber em que cena está */  \
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
        MUSICA(MUSICA_SILENCIO);
        MUSICA(MUSICA_WITHER);
        RETRATO(IMG_ICONE_ALDEAO);
        FALA(NOME_APRESENTADOR, "Prepare-se, ele é muito mais forte que o creeper!");
        break;
    case CENA_BOSS_WARDEN:
        MUSICA(MUSICA_SILENCIO);
        MUSICA(MUSICA_WARDEN);
        RETRATO(IMG_ICONE_ALDEAO_ASSUSTADO);
        FALA(NOME_APRESENTADOR, "...shh, não deixe ele te ouvir!");
        break;
    case CENA_BOSS_ENDER_DRAGON:
        MUSICA(MUSICA_SILENCIO);
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
        MUSICA(MUSICA_SILENCIO);
        MUSICA(MUSICA_HEROBRINE);
        RETRATO(IMG_ICONE_HEROBRINE);
        FALA("\"Você\"", "O que estou fazendo aqui?");
        break;
    case CENA_DERROTA:
        MUSICA(MUSICA_SILENCIO);
        RETRATO(IMG_ICONE_ALDEAO_ASSUSTADO);
        FALA(NOME_APRESENTADOR, "Ei cara, levante, precisamos de você!");
        break;
    case CENA_PARADA:
        MUSICA(MUSICA_SILENCIO);
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
        MUSICA(MUSICA_SILENCIO);
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
        MUSICA(MUSICA_SILENCIO);
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
        MUSICA(MUSICA_PIGSTEP);
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
        MUSICA(MUSICA_SILENCIO);
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
        MUSICA(MUSICA_SILENCIO);
        CENARIO(IMG_H3_CENA_5);
        SOM(SOM_ALDEAO);
        RETRATO(IMG_ICONE_ALDEAO);
        FALA(NOME_APRESENTADOR, "Hmm...");
        FALA(NOME_APRESENTADOR, "Parabéns... eu acho.");
        FALA(NOME_APRESENTADOR, "Bem enquanto você lutava com suas respostas, eu reparei num quarto no final da sala.");

        // Apresentador aponta sala do fim do corredor
        CENARIO(IMG_H4_CENA_1);
        SOM(SOM_VINE_BOOM);
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
        FALA("???", "Cuidado com o que te aguarda. A curiosidade sempre mata o gato.");

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
        MUSICA(MUSICA_SILENCIO);
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
        
        MUSICA(MUSICA_SILENCIO);
        CENARIO(IMG_FUNDO_PRETO);
        RETRATO(IMG_NENHUMA);
        FALA("?", "\"Ei... acorde!\"");
        
        CENARIO(IMG_H5_CENA_2);
        RETRATO(IMG_ICONE_ALDEAO);
        FALA(NOME_APRESENTADOR, "Você está bem?");

        // Villager e Player acordando
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
        CENARIO(IMG_FUNDO_HEROBRINE);

        RETRATO(IMG_ICONE_HEROBRINE);
        FALA("\"Você\"", "...");
        
        MUSICA(MUSICA_SILENCIO);
        RETRATO(IMG_ICONE_STEVE);
        FALA("Você", "...");

        CENARIO(IMG_FUNDO_PRETO);

        RETRATO(IMG_NENHUMA);
        FALA("Você", "Tudo isso valeu a pena?");

        RETRATO(IMG_ICONE_ALDEAO);
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
        MUSICA(MUSICA_PRINCIPAL);
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

/* ---- 2. Reações do apresentador ---- */

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

Status historia_anexar_reacao(Dialogo *dialogo, bool acertou, const char *resposta_certa,
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

/* ---- 3. Utilidades ---- */

Status historia_carregar_cena(Dialogo *dialogo, CenaId cena)
{
    if (dialogo == NULL) {
        return STATUS_ERRO_PARAMETRO;
    }
    dialogo_limpar(dialogo);
    return historia_anexar_cena(dialogo, cena);
}

CenaId historia_cena_do_boss(BossId id)
{
    if ((int)id < 0 || id >= BOSS_QUANTIDADE) {
        return CENA_QUANTIDADE;
    }
    return (CenaId)(CENA_BOSS_CREEPER + (int)id);
}

/* ---- 4. Checkpoints e nomes das cenas (usados pela revelação e pelo save) ---- */

/** Cena que antecede cada boss. Para mudar a história antes de um boss, troque só aqui. */
static const struct {
    CenaId cena;
    BossId boss;
} CHECKPOINTS[] = {
    { CENA_HISTORIA_2, BOSS_WITHER },        /* indo para o Nether */
    { CENA_HISTORIA_3, BOSS_WARDEN },
    { CENA_HISTORIA_4, BOSS_ENDER_DRAGON },  /* indo para o End */
    { CENA_HISTORIA_5, BOSS_HEROBRINE }      /* voltando do End */
};

#define QUANTIDADE_CHECKPOINTS ((int)(sizeof(CHECKPOINTS) / sizeof(CHECKPOINTS[0])))

CenaId historia_cena_antes_do_boss(BossId boss)
{
    int indice;

    for (indice = 0; indice < QUANTIDADE_CHECKPOINTS; indice++) {
        if (CHECKPOINTS[indice].boss == boss) {
            return CHECKPOINTS[indice].cena;
        }
    }
    return CENA_QUANTIDADE;
}

BossId historia_boss_da_cena_salva(CenaId cena)
{
    int boss;

    /* Gravam o save: a história antes de cada boss e a entrada dos bosses que têm essa história
     * (do Wither em diante; o começo do jogo, até o Creeper, não grava) */
    for (boss = 0; boss < BOSS_QUANTIDADE; boss++) {
        CenaId historia = historia_cena_antes_do_boss((BossId)boss);

        if (historia != CENA_QUANTIDADE
            && (cena == historia || cena == historia_cena_do_boss((BossId)boss))) {
            return (BossId)boss;
        }
    }
    return BOSS_QUANTIDADE;
}

/** Nome de cada cena no save. Com [CENA_X] = "...", a ordem do enum pode mudar sem quebrar saves. */
static const char *const NOMES_CENAS[CENA_QUANTIDADE] = {
    [CENA_INTRO]             = "INTRO",
    [CENA_BOSS_CREEPER]      = "BOSS_CREEPER",
    [CENA_BOSS_WITHER]       = "BOSS_WITHER",
    [CENA_BOSS_WARDEN]       = "BOSS_WARDEN",
    [CENA_BOSS_ENDER_DRAGON] = "BOSS_ENDER_DRAGON",
    [CENA_BOSS_HEROBRINE]    = "BOSS_HEROBRINE",
    [CENA_DERROTA]           = "DERROTA",
    [CENA_PARADA]            = "PARADA",
    [CENA_HISTORIA_1]        = "HISTORIA_1",
    [CENA_HISTORIA_2]        = "HISTORIA_2",
    [CENA_HISTORIA_3]        = "HISTORIA_3",
    [CENA_HISTORIA_4]        = "HISTORIA_4",
    [CENA_HISTORIA_5]        = "HISTORIA_5",
    [CENA_HISTORIA_FIM]      = "HISTORIA_FIM"
};

const char *historia_nome_cena(CenaId cena)
{
    if ((int)cena < 0 || cena >= CENA_QUANTIDADE || NOMES_CENAS[cena] == NULL) {
        return "";
    }
    return NOMES_CENAS[cena];
}

CenaId historia_cena_pelo_nome(const char *nome)
{
    int cena;

    if (nome == NULL) {
        return CENA_QUANTIDADE;
    }
    for (cena = 0; cena < CENA_QUANTIDADE; cena++) {
        if (NOMES_CENAS[cena] != NULL && strcmp(NOMES_CENAS[cena], nome) == 0) {
            return (CenaId)cena;
        }
    }
    return CENA_QUANTIDADE;
}

#endif /* USE_RAYLIB */
