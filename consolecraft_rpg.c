#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#define TAM 20
#define TAM_VILA 10
#define MAX_VILAS_ENCONTRADAS 20
//P.S, quando for usar os emojis, usa esses (tem a mesma quantidade de bytes, se preferir mude para outros emojis com os mesmos bytes): 🤠💀🗻💧🟩
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

struct SlotItem{
    int id;
    int quantidade;
};

//função estética para esconder o cursor que aparece toda vez que o mapa é recarregado
void hideCursor() {
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 100;
    info.bVisible = FALSE;
    SetConsoleCursorInfo(consoleHandle, &info);
}
void gotoxy(int x, int y){
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}


//função para criar o mundo inicial
void criarMundo(int seed, char **mundo,char **armazenamento){
    srand(time(NULL));
    for(int i = 0; i<TAM;i++){
        for(int j = 0;j<TAM;j++){
            seed = rand() % 100;
            if(seed < 2){
                mundo[i][j] = 'V';
                armazenamento[i][j] = mundo[i][j];
            }
            else if(seed < 10){
                mundo[i][j] = '^';
                armazenamento[i][j] = mundo[i][j];
            }
            else if(seed < 40){
                mundo[i][j] = '~';
                armazenamento[i][j] = mundo[i][j];
            }
            else if(seed < 100){
                mundo[i][j] = '.';
                armazenamento[i][j] = mundo[i][j];
            }
        }
    }
}
void notificacaoUI(const char* mensagem){
    gotoxy(0,0);
    printf("%-40s",mensagem);
}
void desenharUI(char **mundo,const char *mensagem){
    notificacaoUI(mensagem);
    printf("|");
    gotoxy(0,1);
    printf("________________________________________\n");
    gotoxy(0,2);
    for(int i = 0;i<TAM;i++){
        for(int j=0;j<TAM;j++){
            printf("%c ",mundo[i][j]);
        }
        printf("\n");
    }
    printf("\n");
    printf("Digite um movimento estilo WASD || Para sair do jogo, digite X \n");
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
void zumbi(char **mundo,char **armazenamento, struct Inimigo inimigo1[],int *xzumbi,int *yzumbi,int *quantidade,int *x,int *y){
    char zumbi = 'Z';
    int quantidadeSpawn = rand() % 10 + 1;
    for(int i = 0;i<quantidadeSpawn;i++){
        *xzumbi = rand() % TAM;
        *yzumbi = rand() % TAM;
        inimigo1[*quantidade].id = 1;
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
        inimigo2[*quantidade].id = 2;
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
int ataque(int vida, int ataque, struct Inimigo inimigo1[], int indice,struct SlotItem mochila[]){
    char comando;
    printf("Você está atacando >:D \n");
    printf("Vida = %d \n",vida);
    while(vida !=0 && inimigo1[indice].vida !=0){
        puts("Digite Z para atacar \n");
        scanf(" %c",&comando);
        if(comando == 'z' || comando == 'Z'){
            inimigo1[indice].vida = inimigo1[indice].vida - ataque;
            printf("Você usou um ataque com %d de dano \n Vida atual do inimigo = %d \n", ataque, inimigo1[indice].vida);
            vida = vida - inimigo1[indice].ataque;
            printf("O inimigo usou um ataque com %d de dano \n Sua vida atual = %d \n", inimigo1[indice].ataque, vida);
        }
        if(vida <= 0){
            return 1;
        }
        else if(inimigo1[indice].vida <= 0){
            adicionarItemInimigo(inimigo1,mochila,indice);
            return 0;
        }
    }
}

void inventario(struct SlotItem mochila[]){
    printf("---Inventário---\n");
    for(int i=0;i<15;i++){
        printf("%d = ",i);

        switch(mochila[i].id){
            case -1:
                printf("Vazio");
            break;

            case 1:
                printf("Madeira");
            break;

            case 2:
                printf("Osso de esqueleto");
            break;
        
            default:
                printf("?");
            break;
        }

        if(mochila[i].id != -1){
            printf(" (Quantidade: %d)",mochila[i].quantidade);
        }
        printf("\n");
    }
    printf("Pressione Enter para fechar \n");
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
    char *wasd = &movimento;
    scanf(" %c",wasd);
    switch(*wasd){
        case 'w':
        case 'W':
           if (*x > 0){
            mundo[*x][*y] = armazenamento[*x][*y];
            (*x)--;
            mundo[*x][*y] = jogador;
            return 2;
           }
           break;
        case 'a':
        case 'A':
           if (*y > 0){
            mundo[*x][*y] = armazenamento[*x][*y];
            (*y)--;
            mundo[*x][*y] = jogador;
            return 3;
           }
           break;
        case 's':
        case 'S':
            if (*x < tamanho-1){
             mundo[*x][*y] = armazenamento[*x][*y];
             (*x)++;
             mundo[*x][*y] = jogador;
             return 4;
            }
            break;
        case 'd':
        case 'D':
           if (*y < tamanho-1){
            mundo[*x][*y] = armazenamento[*x][*y];
            (*y)++;
            mundo[*x][*y] = jogador;
            return 5;
           }
           break;
        case 'i':
        case 'I':
           return 6;
           break;
        case 'x':
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
                if(seedVila < 8){
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
        system("cls");

        for(int i = 0; i < TAM_VILA;i++){
            for(int j = 0;j< TAM_VILA;j++){
                printf("%c ",coordenadasVila[i][j]);
            }
            printf(" \n");
        }

        int x_ant = x;
        int y_ant = y;
        movimentoJogador(coordenadasVila,armazenamentoVila,'P',&x,&y,TAM_VILA);
        if(x_ant == x && y_ant == y){
            if(x == 0 || x == TAM_VILA-1 || y == 0 || y == TAM_VILA-1){
                system("cls");
                printf("Você está saindo da vila");
                Sleep(2000);
                break;
            }
        }
    }
    
    for(int i = 0;i < TAM_VILA;i++){
        free(coordenadasVila[i]);
        free(armazenamentoVila[i]);
    }
    free(coordenadasVila);
    free(armazenamentoVila);
}
int main(){
    system("cls");
    SetConsoleOutputCP(CP_UTF8);
    hideCursor();
    struct Inimigo inimigo[20];
    int quantidadeInimigos = 0;

    struct Vila vilas[MAX_VILAS_ENCONTRADAS];
    int vilasEncontradas = 0;

    struct SlotItem mochila[15];
    for(int i =0;i<15;i++){
        mochila[i].id = -1;
        mochila[i].quantidade = 0;
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

    int x,y;
    int xzumbi,yzumbi;
    int xesqueleto,yesqueleto;
    criarMundo(*pseed,mundo,armazenamento);
    jogador(mundo,armazenamento,&x,&y);
    zumbi(mundo,armazenamento,inimigo,&xzumbi,&yzumbi,&quantidadeInimigos,&x,&y);
    esqueleto(mundo,armazenamento,inimigo,&xesqueleto,&yesqueleto,&quantidadeInimigos,&x,&y);
    printf("Mensagem do jogo                        |\n________________________________________\n");
    for(int i = 0;i<TAM;i++){
        for(int j=0;j<TAM;j++){
            printf("%c ",mundo[i][j]);
        }
        printf("\n");
    }
    printf("Elementos do jogo: \n");
    printf("Montanha = ^ \n Água = ~ \n Terra = . \n Jogador = P \n");
    printf("Pressione qualquer tecla para continuar");
    getchar();
    system("cls");

    char mensagemNotificacao[100] = "";
    while (1){
        //Geração do mundo, irá ser gerado enquanto o while não for 1
        desenharUI(mundo,mensagemNotificacao);
        mensagemNotificacao[0] = '\0';

        int armazenarComando = movimentoJogador(mundo,armazenamento,'P',&x,&y,TAM);
        if(armazenarComando == 1){
            break;
        }
        if(armazenarComando == 6){
            system("cls");
            inventario(mochila);
            getchar();
            getchar();
            desenharUI(mundo,mensagemNotificacao);
        }
        if(armazenarComando == 99){
            strcpy(mensagemNotificacao,"Movimento inválido, use WASD \n");
        }
        if(armazenamento[x][y] == '.'){
            adicionarItemMundo(mochila,1);
            strcpy(mensagemNotificacao, "Você coletou +1 madeira!");
        }
        //Checagem da posição do jogador e do inimigo
        for(int i = 0; i<quantidadeInimigos;i++){
            if(inimigo[i].estadoAtual && x == inimigo[i].x && y == inimigo[i].y){
                system("cls");
                if(ataque(vidaInicialJogador,ataqueInicialJogador,inimigo,i,mochila)){
                    printf("\nGame Over :<\n Reinicie o jogo! \n");
                    exit(0);
                }
                else{
                    inimigo[i].estadoAtual = 0;
                    system("cls");
                    printf("--------------------------\n");
                    printf("Você venceu o inimigo!\n Pressione Enter para continuar! \n");
                    getchar();
                    getchar();
                    desenharUI(mundo,mensagemNotificacao);
                }
            }
        }
        
        //Sistema para detectar se o jogador vai entrar em uma vila, o jogo identifica se a posição do jogador é igual a posição da vila no mundo sem modificação(o mundo de "armazenamento")
        if(armazenamento[x][y] == 'V'){
            vila(armazenarComando,x,y,vilas,&vilasEncontradas);
        }

    }
    for(int i = 0;i<TAM;i++){
        free(mundo[i]);
        free(armazenamento[i]);
    }
    free(mundo);
    free(armazenamento);
}
