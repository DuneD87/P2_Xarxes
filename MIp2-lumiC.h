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
int LUMI_ferDesregistre(const char * adrMiLoc);


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
int LUMI_haArribatAlgunaCosa(int sck);

/**
 * @brief Ens diu si ha arribat alguna cosa en temps (segons)
 * @pre Temps >= 0
 * @post Retorna el descriptor actiu del socket per el qual ens ha arribat un missatge
 */
int LUMI_haArribatAlgunaCosaEnTemps(int sck,int temps);

/**
 * @brief Obte IP i Port UDP
 * @pre ---
 * @post Donat un socket UDP, ens dona la IP i port UDP associats a aquest socket
 */
int LUMI_trobarSckNom(int sckUDP, char * ipLoc, int * portLoc );