/*************************************************************
* Author: prushh
* File: main.c
* Purpose: Show menu and start the game
*/

#include <stdio.h>
#include <stdlib.h>
#include "gamelib.h"

int main(int argc, char const *argv[]){
    system("clear");
    int scelta= -1; //Variabile di appoggio per scelta funzione da menù
        do {
            printf(	"\t\t\t\t\t------------------------------\n"
    					"\t\t\t\t\t|  Campeggio Lake Trasymen   |\n"
    					"\t\t\t\t\t|            MENU            |\n"
    					"\t\t\t\t\t------------------------------\n"
    					"\t\t\t\t\t| 1 - Crea mappa             |\n"
    					"\t\t\t\t\t| 2 - Gioca                  |\n"
    					"\t\t\t\t\t| 3 - Termina gioco          |\n"
    					"\t\t\t\t\t------------------------------\n\n"
    			  );
            printf("Effettua scelta: ");
            scanf(" %d", &scelta);
            system("clear");
            if(!check_buffer()){
                continue;
            }else{
                switch (scelta) {
                    case 1:
                    //Passo i due parametri per 'gestire file' all'interno della funzione
                        crea_mappa(&argc, argv);
                        break;
                    case 2:
                        gioca();
                        break;
                    case 3:
                        termina_gioco();
                        break;
                    default:
                        printf("Comando errato, si prega di selezionare una voce presente nel menù...\n\n");
                }
            }
        } while(!((scelta > 0) && (scelta <4)) || !(0));

    return 0;
}
