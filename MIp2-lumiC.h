/**************************************************************************/

#include "MIp2-t.h"

/*                                                                        */
/* P2 - MI amb sockets TCP/IP - Part II                                   */
/* Fitxer capçalera de lumiC.c                                            */
/*                                                                        */
/* Autors: X, Y                                                           */
/*                                                                        */
/**************************************************************************/

/* Declaració de funcions EXTERNES de lumiC.c, és a dir, d'aquelles       */
/* funcions que es faran servir en un altre fitxer extern a lumiC.c,      */
/* p.e., int LUMIc_FuncioExterna(arg1, arg2...) { }                       */
/* El fitxer extern farà un #include del fitxer lumiC.h a l'inici, i      */
/* així les funcions seran conegudes en ell.                              */
/* En termes de capes de l'aplicació, aquest conjunt de funcions          */
/* EXTERNES formen la interfície de la capa LUMI, la part del client      */

int Log_CreaFitx(const char *NomFitxLog);
int Log_Escriu(int FitxLog, const char *MissLog);
int Log_TancaFitx(int FitxLog);
int LUMI_obtenirHost(const char* adrLumi, char* host);

/*
 * @brief Registra un usuari al node que li pertoca
 * @pre ---
 * @post S'ha registrat l'usuari al node corresponen. Aixo significa que el
 *      servidor ha guardat la seva adreca i l'ha posat com a disponible
 * @return 0 si tot ha anat be, 1 si no ha trobat l'usuari
 */
int LUMI_ferRegistre(int sck,char *ipRem, int portUDP,const char * adrMiLoc, int LogFile);

/*
 * @brief Desregistra un usuari al node que li pertoca
 * @pre ---
 * @post S'ha desregistrar l'usuari al node corresponen. Aixo significa que el
 *      servidor ha eliminat la seva adreca i l'ha posat com a no disponible
 * @return 0 si tot ha anat be, 1 si no ha trobat l'usuari
 */
int LUMI_ferDesregistre(const char * adrLumiLoc, const char * ipRem, int portUDP, int sckUDP, int LogFile);


/*
 * @brief Localitza l'usuari
 * @pre Usuari local registrat al seu node
 * @post Ens retorna IP i port del usuari al que intentem localitzar
 * @return 0 si tot ha anat be, 1 si no ha trobat l'usuari
 */
int LUMI_localitzaUsuari(const char * adrMiRem, char * ipRem, int * portRem, int LogFile);

/**
 * @brief Envia un missatge
 * @pre ---
 * @post S'ha enviat un missatge a l'adreca ipRem amb port portRem
 * @return -1 si no ha funcionat, qualsevol valor positiu si funciona
 */
int LUMI_enviaMissatge(int Sck, const char *IPrem, int portUDPrem, const char *SeqBytes, int LongSeqBytes, int LogFile);

/**
 * @brief Rep un missatge
 * @pre ---
 * @post S'ha rebut un missatge del socket i s'ha actualitzat la IP remota i el port remot
 * @return -1 si no ha funcionat, qualsevol valor positiu si funciona
 */
int LUMI_repMissatge(int Sck, char *IPrem, int *portUDPrem, char *seqBytes, int LongSeqBytes, int LogFile);


/**
 * @brief Construeix el protocol
 * @pre ---
 * @post S'ha construit una cadena de caracters sense \0 ni \n amb format: R @MI
 */
int LUMI_construeixProtocolLUMI(const char *adrMI, char * petRegistre);

/**
 * @brief Ens diu si ha arribat alguna cosa
 * @pre ---
 * @post Retorna el descriptor actiu del socket per el qual ens ha arribat un missatge
 */
int LUMI_haArribatAlgunaCosa(int sck, int sckTCP);

/**
 * @brief Ens diu si ha arribat alguna cosa en temps (segons)
 * @pre Temps >= 0
 * @post Retorna el descriptor actiu del socket per el qual ens ha arribat un missatge
 */
int LUMI_haArribatAlgunaCosaEnTemps(int sck,int sckTCP,int temps);

/**
 * @brief Obte IP i Port UDP
 * @pre ---
 * @post Donat un socket UDP, ens dona la IP i port UDP associats a aquest socket
 */
int LUMI_trobarSckNom(int sckUDP, char * ipLoc, int * portLoc );


/**
 * @brief Inicia el procediment de registre
 * @pre ---
 * @post S'ha registrat correctament l'usuari
 */
int LUMI_registre(int sckUdp, int portUdp, char * miss, const char * adrLumiLoc, char * ipRem, int nBytesLoc, int portUdpLoc, int LogFile);


/**
 * @brief Construeix el missatge de localitzacio
 * @pre ---
 * @post Donades dues adreces LUMI, construeix i envia, un missatge de localitzacio al servidor. Retorna el nombre de bytes si tot ha anat be, -1 si hi ha hagut error
 */
int LUMI_construirMissatgeLoc(const char * miss, const char * adrLumiLoc, int nBytesLoc, char * missLoc, int LogFile); 

/**
 * @brief Extreu IP i port del missatge
 * @pre missatge en format valid
 * @post S'han emplenat ipDesti i portTcp amb l'informacio continguda en el missatge
 */
void LUMI_extreureIpPort(const char * miss, char * ipDesti, int * portTcp); 

/**
 * @brief Missatge de resposta de localitzacio
 * @pre miss en format valid
 * @post S'ha construit i enviat el missatge de localitzacio amb la IP i port TCP del client, i l'adreca LUMI destinataria
 */
int LUMI_construirMissatgeLocResp(int sckUdp, int sckTCP, const char * miss, const char * ipLoc, int portTcp, const char * ipRem, int portUdp, int LogFile);


/**
 * @brief Gestiona els diferents missatges que rep el client
 * @pre missResposta ha de ser un vector de caracters sense \0 ni \n
 * @return Retorna un enter que ens diu el que ha de fer el client
 *          #codi       #actuacio
 *            5         Construir missatge de localitzacio
 *            0         Realitzar conexio amb client remot
 *            1         Mostrar missatge que client no existeix
 *            2         Mostrar missatge que format incorrecte/host no trobat
 *            3         Mostrar missatge que client ocupat
 *            4         Mostrar missatge que client offline
 */
int LUMI_gestionaMissatge(const char * missResposta, int nBytes, int sckUdp, int sckTCP, const char * ipInterface, char * ipRem, int portUdp, int portTCPloc, int LogFile);

/**
 * @brief Connecta dos clients
 * @pre nicLoc, nicRem i ipRem,ipLoc han de ser vectors de chars de c amb \0 al final
 * @post Realitza connexio entre dos clients mitjançant un socket TCP, ip remota valida, i port tcp valid.
 *      sesc ens indica el socket d'escolta
 *      ipRem, en cas de que fem de client, ip remota a la que ens conectem
 *      ipLoc, en cas de fer de servidor, l'ip a la que es conectaran
 *      nicLoc i nicRem, nick del remot i del local
 *      portTCPloc i portTCPrem ens indica el port del local i del remot
 *      tipus ens indica si fem de client o servidor
 * @return scon ( socket de connexio )     
 */
int LUMI_connexio(int sesc, char * ipRem, char * ipLoc, const char * nicLoc, char * nicRem, int * portTCPloc, int * portTCPrem, int tipus);


/**
 * @brief Realitza una conversa entre 2 clients LUMI
 * @pre scon es un socket valid TCP
 * @post Estableix una conversa entre 2 clients via socket de connexio 'scon', ip
 * @return 
 */
int LUMI_conversa(int scon, int sckUdp, char * ipRem, int * portUdp, int LogFile, const char * exitAp);

/**
 * @brief Demana conversa 
 * @pre ---
 * @post Demana conversa a un client via ip i port TCP
 */
int LUMI_demanaConv(const char* missResposta, char* ipDesti, int* portTCP, int sckTCP, char * ipLoc, const char * nicLoc, char * nicRem, int * portTCPloc);