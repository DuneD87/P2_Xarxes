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
        char domini[40];
} taulaClients;


/**
 * @brief Obte el host d'una adreça lumi
 * @pre adrLumi ha de ser un vector de characters de c acabat en \0
 * @post S'ha emplenat el vector de caracters host amb el host obtingut de l'adreça lumi
 *      i s'ha retornat el nombre de bytes del host.
 */
int LUMIS_obtenirHost(const char * adrLumi, char * host);

/**
 * @brief Ens diu si ha arribat alguna cosa
 * @pre Socket valid
 * @post Retorna el descriptor actiu del socket d'escolta per el qual ens ha 
 * arribat una peticio
 */
int LUMIS_HaArribatAlgunaCosa(int sck);

/**
 * @brief Emplena la taula d'usuaris
 * @pre Fitxer nodelumi.cfg ha d'exisistir a la carpeta dels servidor
 * @post S'ha emplenat la taula amb els diferents clients trobats al fitxer
 */
int LUMIS_emplenaTaula(taulaClients *taulaCli);

/**
 * @brief Inicia socket de escolta
 * @pre ---
 * @post S'ha iniciat un socket d'escolta UDP per on rebre peticions de clients
 */
int LUMIS_IniciaSockEsc(const char* iploc, int portUDP);

/**
 * @brief Serveix diferents peticions als clients de forma sequencial
 * @pre S'ha emplenat previament la taula de clients
 * @post Llegeix un missatge i segons el protocol, serveix peticions als clients
 */
int LUMIS_ServeixPeticions(int sck, taulaClients *taulaCli, int logFile);

/**
 * @brief Cerca al client
 * @pre ---
 * @post Retorna l'index de la taula on es troba el client, -1 si no el troba
 */
int LUMIS_cercarClient(taulaClients *taulaCli, char * client, int nBytes);


/**
 * @brief Finalitza y tenca el socket d'escolta
 */
int LUMIS_Finalitza(int sck);

/**
 * @brief Guarda el port o adreça que conté el missatge entre els ':' a "adr"
 * @pre --
 * @post Retorna la mida del porto adreça llegida
 */
int LUMIS_guardaAdr(char* adr, char*miss, int y, int x, char cmp);

/**
 * @brief Porta a terme el desregistre d'un client
 * @pre --
 * @post Retorna la mida missatge de confirmació enviat
 */
int LUMIS_procesDesregistre(int sck,taulaClients *taulaCli, char *miss, int bytes_llegits, int logFile, char *ipRem, int portRem, char *resp);

/**
 * @brief Porta a terme el proces de rebuda de resposta de Localització
 * @pre --
 * @post Retorna la mida del missatge enviat.
 */
int LUMIS_procesRespLoc(int sck,taulaClients *taulaCli, char *miss, int logFile, const char * ipRem);

/**
 * @brief Porta a terme el proces de Localització d'un client
 * @pre --
 * @post Retorna la mida del missatge enviat
 */
int LUMIS_procesLocalitzacio(int sck,taulaClients *taulaCli, char *miss, int bytes_llegits, int logFile);

/**
 * @brief Porta a terme el registre d'un client
 * @pre --
 * @post Retorna la mida missatge de confirmació enviat
 */
int LUMIS_procesRegistre(int sck,taulaClients *taulaCli, char *miss, int bytes_llegits, int logFile, char *ipRem, int portRem, char* resp);
