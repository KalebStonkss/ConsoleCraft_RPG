#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#define TAM 10
struct Inimigo{
    int ataque;
    int vida;
    int x;
    int y;
};
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
void ataque(int vida, int ataque){
    printf("Você está atacando >:D \n");
    printf("Vida = %d \n",vida);
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
    puts("Digite um movimento estilo WASD || Para sair do jogo, digite X \n");
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
           printf("Movimento inválido, use WASD \n");
    }
    return 0;
}
int main(){
    system("cls");
    SetConsoleOutputCP(CP_UTF8);

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
    printf("Montanha = ^ \n Água = ~ \n Terra = . \n Jogador = P");
    while (1){
        if(movimentoJogador(mundo,armazenamento,'P',&x,&y)){
            break;
        }
        //Geração do mundo, irá ser gerado enquanto o while não for 1
        system("cls");
        for(int i = 0;i<TAM;i++){
            for(int j=0;j<TAM;j++){
                printf("%c ",mundo[i][j]);
            }
            printf("\n");
        }

        //Checagem da posição do jogador e do inimigo
        for(int i = 0; i<quantidadeInimigos;i++){
            if(x == inimigo[i].x && y == inimigo[i].y){
                ataque(vidaInicialJogador,ataqueInicialJogador);
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
