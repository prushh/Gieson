/*************************************************************
* Author: prushh
* File: gamelib.h
* Purpose: Create enum, struct and function prototypes
*/

//Definisco il tipo delle varie zone (tipo_zona)
typedef enum tipo_zona{
    cucina= 1,
    soggiorno,
    rimessa,
    strada,
    lungo_lago,
    uscita_campeggio
} tZona;

//Definisco il tipo dei vari oggetti (tOggetto)
typedef enum tipo_oggetto{
    cianfrusaglia= 0,
    bende,
    coltello,
    pistola,
    benzina,
    adrenalina
} tOggetto;

//Definisco i possibili stati di salute del giocatore (tStato)
typedef enum stato_giocatore{
    morto= 1,
    ferito,
    vivo
} tStato;

//Definisco la struttura Zona (sZona)
typedef struct Zona{
    tZona zona;
    tOggetto oggetto;
    struct Zona *zona_successiva; //Rappresenta il puntatore alla zona successiva
}sZona;

//Definisco la struttura Giocatore (sGiocatore)
typedef struct Giocatore{
    tStato stato;
    sZona *posizione; //Rappresenta la posizione del giocatore nella mappa
    unsigned short zaino[6];
} sGiocatore;

//Prototipo della funzione che ripulisce buffer stdin
int check_buffer();
//Prototipo delle funzioni che richiamo nel main()
void crea_mappa(int *argc, char const *argv[]);
void gioca();
void termina_gioco();
