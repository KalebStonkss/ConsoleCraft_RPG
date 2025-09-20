# ConsoleCraft RPG - Jogo no Terminal em C 🕹🗺
## Introdução 
  - Esse é um projeto pessoal e minha primeira experiência criando um jogo no terminal com a linguagem C
  - O jogo foi feito com o intuito de ser um RPG inspirado em jogos como o Minecraft, com um mundo vasto (não tão vasto assim kk) para o jogador explorar
  - Feito apenas para fins didáticos, esse projeto não tem afiliação com a Mojang/Microsoft
## Como rodar no terminal 📜
### Windows
  - **Requisitos Mínimos**: GCC instalado no computador (por meio do MinGW ou WSL)
  - No prompt de comando, digite "chcp 65001"(sem as aspas) para o Windows detectar acentuações corretamente
  - No Prompt de Comando, digite "gcc -o consolecraft.exe consolecraft_rpg.c"(sem as aspas), dê enter
  - Depois, digite "consolecraft.exe"(sem as aspas) e dê enter para o jogo começar
## Elementos do Jogo
  -  **^**  significa montanha (⛰)
  -  **~**  significa água (💧)
  -  &nbsp;**.**&nbsp;  significa terra (🌳)
  - P é o jogador, você :D
## Progresso atual do código
  - Geração aleatória de zumbis e esqueletos pelo mapa
  - Modo de ataque
