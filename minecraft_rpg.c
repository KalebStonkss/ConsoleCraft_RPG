#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define TAM 10
void criarMundo(int seed, char **mundo){
    srand(time(NULL));
    for(int i = 0; i<TAM;i++){
        for(int j = 0;j<TAM;j++){
            seed = rand() % 100;
            if(seed < 10){
                mundo[i][j] = '^';
            }
            else if(seed < 40){
                mundo[i][j] = '~';
            }
            else if(seed < 100){
                mundo[i][j] = '.';
            }
        }
    }
}
void jogador(char **mundo){
    char jogador = 'P';
    char *pjogador = &jogador;
    int x = rand() % TAM;
    int y = rand() % TAM;
    mundo[x][y] = *pjogador;
}
int main(){
    int seed = 0;
    int *pseed = &seed;
    char **mundo = (char **)malloc(TAM * sizeof(char *));
    for(int i = 0;i<TAM;i++){
        mundo[i] = (char *)calloc(TAM, sizeof(char));
    }
    criarMundo(*pseed,mundo);
    jogador(mundo);
    for(int i = 0;i<TAM;i++){
        for(int j=0;j<TAM;j++){
            printf("%c ",mundo[i][j]);
        }
        printf("\n");
    }
    for(int i = 0;i<TAM;i++){
        free(mundo[i]);
    }
    free(mundo);
}