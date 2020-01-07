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


/*
 * @brief Registra un usuari al node que li pertoca
 * @pre ---
 * @post S'ha registrat l'usuari al node corresponen. Aixo significa que el
 *      servidor ha guardat la seva adreca i l'ha posat com a disponible
 * @return 0 si tot ha anat be, 1 si no ha trobat l'usuari
 */
int LUMI_ferRegistre(int sck,char *ipRem, int portUDP,const char * adrMiLoc);

/*
 * @brief Desregistra un usuari al node que li pertoca
 * @pre ---
 * @post S'ha desregistrar l'usuari al node corresponen. Aixo significa que el
 *      servidor ha eliminat la seva adreca i l'ha posat com a no disponible
 * @return 0 si tot ha anat be, 1 si no ha trobat l'usuari
 */
int LUMI_ferDesregistre(const char * adrLumiLoc, const char * ipRem, int portUDP, int sckUDP);


/*
 * @brief Localitza l'usuari
 * @pre Usuari local registrat al seu node
 * @post Ens retorna IP i port del usuari al que intentem localitzar
 * @return 0 si tot ha anat be, 1 si no ha trobat l'usuari
 */
int LUMI_localitzaUsuari(const char * adrMiRem, char * ipRem, int * portRem);

/**
 * @brief Envia un missatge
 * @pre ---
 * @post S'ha enviat un missatge a l'adreca ipRem amb port portRem
 * @return -1 si no ha funcionat, qualsevol valor positiu si funciona
 */
int LUMI_enviaMissatge(int Sck, const char *IPrem, int portUDPrem, const char *SeqBytes, int LongSeqBytes);

/**
 * @brief Rep un missatge
 * @pre ---
 * @post S'ha rebut un missatge del socket i s'ha actualitzat la IP remota i el port remot
 * @return -1 si no ha funcionat, qualsevol valor positiu si funciona
 */
int LUMI_repMissatge(int Sck, char *IPrem, int *portUDPrem, char *seqBytes, int LongSeqBytes);


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
int LUMI_registre(int sckUdp, int portUdp, char * miss, const char * adrLumiLoc, char * ipRem, int nBytesLoc, int portUdpLoc);


/**
 * @brief Construeix el missatge de localitzacio
 * @pre ---
 * @post Donades dues adreces LUMI, construeix i envia, un missatge de localitzacio al servidor. Retorna el nombre de bytes si tot ha anat be, -1 si hi ha hagut error
 */
int LUMI_construirMissatgeLoc(const char * miss, const char * adrLumiLoc, int nBytesLoc, char * missLoc); 

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
int LUMI_construirMissatgeLocResp(int sckUdp, int sckTCP, const char * miss, const char * ipLoc, int portTcp, const char * ipRem, int portUdp);


