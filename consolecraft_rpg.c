#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#ifdef _WIN32
    #include <windows.h>
#else
    #include <unistd.h>
    #include <locale.h>
#endif
#define TAM 30

#define TAM_VILA 10
#define MAX_VILAS_ENCONTRADAS 20

#define TAM_CAVE 10
#define MAX_CAVERNAS_ENCONTRADAS 15

#define MAX_MENSAGENS 20
#define MAX_TAM_MENSAGENS 80

#define ANSI_BG_AZUL_ESCURO "\x1b[44m"
#define ANSI_BG_VERDE "\x1b[42m"
#define ANSI_COLOR_FOSCO "\x1b[90m" // Cinza escuro (Bright Black)
#define ANSI_BG_YELLOW   "\x1b[43m" // Fundo Amarelo
#define ANSI_BG_LIGHT_GREEN "\x1b[102m" // Fundo Cinza
#define ANSI_RESET       "\x1b[0m"   // Reseta para a cor padrão

//P.S, quando for usar os emojis, usa esses (tem a mesma quantidade de bytes, se preferir mude para outros emojis com os mesmos bytes): 🤠💀🗻💧🟩

struct Receita{
    char itemDesejado;
    char id_um;
    char id_dois;
    char id_tres;
    int quantidade_um;
    int quantidade_dois;
    int quantidade_tres;
    int totalItems;
    int seraCraftado;
};

struct Inimigo{
    int id;
    int ataque;
    int vida;
    int x;
    int y;
    int estadoAtual;
    int item;
};

struct Vila{
    int x;
    int y;
    char mapa[TAM_VILA][TAM_VILA];
};

struct Caverna{
    int x;
    int y;
    char mapaCaverna[TAM_CAVE][TAM_CAVE];
};

struct SlotItem{
    int id;
    int quantidade;
};

void limparTela(){
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void dormir(int ms){
    #ifdef _WIN32
        Sleep(ms);
    #else
        usleep(ms * 1000);
    #endif
}
//função estética para esconder o cursor que aparece toda vez que o mapa é recarregado
void hideCursor() {
    #ifdef _WIN32
        HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
        CONSOLE_CURSOR_INFO info;
        info.dwSize = 100;
        info.bVisible = FALSE;
        SetConsoleCursorInfo(consoleHandle, &info);
    #else
        printf("\033[?25l");
    #endif
}
void gotoxy(int x, int y){
    #ifdef _WIN32
        COORD coord;
        coord.X = x;
        coord.Y = y;
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
    #else
        printf("\033[%d;%dH",y+1,x+1);
    #endif
}

void imprimirComEmojis(char caractere){
    switch(caractere){
        case 'P':
            printf(ANSI_BG_VERDE "%-4s" ANSI_RESET, "🤠");
            break;
        case '.':
            printf(ANSI_BG_VERDE "%-4s" ANSI_RESET, "🟩");
            break;
        case '~':
            printf(ANSI_BG_AZUL_ESCURO "%-4s" ANSI_RESET, "🟦");
            break;
        case '^':
            printf(ANSI_BG_VERDE "%-4s" ANSI_RESET, "🗻");
            break;
        case 'Z':
        case 'E':
            printf(ANSI_BG_VERDE "%-4s" ANSI_RESET, "💀");
            break;
        case 'V':
        case 'H':
            printf(ANSI_BG_VERDE "%-4s" ANSI_RESET, "🏠");
            break;
        case 'F':
            printf("%-4s", "🔩");
            break;
        case '|':
            printf("%-4s", "🌑");
            break;
        case 'A':
            printf(ANSI_BG_LIGHT_GREEN);
            printf("%-4s", "🌾");
            printf(ANSI_RESET);
            break;
        default:
            printf("%-4c", caractere);
            break;
    }
}
void imprimirFosco(char caractere){
    if(caractere == 'P'){
        printf("%-4s", "🤠");
    }
    else{
        printf("%-4s", "🌑");
    }
}
void destacarJogador(char **mundo, int jogador_x,int jogador_y){
    limparTela();

    gotoxy(0,2);
    for(int i=0;i<TAM;i++){
        for(int j=0;j<TAM;j++){
            if(i == jogador_x && j == jogador_y){
                printf(ANSI_BG_YELLOW);
                imprimirFosco(mundo[i][j]);
                printf(ANSI_RESET);
            }
            else{
                printf(ANSI_COLOR_FOSCO);
                imprimirFosco(mundo[i][j]);
                printf(ANSI_RESET);
            }
        }
        printf("\n");
    }
    printf("\nAntes de começar, o jogador 🤠 irá se destacar no mapa!\n");
    fflush(stdout);
    dormir(3000);
}
//função para criar o mundo inicial
void criarMundo(int seed, char **mundo,char **armazenamento, int **mapaMascaraMineracao){
    srand(time(NULL));
    for(int i = 0; i<TAM;i++){
        for(int j = 0;j<TAM;j++){
            seed = rand() % 100;
            if(seed < 2){
                mundo[i][j] = 'V';
                armazenamento[i][j] = mundo[i][j];
                mapaMascaraMineracao[i][j] = 0;
            }
            else if(seed < 10){
                mundo[i][j] = '^';
                armazenamento[i][j] = mundo[i][j];
                if(rand() % 100 < 75){
                    mapaMascaraMineracao[i][j] = 1;
                }
                else {
                    mapaMascaraMineracao[i][j] = 0;
                }
            }
            else if(seed < 40){
                mundo[i][j] = '~';
                armazenamento[i][j] = mundo[i][j];
                mapaMascaraMineracao[i][j] = 0;
            }
            else if(seed < 100){
                mundo[i][j] = '.';
                armazenamento[i][j] = mundo[i][j];
                if(rand() % 100 < 50){
                    mapaMascaraMineracao[i][j] = 1;
                }
                else{
                    mapaMascaraMineracao[i][j] = 0;
                }
            }
        }
    }
}
void ajuda(){
    printf("I -> Inventário\nC -> Crafting\nM -> Minerar");
}
void notificacaoUI(const char* mensagem,int novasMensagens){
    gotoxy(0,0);
    char buffer[MAX_TAM_MENSAGENS+50];
    if(novasMensagens > 1){
        sprintf(buffer,"[...] %s (Digite L para ver mensagens antigas)",mensagem);
    }
    else{
        strcpy(buffer,mensagem);
    }
    printf("%-60s",buffer);
}
void desenharUI(char **mundo,const char log[][MAX_TAM_MENSAGENS],int novasMensagens){
    #ifdef _WIN32
        
    #else
        limparTela();
    #endif
    notificacaoUI(log[MAX_MENSAGENS-1],novasMensagens);
    printf("|");
    gotoxy(0,1);
    printf("____________________________________________________________\n");
    gotoxy(0,2);
    for(int i = 0;i<TAM;i++){
        for(int j=0;j<TAM;j++){
            imprimirComEmojis(mundo[i][j]);
        }
        printf("\n");
    }
    printf("\n");
    printf("Digite um movimento estilo WASD || Para sair do jogo, digite X \n");
}
void adicionarLog(char log[][MAX_TAM_MENSAGENS], const char* novaMensagem,int *cont){
    for(int i = 0;i<MAX_MENSAGENS-1;i++){
        strcpy(log[i],log[i+1]);
    }
    strcpy(log[MAX_MENSAGENS-1],novaMensagem);
    (*cont)++;
}
void exibirLog(char log[][MAX_TAM_MENSAGENS]){
    printf("-- Caixa de Correio --\n");
    printf("______________________\n\n");
    for(int i = 0;i< MAX_MENSAGENS;i++){
        if(log[i][0] != '\0'){
            printf("%s\n",log[i]);
        }
    }
}

void adicionarItemCrafting(struct SlotItem mochila[],int indice){
    int item_escolhido = indice;
    for(int i=0;i<15;i++){
        if(mochila[i].id == item_escolhido){
            mochila[i].quantidade++;
            return;
        }
    }
    for(int i=0;i<15;i++){
        if(mochila[i].id == -1){
            mochila[i].id = item_escolhido;
            mochila[i].quantidade = 1;
            return;
        }
    }
}
void adicionarItemInimigo(struct Inimigo inimigo1[], struct SlotItem mochila[],int indice){
    int item_escolhido = inimigo1[indice].item;
    for(int i=0;i<15;i++){
        if(mochila[i].id == item_escolhido){
            mochila[i].quantidade++;
            return;
        }
    }
    
    for(int i=0;i<15;i++){
        if(mochila[i].id == -1){         
            mochila[i].id = item_escolhido;
            mochila[i].quantidade = 1;
            return;
        }
    }
}
void adicionarItemMundo(struct SlotItem mochila[], int id_item){
    int item_escolhido = id_item;
    for(int i=0;i<15;i++){
        if(mochila[i].id == item_escolhido){
            mochila[i].quantidade++;
            return;
        }
    }
    
    for(int i=0;i<15;i++){
        if(mochila[i].id == -1){         
            mochila[i].id = item_escolhido;
            mochila[i].quantidade = 1;
            return;
        }
    }
}

const char* bibliotecaIDs(int id){
    switch(id){
        //Vazio
        case -1: return "Vazio";

        //ID(1-100) -> Elementos Básicos
        case 1: return "Madeira";
        case 2: return "Osso de esqueleto";
        case 3: return "Pedra";
        case 4: return "Ferro";
        case 5: return "Prisma Luminosa";

        //ID(101-500) -> Elementos Compostos (precisam de crafting para existir)
        case 101: return "Picareta de Madeira";
        case 102: return "Picareta de Ferro";
        case 103: return "Balde (vazio)";

        //ID(501-550) -> Inimigos/Personagens
        case 501: return "Zumbi";
        case 502: return "Esqueleto";

        //Caso desconhecido/genérico
        default: return "?";
    }
}

void zumbi(char **mundo,char **armazenamento, struct Inimigo inimigo1[],int *xzumbi,int *yzumbi,int *quantidade,int *x,int *y){
    char zumbi = 'Z';
    int quantidadeSpawn = rand() % 10 + 1;
    for(int i = 0;i<quantidadeSpawn;i++){
        *xzumbi = rand() % TAM;
        *yzumbi = rand() % TAM;
        inimigo1[*quantidade].id = 501;
        inimigo1[*quantidade].vida = 35;
        inimigo1[*quantidade].ataque = 10;
        inimigo1[*quantidade].x = *xzumbi;
        inimigo1[*quantidade].y = *yzumbi;
        inimigo1[*quantidade].estadoAtual = 1;
        //solução usada pra caso o inimigo queira nascer no mesmo quadrado do jogar, seria bizarro lidar com um inimigo logo no primeiro segundo do jogo xD
        while(mundo[*xzumbi][*yzumbi] == mundo[*x][*y]){
            *xzumbi = rand() % TAM;
            *yzumbi = rand() % TAM;
        }
        mundo[*xzumbi][*yzumbi] = zumbi;

        (*quantidade)++;
    }
}
void esqueleto(char **mundo,char **armazenamento, struct Inimigo inimigo2[],int *xesqueleto, int *yesqueleto,int *quantidade,int *x,int *y){
    char esqueleto = 'E';
    int quantidadeSpawn = rand() % 5 + 1;
    for(int i = 0; i<quantidadeSpawn;i++){
        *xesqueleto = rand() % TAM;
        *yesqueleto = rand() % TAM;
        inimigo2[*quantidade].id = 502;
        inimigo2[*quantidade].item = 2;
        inimigo2[*quantidade].vida = 30;
        inimigo2[*quantidade].ataque = 20;
        inimigo2[*quantidade].x = *xesqueleto;
        inimigo2[*quantidade].y = *yesqueleto;
        inimigo2[*quantidade].estadoAtual = 1;
        while(mundo[*xesqueleto][*yesqueleto] == mundo[*x][*y]){
            *xesqueleto = rand() % TAM;
            *yesqueleto = rand() % TAM;
        }
        mundo[*xesqueleto][*yesqueleto] = esqueleto;
        (*quantidade)++;
    }
}
int ataque(int *vida, int ataque, struct Inimigo inimigo1[], int indice,struct SlotItem mochila[]){
    int vidaInimigoAntes = inimigo1[indice].vida;
    char comando;
    comando = toupper(comando);
    printf("Você está atacando um %s >:D \n",bibliotecaIDs(inimigo1[indice].id));
    printf("Vida = %d \n",*vida);
    while(vida !=0 && inimigo1[indice].vida !=0){
        puts("[Z] Atacar | [X] Escapar\n");
        scanf(" %c",&comando);
        if(comando == 'x'){
            limparTela();
            printf("🤠 - Não tô muito afim de lutar agora \n");
            inimigo1[indice].vida = vidaInimigoAntes;
            dormir(1500);
            return 2;
        }
        if(comando == 'z'){
            inimigo1[indice].vida = inimigo1[indice].vida - ataque;
            printf("Você usou um ataque com %d de dano \n Vida atual do inimigo = %d \n", ataque, inimigo1[indice].vida);
            dormir(1000);
            *vida = *vida - inimigo1[indice].ataque;
            printf("O inimigo usou um ataque com %d de dano \n Sua vida atual = %d \n", inimigo1[indice].ataque, *vida);
        }
        if(*vida <= 0){
            return 1;
        }
        else if(inimigo1[indice].vida <= 0){
            adicionarItemInimigo(inimigo1,mochila,indice);
            return 0;
        }
    }
    return 2;
}

void equipamento(struct SlotItem mochila[]){
    int peitoral = 1;
    int calca = 1;
    int botas = 1;
    int escudo = 0;

    printf("\n");
    printf("         //////////         \n");
    printf("        ////////////        \n");
    printf("        ////////////        \n");
    printf("         //////////         \n");
    printf("\n");
    printf("   ██████          ██████   \n");
    printf(" ██████████████████████████ \n");
    printf("████████████████████████████\n");
    printf("████████████████████████████\n");
    printf("    ████████████████████    \n");
    printf("    ████████████████████    \n");
    printf("    ████████████████████    \n");
    printf("\n");
    printf("    ████████████████████    \n");
    printf("    ████████████████████    \n");
    printf("    ████████    ████████    \n");
    printf("    ████████    ████████    \n");
    printf("    ████████    ████████    \n");
    printf("    ████████    ████████    \n");
    printf("\n");
    printf("    ████████    ████████    \n");
    printf("████████████    ████████████\n");
    printf("████████████    ████████████\n");
    printf("████████████    ████████████\n");                                                            
}    

void inventario(struct SlotItem mochila[]){
    printf("---Inventário---\n");
    for(int i=0;i<15;i++){
        printf("%d = ",i);
        printf("%s",bibliotecaIDs(mochila[i].id));

        if(mochila[i].id != -1){
            printf(" (Quantidade: %d)",mochila[i].quantidade);
        }
        printf("\n");
    }
    printf("Pressione Enter para fechar \n");
}

int verificarMochila(struct SlotItem mochila[],int id_escolhido){
    for(int i = 0;i<15;i++){
        if(mochila[i].id == id_escolhido){
            return 1;
        }
    }
    return 0;
}

void bancoReceitas(struct Receita receitas[], int *quantidadeReceitas){
    int i = 0;
    receitas[i].itemDesejado = 101;
    receitas[i].quantidade_um = 5;
    receitas[i].id_um = 1;
    receitas[i].id_dois = 0;
    receitas[i].quantidade_dois = 0;
    receitas[i].id_tres = 0;
    receitas[i].quantidade_tres = 0;
    i++;

    receitas[i].itemDesejado = 102;
    receitas[i].quantidade_um = 3;
    receitas[i].id_um = 4;
    receitas[i].id_dois = 1;
    receitas[i].quantidade_dois = 2;
    receitas[i].id_tres = 0;
    receitas[i].quantidade_tres = 0;
    i++;

    receitas[i].itemDesejado = 103;
    receitas[i].quantidade_um = 3;
    receitas[i].id_um = 4;
    receitas[i].id_dois = 0;
    receitas[i].quantidade_dois = 0;
    receitas[i].id_tres = 0;
    receitas[i].quantidade_tres = 0;
    i++;
    *quantidadeReceitas = i;
}

int verificarCrafting(struct Receita receitas,struct SlotItem mochila[]){
    int achouItemUm = 0;
    int achouItemDois = 0;
    int achouItemTres = 0;


    if(receitas.id_um <=0){
        achouItemUm = 1;
    }
    if(receitas.id_dois <=0){
        achouItemDois = 1;
    }
    if(receitas.id_tres <=0){
        achouItemTres = 1;
    }
    for(int i = 0;i<15;i++){
        if(achouItemUm == 0 && mochila[i].id == receitas.id_um && mochila[i].quantidade >= receitas.quantidade_um){
            achouItemUm = 1;
        }
        if(achouItemDois == 0 && mochila[i].id == receitas.id_dois && mochila[i].quantidade >= receitas.quantidade_dois){
            achouItemDois = 1;
        }
        if(achouItemTres == 0 && mochila[i].id == receitas.id_tres && mochila[i].quantidade >= receitas.quantidade_tres){
            achouItemTres = 1;
        }
    }
    if(achouItemUm && achouItemDois && achouItemTres){
        return 1;
    }
    return 0;
}

void removerItemsCrafting(struct SlotItem mochila[],struct Receita receita_escolhida){
    if(receita_escolhida.id_um > 0){
        for(int i = 0;i<15;i++){
            if(mochila[i].id == receita_escolhida.id_um){
                mochila[i].quantidade = mochila[i].quantidade - receita_escolhida.quantidade_um;
                if(mochila[i].quantidade <= 0){
                    mochila[i].id = -1;
                    mochila[i].quantidade = 0;
                }
                break;
            }
        }
    }
    if(receita_escolhida.id_dois > 0){
        for(int i = 0;i<15;i++){
            if(mochila[i].id == receita_escolhida.id_dois){
                mochila[i].quantidade = mochila[i].quantidade - receita_escolhida.quantidade_dois;
                if(mochila[i].quantidade <= 0){
                    mochila[i].id = -1;
                    mochila[i].quantidade = 0;
                }
                break;
            }
        }
    }
    if(receita_escolhida.id_tres > 0){
        for(int i = 0;i<15;i++){
            if(mochila[i].id == receita_escolhida.id_tres){
                mochila[i].quantidade = mochila[i].quantidade - receita_escolhida.quantidade_tres;
                if(mochila[i].quantidade <= 0){
                    mochila[i].id = -1;
                    mochila[i].quantidade = 0;
                }
                break;
            }
        }
    }
}
void crafting(struct SlotItem mochila[], struct Receita receitas[],int totalReceitas){
    int id_escolhido;
    int podeFabricar[100] = {0};
    printf("Você está no sistema de crafting\n");
    inventario(mochila);
    printf("\n");
    printf(" _______________________\n| Sistema de crafting ⛏ |\n|_______________________|\n");
    for(int i = 0;i < totalReceitas;i++){
        const char *nomeDesejado = bibliotecaIDs(receitas[i].itemDesejado);
        printf("[%d] %s (Requisitos: ",i,nomeDesejado);
        if(receitas[i].id_um > 0){
            printf("%dx %s",receitas[i].quantidade_um,bibliotecaIDs(receitas[i].id_um));
        }
        if(receitas[i].id_dois > 0){
            printf("| %dx %s",receitas[i].quantidade_dois,bibliotecaIDs(receitas[i].id_dois));
        }
        if(receitas[i].id_tres > 0){
            printf("| %dx %s",receitas[i].quantidade_tres,bibliotecaIDs(receitas[i].id_tres));
        }
        if(verificarCrafting(receitas[i],mochila)){
            printf(" - Fabricável ✅)\n");
            podeFabricar[i] = 1;
        }
        else{
            printf("- Não Fabricável)\n");
        }
    } 
    printf("_______________________\n");
    puts("Digite o número da receita que deseja craftar: ");
    scanf("%d",&id_escolhido);
    if(id_escolhido < 0 || id_escolhido >= totalReceitas){
        printf("Sua escolha não está na lista de receitas \n");
        return;
    }
    
    if(podeFabricar[id_escolhido] == 1){
        adicionarItemCrafting(mochila,receitas[id_escolhido].itemDesejado);
        removerItemsCrafting(mochila,receitas[id_escolhido]);
        printf("Item adicionado com sucesso! \n");
    }
    else{
        printf("Você não pode fabricar esse item ainda");
    }
}

void jogador(char **mundo,char **armazenamento,int *x,int *y){
    char jogador = 'P';
    char *pjogador = &jogador;
    int vida = 100;
    int ataque = 15;
    *x = rand() % TAM;
    *y = rand() % TAM;
    mundo[*x][*y] = *pjogador;
}
int movimentoJogador(char **mundo,char **armazenamento,char jogador,int *x,int *y,int tamanho){
    char movimento;
    scanf(" %c",&movimento);
    movimento = toupper(movimento);
    switch(movimento){
        case 'W':
           if (*x > 0){
            mundo[*x][*y] = armazenamento[*x][*y];
            (*x)--;
            mundo[*x][*y] = jogador;
            return 2;
           }
           break;
        case 'A':
           if (*y > 0){
            mundo[*x][*y] = armazenamento[*x][*y];
            (*y)--;
            mundo[*x][*y] = jogador;
            return 3;
           }
           break;
        case 'S':
            if (*x < tamanho-1){
             mundo[*x][*y] = armazenamento[*x][*y];
             (*x)++;
             mundo[*x][*y] = jogador;
             return 4;
            }
            break;
        case 'D':
           if (*y < tamanho-1){
            mundo[*x][*y] = armazenamento[*x][*y];
            (*y)++;
            mundo[*x][*y] = jogador;
            return 5;
           }
           break;
        case 'I':
           return 6;
           break;
        case 'C':
           return 7;
           break;
        case 'M':
           return 8;
           break;
        case 'L':
           return 9;
           break;
        case 'E':
           return 10;
           break;
        case '?':
           return 98;
           break;
        case 'X':
           return 1;
           break;
        default:
           return 99;
    }
    return 0;
}
void vila(int comando,int jogador_x, int jogador_y, struct Vila vilas[],int *indice_vilas){
    struct Vila *vilaAtual = NULL;
    int seedVila;
    for(int i=0;i< *indice_vilas;i++){
        if(vilas[i].x == jogador_x && vilas[i].y == jogador_y){
            vilaAtual = &vilas[i];
            break;
        }
    }

    if(vilaAtual == NULL){
        vilaAtual = &vilas[*indice_vilas];

        vilaAtual->x = jogador_x;
        vilaAtual->y = jogador_y;

        srand(time(NULL));
        for(int i = 0; i<TAM_VILA;i++){
            for(int j = 0;j<TAM_VILA;j++){
                seedVila = rand() % 100;
                if(seedVila < 4){
                    vilaAtual->mapa[i][j] = 'A';
                }
                else if(seedVila < 12){
                    vilaAtual->mapa[i][j] = 'H';                    
                }
                else if(seedVila < 100){
                    vilaAtual->mapa[i][j] = '.';                    
                }
            }
        }
        (*indice_vilas)++;
        printf("Você entrou em uma vila nova! \n");
        
    }
    int x,y;
    char **coordenadasVila = (char **)malloc(TAM_VILA * sizeof(char *));
    char **armazenamentoVila = (char **)malloc(TAM_VILA * sizeof(char *));

    if(coordenadasVila == NULL || armazenamentoVila == NULL){
        printf("Erro de alocação de memória \n");
        return;
    }
    for(int i = 0; i<TAM_VILA;i++){
        coordenadasVila[i] = (char *)calloc(TAM_VILA,sizeof(char));
        armazenamentoVila[i] = (char *)calloc(TAM_VILA,sizeof(char));

        if(coordenadasVila[i] == NULL || armazenamentoVila[i] == NULL){
            printf("Erro de alocação de memória \n");
            return;
        }
    }

    for(int i =0;i<TAM_VILA;i++){
        for(int j=0;j<TAM_VILA;j++){
            coordenadasVila[i][j] = vilaAtual->mapa[i][j];
            armazenamentoVila[i][j] = vilaAtual->mapa[i][j];
        }
    }
    switch(comando){
        case 2:
            x = TAM_VILA-1;
            y = TAM_VILA/2;
            break;
        case 3:
            x = TAM_VILA/2;
            y = TAM_VILA-1;
            break;
        case 4:
            x = 0;
            y = TAM_VILA/2;
            break;
        case 5:
            x = TAM_VILA/2;
            y = 0;
            break;
    }
    coordenadasVila[x][y] = 'P';
    while(1){
        limparTela();

        for(int i = 0; i < TAM_VILA;i++){
            for(int j = 0;j< TAM_VILA;j++){
                imprimirComEmojis(coordenadasVila[i][j]);
            }
            printf(" \n");
        }

        int x_ant = x;
        int y_ant = y;
        movimentoJogador(coordenadasVila,armazenamentoVila,'P',&x,&y,TAM_VILA);
        if(x_ant == x && y_ant == y){
            if(x == 0 || x == TAM_VILA-1 || y == 0 || y == TAM_VILA-1){
                limparTela();
                printf("Você está saindo da vila");
                dormir(2000);
                break;
            }
        }
        if(armazenamentoVila[x][y] == 'H'){
            limparTela();
            printf("👨 \n");
            printf("Olá, você parece ser novo por aqui!");
            getchar();
            getchar();
            limparTela();
        }
    }
    
    for(int i = 0;i < TAM_VILA;i++){
        free(coordenadasVila[i]);
        free(armazenamentoVila[i]);
    }
    free(coordenadasVila);
    free(armazenamentoVila);
}

void minerar(int jogador_x, int jogador_y, struct Caverna cavernas[],int *indice_cavernas,struct SlotItem mochila[]){
    struct Caverna *cavernaAtual = NULL;
    int seedCaverna;

    for(int i = 0; i< *indice_cavernas;i++){
        if(cavernas[i].x == jogador_x && cavernas[i].y == jogador_y){
            cavernaAtual = &cavernas[i];
            break;
        }
    }

    if(cavernaAtual == NULL){
        cavernaAtual = &cavernas[*indice_cavernas];

        cavernaAtual->x = jogador_x;
        cavernaAtual->y = jogador_y;
        srand(time(NULL));
        for(int i = 0; i<TAM_CAVE;i++){
            for(int j = 0;j<TAM_CAVE;j++){
                seedCaverna = rand() % 100;
                if(seedCaverna < 8){
                    cavernaAtual->mapaCaverna[i][j] = 'F';                    
                }
                else if(seedCaverna < 100){
                    cavernaAtual->mapaCaverna[i][j] = '|';                    
                }
            }
        }
        (*indice_cavernas)++;
        printf("Você encontrou uma caverna nova! \n");
    }

    int x = TAM_CAVE/2;
    int y = 0;
    char **coordenadasCaverna = (char **)malloc(TAM_CAVE * sizeof(char *));
    char **armazenamentoCaverna = (char **)malloc(TAM_CAVE * sizeof(char *));
    if(coordenadasCaverna == NULL || armazenamentoCaverna == NULL){
        printf("Erro de alocação de memória \n");
        return;
    }
    for(int i = 0; i<TAM_CAVE;i++){
        coordenadasCaverna[i] = (char *)calloc(TAM_CAVE,sizeof(char));
        armazenamentoCaverna[i] = (char *)calloc(TAM_CAVE,sizeof(char));

        if(coordenadasCaverna[i] == NULL || armazenamentoCaverna[i] == NULL){
            printf("Erro de alocação de memória \n");
            return;
        }
    }
    
    for(int i=0;i<TAM_CAVE;i++){
        for(int j=0;j<TAM_CAVE;j++){
            coordenadasCaverna[i][j] = cavernaAtual->mapaCaverna[i][j];
            armazenamentoCaverna[i][j] = cavernaAtual->mapaCaverna[i][j];
        }
    }
    coordenadasCaverna[x][y] = 'P';
    while(1){
        limparTela();
        
        for(int i=0;i<TAM_CAVE;i++){
            for(int j=0;j<TAM_CAVE;j++){
                imprimirComEmojis(coordenadasCaverna[i][j]);
            }
            printf(" \n");
        }
        int x_ant = x;
        int y_ant = y;
        if(armazenamentoCaverna[x][y] == 'F' && verificarMochila(mochila,101) == 1){
            adicionarItemMundo(mochila,4);
            armazenamentoCaverna[x][y] = '|';
            printf("Você minerou 1 ferro! \n");
        }
        else if(armazenamentoCaverna[x][y] == 'F' && verificarMochila(mochila,101) == 0){
            printf("Parece que precisa de uma picareta melhor para isso \n");
        }

        movimentoJogador(coordenadasCaverna,armazenamentoCaverna,'P',&x,&y,TAM_CAVE);

        if(x_ant == x && y_ant == y){
            if(x == 0 || x == TAM_CAVE-1 || y == 0 || y == TAM_CAVE-1){
                limparTela();
                printf("Você está saindo da caverna");
                dormir(2000);
                break;
            }
        }
    }

    for(int i = 0;i<TAM_CAVE;i++){
        free(coordenadasCaverna[i]);
        free(armazenamentoCaverna[i]);
    }
    free(coordenadasCaverna);
    free(armazenamentoCaverna);
}
int main(){
    limparTela();
    #ifdef _WIN32
        SetConsoleOutputCP(CP_UTF8);
    #else
        setlocale(LC_ALL,"");
    #endif
    hideCursor();
    struct Receita receitas[100];
    int quantidadeReceitas = 0;
    bancoReceitas(receitas,&quantidadeReceitas);

    struct Inimigo inimigo[20];
    int quantidadeInimigos = 0;

    struct Vila vilas[MAX_VILAS_ENCONTRADAS];
    int vilasEncontradas = 0;

    struct Caverna cavernas[MAX_CAVERNAS_ENCONTRADAS];
    int cavernasEncontradas = 0;

    struct SlotItem mochila[15];
    for(int i =0;i<15;i++){
        mochila[i].id = -1;
        mochila[i].quantidade = 0;
    }

    char logMensagens[MAX_MENSAGENS][MAX_TAM_MENSAGENS];
    for(int i = 0;i< MAX_MENSAGENS;i++){
        logMensagens[i][0] = '\0';
    }

    int vidaInicialJogador = 100;
    int ataqueInicialJogador = 15;
    int seed = 0;
    int *pseed = &seed;
    char **mundo = (char **)malloc(TAM * sizeof(char *));
    for(int i = 0;i<TAM;i++){
        mundo[i] = (char *)calloc(TAM, sizeof(char));
    }

    char **armazenamento = (char **)malloc(TAM * sizeof(char *));
    for(int i = 0;i<TAM;i++){
        armazenamento[i] = (char *)calloc(TAM, sizeof(char));
    }

    int **mapaMascaraMineracao = (int **)malloc(TAM * sizeof(int *));
    for(int i = 0;i<TAM;i++){
        mapaMascaraMineracao[i] = (int *)calloc(TAM, sizeof(int));
    }
    int x,y;
    int xzumbi,yzumbi;
    int xesqueleto,yesqueleto;
    criarMundo(*pseed,mundo,armazenamento,mapaMascaraMineracao);
    jogador(mundo,armazenamento,&x,&y);
    zumbi(mundo,armazenamento,inimigo,&xzumbi,&yzumbi,&quantidadeInimigos,&x,&y);
    esqueleto(mundo,armazenamento,inimigo,&xesqueleto,&yesqueleto,&quantidadeInimigos,&x,&y);
    printf("Mensagem do jogo                                            |\n____________________________________________________________\n");
    for(int i = 0;i<TAM;i++){
        for(int j=0;j<TAM;j++){
            imprimirComEmojis(mundo[i][j]);
        }
        printf("\n");
    }
    printf("Elementos do jogo: \n");
    printf("Montanha = 🗻 | Água = 💧 | Terra = 🟩 | Jogador = 🤠 \n");
    printf("______________________________________________\n");
    printf("WASD para movimento, X para sair, para outros comandos digite ? \n\n");
    printf("Pressione qualquer tecla para continuar: ");
    getchar();
    limparTela();
    destacarJogador(mundo,x,y);
    limparTela();
    int mensagensTurnoAnterior = 0;
    while (1){
        //Geração do mundo, irá ser gerado enquanto o while não for 1
        int mensagensPorTurno = 0;
        desenharUI(mundo,logMensagens,mensagensTurnoAnterior);
        int chance = rand() % 100;

        int x_ant = x;
        int y_ant = y;

        int armazenarComando = movimentoJogador(mundo,armazenamento,'P',&x,&y,TAM);
        if(armazenarComando == 1){
            break;
        }
        if(armazenarComando == 6){
            limparTela();
            inventario(mochila);
            getchar();
            getchar();
        }
        if(armazenarComando == 7){
            limparTela();
            crafting(mochila,receitas,quantidadeReceitas);
            getchar();
            getchar();
            limparTela();
        }
        if(armazenarComando == 8){
            if(mapaMascaraMineracao[x][y] == 1){
                minerar(x,y,cavernas,&cavernasEncontradas,mochila);
            }
            else{
                adicionarLog(logMensagens,"Você tentou minerar, mas nada encontrou. \n",&mensagensPorTurno);
            }
        }
        if(armazenarComando == 9){
            limparTela();
            exibirLog(logMensagens);
            getchar();
            getchar();
            limparTela();
        }
        if(armazenarComando == 10){
            limparTela();
            equipamento(mochila);
            getchar();
            getchar();
            limparTela();
        }
        if(armazenarComando == 98){
            limparTela();
            ajuda();
            getchar();
            getchar();
        }
        if(armazenarComando == 99){
            adicionarLog(logMensagens,"Movimento inválido, use WASD \n",&mensagensPorTurno);
        }
        if(armazenamento[x][y] == '.' && chance < 25){
            adicionarItemMundo(mochila,1);
            adicionarLog(logMensagens, "Você coletou +1 madeira!",&mensagensPorTurno);
        }
        if(armazenamento[x][y] == '~' && chance < 10){
            adicionarItemMundo(mochila,5);
            adicionarLog(logMensagens, "Você coletou +1 prisma luminosa",&mensagensPorTurno);
        }
        //Checagem da posição do jogador e do inimigo
        for(int i = 0; i<quantidadeInimigos;i++){
            if(inimigo[i].estadoAtual && x == inimigo[i].x && y == inimigo[i].y){
                limparTela();
                int luta = ataque(&vidaInicialJogador,ataqueInicialJogador,inimigo,i,mochila);
                if(luta == 1){
                    printf("\nGame Over :<\n Reinicie o jogo! \n");
                    exit(0);
                }
                if(luta == 2){
                    x = x_ant;
                    y = y_ant;
                    char tipoInimigo = (inimigo[i].id == 1) ? 'Z' : 'E';
                    mundo[inimigo[i].x][inimigo[i].y] = tipoInimigo; 

                    mundo[x][y] = 'P';
                    limparTela();
                }
                else{
                    inimigo[i].estadoAtual = 0;
                    limparTela();
                    printf("--------------------------\n");
                    printf("Você venceu o inimigo!\n Pressione Enter para continuar! \n");
                    getchar();
                    getchar();
                }
            }
        }
        
        //Sistema para detectar se o jogador vai entrar em uma vila, o jogo identifica se a posição do jogador é igual a posição da vila no mundo sem modificação(o mundo de "armazenamento")
        if(armazenamento[x][y] == 'V'){
            vila(armazenarComando,x,y,vilas,&vilasEncontradas);
        }

        if(mensagensPorTurno == 0){
            adicionarLog(logMensagens, "", &mensagensPorTurno);
        }
        mensagensTurnoAnterior = mensagensPorTurno;
    }
    for(int i = 0;i<TAM;i++){
        free(mundo[i]);
        free(armazenamento[i]);
    }
    free(mundo);
    free(armazenamento);
}
