/**************************************************************************/
/*                                                                        */
/* P2 - MI amb sockets TCP/IP - Part II                                   */
/* Fitxer capçalera de lumiC.c                                            */
/*                                                                        */
/* Autors: X, Y                                                           */
/*                                                                        */
/**************************************************************************/

/* Declaració de funcions EXTERNES de lumiS.c, és a dir, d'aquelles       */
/* funcions que es faran servir en un altre fitxer extern a lumiS.c,      */
/* p.e., int LUMIs_FuncioExterna(arg1, arg2...) { }                       */
/* El fitxer extern farà un #include del fitxer lumiS.h a l'inici, i      */
/* així les funcions seran conegudes en ell.                              */
/* En termes de capes de l'aplicació, aquest conjunt de funcions          */
/* EXTERNES formen la interfície de la capa LUMI, la part del servidor    */


/* DECLARACIÓ DE VARIABLES */
const int max = 500; //mida maxima de la taula de clients
struct sckAdd{ //Per poder retornar l'adreça del socket en una funcio
    int adIP;
    int portUDP;
};

struct data{
    char* adMi; //farà de "key", ens servirà per fer cerques més eficients
    sckAdd sck;
};

const int portUDP = 2020; 
data *taulaClients = malloc(data); //És una "taula" que imita un map, per fer cerques més ràpides
int clientsTotal = 0;
void *root = 0;
sckAdd peticionsRem[max]; //Taula per guardar adreçes de les peticions d'altres servidors

/* DECLARACIÓ DE FUNCIONS */
int LUMI_HaArribatAlgunaCosa(int sck);
char* recievefrom(int sck, char* adMI);
