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
