/**************************************************************************/
/*                                                                        */
/* P2 - MI amb sockets TCP/IP - Part II                                   */
/* Fitxer capçalera de lumiC.c                                            */
/*                                                                        */
/* Autors: X, Y                                                           */
/*                                                                        */
/**************************************************************************/

/* Declaració de TIPUS de variables                                       */

typedef struct sckAdd{ 
    char adIP[16];
    int portUDP;
} sckAdd;

typedef struct data{
    char adMi[40]; 
    sckAdd sck;
} data;

typedef struct taulaClients{
	data taulaCli[200];
	int size;
} taulaClients;

/* Declaració de funcions EXTERNES de lumiS.c, és a dir, d'aquelles       */
/* funcions que es faran servir en un altre fitxer extern a lumiS.c,      */
/* p.e., int LUMIs_FuncioExterna(arg1, arg2...) { }                       */
/* El fitxer extern farà un #include del fitxer lumiS.h a l'inici, i      */
/* així les funcions seran conegudes en ell.                              */
/* En termes de capes de l'aplicació, aquest conjunt de funcions          */
/* EXTERNES formen la interfície de la capa LUMI, la part del servidor    */


/* DECLARACIÓ DE FUNCIONS EXTERNES */
int LUMIS_HaArribatAlgunaCosa(int sck);
int LUMIS_emplenaTaula(taulaClients *taulaCli);
int LUMIS_IniciaSockEsc(char* iploc, int portUDP);
int LUMIS_IniciaFitxer(const char* nomFitx);
int LUMIS_ServeixPeticions(int sck, taulaClients *taulaCli, int logFile);
int LUMIS_Finalitza(int sck);

