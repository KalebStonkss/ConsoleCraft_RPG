#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define TAM 10

int main(){
    char **mundo = (char **)malloc(TAM * sizeof(char *));
    for(int i = 0;i<TAM;i++){
        mundo[i] = (char *)calloc(TAM, sizeof(char));
    }
    srand(time(NULL));
    for(int i = 0; i<TAM;i++){
        for(int j = 0;j<TAM;j++){
            int seed = rand() % 100;
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
    for(int i = 0;i<TAM;i++){
        for(int j=0;j<TAM;j++){
            printf("%c ",mundo[i][j]);
        }
        printf("\n");
    }
    for(int i = 0;i<TAM;i++){
        free(mundo);
    }
    free(mundo);
}