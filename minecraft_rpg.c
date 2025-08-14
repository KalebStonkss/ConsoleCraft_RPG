#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define TAM 10

struct Inimigo{
    int ataque;
    int vida;
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
void zumbi(char **mundo,char **armazenamento,int *xzumbi,int *yzumbi,int *x,int *y){
    struct Inimigo inimigo1;
    char zumbi = 'Z';
    inimigo1.vida = 35;
    inimigo1.ataque = 10;
    int quantidade = rand() % 10 + 1;
    for(int i = 0;i<quantidade;i++){
        *xzumbi = rand() % TAM;
        *yzumbi = rand() % TAM;
        while(mundo[*xzumbi][*yzumbi] == mundo[*x][*y]){
            *xzumbi = rand() % TAM;
            *yzumbi = rand() % TAM;
        }
        mundo[*xzumbi][*yzumbi] = zumbi;
    }
}
void jogador(char **mundo,char **armazenamento,int *x,int *y){
    char jogador = 'P';
    char *pjogador = &jogador;
    int vida = 100;
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
    zumbi(mundo,armazenamento,&xzumbi,&yzumbi,&x,&y);
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
        for(int i = 0;i<TAM;i++){
            for(int j=0;j<TAM;j++){
                printf("%c ",mundo[i][j]);
            }
            printf("\n");
        }
    }
    for(int i = 0;i<TAM;i++){
        free(mundo[i]);
        free(armazenamento[i]);
    }
    free(mundo);
    free(armazenamento);
}
