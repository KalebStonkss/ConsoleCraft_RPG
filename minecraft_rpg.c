#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#define TAM 20
//P.S, quando for usar os emojis, usa esses (tem a mesma quantidade de bytes, se preferir mude para outros emojis com os mesmos bytes): 🤠💀🗻💧🟩
struct Inimigo{
    int ataque;
    int vida;
    int x;
    int y;
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
            if(seed < 10){
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
void desenharUI(char **mundo){
    gotoxy(0,0);
    for(int i = 0;i<TAM;i++){
        for(int j=0;j<TAM;j++){
            printf("%c ",mundo[i][j]);
        }
        printf("\n");
    }
    printf("\n");
    printf("Digite um movimento estilo WASD || Para sair do jogo, digite X \n");
}
void zumbi(char **mundo,char **armazenamento, struct Inimigo inimigo1[],int *xzumbi,int *yzumbi,int *quantidade,int *x,int *y){
    char zumbi = 'Z';
    int quantidadeSpawn = rand() % 10 + 1;
    for(int i = 0;i<quantidadeSpawn;i++){
        *xzumbi = rand() % TAM;
        *yzumbi = rand() % TAM;
        inimigo1[*quantidade].vida = 35;
        inimigo1[*quantidade].ataque = 10;
        inimigo1[*quantidade].x = *xzumbi;
        inimigo1[*quantidade].y = *yzumbi;
        while(mundo[*xzumbi][*yzumbi] == mundo[*x][*y]){
            *xzumbi = rand() % TAM;
            *yzumbi = rand() % TAM;
        }
        mundo[*xzumbi][*yzumbi] = zumbi;

        (*quantidade)++;
    }
}
int ataque(int vida, int ataque, struct Inimigo inimigo1[], int indice){
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
            return 0;
        }
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
int movimentoJogador(char **mundo,char **armazenamento,char jogador,int *x,int *y){
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
           }
           break;
        case 'a':
        case 'A':
           if (*y > 0){
            mundo[*x][*y] = armazenamento[*x][*y];
            (*y)--;
            mundo[*x][*y] = jogador;
           }
           break;
        case 's':
        case 'S':
            if (*x < TAM-1){
             mundo[*x][*y] = armazenamento[*x][*y];
             (*x)++;
             mundo[*x][*y] = jogador;
            }
            break;
        case 'd':
        case 'D':
           if (*y < TAM-1){
            mundo[*x][*y] = armazenamento[*x][*y];
            (*y)++;
            mundo[*x][*y] = jogador;
           }
           break;
        case 'x':
        case 'X':
           return 1;
           break;
        default:
           system("cls");
           printf("Movimento inválido, use WASD \n");
    }
    return 0;
}
int main(){
    system("cls");
    SetConsoleOutputCP(CP_UTF8);
    hideCursor();
    struct Inimigo inimigo[20];
    int quantidadeInimigos = 0;

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

    criarMundo(*pseed,mundo,armazenamento);
    jogador(mundo,armazenamento,&x,&y);
    zumbi(mundo,armazenamento,inimigo,&xzumbi,&yzumbi,&quantidadeInimigos,&x,&y);

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
    while (1){
        //Geração do mundo, irá ser gerado enquanto o while não for 1
        desenharUI(mundo);

        if(movimentoJogador(mundo,armazenamento,'P',&x,&y)){
            break;
        }

        //Checagem da posição do jogador e do inimigo
        for(int i = 0; i<quantidadeInimigos;i++){
            if(x == inimigo[i].x && y == inimigo[i].y){
                system("cls");
                if(ataque(vidaInicialJogador,ataqueInicialJogador,inimigo,i)){
                    printf("\nGame Over :<\n Reinicie o jogo! \n");
                    exit(0);
                }
                else{
                    system("cls");
                    printf("--------------------------\n");
                    printf("Você venceu o inimigo!\n Pressione Enter para continuar! \n");
                    getchar();
                    getchar();
                    desenharUI(mundo);
                }
            }
        }

    }
    for(int i = 0;i<TAM;i++){
        free(mundo[i]);
        free(armazenamento[i]);
    }
    free(mundo);
    free(armazenamento);
}
