#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#ifdef _WIN32
    #include <windows.h>
    #include <conio.h>
#else
    #include <unistd.h>
    #include <locale.h>
    #include <termios.h>
    #include <fcntl.h>
#endif

#ifndef _WIN32
// Função para verificar se uma tecla foi pressionada no Linux (não-bloqueante)
int kbhit(void) {
    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if(ch != EOF) {
        ungetc(ch, stdin);
        return 1;
    }

    return 0;
}

// Função para pegar o caractere sem esperar Enter no Linux
int getch(void) {
    struct termios oldattr, newattr;
    int ch;
    tcgetattr( STDIN_FILENO, &oldattr );
    newattr = oldattr;
    newattr.c_lflag &= ~( ICANON | ECHO );
    tcsetattr( STDIN_FILENO, TCSANOW, &newattr );
    ch = getchar();
    tcsetattr( STDIN_FILENO, TCSANOW, &oldattr );
    return ch;
}
#endif

#define TAM 30

#define TAM_VILA 10
#define MAX_VILAS_ENCONTRADAS 20

#define TAM_CAVE 10
#define MAX_CAVERNAS_ENCONTRADAS 15

#define TAM_DUNGEON 30
#define MAX_DUNGEONS_ENCONTRADAS 10

#define MAX_MENSAGENS 20
#define MAX_TAM_MENSAGENS 80

#define ANSI_BG_AZUL_ESCURO "\x1b[44m"
#define ANSI_BG_VERDE "\x1b[42m"
#define ANSI_COLOR_FOSCO "\x1b[90m" // Cinza escuro (Bright Black)
#define ANSI_BG_YELLOW "\x1b[43m" // Fundo Amarelo
#define ANSI_BG_LIGHT_GREEN "\x1b[102m" // Fundo Cinza
#define ANSI_BG_LARANJA_ESCURO "\x1b[43m\x1b[2m" // Fundo Laranja Escuro
#define ANSI_BG_MARROM "\x1b[41m\x1b[2m"
#define ANSI_BG_CINZA_ESCURO "\x1b[5;100m"
#define ANSI_RESET "\x1b[0m"   // Reseta para a cor padrão

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
    int quantidadeItem;
    int espacoLuta;
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

struct Dungeon{
    int x;
    int y;
    int salas;
    char mapaDungeon[TAM_DUNGEON][TAM_DUNGEON];
    char campoVisao[TAM_DUNGEON][TAM_DUNGEON];
};

struct SlotItem{
    int id;
    int quantidade;
};

struct Produto{
    int id;
    int quantidade;
    int preco;
};

void limparBuffer(){
    int c;
    while((c = getchar()) != '\n' && c != EOF){ }
}
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
// P = 🤠
// . = 🟩
// ~ = 🟦
// ^ = 🗻
// ! = 🔘
// @ = 🛑
// | = 🌑
// A = 🌾
// B = 🏫
// F = 🔩
// I = 🧓
// M = 🟫
// V|H = 🏠
// Z|E = 💀
// 0 = 📕
// 1 = 📘
// 2 = 📗
// 3 = 📙
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
        case '!':
            printf("%-4s", "🔘");
            break;
        case ' ':
            printf("  ");
            break;
        case '@':
            printf(ANSI_BG_LARANJA_ESCURO "%-4s" ANSI_RESET, "🛑");
            break;
        case '#':
            printf("%-4s","🔳");
            break;
        case 'Z':
        case 'E':
        case 'S':
            printf(ANSI_BG_VERDE "%-4s" ANSI_RESET, "💀");
            break;
        case 'V':
        case 'H':
            printf(ANSI_BG_VERDE "%-4s" ANSI_RESET, "🏠");
            break;
        case 'F':
            printf("%-4s", "🔩");
            break;
        case 'G':
            printf("%-4s","🌕");
            break;
        case 'I':
            printf("%-4s", "🧓");
            break;
        case 'M':
             printf(ANSI_BG_MARROM "%-4s" ANSI_RESET, "🟫");
            break;
        case '|':
            printf("%-4s", "🌑");
            break;
        case 'A':
            printf(ANSI_BG_LIGHT_GREEN);
            printf("%-4s", "🌾");
            printf(ANSI_RESET);
            break;
        case 'B':
            printf(ANSI_BG_LIGHT_GREEN);
            printf("%-4s", "🏫");
            printf(ANSI_RESET);
            break;
        case 'D':
            printf("%-4s","💎");
            break;
        case '0':
            printf(ANSI_BG_CINZA_ESCURO "%-4s" ANSI_RESET, "📕");
            break;
        case '1':
            printf(ANSI_BG_CINZA_ESCURO "%-4s" ANSI_RESET, "📘");
            break;
        case '2':
            printf(ANSI_BG_CINZA_ESCURO "%-4s" ANSI_RESET, "📗");
            break;
        case '3':
            printf(ANSI_BG_CINZA_ESCURO "%-4s" ANSI_RESET, "📙");
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
void criarMundo(int seed, char **mundo,char **armazenamento, int **mapaMascaraMineracao, int **mapaDungeons){
    srand(time(NULL));
    for(int i = 0; i<TAM;i++){
        for(int j = 0;j<TAM;j++){
            seed = rand() % 100;
            if(seed < 2){
                mundo[i][j] = 'V';
                armazenamento[i][j] = mundo[i][j];

                mapaMascaraMineracao[i][j] = 0;
                mapaDungeons[i][j] = 0;
            }
            else if(seed < 10){
                mundo[i][j] = '^';
                armazenamento[i][j] = mundo[i][j];
                if(rand() % 100 < 75){
                    mapaMascaraMineracao[i][j] = 1;
                    mapaDungeons[i][j] = 0;
                }
                else {
                    mapaMascaraMineracao[i][j] = 0;
                    mapaDungeons[i][j] = 1;
                }
            }
            else if(seed < 40){
                mundo[i][j] = '~';
                armazenamento[i][j] = mundo[i][j];
                mapaMascaraMineracao[i][j] = 0;
                mapaDungeons[i][j] = 0;
            }
            else if(seed < 100){
                mundo[i][j] = '.';
                armazenamento[i][j] = mundo[i][j];
                if(rand() % 100 < 50){
                    mapaMascaraMineracao[i][j] = 1;
                    mapaDungeons[i][j] = 0;
                }
                else{
                    mapaMascaraMineracao[i][j] = 0;
                    mapaDungeons[i][j] = 0;
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
int movimentoConstanteJogador(char movimento,char **mundo,char **armazenamento,char jogador, int *x,int *y,int tamanho){
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
    } return 0;
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
void adicionarItemInimigo(struct Inimigo inimigo1[], struct SlotItem mochila[],int indice,int quantidadeItems){
    int item_escolhido = inimigo1[indice].item;
    int cont = 0;

    for(int i=0;i<15;i++){
        if(mochila[i].id == item_escolhido){
            while(cont < quantidadeItems){
                mochila[i].quantidade++;
                cont++;
            }
            return;
        }
    }
    
    for(int i=0;i<15;i++){
        if(mochila[i].id == -1){         
            mochila[i].id = item_escolhido;
            mochila[i].quantidade = quantidadeItems;
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
        case 6: return "Ouro";
        case 7: return "Trigo";
        case 8: return "Maçã";
        case 9: return "Cana de Açúcar";
        case 10: return "Diamante";
        case 11: return "Linha";

        //ID(101-500) -> Elementos Compostos (precisam de crafting/compra para existir)
        case 101: return "Picareta de Madeira";//Craftável
        case 102: return "Picareta de Ferro";//Craftável
        case 103: return "Balde (vazio)";//Craftável
        case 104: return "Espada de Madeira";//Craftável
        case 105: return "Espada de Ferro";//Craftável
        case 106: return "Espada de Pedra";//Craftável
        case 107: return "Espada de Diamante";//Craftável
        case 108: return "Foice Gigante";//Craftável
        case 109: return "Arco e Flecha";//Craftável
        case 110: return "";
        case 111: return "";
        case 112: return "";
        case 113: return "";
        case 114: return "Escudo de Madeira";
        case 115: return "Escudo de Ferro";
        case 116: return "Escudo de Diamante";
        case 117: return "Escudo Refletor";
        case 118: return "Ursinho de Pelúcia";
        case 119: return "Bolo";
        case 120: return "Pão";
        case 121: return "Torta de Maçã";
        case 122: return "Poção de Cura(pequeno)";
        case 123: return "Poção de Cura(grande)";
        case 124: return "Picareta de Diamante";//Craftável
        case 125: return "Picareta de Pedra";//Craftável

        //ID(501-550) -> Inimigos/Personagens
        case 501: return "Zumbi";
        case 502: return "Esqueleto";
        case 503: return "Aranha";

        //Caso desconhecido/genérico
        default: return "?";
    }
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


//Funções de criar e gerenciar inimigos/ataques
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
        inimigo1[*quantidade].espacoLuta = 6;
        inimigo1[*quantidade].quantidadeItem = 1;
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
        inimigo2[*quantidade].espacoLuta = 6;
        inimigo2[*quantidade].quantidadeItem = 1;
        while(mundo[*xesqueleto][*yesqueleto] == mundo[*x][*y]){
            *xesqueleto = rand() % TAM;
            *yesqueleto = rand() % TAM;
        }
        mundo[*xesqueleto][*yesqueleto] = esqueleto;
        (*quantidade)++;
    }
}
void aranha(char **mundo,char **armazenamento, struct Inimigo inimigo3[],int *xaranha, int *yaranha,int *quantidade,int *x,int *y){
    char aranha = 'S';
    int quantidadeSpawn = rand() % 8 + 2;
    for(int i = 0;i < quantidadeSpawn;i++){
        *xaranha = rand() % TAM;
        *yaranha = rand() % TAM;
        inimigo3[*quantidade].id = 503;
        inimigo3[*quantidade].item = 11;
        inimigo3[*quantidade].vida = 25;
        inimigo3[*quantidade].ataque = 4;
        inimigo3[*quantidade].x = *xaranha;
        inimigo3[*quantidade].y = *yaranha;
        inimigo3[*quantidade].estadoAtual = 1;
        inimigo3[*quantidade].espacoLuta = 8;
        inimigo3[*quantidade].quantidadeItem = 2;
        while(mundo[*xaranha][*yaranha] == mundo[*x][*y]){
            *xaranha = rand() % TAM;
            *yaranha = rand() % TAM;
        }
        mundo[*xaranha][*yaranha] = aranha;
        (*quantidade)++;
    }
}
void interfaceAtaque(char **coordenadasLuta, char **armazenamentoLuta,int *x, int *y,int espacoTotal,struct Inimigo inimigo1[],int *vida, int indice, int movimentoLivre){
    
    //int coordenadasJogador[x][y] = [2][2];
    //int coordenadasInimigo[x][y] = [2][4];
    gotoxy(0,0);
    printf("Você está atacando um %s >:D \n",bibliotecaIDs(inimigo1[indice].id));
    printf("Vida = %d \n",*vida);
    for(int i = 0;i<espacoTotal;i++){
        for(int j = 0;j<espacoTotal;j++){
            imprimirComEmojis(coordenadasLuta[i][j]);
        }
        printf("\n");
    }
    printf("\n");
    
    if(movimentoLivre){
        while(kbhit()) getch();
        time_t tempoInicial = time(NULL);
        double limite = 5.0;
        limparTela();
        while(1){
            time_t tempoUsado = time(NULL);
            double tempoCorrido = difftime(tempoUsado,tempoInicial);
            if(tempoCorrido >= limite){
                break;
            }

            gotoxy(0,2);
            for(int i = 0;i<espacoTotal;i++){
                for(int j = 0;j<espacoTotal;j++){
                    imprimirComEmojis(coordenadasLuta[i][j]);
                }
                printf("\n");
            }
            if(kbhit()){
                char tecla = getch();
                movimentoConstanteJogador(tecla,coordenadasLuta,armazenamentoLuta,'P',x,y,espacoTotal);
            }
            dormir(50);
        }

    }
}
int mira(char **coordenadasLuta, char **armazenamentoLuta,int espacoTotal,struct Inimigo inimigo1[],int *vida, int indice, int movimentoLivre){
    int mira[11] = {3,0,0,1,1,2,1,1,0,0,0};
    int armazenamentoMira[11] = {0,0,0,1,1,2,1,1,0,0,0};
    int pontoAtual = 0;
    int direcaoDireita = 1;

    int tempX = 0;
    int tempY = 0;
    time_t tempoInicial = time(NULL);
    double limite = 10.0;

    hideCursor();
    interfaceAtaque(coordenadasLuta,armazenamentoLuta,&tempX,&tempY,espacoTotal,inimigo1,vida,indice,0);
    printf("Digite [Enter] para calibrar sua mira \n");
    while(1){
        char pontoEscolhido;
        time_t tempoUsado = time(NULL);
        double tempoCorrido = difftime(tempoUsado,tempoInicial);
        if(tempoCorrido >= limite){
            break;
        }
        hideCursor();
        gotoxy(0,espacoTotal+4);
        for(int i = 0;i<11;i++){
            switch(mira[i]){
                case 0:
                    printf("🟥");
                break;

                case 1:
                    printf("🟨");
                break;
                    
                case 2:
                    printf("🟩");
                break;
    
                case 3:
                    printf("🔘");
                break;
            }
        }
        printf("\n");

        if(kbhit()){
            pontoEscolhido = getch();
            if(pontoEscolhido == '\n' || pontoEscolhido == '\r'){
                return armazenamentoMira[pontoAtual];
                break;
            }
        }

        mira[pontoAtual] = armazenamentoMira[pontoAtual];

        if(pontoAtual < 10 && direcaoDireita == 1){
            pontoAtual++;
        }
        if(pontoAtual == 10){
            direcaoDireita = 0;
            pontoAtual--;
        }
        if(pontoAtual < 10 && direcaoDireita == 0){
            pontoAtual--;
        }
        if(pontoAtual == 0 && direcaoDireita == 0){
            direcaoDireita = 1;
        }

        mira[pontoAtual] = 3;

        dormir(50);
    }
    return 0;
    //printf("🔘🟥🟥🟨🟨🟩🟨🟨🟥🟥🟥");
}
int atacar(struct SlotItem mochila[]){
    int escolha;
    int itemEscolhido = -1;
    int *inventarioAtaqueCurto = (int *)calloc(5, sizeof(int));
    int *inventarioAtaqueLongo = (int *)calloc(5, sizeof(int));
    if(inventarioAtaqueCurto == NULL || inventarioAtaqueLongo == NULL){
        printf("Erro de alocação de memória \n");
        return -1;
    }
    for(int i = 0;i<5;i++){
        if(verificarMochila(mochila,104+i)){
            inventarioAtaqueCurto[i] = 104+i;
        }
    }
    for(int i = 0;i<5;i++){
        if(verificarMochila(mochila,109+i)){
            inventarioAtaqueLongo[i] = 109+i;
        }
    }
    printf("Digite o número do equipamento que quer usar \n");
    printf("Ataque Curto \n");
    for(int i = 0;i<5;i++){
        printf("[%d] - %s \n",i,bibliotecaIDs(inventarioAtaqueCurto[i]));
    }
    printf("\nAtaque Longo \n");
    for(int i = 0;i<5;i++){
        printf("[%d] - %s \n",i+5,bibliotecaIDs(inventarioAtaqueLongo[i]));
    }
    scanf("%d",&escolha);
    if(escolha >= 0 && escolha < 5){
        if(inventarioAtaqueCurto[escolha] >= 104){
            itemEscolhido = inventarioAtaqueCurto[escolha];
        }
    }
    if(escolha >= 5 && escolha < 10){
        if(inventarioAtaqueLongo[escolha - 5] >= 109){
            itemEscolhido = inventarioAtaqueLongo[escolha - 5];
        }
    }
    free(inventarioAtaqueCurto);
    free(inventarioAtaqueLongo);
    return itemEscolhido;
}
int defender(struct SlotItem mochila[]){
    int escolha;
    int itemEscolhido = -1;

    int *inventarioEscudo = (int *)calloc(5,sizeof(int));
    int *inventarioCura = (int *)calloc(5,sizeof(int));

    if(inventarioEscudo == NULL || inventarioCura == NULL){
        printf("Erro de alocação de memória \n");
        return -1;
    }

    for(int i = 0;i<5;i++){
        if(verificarMochila(mochila,114+i)){
            inventarioEscudo[i] = 114+i;
        }
    }
    for(int i = 0;i<5;i++){
        int cont = 0;
        if(verificarMochila(mochila,119+i)){
            inventarioCura[i] = 119+i;
            cont++;
        }
        else if(verificarMochila(mochila,7)){
            inventarioCura[i] = 7;
            cont++;
        }
        else if(verificarMochila(mochila,8)){
            inventarioCura[i] = 8;
            cont++;
        }
        else if(verificarMochila(mochila,9)){
            inventarioCura[i] = 9;
            cont++;
        }
        if(cont == 5){
            break;
        }
    }

    printf("Digite o número do equipamento que deseja usar para defesa \n");
    printf("Escudos \n");
    for(int i = 0;i<5;i++){
        printf("[%d] - %s \n",i,bibliotecaIDs(inventarioEscudo[i]));
    }
    printf("\nComida/Cura \n");
    for(int i = 0;i<5;i++){
        printf("[%d] - %s \n",i+5,bibliotecaIDs(inventarioCura[i]));
    }
    scanf("%d",&escolha);
    if(escolha >= 0 && escolha < 5){
        if(inventarioEscudo[escolha] >= 114){
            itemEscolhido = inventarioEscudo[escolha];
        }
    }
    if(escolha >=5 && escolha < 10){
        if(inventarioCura[escolha - 5] >= 119 || inventarioCura[escolha - 5] == 7 || inventarioCura[escolha - 5] == 8 || inventarioCura[escolha - 5] == 9){
            itemEscolhido = inventarioCura[escolha - 5];
        }
    }
    free(inventarioEscudo);
    free(inventarioCura);
    return itemEscolhido;
}
int mainAtaque(int *vida, int ataque, struct Inimigo inimigo1[], int indice,struct SlotItem mochila[]){
    int espacoTotal = inimigo1[indice].espacoLuta;

    int x_jogador = espacoTotal/2;
    int y_jogador = 0;

    int x_inimigo = espacoTotal/2;
    int y_inimigo = espacoTotal-1;

    char **coordenadasLuta = (char **)malloc(espacoTotal * sizeof(char *));
    char **armazenamentoLuta = (char **)malloc(espacoTotal * sizeof(char *));
    if(coordenadasLuta == NULL || armazenamentoLuta == NULL){
        printf("Erro de alocação de memória \n");
        return -1;
    }
    for(int i = 0;i<espacoTotal;i++){
        coordenadasLuta[i] = (char *)calloc(espacoTotal,sizeof(char));
        armazenamentoLuta[i] = (char *)calloc(espacoTotal,sizeof(char));

        if(coordenadasLuta[i] == NULL || armazenamentoLuta[i] == NULL){
            printf("Erro de alocação de memória \n");
            return -1;
        }
    }
    for(int i = 0;i<espacoTotal;i++){
        for(int j = 0;j<espacoTotal;j++){
            coordenadasLuta[i][j] = '|';
            armazenamentoLuta[i][j] = '|';
        }
        printf("\n");
    }
    coordenadasLuta[x_jogador][y_jogador] = 'P';
    coordenadasLuta[x_inimigo][y_inimigo] = 'E';

    

    int vidaInimigoAntes = inimigo1[indice].vida;
    char comando;
    interfaceAtaque(coordenadasLuta,armazenamentoLuta,&x_jogador,&y_jogador,espacoTotal,inimigo1,vida,indice,0);
    while(vida !=0 && inimigo1[indice].vida !=0){
        puts("[A] Atacar | [D] Defesa | [M] Movimento Livre | [X] Escapar\n");
        scanf(" %c",&comando);
        comando = toupper(comando);
        if(comando == 'X'){
            limparTela();
            printf("🤠 - Não tô muito afim de lutar agora \n");
            inimigo1[indice].vida = vidaInimigoAntes;
            dormir(1500);
            for(int i = 0;i<espacoTotal;i++){
                free(coordenadasLuta[i]);
                free(armazenamentoLuta[i]);
            }
            free(coordenadasLuta);
            free(armazenamentoLuta);
            return 2;
        }
        if(comando == 'A'){
            int IdAtaque = atacar(mochila);
            double distanciaJogadorInimigo = sqrt(pow(x_jogador - x_inimigo,2) + pow(y_jogador - y_inimigo,2));
            if(IdAtaque > 103 && IdAtaque < 109 && distanciaJogadorInimigo <= 1.3){
                switch(IdAtaque){
                    case 104:
                        ataque = 20;
                    break;

                    case 105:
                        ataque = 25;
                    break;

                    default:
                        ataque = 15;
                    break;   
                }
            }
            if(IdAtaque > 103 && IdAtaque < 109 && distanciaJogadorInimigo > 1.3){
                limparTela();
                interfaceAtaque(coordenadasLuta,armazenamentoLuta,&x_jogador,&y_jogador,espacoTotal,inimigo1,vida,indice,0);
                printf("Você até tentou atacar, mas o inimigo estava muito longe pra alcançar ele \n");
                continue;
            }
            if(IdAtaque > 108 && IdAtaque < 115 && distanciaJogadorInimigo > 1.3){
                limparTela();
                interfaceAtaque(coordenadasLuta,armazenamentoLuta,&x_jogador,&y_jogador,espacoTotal,inimigo1,vida,indice,0);
                int tipoMira = mira(coordenadasLuta,armazenamentoLuta,espacoTotal,inimigo1,vida,indice,0);
                switch(tipoMira){
                    case 2:
                        ataque = 40;
                    break;
                    
                    case 1:
                        ataque = 20;
                    break;

                    case 0:
                        ataque = 0;
                    break;

                    default:
                        ataque = 0;
                    break;
                }
            }
            if(IdAtaque > 108 && IdAtaque < 115 && distanciaJogadorInimigo <= 1.3){
                limparTela();
                interfaceAtaque(coordenadasLuta,armazenamentoLuta,&x_jogador,&y_jogador,espacoTotal,inimigo1,vida,indice,0);
                printf("O inimigo teve bons reflexos e te atacou antes que começasse a mirar!\n");
                dormir(1000);
                *vida = *vida - inimigo1[indice].ataque;
                continue;
            }

            inimigo1[indice].vida = inimigo1[indice].vida - ataque;
            limparTela();
            interfaceAtaque(coordenadasLuta,armazenamentoLuta,&x_jogador,&y_jogador,espacoTotal,inimigo1,vida,indice,0);
            printf("Você usou um ataque com %d de dano \n Vida atual do inimigo = %d \n\n", ataque, inimigo1[indice].vida);
            dormir(1000);
            *vida = *vida - inimigo1[indice].ataque;
            printf("O inimigo usou um ataque com %d de dano \n Sua vida atual = %d \n", inimigo1[indice].ataque, *vida);
            dormir(1000);
            limparTela();
            interfaceAtaque(coordenadasLuta,armazenamentoLuta,&x_jogador,&y_jogador,espacoTotal,inimigo1,vida,indice,0);
        }
        if(comando == 'D'){
            int escolha;
            limparTela();
            interfaceAtaque(coordenadasLuta,armazenamentoLuta,&x_jogador,&y_jogador,espacoTotal,inimigo1,vida,indice,0);
            printf("[0] - Escudo/Cura \n[1] - Observar\n[2] - Interagir\n");
            scanf("%d",&escolha);
            if(escolha == 0){
                int IdDefesa = defender(mochila);
                switch(IdDefesa){
                    case 119:
                        *vida = *vida + 20;
                    break;
                    case 120:
                        *vida = *vida + 3;
                    break;
                    case 121:
                        *vida = *vida + 8;
                    break;
                    case 122:
                        *vida = *vida + 15;
                    break;
                    case 123:
                        *vida = *vida + 30;
                    break;

                    default:
                        *vida = *vida + 0;
                     break;
                }
                if(*vida > 100){
                    *vida = 100;
                }
            }
            if(escolha == 1){
                printf("Você está observando calmamente o inimigo\n");
            }
            if(escolha == 2){
                printf("Você escolheu agir\n");
            }
            
        }
        if(comando == 'M'){
            interfaceAtaque(coordenadasLuta,armazenamentoLuta,&x_jogador,&y_jogador,espacoTotal,inimigo1,vida,indice,1);
            limparTela();
            interfaceAtaque(coordenadasLuta,armazenamentoLuta,&x_jogador,&y_jogador,espacoTotal,inimigo1,vida,indice,0);
        }
        if(*vida <= 0){
            for(int i = 0;i<espacoTotal;i++){
                free(coordenadasLuta[i]);
                free(armazenamentoLuta[i]);
            }
            free(coordenadasLuta);
            free(armazenamentoLuta);
            return 1;
        }
        else if(inimigo1[indice].vida <= 0){
            for(int i = 0;i<espacoTotal;i++){
                free(coordenadasLuta[i]);
                free(armazenamentoLuta[i]);
            }
            free(coordenadasLuta);
            free(armazenamentoLuta);
            adicionarItemInimigo(inimigo1,mochila,indice,inimigo1[indice].quantidadeItem);
            return 0;
        }
    }
    return 2;
}


//Funções de crafting
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

    receitas[i].itemDesejado = 104;
    receitas[i].quantidade_um = 3;
    receitas[i].id_um = 1;
    receitas[i].id_dois = 0;
    receitas[i].quantidade_dois = 0;
    receitas[i].id_tres = 0;
    receitas[i].quantidade_tres = 0;
    i++;

    receitas[i].itemDesejado = 105;
    receitas[i].quantidade_um = 1;
    receitas[i].id_um = 1;
    receitas[i].id_dois = 4;
    receitas[i].quantidade_dois = 2;
    receitas[i].id_tres = 0;
    receitas[i].quantidade_tres = 0;
    i++;

    receitas[i].itemDesejado = 106;
    receitas[i].quantidade_um = 1;
    receitas[i].id_um = 1;
    receitas[i].id_dois = 3;
    receitas[i].quantidade_dois = 2;
    receitas[i].id_tres = 0;
    receitas[i].quantidade_tres = 0;
    i++;

    receitas[i].itemDesejado = 107;
    receitas[i].quantidade_um = 1;
    receitas[i].id_um = 1;
    receitas[i].id_dois = 10;
    receitas[i].quantidade_dois = 2;
    receitas[i].id_tres = 0;
    receitas[i].quantidade_tres = 0;
    i++;

    receitas[i].itemDesejado = 108;
    receitas[i].quantidade_um = 3;
    receitas[i].id_um = 1;
    receitas[i].id_dois = 4;
    receitas[i].quantidade_dois = 6;
    receitas[i].id_tres = 0;
    receitas[i].quantidade_tres = 0;
    i++;

    receitas[i].itemDesejado = 109;
    receitas[i].quantidade_um = 3;
    receitas[i].id_um = 11;
    receitas[i].id_dois = 1;
    receitas[i].quantidade_dois = 4;
    receitas[i].id_tres = 4;
    receitas[i].quantidade_tres = 1;
    i++;

    receitas[i].itemDesejado = 124;
    receitas[i].quantidade_um = 2;
    receitas[i].id_um = 1;
    receitas[i].id_dois = 10;
    receitas[i].quantidade_dois = 3;
    receitas[i].id_tres = 0;
    receitas[i].quantidade_tres = 0;
    i++;

    receitas[i].itemDesejado = 125;
    receitas[i].quantidade_um = 2;
    receitas[i].id_um = 1;
    receitas[i].id_dois = 3;
    receitas[i].quantidade_dois = 3;
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
    int id_escolhido = -1;
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
    limparBuffer();
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

void economiaJogo(struct SlotItem mochila[], int tipoAldeao){
    int totalProdutos = rand() % 5;
    if(totalProdutos == 0){
        printf("Sigh, sinto muito jovem, mas saqueadores roubaram nossos produtos e estamos sem nada no momento\n");
        getchar();
        getchar();
        return;
    }

    struct Produto produto[totalProdutos];

    int escolha = -1;
    int itemEscolhido = -1;
    int totalMoedasJogador;
    if(verificarMochila(mochila,6)){
        for(int i = 0;i<15;i++){
            if(mochila[i].id == 6){
                totalMoedasJogador = mochila[i].quantidade;
            }
        }
    }
    else{
        totalMoedasJogador = 0;
    }
    int itensFazendeiro[6] = {7,8,9,119,120,121};

    if(tipoAldeao == 1){
        int randomItens = 0;
        for(int i = 0; i < totalProdutos;i++){
            randomItens = rand() % 6;
            produto[i].id = itensFazendeiro[randomItens];
            produto[i].quantidade = rand() % 5 + 1;
            produto[i].preco = 10;
        }
        
    }
    int limpador;
    while ((limpador = getchar()) != '\n' && limpador != EOF);
    while(1){
        printf("\n🌕 Moedas disponíveis: %d\n",totalMoedasJogador);    
        printf("\n        === Produtos ===\n--------------------------------");
        for(int i = 0; i < totalProdutos;i++){
            printf("\n [%d] |%s (%dx) -- 🌕 %d ouros|\n",i,bibliotecaIDs(produto[i].id),produto[i].quantidade,produto[i].preco);
        }
        int scanear = fgetc(stdin);

        if(scanear == '\n'){
            break;
        }
        ungetc(scanear,stdin);

        if(scanf("%d",&escolha) != 1){
            int c;
            while((c = getchar()) != '\n' && c != EOF);
            limparTela();
            printf("\n");
            continue;
        }
        int c;
        while((c = getchar()) != '\n' && c != EOF);

        if(escolha >=0 && escolha < totalProdutos){
            if(totalMoedasJogador >= produto[escolha].preco){
                itemEscolhido = produto[escolha].id;
                int cont = 0;

                while(cont < produto[escolha].quantidade){
                    adicionarItemMundo(mochila,itemEscolhido);
                    cont++;
                }
                for(int i = 0;i<15;i++){
                    if(mochila[i].id == 6 && mochila[i].quantidade > 0){
                        mochila[i].quantidade = mochila[i].quantidade - produto[escolha].preco;
                        totalMoedasJogador -= produto[escolha].preco;
                        break;
                    }
                    if(mochila[i].id == 6 && mochila[i].quantidade <= 0){
                        mochila[i].quantidade = 0;
                        mochila[i].id = -1;
                        totalMoedasJogador = 0;
                        break;
                    }
                }

                produto[escolha].id = -1;
                produto[escolha].quantidade = 0;
                produto[escolha].preco = 0;

                for(int i = escolha; i < totalProdutos-1;i++){
                    produto[i].id = produto[i+1].id;
                    produto[i].quantidade = produto[i+1].quantidade;
                    produto[i].preco = produto[i+1].preco;
                }

                totalProdutos--;
                limparTela();
                printf("👨‍🌾 - Obrigado pela compra! Volte sempre\n");
                continue;
            }
            else{
                limparTela();
                printf("👨‍🌾 - Hmm, parece que você não tem moeda suficiente pra comprar isso não\n");
                continue;
            }
        }
        else{
            limparTela();
            printf("\n");
            continue;
        }
        printf("\n        === Produtos ===\n--------------------------------");
        for(int i = 0; i < totalProdutos;i++){
            printf("\n [%d] |%s (%dx) -- 🌕 %d ouros|\n",i,bibliotecaIDs(produto[i].id),produto[i].quantidade,produto[i].preco);
        }
    } 
}
void interfaceFazenda(struct SlotItem mochila[]){
    printf(" 👨‍🌾 - Olá! Cê entrou no melhor lugar pra comida da vila! Quer comprar alguma? \n");
    economiaJogo(mochila,1);
}
void interfaceBiblioteca(){
    int x = 5;
    int y = 0;
    int mascaraBiblioteca[10][10] = 
    {
    {0,0,0,0,0,0,0,0,0,0},
    {0,0,0,4,0,0,5,5,5,5},
    {0,0,0,4,0,0,0,0,0,0},
    {0,2,0,4,0,0,6,6,6,6},
    {4,4,4,4,0,0,0,0,0,0},
    {0,3,0,0,0,0,7,7,7,7},
    {0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,8,8,8,8},
    {0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0}
    };
    char **coordenadasBiblioteca = (char **)malloc(10 * sizeof(char *));
    char **armazenamentoBiblioteca = (char **)malloc(10 * sizeof(char *));
    if(coordenadasBiblioteca == NULL || armazenamentoBiblioteca == NULL){
        printf("Erro de alocação de memória \n");
        return;
    }
    for(int i = 0; i<10;i++){
        coordenadasBiblioteca[i] = (char *)calloc(10,sizeof(char));
        armazenamentoBiblioteca[i] = (char *)calloc(10,sizeof(char));

        if(coordenadasBiblioteca[i] == NULL || armazenamentoBiblioteca[i] == NULL){
            printf("Erro de alocação de memória \n");
            return;
        }
    }
    for(int i = 0;i<10;i++){
        for(int j = 0;j<10;j++){
            switch(mascaraBiblioteca[i][j]){
                case 0: coordenadasBiblioteca[i][j] = '@';
                break;
                case 1: coordenadasBiblioteca[i][j] = 'P';
                break;
                case 2: coordenadasBiblioteca[i][j] = 'I';
                break;
                case 3: coordenadasBiblioteca[i][j] = '!';
                break;
                case 4: coordenadasBiblioteca[i][j] = 'M';
                break;
                case 5: coordenadasBiblioteca[i][j] = '0';
                break;
                case 6: coordenadasBiblioteca[i][j] = '1';
                break;
                case 7: coordenadasBiblioteca[i][j] = '2';
                break;
                case 8: coordenadasBiblioteca[i][j] = '3';
                break;
            }
        }
    }
    for(int i = 0;i<10;i++){
        for(int j = 0;j<10;j++){
            armazenamentoBiblioteca[i][j] = coordenadasBiblioteca[i][j];
        }
    }
    coordenadasBiblioteca[x][y] = 'P';

    while(1){
        limparTela();
        
        for(int i = 0;i<10;i++){
            for(int j = 0;j<10;j++){
                imprimirComEmojis(coordenadasBiblioteca[i][j]);
            }
            printf("\n");
        }

        int x_ant = x;
        int y_ant = y;

        if(armazenamentoBiblioteca[x][y] == '!'){
            printf("Olá viajante! Bem vindo a biblioteca da vila, o que você deseja?\n");
            getchar();
        }

        movimentoJogador(coordenadasBiblioteca,armazenamentoBiblioteca,'P',&x,&y,10);

        if(x_ant == x && y_ant == y){
            if(x == 0 || x == 10-1 || y == 0 || y == 10-1){
                break;
            }
        }

        if(armazenamentoBiblioteca[x][y] == '0' || armazenamentoBiblioteca[x][y] == '1' || armazenamentoBiblioteca[x][y] == '2' || armazenamentoBiblioteca[x][y] == '3'){
            x = x_ant;
            y = y_ant;
            limparTela();
            printf("Você está observando um livro, mas desiste de ler pois está pensando demais no meme do tubarão estiloso \n");
            getchar();
            getchar();
            limparTela();
        }
    }

    for(int i = 0;i < 10;i++){
        free(coordenadasBiblioteca[i]);
        free(armazenamentoBiblioteca[i]);
    }
    free(coordenadasBiblioteca);
    free(armazenamentoBiblioteca);
    //{🛑🛑🛑🛑🛑🛑🛑🛑🛑🛑}
    //{🛑🛑🛑🟫🛑🛑📕📕📕📕 }
    //{🛑🧓🛑🟫🛑🛑🛑🛑🛑🛑}
    //{🟫🟫🟫🟫🛑🛑📘📘📘📘 }
    //{🤠🔘🛑🛑🛑🛑🛑🛑🛑🛑}
    //{🛑🛑🛑🛑🛑🛑📗📗📗📗 }
    //{🛑🛑🛑🛑🛑🛑🛑🛑🛑🛑}
    //{🛑🛑🛑🛑🛑🛑📙📙📙📙 }
    //{🛑🛑🛑🛑🛑🛑🛑🛑🛑🛑}
    //{🛑🛑🛑🛑🛑🛑🛑🛑🛑🛑}

    //{0,0,0,0,0,0,0,0,0,0}
    //{0,0,0,4,0,0,5,5,5,5}
    //{0,2,0,4,0,0,0,0,0,0}
    //{4,4,4,4,0,0,6,6,6,6}
    //{1,3,0,0,0,0,0,0,0,0}
    //{0,0,0,0,0,0,7,7,7,7}
    //{0,0,0,0,0,0,0,0,0,0}
    //{0,0,0,0,0,0,8,8,8,8}
    //{0,0,0,0,0,0,0,0,0,0}
    //{0,0,0,0,0,0,0,0,0,0}

}
void vila(int comando,int jogador_x, int jogador_y, struct Vila vilas[],int *indice_vilas, struct SlotItem mochila[]){
    struct Vila *vilaAtual = NULL;
    int seedVila;
    int biblioteca = 0;
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
                else if(seedVila < 13 && !biblioteca){
                    vilaAtual->mapa[i][j] = 'B';
                    biblioteca = 1;
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
                dormir(1000);
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
        if(armazenamentoVila[x][y] == 'A'){
            limparTela();
            interfaceFazenda(mochila);
            limparTela();
        }
        if(armazenamentoVila[x][y] == 'B'){
            limparTela();
            interfaceBiblioteca();
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
    int primeiraVezCaverna = 0;

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
                if(seedCaverna < 2){
                    cavernaAtual->mapaCaverna[i][j] = 'D';
                }
                else if(seedCaverna < 6){
                    cavernaAtual->mapaCaverna[i][j] = 'G';
                }
                else if(seedCaverna < 14){
                    cavernaAtual->mapaCaverna[i][j] = 'F';                    
                }
                else if(seedCaverna < 100){
                    cavernaAtual->mapaCaverna[i][j] = '|';                    
                }
            }
        }
        (*indice_cavernas)++;
        primeiraVezCaverna = 1;
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

    char mensagemNotificacao[100] = " ";
    int mostrarMensagem = 0;
    int cont = 0;
    while(1){
        gotoxy(0,0);
        if(primeiraVezCaverna == 1 && cont == 0){
            strcpy(mensagemNotificacao,"Você encontrou uma caverna nova! \n");
            mostrarMensagem = 1;
        }

        gotoxy(0,1);
        for(int i=0;i<TAM_CAVE;i++){
            for(int j=0;j<TAM_CAVE;j++){
                imprimirComEmojis(coordenadasCaverna[i][j]);
            }
            printf(" \n");
        }
        if(mostrarMensagem){
            printf("%-60s\n",mensagemNotificacao);
            mostrarMensagem = 0;
        }
        else{
            printf("%-60s\n"," ");
        }

        int x_ant = x;
        int y_ant = y;

        gotoxy(0,TAM_CAVE + 2);
        printf("\033[J");
        fflush(stdout);
        int armazenarComando = movimentoJogador(coordenadasCaverna,armazenamentoCaverna,'P',&x,&y,TAM_CAVE);
        cont++;
        strcpy(mensagemNotificacao,"");

        if(armazenarComando == 8){
            mostrarMensagem = 1;
            if(armazenamentoCaverna[x][y] == '|' && (verificarMochila(mochila,102) || verificarMochila(mochila,101) || verificarMochila(mochila,124) || verificarMochila(mochila,125)) == 1){
                adicionarItemMundo(mochila,3);
                strcpy(mensagemNotificacao,"Você minerou 1 pedra! ");
            }
            else if(armazenamentoCaverna[x][y] == '|' && (verificarMochila(mochila,102) || verificarMochila(mochila,101) || verificarMochila(mochila,124) || verificarMochila(mochila,125)) == 0){
                strcpy(mensagemNotificacao,"Você não tem picareta em mãos ");
            }
            if(armazenamentoCaverna[x][y] == 'G' && (verificarMochila(mochila,102) || verificarMochila(mochila,124)) == 1){
                adicionarItemMundo(mochila,6);
                armazenamentoCaverna[x][y] = '|';
                strcpy(mensagemNotificacao,"Você minerou 1 ouro! ");
            }
            else if(armazenamentoCaverna[x][y] == 'G' && (verificarMochila(mochila,102) || verificarMochila(mochila,124)) == 0){
                strcpy(mensagemNotificacao,"Parece que precisa de uma picareta melhor para isso ");
            }
        
            if(armazenamentoCaverna[x][y] == 'F' && (verificarMochila(mochila,102) || verificarMochila(mochila,124) || verificarMochila(mochila,125)) == 1){
                adicionarItemMundo(mochila,4);
                armazenamentoCaverna[x][y] = '|';
                strcpy(mensagemNotificacao,"Você minerou 1 ferro! ");
            }
            else if(armazenamentoCaverna[x][y] == 'F' && (verificarMochila(mochila,102) || verificarMochila(mochila,124) || verificarMochila(mochila,125)) == 0){
                strcpy(mensagemNotificacao,"Parece que precisa de uma picareta melhor para isso ");
            }

            if(armazenamentoCaverna[x][y] == 'D' && (verificarMochila(mochila,102) || verificarMochila(mochila,124)) == 1){
                adicionarItemMundo(mochila,10);
                armazenamentoCaverna[x][y] = '|';
                strcpy(mensagemNotificacao,"Você minerou 1 diamante! ");
            }
            else if(armazenamentoCaverna[x][y] == 'D' && (verificarMochila(mochila,102) || verificarMochila(mochila,124)) == 0){
                strcpy(mensagemNotificacao,"Parece que precisa de uma picareta melhor para isso");
            }
            gotoxy(0,TAM_CAVE+2);
            printf("%-60s", mensagemNotificacao); 
            if(kbhit()) getch();
            while(1){
                char c = getch();
                if (c == 'w' || c == 'a' || c == 's' || c == 'd' || c == '\n' || c == '\r'){
                    break;
                }
            }
            gotoxy(0,TAM_CAVE+2);
            printf("%-60s"," ");
            mostrarMensagem = 0;
            strcpy(mensagemNotificacao, "");
            continue;
        }

        if(x_ant == x && y_ant == y){
            if(x == 0 || x == TAM_CAVE-1 || y == 0 || y == TAM_CAVE-1){
                limparTela();
                printf("Você está saindo da caverna");
                dormir(1000);
                break;
            }
        }
        dormir(50);
    }

    for(int i = 0;i<TAM_CAVE;i++){
        free(coordenadasCaverna[i]);
        free(armazenamentoCaverna[i]);
    }
    free(coordenadasCaverna);
    free(armazenamentoCaverna);
}

void dungeon(int jogador_x, int jogador_y, struct Dungeon dungeons[],int *indice_dungeons, struct SlotItem mochila[], struct Inimigo inimigo[]){
    struct Dungeon *dungeonAtual = NULL;
    int tileVisto[TAM_DUNGEON][TAM_DUNGEON] = {0};

    for(int i = 0;i< *indice_dungeons;i++){
        if(dungeons[i].x == jogador_x && dungeons[i].y == jogador_y){
            dungeonAtual = &dungeons[i];
            for(int i = 0; i < TAM_DUNGEON;i++){
                for(int j = 0; j< TAM_DUNGEON;j++){
                    tileVisto[i][j] = dungeonAtual->campoVisao[i][j];
                }
            }
            break;
        }
    }

    if(dungeonAtual == NULL){

        dungeonAtual = &dungeons[*indice_dungeons];

        dungeonAtual->x = jogador_x;
        dungeonAtual->y = jogador_y;

        for(int i = 0;i<TAM_DUNGEON;i++){
            for(int j = 0;j<TAM_DUNGEON;j++){
                dungeonAtual->mapaDungeon[i][j] = '#';
            }
        }

        dungeonAtual->salas = rand() % 10 + 3;

        int **centroDungeons = (int **)malloc(dungeonAtual->salas * sizeof(int *));
        if(centroDungeons == NULL){
            printf("Erro de alocação de memória \n");
            return;
        }  
        for(int i = 0;i<dungeonAtual->salas;i++){
            centroDungeons[i] = (int *)calloc(2,sizeof(int));

            if(centroDungeons[i] == NULL){
                printf("Erro de alocação de memória \n");
                return;
            } 
        }

        int salaComCorredor[dungeonAtual->salas];
        
        int cont = 0;
        int contConexoes = 0;
        while(cont < dungeonAtual->salas){
            int podeConstruir = 1;

            int largura = rand() % 5 + 3;
            int altura = rand() % 5 + 3;
            
            int x_Sala = rand() % (TAM_DUNGEON - largura);
            int y_Sala = rand() % (TAM_DUNGEON - altura);

            int centroX = x_Sala + (largura/2);
            int centroY = y_Sala + (altura/2);

            salaComCorredor[cont] = 0;

            int checarInicioX = x_Sala - 2;
            int checarInicioY = y_Sala - 2;
            int checarFimX = x_Sala + largura + 2;
            int checarFimY = y_Sala + altura + 2;

            if(checarInicioX < 0){
                checarInicioX = 0;
            }
            else if(checarInicioY < 0){
                checarInicioY = 0;
            }
            else if(checarFimX > TAM_DUNGEON){
                checarFimX = TAM_DUNGEON;
            }
            else if(checarFimY > TAM_DUNGEON){
                checarFimY = TAM_DUNGEON;
            }

            for(int i = checarInicioX;i<checarFimX;i++){
                for(int j = checarInicioY;j<checarFimY;j++){
                    if(dungeonAtual->mapaDungeon[i][j] == '|'){
                        podeConstruir = 0;
                        i = checarFimX;
                        break;
                    }
                }
            }
            if(podeConstruir){
                centroDungeons[cont][0] = centroX;
                centroDungeons[cont][1] = centroY;
                for(int i = x_Sala;i < x_Sala + largura; i++){
                    for(int j = y_Sala;j<y_Sala + altura;j++){
                        dungeonAtual->mapaDungeon[i][j] = '|';
                    }
                }
                cont++;
            }
        }

        while(contConexoes < cont-1){
            for(int i = 0; i < cont-1;i++){
                if(salaComCorredor[i] == 0){
                    int xInicial = centroDungeons[i][0];
                    int yInicial = centroDungeons[i][1];

                    int xFinal = centroDungeons[i+1][0];
                    int yFinal = centroDungeons[i+1][1];

                    while(xInicial != xFinal || yInicial != yFinal){
                        if(xInicial < xFinal && xInicial < TAM_DUNGEON){
                            dungeonAtual->mapaDungeon[xInicial++][yInicial] = '|';
                        }
                        else if(yInicial < yFinal && yInicial < TAM_DUNGEON){
                            dungeonAtual->mapaDungeon[xInicial][yInicial++] = '|';
                        }
                        else if(xInicial > xFinal && xInicial > 0){
                            dungeonAtual->mapaDungeon[xInicial--][yInicial] = '|';
                        }
                        else if(yInicial > yFinal && yInicial > 0){
                            dungeonAtual->mapaDungeon[xInicial][yInicial--] = '|';
                        }
                    }
                    salaComCorredor[i] = 1;
                    contConexoes++;
                }
            }
        }
        
        int xCorredorInicial = TAM_DUNGEON-1;
        int xCorredorFinal = centroDungeons[0][0];

        int yCorredorInicial = TAM_DUNGEON/2;
        int yCorredorFinal = centroDungeons[0][1];
        
        while(xCorredorInicial != xCorredorFinal || yCorredorInicial != yCorredorFinal){
            
            if(xCorredorInicial > xCorredorFinal && xCorredorInicial > 0){
                dungeonAtual->mapaDungeon[xCorredorInicial--][yCorredorInicial] = '|';
            }
            else if(yCorredorInicial < yCorredorFinal && yCorredorInicial < TAM_DUNGEON){
                dungeonAtual->mapaDungeon[xCorredorInicial][yCorredorInicial++] = '|';
            }
            else if(yCorredorInicial > yCorredorFinal && yCorredorInicial > 0){
                dungeonAtual->mapaDungeon[xCorredorInicial][yCorredorInicial--] = '|';
            }
            else if(xCorredorInicial < xCorredorFinal && xCorredorInicial < TAM_DUNGEON-1){
                dungeonAtual->mapaDungeon[xCorredorInicial++][yCorredorInicial] = '|';
            }
        }

        for(int i = 0; i < TAM_DUNGEON; i++){
            for(int j = 0; j< TAM_DUNGEON;j++){
                if(dungeonAtual->mapaDungeon[i][j] == '#'){
                    int colisaoChao = 0;

                    for(int di = -1; di <= 1;di++){
                        for(int dj = -1; dj <= 1;dj++){
                            int vizinhoI = i+di;
                            int vizinhoJ = j+dj;

                            if(vizinhoI > TAM_DUNGEON){
                                vizinhoI = TAM_DUNGEON;
                            }
                            if(vizinhoI < 0){
                                vizinhoI = 0;
                            }
                            if(vizinhoJ > TAM_DUNGEON){
                                vizinhoJ = TAM_DUNGEON;
                            }
                            if(vizinhoJ < 0){
                                vizinhoJ = 0;
                            }

                            if(dungeonAtual->mapaDungeon[vizinhoI][vizinhoJ] == '|'){
                                colisaoChao = 1;
                            }
                        }
                    }
                    if(colisaoChao == 0){
                        dungeonAtual->mapaDungeon[i][j] = ' ';
                    }
                }
            }
        }
        (*indice_dungeons)++;
        for(int i = 0;i<dungeonAtual->salas;i++){
            free(centroDungeons[i]);
        }
        free(centroDungeons);
    }

    char **coordenadasDungeon = (char **)malloc(TAM_DUNGEON * sizeof(char *));
    char **armazenamentoDungeon = (char **)malloc(TAM_DUNGEON * sizeof(char *));

    if(coordenadasDungeon == NULL || armazenamentoDungeon == NULL){
        printf("Erro de alocação de memória \n");
        return;
    }
    for(int i = 0;i<TAM_DUNGEON;i++){
        coordenadasDungeon[i] = (char *)calloc(TAM_DUNGEON, sizeof(char));
        armazenamentoDungeon[i] = (char *)calloc(TAM_DUNGEON, sizeof(char));

        if(coordenadasDungeon[i] == NULL || armazenamentoDungeon[i] == NULL){
            printf("Erro de alocação de memória \n");
            return;
        }
    }

    for(int i = 0;i<TAM_DUNGEON;i++){
        for(int j = 0;j<TAM_DUNGEON;j++){

            coordenadasDungeon[i][j] = dungeonAtual->mapaDungeon[i][j];
            armazenamentoDungeon[i][j] = dungeonAtual->mapaDungeon[i][j];
        }
    }

    if(coordenadasDungeon[TAM_DUNGEON-1][TAM_DUNGEON/2] == '#'){
        coordenadasDungeon[TAM_DUNGEON-1][TAM_DUNGEON/2] = '|';
    }
    coordenadasDungeon[TAM_DUNGEON-1][TAM_DUNGEON/2] = 'P';
    int x = TAM_DUNGEON-1;
    int y = TAM_DUNGEON/2;

    int raio = 4;

    while(1){
        //função pra gerar círculo de visão
        for(int i = x - raio; i <= raio + x;i++){
            for(int j = y - raio; j <= raio + y;j++){
                
                if(i < 0 || i >= TAM_DUNGEON || j < 0 || j >= TAM_DUNGEON){
                    continue;
                }

                int distanciaX = i - x;
                int distanciaY = j - y;

                if((distanciaX*distanciaX) + (distanciaY*distanciaY) <= raio*raio){
                    tileVisto[i][j] = 1;
                }
            }
        }

        gotoxy(0,0);
        for(int i=0;i<TAM_DUNGEON;i++){
            for(int j=0;j<TAM_DUNGEON;j++){
                if(tileVisto[i][j] == 1){
                    if(i == x && j == y){
                        imprimirComEmojis('P');
                    }
                    else{
                        imprimirComEmojis(coordenadasDungeon[i][j]);
                    }
                }
                else{
                    imprimirComEmojis(' ');
                }
                dungeonAtual->campoVisao[i][j] = tileVisto[i][j];
            }
            printf(" \n");
        }


        int x_ant = x;
        int y_ant = y;

        movimentoJogador(coordenadasDungeon,armazenamentoDungeon,'P',&x,&y,TAM_DUNGEON);

        if(armazenamentoDungeon[x][y] == '#'){
            coordenadasDungeon[x][y] = '#';
            coordenadasDungeon[x_ant][y_ant] = 'P';
            x = x_ant;
            y = y_ant;
        }


        if(x_ant == x && y_ant == y){
            if(x == 0 || x == TAM_DUNGEON - 1 || y == 0 || y == TAM_DUNGEON -1){
                limparTela();
                printf("Você está saindo da dungeon \n");
                dormir(1000);
                break;
            }
        }
    }

    for(int i = 0;i<TAM_DUNGEON;i++){
        free(coordenadasDungeon[i]);
        free(armazenamentoDungeon[i]);
    }
    free(coordenadasDungeon);
    free(armazenamentoDungeon);
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

    struct Dungeon dungeons[MAX_DUNGEONS_ENCONTRADAS];
    int dungeonsEncontradas = 0;

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

    int **mapaDungeons = (int **)malloc(TAM * sizeof(int *));
    for(int i = 0;i<TAM;i++){
        mapaDungeons[i] = (int *)calloc(TAM, sizeof(int));
    }
    int x,y;
    int xzumbi,yzumbi;
    int xesqueleto,yesqueleto;
    int xaranha,yaranha;
    criarMundo(*pseed,mundo,armazenamento,mapaMascaraMineracao,mapaDungeons);
    jogador(mundo,armazenamento,&x,&y);
    zumbi(mundo,armazenamento,inimigo,&xzumbi,&yzumbi,&quantidadeInimigos,&x,&y);
    esqueleto(mundo,armazenamento,inimigo,&xesqueleto,&yesqueleto,&quantidadeInimigos,&x,&y);
    aranha(mundo,armazenamento,inimigo,&xaranha,&yaranha,&quantidadeInimigos,&x,&y);
    printf("Mensagem do jogo                                            |\n____________________________________________________________\n");
    for(int i = 0;i<TAM;i++){
        for(int j=0;j<TAM;j++){
            imprimirComEmojis(mundo[i][j]);
        }
        printf("\n");
    }
    printf("Elementos do jogo: \n");
    printf("Montanha = 🗻 | Água = 🟦 | Terra = 🟩 | Jogador = 🤠 \n");
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

        printf("\033[J");
        int armazenarComando = movimentoJogador(mundo,armazenamento,'P',&x,&y,TAM);
        if(armazenarComando == 1){
            break;
        }
        if(armazenarComando == 6){
            limparTela();
            inventario(mochila);
            getchar();
            getchar();
            continue;
        }
        if(armazenarComando == 7){
            limparTela();
            crafting(mochila,receitas,quantidadeReceitas);
            getchar();
            limparTela();
            continue;
        }
        if(armazenarComando == 8){
            if(mapaMascaraMineracao[x][y] == 1){
                limparTela();
                minerar(x,y,cavernas,&cavernasEncontradas,mochila);
                continue;
            }
            if(mapaDungeons[x][y] == 1){
                limparTela();
                dungeon(x,y,dungeons,&dungeonsEncontradas,mochila,inimigo);
                continue;
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
            continue;
        }
        if(armazenarComando == 10){
            limparTela();
            equipamento(mochila);
            getchar();
            getchar();
            limparTela();
            continue;
        }
        if(armazenarComando == 98){
            limparTela();
            ajuda();
            getchar();
            getchar();
            continue;
        }
        if(armazenarComando == 99){
            adicionarLog(logMensagens,"Movimento inválido, use WASD \n",&mensagensPorTurno);
            continue;
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
                int luta = mainAtaque(&vidaInicialJogador,ataqueInicialJogador,inimigo,i,mochila);
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
            vila(armazenarComando,x,y,vilas,&vilasEncontradas,mochila);
        }

        if(mensagensPorTurno == 0){
            adicionarLog(logMensagens, "", &mensagensPorTurno);
        }
        mensagensTurnoAnterior = mensagensPorTurno;
    }
    for(int i = 0;i<TAM;i++){
        free(mundo[i]);
        free(armazenamento[i]);
        free(mapaMascaraMineracao[i]);
        free(mapaDungeons[i]);
    }
    free(mundo);
    free(armazenamento);
    free(mapaMascaraMineracao);
    free(mapaDungeons);
}
