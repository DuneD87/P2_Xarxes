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
struct data{
    char* adMi;
    int adIP;
    int portUDP;
    int actiu; //0 -> inactiu; 1 -> actiu
};

data taulaClients[max];

/* DECLARACIÓ DE FUNCIONS */
int LUMI_HaArribatAlgunaCosa(int sck);
