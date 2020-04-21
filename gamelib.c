/*************************************************************
* Author: prushh
* File: gamelib.c
* Purpose: Core of the game
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "gamelib.h"

static sZona *prima_zona= NULL; //Rappresenta la Prima ZONA della mappa
static sZona *ultima_zona= NULL; //Rappresenta l'Ultima ZONA della mappa
static sGiocatore marzia, giacomo; //Dichiaro i due protagonisti del gioco
//Utilizzata per gestire entrambi i giocatori nei loro corrispettivi turni
static sGiocatore *player;

static const int LENGTH=6;
static const int matr_oggetti[6][6]= {
    //cianfrusaglia, bende, coltello, pistola, benzina, adrenalina
     {30,            20,    40,       0,       0,       10},   //cucina
     {20,            10,    10,       30,      0,       30},   //soggiorno
     {20,            10,    30,       0,       30,      10},   //rimessa
     {80,            0,     10,       0,       10,      0},    //strada
     {70,            0,     10,       0,       20,      0},    //lungo_lago
     {90,            0,     10,       0,       0,       0}     //uscita_campeggio
};

//--------------------------------------------------------> PROTOTIPO DELLE FUNZIONI GESTIONE MAPPA
//Prototipo delle funzioni principali in crea_zona()
static void ins_zona();
static void ins_zona_file(FILE *file);
static void canc_zona();
static void stampa_mappa();
static void chiudi_mappa();
//Prototipo della funzione per controllo password
static int check_pwd_mappa();
//Prototipo della funzione che mi consente l'inserimento nel campo 'zona'
static tZona menu_tipo_zona();
//Prototipo delle funzioni che mi permettono di valorizzare (RND) il campo 'oggetto'
static tOggetto rand_tipo_oggetto(tZona *t_zona);
static void row_matr(int *array_tmp, int *row);
static void ordina_array_cre(int *array_tmp);
static tOggetto det_ogg_matr(int row);
//Prototipo delle funzioni che mi stampano l'enum corrispondente a seconda del tipo
static void stampa_tipo_oggetto(tOggetto t_ogg, int capo);
static void stampa_tipo_zona(tZona t_zona);
static void stampa_stato_giocatore(tStato stato);

static int progress= 0; //Flag per ciclo crea_mappa()
static int num_zone= 0; //Contatore per il numero delle zone nella mappa
//Array con password predefinita per accesso a crea_mappa()
static char pwd_mappa[16]= {'N', 'U', 'L', 'L'};

//--------------------------------------------------------> PROTOTIPO DELLE FUNZIONI GESTIONE GIOCO
//Prototipo delle funzioni principali in gioca()
static void avanza(int *eseguita);
static void mostra_oggetto(int *eseguita, int *ricerca, int *t);
static void prendi_oggetto(int *eseguita, int *ricerca, int *t);
static void cura(int *eseguita);
static void usa_adrenalina(int *eseguita, int *bonus);
//Prototipo delle funzioni secondarie in gioca()
static void inizializzo_giocatori(int *t);
static void cambio_turno(int *t, int *bonus, int *cont);
/* Prototipo della funzione che mi permette di vedere se un determinato
   oggetto è presente nello 'zaino' */
static int oggetto_presente(tOggetto t_ogg);
//Prototipo della funzione che mi stampa il riepilogo del giocatore
static void stampa_giocatore(int *eseguita);
//Prototipo della funzione che mi stampa * in base al parametro dato
static void stampa_asterischi(int *count);
//Prototipo della funzione che mi determina l'apparizione di Gieson
static int gieson();
//Prototipo delle funzioni che utilizzo quando appare Gieson
static void fase_difesa(int *benz, int *t);
static int menu_scelta_difesa();
//Prototipo delle funzioni che vanno ad applicare le varie tipologie di difesa, suddivise per arma
static int difesa_coltello();
static int difesa_pistola();
static int difesa_benzina();
//Prototipo delle funzioni che mi stampano messaggi a video
static void vittoria();
static void game_over();
static void show_gieson();
//Prototipo della funzione che mi rende riallocabile la memoria heap allocata in precedenza
static void free_mappa();
//Prototipo della funzione che mi scrivono/leggono la mappa su/da file
static void salva_mappa_file();
static void leggi_mappa_file();

void crea_mappa(int *argc, char const *argv[]){
    if((*argc== 2) && (strcmp(pwd_mappa, "NULL")== 0)){
        FILE *file_handle= fopen(argv[1], "r");
        //Controllo se il file esiste ed è stato aperto correttamente
        if(file_handle== NULL){
            printf("Impossibile aprire il file!\n\n");
        }else{
            //Procedo alla lettura e inserimento zone nella mappa
            ins_zona_file(file_handle);
        }
        fclose(file_handle);
    }
    if(check_pwd_mappa()){//Controllo se la password inserita è corretta
        int scelta= -1; //Variabile di appoggio per scelta funzione da menù
        progress= 0;
        do{
            printf(	"\t\t\t\t\t---------------------------------------\n"
    					"\t\t\t\t\t|       Campeggio Lake Trasymen       |\n"
                        "\t\t\t\t\t|              CREA MAPPA             |\n"
    					"\t\t\t\t\t---------------------------------------\n"
    					"\t\t\t\t\t| 1 - Inserisci Zona                  |\n"
    					"\t\t\t\t\t| 2 - Cancella l'ultima Zona inserita |\n"
    					"\t\t\t\t\t| 3 - Stampa i campi di tutte le Zone |\n"
    					"\t\t\t\t\t| 4 - Leggi mappa da file             |\n"
                        "\t\t\t\t\t| 5 - Salva mappa su file             |\n"
    					"\t\t\t\t\t| 6 - Cancella mappa complessiva      |\n"
                        "\t\t\t\t\t| 7 - Termina modifica                |\n"
    					"\t\t\t\t\t---------------------------------------\n\n"
    			  );
            printf("Effettua scelta: ");
            scanf(" %d", &scelta);
            system("clear");
            if(!check_buffer()){
              continue;
            }else{
                switch (scelta) {
                    case 1:
                        ins_zona();
                        break;
                    case 2:
                        canc_zona();
                        break;
                    case 3:
                        stampa_mappa();
                        break;
                    case 4:
                        leggi_mappa_file();
                        break;
                    case 5:
                        salva_mappa_file();
                        break;
                    case 6:
                        free_mappa();
                        break;
                    case 7:
                        chiudi_mappa();
                        break;
                    default:
                        printf("Comando errato, si prega di selezionare una voce esistente...\n\n");
                }
            }
        }while(!((scelta > 0) && (scelta <8)) || !(progress));
    }
}

/* Controllo se è stata impostata una password per accedere
 * alla gestione della mappa
 * -> return 0/1 (0: Errata, 1: Corretta);
 */
static int check_pwd_mappa(){
    //strcmp(): torna 0 quando le due stringhe sono uguali
    if(strcmp(pwd_mappa, "NULL")!= 0){
        char pwd_tmp[255];
        printf("Inserisci password per accedere: ");
        scanf(" %s", pwd_tmp);
        system("clear");
        printf("Controllo in corso...\n");
        if(strcmp(pwd_mappa, pwd_tmp)== 0){
            printf("Password corretta!\n\n");
            return 1;
        }else{
            printf("Password errata!\n\n");
            return 0;
        }
    }else if(strcmp(pwd_mappa, "NULL")== 0){
        return 1; //Caso di partenza, la password predefinita è "NULL"
    }
    return 0;
}

/* Controllo se sono rimasti dei caratteri all'interno del
 * buffer stdin, in caso affermativo rieffettuo l'inserimento.
 */
int check_buffer(){
    char buffer[100];
    fgets(buffer, 100, stdin);
    if(strlen(buffer) > 1){
        printf("Errore input: pulitura buffer in corso e ripristino...\n\n");
        return 0;
    }else{
        return 1;
    }
}

/* Alloco memoria per una nuova struct Zona (sZona), richiedo l'inserimento
 * per il campo 'zona' e valorizzo il campo 'oggetto' in maniera random,
 * secondo le probabilità descritte nella matrice. Distinguo il caso in
 * cui non ci sono ancora zone inserite da quello in cui è presente
 * almeno una zona.
 */
static void ins_zona(){
    sZona *tmp= (sZona*) malloc(sizeof(sZona));
    if(tmp== NULL){
        printf("Errore nell'allocazione della nuova zona...\n");
        exit(EXIT_FAILURE);
    }else{
        tmp->zona= menu_tipo_zona();
        tmp->oggetto= rand_tipo_oggetto(&tmp->zona);
        tmp->zona_successiva= NULL;
        if(prima_zona== NULL){
            prima_zona= tmp;
            ultima_zona= prima_zona;
        }else{
            ultima_zona->zona_successiva= tmp;
            ultima_zona= tmp;
        }
        num_zone++;
    }
}

/* Faccio inserire dall'utente il nome del file da leggere,
 * se quest'ultimo è stato trovato ed aperto correttamente
 * procedo con l'inserimento di tutte le zone altrimenti
 * faccio uscire un messaggio di errore.
 */
static void leggi_mappa_file(){
    char nome_file[255];
    printf("Scegli file (nomefile.txt): ");
    scanf(" %s", nome_file);
    FILE *file_handle= fopen(nome_file, "r");
    //Controllo se il file esiste ed è stato aperto correttamente
    if(file_handle== NULL){
        printf("Impossibile aprire il file!\n\n");
    }else{
        //Procedo alla lettura e inserimento zone nella mappa
        ins_zona_file(file_handle);
    }
    fclose(file_handle);
}

/* Alloco memoria per nuove struct Zona (sZona), leggo da file la zona
 * e l'oggetto grazie alla fscanf così da inserirli all'interno della struct.
 * Distinguo il caso in cui non ci sono ancora zone inserite da quello
 * in cui è presente almeno una zona. Ripeto queste operazioni all'interno
 * di un ciclo fino a che non arrivo alla fine del file (END OF FILE).
 * @FILE *file;
 */
static void ins_zona_file(FILE *file){
    int zona= -1, ogg= -1, esito= 0;
    while((esito= fscanf(file, "%d %d", &zona, &ogg))!= EOF){
        //fscanf restituisce il numero di elementi correttamente assegnati
        if(((zona < 1)||(zona > 6))||((ogg < 0)||(ogg > 5))||(!esito)){
            printf("Errore lettura file, alcuni dati potrebbero"
                   " essere stati inseriti perchè validi, si consiglia"
                   " di stampare la mappa...\n\n");
            break;
        }
        sZona *tmp= (sZona*) malloc(sizeof(sZona));
        if(tmp== NULL){
            printf("Errore nell'allocazione della nuova zona...\n");
            exit(EXIT_FAILURE);
        }else{
            tmp->zona= (tZona)zona;
            tmp->oggetto= (tOggetto) ogg;
            tmp->zona_successiva= NULL;
            if(prima_zona== NULL){
                prima_zona= tmp;
                ultima_zona= prima_zona;
            }else{
                ultima_zona->zona_successiva= tmp;
                ultima_zona= tmp;
            }
            num_zone++;
        }
        zona= -1, ogg= -1;
    }
    if(esito== EOF){
        printf("Lettura mappa da file completata...\n\n");
    }
}

/* Scorro tutta la lista così da scrivere su file, rispettivamente
 * per ogni nodo, il tipo_zona ed il tipo_oggetto. Faccio scegliere
 * inoltre all'utente che nome dare al file e controllo che quest'ultimo
 * abbia estensione 'txt'.
 */
static void salva_mappa_file(){
    if(prima_zona!= NULL){
        FILE *file;
        char nome_file[255], *ext;
        printf("Salva file come (nomefile.txt): ");
        scanf(" %s", nome_file);
        if(((ext = strrchr(nome_file, '.'))!= NULL)&&(strcmp(ext, ".txt")== 0)){
            file=fopen(nome_file, "w");
            if(file== NULL){
                printf("Impossibile salvare file...\n\n");
            }else if(prima_zona==NULL){
                printf("Inserire almeno una zona prima di salvare la mappa su file...\n\n");
            }else{
                sZona *tmp= prima_zona;
                while(tmp!= NULL){
                    fprintf(file, "%d %d\n", tmp->zona, tmp->oggetto);
                    tmp= tmp->zona_successiva;
                }
                printf("Mappa salvata su file!\n\n");
            }
            fclose(file);
        }else{
            printf("Salvare file con estensione '.txt' per un corretto funzionamento...\n\n");
        }
    }else{
        printf("Non ci sono zone da salvare su file...\n\n");
    }
}

/* Stampo il menù per la scelta della zona che si andrà a creare,
 * se questa non sarà stata inserita correttamente rieffettuo
 * l'inserimento.
 * -> return (tZona)tipo_zona;
 */
static tZona menu_tipo_zona(){
    int tipo_zona; //Variabile di appoggio per scelta zona da menù
    printf("Scegli che tipo di Zona vuoi creare:\n");
    printf("1 - Cucina\n");
    printf("2 - Soggiorno\n");
    printf("3 - Rimessa\n");
    printf("4 - Strada\n");
    printf("5 - Lungo lago\n");
    printf("6 - Uscita campeggio\n\n");
    do{
        printf("Effettua scelta: ");
        scanf(" %d", &tipo_zona);
        if(!check_buffer()){
            continue;
        }else{
            if((tipo_zona < 1) || (tipo_zona > 6)){
                printf("Comando errato, si prega di selezionare una zona presente...\n\n");
            }
        }
    }while(!((tipo_zona > 0) && (tipo_zona <7)));
    return (tZona)tipo_zona;
}

/* Valorizzo la varaibile tOggetto tipo_oggetto con il risultato
 * della funzione det_ogg_matr(int row) ed in seguito stampo l'oggetto
 * in questione.
 * @tZona *t_zona;
 * -> return (tOggetto)tipo_oggetto;
 */
static tOggetto rand_tipo_oggetto(tZona *t_zona){
    tOggetto tipo_oggetto;
    tipo_oggetto= det_ogg_matr((int)*t_zona - 1); //(*t_zona -1) = riga della matrice
    system("clear");
    printf("Zona scelta: ");
        stampa_tipo_zona(*t_zona);
    printf("Oggetto: ");
        stampa_tipo_oggetto(tipo_oggetto, 1);

    return tipo_oggetto;
}

/* Stampo a video il tipo_oggetto passato come parametro e il carattere
 * '\n' a seconda del valore che assumerà la variabile 'capo'.
 * @tOggetto t_ogg;
 * @int capo;
 */
static void stampa_tipo_oggetto(tOggetto t_ogg, int capo){
    switch (t_ogg) {
        case 0:
            printf("Cianfrusaglia");
            break;
        case 1:
            printf("Bende");
            break;
        case 2:
            printf("Coltello");
            break;
        case 3:
            printf("Pistola");
            break;
        case 4:
            printf("Benzina");
            break;
        case 5:
            printf("Adrenalina");
            break;
    }
    if(capo){
        printf("\n");
    }
}

/* Stampo a video il tipo_zona passato come parametro.
 * @tZona t_zona;
 */
static void stampa_tipo_zona(tZona t_zona){
    switch (t_zona) {
        case 1:
            printf("Cucina\n");
            break;
        case 2:
            printf("Soggiorno\n");
            break;
        case 3:
            printf("Rimessa\n");
            break;
        case 4:
            printf("Strada\n");
            break;
        case 5:
            printf("Lungo lago\n");
            break;
        case 6:
            printf("Uscita campeggio\n");
            break;
    }
}

/* Lavorando con la riga della specifica zona scelta, porto l'array
 * in 'base 100', estraggo un numero random e in base al range in cui si
 * trova risalgo alla posizione nella matrice, distinguendo il caso in cui
 * nella stessa riga è presente lo stesso valore.
 * @int row;
 * -> return (tOggetto)(cont);
 */
static tOggetto det_ogg_matr(int row){
    time_t t;
    srand((unsigned) time(&t));

    unsigned int rnd;
    int array_row[6], //array che conterrà la riga della matrice, specifica della zona scelta
        array_tmp[6], //array di appoggio
        cont= -1,
        result= -1;
    row_matr(array_row, &row);
    ordina_array_cre(array_row);

    for (int  i= 0; i < LENGTH; i++) {
        array_tmp[i]= array_row[i]; //'Copio' array_row in array_tmp
    }
    for (int  i= 1; i < LENGTH; i++) {
        //Porto l'array_tmp in 'base 100' sommando il valore attuale con quello precedente
        array_tmp[i]= array_tmp[i] + array_tmp[i-1];
    }

    rnd= (rand()%100)+1; //Numero random che mi identificherà nel range l'oggetto
    for (int i = (LENGTH-1); i > 0; i--) {
        if ((rnd >= array_tmp[i-1]) && (rnd <= array_tmp[i])) {
            /* Assegno a cont il valore dell'indice,
               in base al range 'valore precedente' e 'valore attuale' */
            cont= i;
            break;
        }
    }
    result= array_row[cont]; //Assegno a result il valore in percentuale in posizione cont

    cont= 0; //Riazzero cont
    for (int i = 0; i < LENGTH; i++) {
        if(result == matr_oggetti[row][i]){
            //Controllo se lo stesso valore è presente più volte nella riga corrispondente alla zona
            cont++;
        }
    }
    if(cont >= 2){
        //Caso in cui è presente lo stesso valore più volte
        rnd= (rand()%100)+1;
        for (int i = 0; i < LENGTH; i++) {
            if(result == matr_oggetti[row][i]){
                /* Se rnd è >= 50 passo al valore successivo,
                   altrimenti prendo quello corrente */
                if(rnd >= 50){
                    rnd= 0;
                    continue;
                }else{
                    cont= i;
                    break;
                }
            }
        }
    }else if(cont == 1){
        //Caso in cui quel valore è presente una sola volta
        for (int i = 0; i < LENGTH; i++) {
            if(result == matr_oggetti[row][i]){
                //Recupero l'indice (col) in cui si trova nella matrice
                cont= i;
                break;
            }
        }
    }

    return (tOggetto)(cont);
}

/* Riempio l'array passato come parametro con i valori della matrice
 * della specifica riga passata anch'essa come parametro.
 * @int *array_tmp (byRef);
 * @int *row (byRef);
 */
static void row_matr(int *array_tmp, int *row){
    for (int i = 0; i < LENGTH; i++) {
        array_tmp[i]= matr_oggetti[*row][i];
    }
}

/* Ordino l'array passato come parametro, in modo crescente.
 * @int *array_tmp (byRef);
 */
static void ordina_array_cre(int *array_tmp){
    for (int i = (LENGTH-1); i > 0; i--) {
        for (int j = 0; j < i; j++) {
            if(array_tmp[j] > array_tmp[j+1]){
                int tmp= array_tmp[j+1];
                array_tmp[j+1]= array_tmp[j];
                array_tmp[j]= tmp;
            }
        }
    }
}

/* Scorro tutta la lista dinamica per stampare ogni zona presente.
 * Distinguo il caso in cui non ci sono zone nella mappa da quello
 * in cui è presenta almeno una zona.
 */
static void stampa_mappa(){
    if(prima_zona == NULL){
        printf("Non ci sono zone da stampare...\n\n");
    }else{
        printf("Stampa in corso di %d zone, premere invio per continuare,"
                " un altro carattere seguito da invio per smettere...\n\n", num_zone);
        sZona *tmp= prima_zona;
        int i= 1;
        while(tmp != NULL){
            printf("--------------------------------------------------\n");
            printf("\t\t\tZONA N° %d\n", i);
            printf("indirizzo: %p\n", tmp);
            printf("tipo_zona: ");
                stampa_tipo_zona(tmp->zona);
            printf("tipo_oggetto (RND): ");
                stampa_tipo_oggetto(tmp->oggetto, 1);
            printf("zona_successiva: %p\n", tmp->zona_successiva);
            printf("--------------------------------------------------\n");
            tmp= tmp->zona_successiva; //Passo ad esaminare la zona successiva
            i++;
            if(tmp!= NULL){
                /* Aspetto che venga premuto un carattere,
                   così da procedere alla zona successiva */
                char c= getchar();
                //Se il carattere è diverso da '\n' smetto di stampare
                if(c!= '\n'){
                    check_buffer()? system("clear"): system("clear");
                    printf("Stampa interrotta...\n\n");
                    break;
                }
            }else{
                printf("Stampa completata!\n");
                printf("Premi invio per continuare...");
                check_buffer()? system("clear"): system("clear");
            }
        }
    }
}

/* Cancello l'ultima zona in fondo alla lista, distinguo il caso in cui
 * non ci sono zone da cancellare, quello in cui è presente una sola zona e
 * quello in cui sono presenti più zone. Se sono presenti più zone scorro
 * tutta la lista fino ad arrivare al penultimo elemento, così da impostare
 * utlima_zona= prev.
 */
static void canc_zona(){
    if(prima_zona== NULL){
        printf("Non ci sono zone da cancellare...\n\n");
    }else{
        if((prima_zona->zona_successiva) == NULL){
            free(prima_zona);
            prima_zona= NULL;
        }else{
            sZona *tmp= prima_zona, *prev= NULL;
            while((tmp->zona_successiva) != NULL){
                if((tmp->zona_successiva->zona_successiva) == NULL){
                    prev= tmp;
                }
                tmp= tmp->zona_successiva;
            }
            free(prev->zona_successiva);
            prev->zona_successiva= NULL;
            ultima_zona= prev;
        }
        num_zone--; //Decremento il numero delle zone
        printf("Zona cancellata, %d zone rimaste...\n", num_zone);
    }
}

/* Controllo che il numero delle zone inserite sia maggiore di 7 e che
 * l'ultima zona inserita sia uguale a 'uscita campeggio'. Inoltre
 * faccio inserire all'utente una password per riaccedere in seguito
 * al crea_mappa().
 */
static void chiudi_mappa(){
    if(num_zone > 7){
        if(ultima_zona->zona == uscita_campeggio){
            progress= 1; //Imposto a 1 il flag per ciclo crea_mappa()
            if(strcmp(pwd_mappa, "NULL")== 0){
                //L'utente imposta una password prima di chiudere la mappa
                do{
                    printf("Inserisci password per accedere in seguito (da 5 a 15 caratteri): ");
                    scanf(" %s", pwd_mappa);
                    system("clear");
                    if(strlen(pwd_mappa) > 15){
                        printf("Password troppo lunga!\n\n");
                    }else if(strlen(pwd_mappa) < 5){
                        printf("Password troppo corta!\n\n");
                    }else{
                        printf("Password inserita!\n\n");
                    }
                }while(!(strlen(pwd_mappa) <= 15) || !(strlen(pwd_mappa) >= 5));
            }
        }else{
            printf("Per uscire dal menù l'ultima zona dovrà essere 'Uscita campeggio'...\n\n");
        }
    }else{
        printf("Devono esserci almeno 8 zone per uscire,"
               "'Uscita campeggio' deve essere l'ultima zona...\n\n");
    }
}

//Stampo tot asterischi in base al valore del parametro
static void stampa_asterischi(int *count){
    for (int i = 0; i < *count; i++) {
        printf("*");
    }
}

void gioca(){
    if(prima_zona!= NULL){
        int scelta= -1, //Variabile di appoggio per scelta funzione da menù
            turno= -1, //Marzia-> 0, Giacomo-> 1
            esito= 0, //Mossa NON effettuata-> 0, Mossa effettuata-> 1
            cont_turni= 1, //Rappresenta il numero dei turni effettuati complessivamente
            combo= 0, //Adrenalina in uso-> combo > 0, Adrenalina NON in uso-> combo= 0
            mostra_ogg[2]= {0, 0}, //[0]-> Marzia, [1]-> Giacomo; 0-> Non mostra_ogg, 1-> Ok mostra_ogg
            uso_benzina= 0; //Benzina in uso-> uso_benzina > 0, Benzina NON in uso-> uso_benzina= 0
        inizializzo_giocatori(&turno);
        do{
            printf(	"\t\t\t\t\t---------------------------------------\n"
    					"\t\t\t\t\t|       Campeggio Lake Trasymen       |\n"
                        "\t\t\t\t\t|                GIOCA!               |\n"
    					"\t\t\t\t\t---------------------------------------\n"
        				"\t\t\t\t\t| 1 - Avanza                          |\n"
        				"\t\t\t\t\t| 2 - Mostra oggetto                  |\n"
        				"\t\t\t\t\t| 3 - Prendi oggetto                  |\n"
                        "\t\t\t\t\t| 4 - Cura                            |\n"
                        "\t\t\t\t\t| 5 - Usa adrenalina                  |\n"
                        "\t\t\t\t\t| 6 - Riepilogo giocatore             |\n"
    					"\t\t\t\t\t---------------------------------------\n"
                        "\t\t\t\t\t                              TURNO: %d\n"
                        "\t\t\t\t\t---------------------------------------\n\n"
    			  , cont_turni);
            //Stampo il nome del player attuale
            if(player== &marzia){
                printf("MARZIA");
            }else if(player== &giacomo){
                printf("GIACOMO");
            }
            //Benzina ed Adrenalina in uso, gli * rappresentano i turni rimasti
            if((uso_benzina > 0)&&(combo > 0)){
                printf(" (\x1b[36m");
                    stampa_asterischi(&combo);
                printf("\x1b[0m)");
                printf("(\x1b[31m");
                    stampa_asterischi(&uso_benzina);
                printf("\x1b[0m) : ");
            }else if(combo > 0){
                printf(" (\x1b[36m");
                    stampa_asterischi(&combo);
                printf("\x1b[0m) : ");
            }else if(uso_benzina > 0){
                //Benzina in USO, gli * rappresentano i turni rimasti
                printf(" (\x1b[31m");
                    stampa_asterischi(&uso_benzina);
                printf("\x1b[0m) : ");
            }else{
                //Benzina ed Adrenalina NON in USO
                printf(" : ");
            }

            scanf(" %d", &scelta);
            system("clear");
            if(!check_buffer()){
                continue;
            }else{
                switch (scelta) {
                    case 1:
                        avanza(&esito);
                        mostra_ogg[turno]= 0;
                        break;
                    case 2:
                        mostra_oggetto(&esito, mostra_ogg, &turno);
                        break;
                    case 3:
                        prendi_oggetto(&esito, mostra_ogg, &turno);
                        break;
                    case 4:
                        cura(&esito);
                        break;
                    case 5:
                        usa_adrenalina(&esito, &combo);
                        break;
                    case 6:
                        stampa_giocatore(&esito);
                        break;
                    default:
                        printf("Comando errato, si prega di selezionare una voce presente nel menù...\n\n");
                        esito= 0;
                }
                /* Se l'operazione è andata a buon fine vado a controllare
                   se la Benzina è in uso. Se non lo è verifico se compare Gieson,
                   altrimenti decremento il contatore di Benzina (4 turni).*/
                if(esito){
                    if(!uso_benzina){
                        if(gieson()){
                            //Se appare Gieson procedo con la fase di difesa
                            fase_difesa(&uso_benzina, &turno);
                        }
                    }else{
                        uso_benzina--;
                    }
                    /* Se l'Adrenalina non è in uso o il giocatore è morto
                       a causa di Gieson effettuo il cambiamento del turno. */
                    if((combo<=1) || (player->stato== morto) || (player->posizione== NULL)){
                        cambio_turno(&turno, &combo, &cont_turni); //Effettuo il cambiamento del turno
                    }else{
                        combo--;
                    }
                }
            }
        }while(!((scelta > 0) && (scelta < 7)) || !(0));
    }else{
        printf("Creare una mappa prima di iniziare a giocare!\n\n");
    }
}

/* Controllo se compare Gieson, distinguendo i casi in cui
 * NESSUNO dei due giocatori è morto, uno dei due si trova
 * in 'uscita_campeggio' o uno dei due giocatori è morto.
 * -> return 0/1 (0: Non compare, 1: Compare);
 */
static int gieson(){
    unsigned int rnd= rand()%101;
    if((player->posizione!= NULL) && (player->stato!= morto)){
        if((player->posizione->zona) != uscita_campeggio){
            if((((marzia.posizione != NULL) && (giacomo.posizione != NULL))
              &&((marzia.stato != morto) && (giacomo.stato != morto)))){
                if(rnd<=30){
                    return 1;
                }else{
                    return 0;
                }
            }else{
                if(rnd<=50){
                    return 1;
                }else{
                    return 0;
                }
            }
        }else{
            if(rnd<=75){
                return 1;
            }else{
                return 0;
            }
        }
    }
    return 0;
}

/* Stampo "GIESON", controllo se il giocatore ha almeno un'arma
 * con cui difendersi , altrimenti quest'ultimo muore dato che non
 * può difendersi.
 * @int *benz (byRef);
 * @int *t (byRef);
 */
static void fase_difesa(int *benz, int *t){
    show_gieson();
    if((player->zaino[coltello] > 0)||(player->zaino[pistola] > 0)||(player->zaino[benzina] > 0)){
        //Controllo se il giocatore ha scelto Benzina come arma per difendersi
        if(menu_scelta_difesa()== 4){
            *benz= 4;
        }else{
            *benz= 0;
        }
    }else{
        printf("Non puoi difenderti!\n");
        player->stato= morto;
        if(player== &giacomo){
            printf("GIACOMO morto!\n");
        }else{
            printf("MARZIA morta!\n");
        }
        game_over();
        printf("Premere invio...");
        check_buffer()? system("clear"): system("clear");
    }
}

/* Stampo un meù per far scegliere al giocatore con quale
 * arma difendersi, scorrendo gli oggetti nel suo zaino.
 * Effettuata la scelta applico le conseguenze della difesa
 * utilizzando quella specifica arma.
 * -> return 2/3/4 (2: Coltello, 3: Pistola, 4: Benzina);
 */
static int menu_scelta_difesa(){
    int t_ogg= -1, i= 2;
    printf("Oggetti disponibili: \n");
    while(i < 5){
        if(oggetto_presente(i)){
            printf("%d - ", (i-1));
            stampa_tipo_oggetto(i, 0);
            printf(": %d\n", player->zaino[i]);
        }
        i++;
    }
    printf("\n");
    i= 0;
    do{
        printf("Con cosa vuoi difenderti? -> ");
        scanf(" %d", &t_ogg);
        if(!check_buffer()){
            continue;
        }else{
            //t_ogg: 1/2/3 (1+1: coltello, 2+1: pistola, 3+1: benzina)
            switch (t_ogg+1) {
                case coltello:
                    i= difesa_coltello();
                    break;
                case pistola:
                    i= difesa_pistola();
                    break;
                case benzina:
                    i= difesa_benzina();
                    break;
                default:
                    printf("Comando errato, si prega di selezionare un'arma presente nello zaino...\n\n");
            }
        }
    //i utilizzato per confermare l'avvenuta difesa da parte dell'arma
    }while(!(i) && ((t_ogg!= coltello)||(t_ogg!= pistola)||(t_ogg!= benzina)));
    return i;
}

/* Funzione che mi controlla se almeno un coltello è
 * presente nello zaino, se è presente verrà utilizzato e quindi
 * eliminato dallo zaino. Inoltre se il giocatore ha come stato 'vivo'
 * passa a 'ferito', mentre se era già in quello stato diventa 'morto'.
 * -> return 2/0 (2: Coltello utilizzato, 0: Coltello non presente);
 */
static int difesa_coltello(){
    if(oggetto_presente(coltello)){
        player->zaino[coltello]--;
        if(player->stato== vivo){
            player->stato= ferito;
            system("clear");
            if(player== &giacomo){
                printf("GIACOMO rimasto ferito...\n");
            }else{
                printf("MARZIA rimasta ferita...\n");
            }
        }else if(player->stato== ferito){
            player->stato= morto;
            system("clear");
            if(player== &giacomo){
                printf("GIACOMO morto, eri già ferito...\n");
            }else{
                printf("MARZIA morta, eri già ferita...\n");
            }
        }
        return 2;
    }else{
        printf("Comando errato, si prega di selezionare un'arma presente nello zaino...\n\n");
        return 0;
    }
}

/* Funzione che mi controlla se almeno una pistola è
 * presente nello zaino, se è presente verrà utilizzata e quindi
 * eliminata dallo zaino. In questo caso non accadra nulla
 * per quanto riguarda lo stato del giocatore.
 * -> return 3/0 (2: Pistola utilizzata, 0: Pistola non presente);
 */
static int difesa_pistola(){
    if(oggetto_presente(pistola)){
        player->zaino[pistola]--;
        system("clear");
        printf("Non accade nulla...\n");
        return 3;
    }else{
        printf("Comando errato, si prega di selezionare un'arma presente nello zaino...\n\n");
        return 0;
    }
}

/* Funzione che mi controlla se almeno una benzina è
 * presente nello zaino, se è presente verrà utilizzata e quindi
 * eliminata dallo zaino. In questo caso Gieson non apparirà
 * per 4 turni, cumilativi per Marzia e Giacomo.
 * -> return 4/0 (2: Benzina utilizzata, 0: Benzina non presente);
 */
static int difesa_benzina(){
    if(oggetto_presente(benzina)){
        player->zaino[benzina]--;
        system("clear");
        printf("Hai allontanato Gieson per 4 turni! (\x1b[31m*\x1b[0m)\n");
        return 4;
    }else{
        printf("Comando errato, si prega di selezionare un'arma presente nello zaino...\n\n");
        return 0;
    }
}

/* Passo alla zona successiva della mappa, distinguendo
 * il caso in cui la zona successiva è uguale a NULL.
 * @int *eseguita (byRef);
 */
static void avanza(int *eseguita){
    if(player->posizione->zona_successiva != NULL){
        player->posizione= player->posizione->zona_successiva;
        printf("Sto avanzando in: ");
        stampa_tipo_zona(player->posizione->zona);
    }else{
        player->posizione= NULL;
        if(player== &giacomo){
            printf("GIACOMO fuori!\n");
        }else{
            printf("MARZIA fuori!\n");
        }
        vittoria();
    }
    *eseguita= 1;
}

/* Mostra l'oggetto presente nella zona in cui si trova il
 * giocatore del turno corrente verificando se sia già stato mostrato
 * oppure no.
 * @int *eseguita (byRef);
 * @int *ricerca (byRef);
 * @int *t (byRef);
 */
static void mostra_oggetto(int *eseguita, int *ricerca, int *t){
    if(!ricerca[*t]){
        printf("Oggetto in zona: ");
            stampa_tipo_oggetto(player->posizione->oggetto, 1);
        *eseguita= 1;
        ricerca[*t]= 1;
    }else{
        *eseguita= 0;
        printf("Oggetto già mostrato...\n");
    }
}

/* Permette di mettere nello zaino l'oggetto mostrato in precedenza,
 * utilizzo ricerca in posizione turno proprio per verificare che
 * prima di prendere l'oggetto, il giocatore l'abbia cercato.
 * @int *eseguita (byRef);
 * @int *ricerca (byRef);
 * @int *t (byRef);
 */
static void prendi_oggetto(int *eseguita, int *ricerca, int *t){
    if((ricerca[*t]) && (player->zaino[player->posizione->oggetto]< 5)){
        printf("Hai messo nello zaino: ");
            stampa_tipo_oggetto(player->posizione->oggetto, 1);
        player->zaino[player->posizione->oggetto]++;
        player->posizione->oggetto= cianfrusaglia;
        *eseguita= 1;
        ricerca[*t]= 0;
    }else if((ricerca[*t]) && (player->zaino[player->posizione->oggetto]== 5)){
        *eseguita= 0;
        printf("Non puoi prendere più di 5 oggetti per tipo!\n\n");
    }else if(!ricerca[*t]){
        *eseguita= 0;
        printf("Devi prima cercare un oggetto!\n\n");
    }
}

/* Se l'oggetto bende è presente all'interno dello zaino
 * e lo stato di salute del giocatore è uguale a 'ferito', rimuovendo
 * le bende lo stato diventa nuovamente 'vivo'.
 * @int *eseguita (byRef);
 */
static void cura(int *eseguita){
    if(oggetto_presente(bende)){
        if(player->stato == ferito){
            printf("Stato salute attuale: ");
                stampa_stato_giocatore(player->stato);
            player->zaino[bende]--;
            printf("Bende utilizzate e rimosse dallo zaino...\n");
            player->stato= vivo;
            printf("Stato salute post-bende: ");
                stampa_stato_giocatore(player->stato);
            *eseguita= 1;
        }else{
            printf("Non sprecare bende inutilmente!\n\n");
            *eseguita= 0;
        }
    }else{
        printf("Non hai bende a disposizione!\n\n");
        *eseguita= 0;
    }
}

/* Se l'oggetto adrenalina è presente all'interno dello zaino
 * la rimuovo, così che utilizzandola il giocatore potrà
 * effettuare due mosse consecutive.
 * @int *eseguita (byRef);
 * @int *bonus (byRef);
 */
static void usa_adrenalina(int *eseguita, int *bonus){
    if(!*bonus){
        if(oggetto_presente(adrenalina)){
            player->zaino[adrenalina]--;
            printf("Adrenalina utilizzata e rimossa dallo zaino... (\x1b[36m*\x1b[0m)\n\n");
            *bonus= 3;
            *eseguita= 1;
        }else{
            printf("Non hai adrenalina a disposizione!\n\n");
            *eseguita= 0;
        }
    }else{
        //Adrenalina ancora in uso, non posso "accumularne" gli effetti
        printf("Adrenalina ancora in circolo, attendere per non rischiare la vita!\n\n");
        *eseguita= 0;
    }
}

/* Funzione che mi restituisce 1 se l'oggetto passato come parametro
 * è presente all'interno dello zaino del giocatore, altrimenti 0.
 * @tOggetto t_ogg;
 * -> return 0/1 (0: Nessun oggetto, 1: Almeno un oggetto);
 */
static int oggetto_presente(tOggetto t_ogg){
    if(player->zaino[t_ogg] > 0){
        return 1;
    }else{
        return 0;
    }
}

/* Stampo lo stato di salute del giocatore a seconda del valore
 * di stato passato come parametro.
 * @tStato stato;
 */
static void stampa_stato_giocatore(tStato stato){
    switch (stato) {
        case 1:
            printf("Morto\n");
            break;
        case 2:
            printf("Ferito\n");
            break;
        case 3:
            printf("Vivo\n");
            break;
    }
}

/* Stampo il riepilogo del giocatore, mostrando di chi si tratta,
 * il suo stato di salute, la posizione e gli oggetto che possiede
 * nello zaino.
 * @int *eseguita (byRef);
 */
static void stampa_giocatore(int *eseguita){
    printf("--------------------------------------------------\n");
    if(player== &marzia){
        printf("\t\t\tMARZIA\n");
    }else if(player== &giacomo){
        printf("\t\t\tGIACOMO\n");
    }
    printf("Stato giocatore: ");
        stampa_stato_giocatore(player->stato);
    printf("Posizione: ");
        stampa_tipo_zona(player->posizione->zona);
    unsigned short i= 0;
    printf("Oggetti nello zaino: \n");
    //Scorro per tutti e 6 gli oggetti
    while(i < 6){
        printf("   • ");
        stampa_tipo_oggetto(i, 0);
        printf(": %d\n", player->zaino[i]);
        i++;
    }
    printf("--------------------------------------------------\n\n");
    *eseguita= 0;
}

/* Funzione utilizzata solamente una volta all'iterno del progetto,
 * vengono inizializzate le due struct Giocatore (sGiocatore) con
 * posizione uguale alla prima zona della mappa e lo zaino che conterrà
 * gli elementi prefissati su pdf.
 * @int *t (byRef);
 */
static void inizializzo_giocatori(int *t){
    marzia.stato= vivo;
    marzia.posizione= prima_zona;
    marzia.zaino[adrenalina]= 2;

    giacomo.stato= vivo;
    giacomo.posizione= prima_zona;
    giacomo.zaino[coltello]= 1;

    //Estraggo un numero casuale per scegliere chi comincia a giocare
    if(rand()%101 >= 50){
        player= &marzia;
        *t= 0;
        printf("Marzia inizia il gioco...\n");
    }else{
        player= &giacomo;
        *t= 1;
        printf("Giacomo inizia il gioco...\n");
    }
}

/* Dopo aver controllato in che situazione ci troviamo, verrà
 * effettuato il cambiamento del turno del giocatore.
 * @int *t (byRef);
 * @int *bonus (byRef);
 * @int *cont (byRef);
 */
static void cambio_turno(int *t, int *bonus, int *cont){
    static short fine_turno= 1;
    if((marzia.stato== morto) && (giacomo.stato== morto)){
        //Entrambi i giocatori sono MORTI
        game_over();
        printf("MARZIA e GIACOMO sono morti!\n");
        termina_gioco();
    }else if((marzia.posizione== NULL) && (giacomo.posizione== NULL)){
        //Entrambi i giocatori sono riusciti ad USCIRE
        vittoria();
        printf("MARZIA e GIACOMO hanno vinto!\n");
        termina_gioco();
    }else if((marzia.posizione== NULL) && (giacomo.stato== morto)){
        //Giacomo morto e Marzia uscita
        printf("MARZIA ha vinto!\n");
        termina_gioco();
    }else if((giacomo.posizione== NULL) && (marzia.stato== morto)){
        //Marzia morta e Giacomo uscito
        printf("GIACOMO ha vinto!\n");
        termina_gioco();
    }else{
        if((marzia.stato== morto)||(marzia.posizione== NULL)){
            // Se Marzia è morta o è uscita continua il gioco Giacomo
            player= &giacomo;
            *cont+= 1;
            *t= 1;
        }else if((giacomo.stato== morto)||(giacomo.posizione== NULL)){
            // Se Giacomo è morto o è uscito continua il gioco Marzia
            player= &marzia;
            *cont+= 1;
            *t= 0;
        }else{
            printf("Premi invio per passare il turno...");
            check_buffer()? system("clear"): system("clear");
            if((fine_turno%2)!= 0){
                //Alterno il turno tra Marzia e Giacomo
                if(*t == 0){
                    player= &giacomo;
                    *t= 1;
                }else if(*t == 1){
                    player= &marzia;
                    *t= 0;
                }
                fine_turno= 2;
            }else{
                fine_turno= 1;
                printf("Estrazione per inizio turno...\n");
                /* Estraggo un numero casuale per scegliere
                   chi comincia il turno */
                if(rand()%101 >= 50){
                    player= &marzia;
                    *t= 0;
                }else{
                    player= &giacomo;
                    *t= 1;
                }
                *cont+= 1;
            }
        }
        //L'Adrenalina poteva essere ancora in uso
        *bonus= 0;
    }
}

/* Rendo riallocabile la memoria heap utilizzata per la creazione
 * della lista dinamica di sZone.
 */
static void free_mappa(){
    if(prima_zona!= NULL){
        sZona *tmp= prima_zona, *app= tmp;
        while(app->zona_successiva!= NULL){
            tmp= app;
            app= app->zona_successiva;
            free(tmp);
        }
        free(app);
        prima_zona= NULL, ultima_zona= NULL, num_zone= 0;
        printf("Cancellazione mappa in corso...\n");
    }else{
        printf("Non ci sono zone da cancellare...\n\n");
    }
}

static void show_gieson(){
    printf("\n");
    printf("\t\t\t\t      \x1b[31m _____ _____ _____ _____  _____ _   _ \x1b[0m\n"
           "\t\t\t\t      \x1b[31m|  __ \\_   _|  ___/  ___||  _  | \\ | |\x1b[0m\n"
           "\t\t\t\t      \x1b[31m| |  \\/ | | | |__ \\ `--. | | | |  \\| |\x1b[0m\n"
           "\t\t\t\t      \x1b[31m| | __  | | |  __| `--. \\| | | | . ` |\x1b[0m\n"
           "\t\t\t\t      \x1b[31m| |_\\ \\_| |_| |___/\\__/ /\\ \\_/ / |\\  |\x1b[0m\n"
           "\t\t\t\t      \x1b[31m \\____/\\___/\\____/\\____/  \\___/\\_| \\_/\x1b[0m\n\n\n");
}

static void vittoria(){
    printf("\n");
    printf("\t\t\t\t\x1b[36m__   __             _    _ _       _ \x1b[0m\n"
           "\t\t\t\t\x1b[36m\\ \\ / /            | |  | (_)     | |\x1b[0m\n"
           "\t\t\t\t\x1b[36m \\ V /___  _   _   | |  | |_ _ __ | |\x1b[0m\n"
           "\t\t\t\t\x1b[36m  \\ // _ \\| | | |  | |/\\| | | '_ \\| |\x1b[0m\n"
           "\t\t\t\t\x1b[36m  | | (_) | |_| |  \\  /\\  / | | | |_|\x1b[0m\n"
           "\t\t\t\t\x1b[36m  \\_/\\___/ \\__,_|   \\/  \\/|_|_| |_(_)\x1b[0m\n\n\n");
}

static void game_over(){
    printf("\n");
    printf("\t\t\t\t\x1b[31m _____                        _____                 \x1b[0m\n"
           "\t\t\t\t\x1b[31m|  __ \\                      |  _  |               \x1b[0m\n"
           "\t\t\t\t\x1b[31m| |  \\/ __ _ _ __ ___   ___  | | | |_   _____ _ __ \x1b[0m\n"
           "\t\t\t\t\x1b[31m| | __ / _` | '_ ` _ \\ / _ \\ | | | \\ \\ / / _ \\ '__|\x1b[0m\n"
           "\t\t\t\t\x1b[31m| |_\\ \\ (_| | | | | | |  __/ \\ \\_/ /\\ V /  __/ |   \x1b[0m\n"
           "\t\t\t\t\x1b[31m \\____/\\__,_|_| |_| |_|\\___|  \\___/  \\_/ \\___|_|   \x1b[0m\n\n\n");
}

//Termino l'esecuzione dopo aver reso riallocabile la memoria heap utilizzata.
void termina_gioco(){
    if(prima_zona!= NULL){
        free_mappa();
    }
    printf("Chiusura programma in corso...\n");
    exit(0);
}
