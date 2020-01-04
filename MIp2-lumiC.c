/**************************************************************************/
/*                                                                        */
/* P2 - MI amb sockets TCP/IP - Part II                                   */
/* Fitxer lumiC.c que implementa la capa d'aplicació de MI, sobre la capa */
/* de transport UDP (fent crides a la "nova" interfície de la capa UDP o  */
/* "nova" interfície de sockets), però només la part client               */
/* Autors: X, Y                                                           */
/*                                                                        */
/**************************************************************************/

/* Inclusió de llibreries, p.e. #include <sys/types.h> o #include "meu.h" */
/*  (si les funcions EXTERNES es cridessin entre elles, faria falta fer   */
/*   un #include "lumi.h")                                                */

#include "MIp2-t.h"
#include "MIp2-dnsC.h"
#include "MIp2-lumiC.h"
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

/* Definició de constants, p.e., #define XYZ       1500                   */

/* Declaració de funcions INTERNES que es fan servir en aquest fitxer     */
/* (les  definicions d'aquestes funcions es troben més avall) per així    */
/* fer-les conegudes des d'aquí fins al final d'aquest fitxer, p.e.,      */
/* int FuncioInterna(arg1, arg2...);                                      */
/* Com a mínim heu de fer les següents funcions INTERNES:                 */

int Log_CreaFitx(const char *NomFitxLog);
int Log_Escriu(int FitxLog, const char *MissLog);
int Log_TancaFitx(int FitxLog);

/* Definició de funcions EXTERNES, és a dir, d'aquelles que es cridaran   */
/* des d'altres fitxers, p.e., int LUMIc_FuncioExterna(arg1, arg2...) { } */
/* En termes de capes de l'aplicació, aquest conjunt de funcions externes */
/* formen la interfície de la capa LUMI, la part del client               */


/* Definició de funcions INTERNES, és a dir, d'aquelles que es faran      */
/* servir només en aquest mateix fitxer. Les seves declaracions es troben */
/* a l'inici d'aquest fitxer.                                             */

/* Crea un fitxer de "log" de nom "NomFitxLog".                           */
/* "NomFitxLog" és un "string" de C (vector de chars imprimibles acabat   */
/* en '\0') d'una longitud qualsevol.                                     */
/* Retorna -1 si hi ha error; l'identificador del fitxer creat si tot va  */

/* bé.                                                                    */
int Log_CreaFitx(const char *NomFitxLog) {

}

/* Escriu al fitxer de "log" d'identificador "FitxLog" el missatge de     */
/* "log" "MissLog".                                                       */
/* "MissLog" és un "string" de C (vector de chars imprimibles acabat      */
/* en '\0') d'una longitud qualsevol.                                     */
/* Retorna -1 si hi ha error; el nombre de caràcters del missatge de      */

/* "log" (sense el '\0') si tot va bé                                     */
int Log_Escriu(int FitxLog, const char *MissLog) {


}

/* Tanca el fitxer de "log" d'identificador "FitxLog".                    */

/* Retorna -1 si hi ha error; un valor positiu qualsevol si tot va bé.    */
int Log_TancaFitx(int FitxLog) {
}

/* Si ho creieu convenient, feu altres funcions INTERNES                  */

int LUMI_ferRegistre(int sck, char *ipRem, int portUDP, const char* adrMiLoc) {
    int i = 0;
    char host[40];
    char c = adrMiLoc[i];
    while (c != '@') {

        i++;
        c = adrMiLoc[i];
    }
    i++; //Saltem @
    c = adrMiLoc[i];
    int j = 0;
    while (c != '\0') {
        host[j] = c;
        j++;
        i++;
        c = adrMiLoc[i];
    }
    host[j] = '\0';
    DNSc_ResolDNSaIP(host, ipRem);
}

int LUMI_enviaMissatge(int Sck, const char* IPrem, int portUDPrem, const char* SeqBytes, int LongSeqBytes) {
    int n = UDP_EnviaA(Sck, IPrem, portUDPrem, SeqBytes, LongSeqBytes);
    return n;
}

int LUMI_repMissatge(int Sck, char* IPrem, int* portUDPrem, char* seqBytes, int LongSeqBytes)
{
    return UDP_RepDe(Sck,IPrem,&(*portUDPrem),seqBytes,LongSeqBytes);
    
}

int LUMI_construeixProtocolLUMI(const char *adrMI, char * petRegistre) {
    int i = 0;
    petRegistre[i] = 'R';
    i++;
    int j = 0;
    char c = adrMI[j];
    while (c != '\0') {
        petRegistre[i] = adrMI[j];
        i++;
        j++;
        c = adrMI[j];
    } 
}

int LUMI_IniciaSckEscolta(const char *ipLoc, int portUDP) {
    return UDP_CreaSock(ipLoc, portUDP);
}

int LUMI_haArribatAlgunaCosa(int sckEsc) {
    fd_set conjunt;
    FD_ZERO(&conjunt); /* esborrem el contingut de la llista */
    FD_SET(0, &conjunt); /* afegim (“marquem”) el teclat a la llista */
    FD_SET(sckEsc, &conjunt); /* afegim (“marquem”) el socket connectat a la llista */
    int sel = T_HaArribatAlgunaCosa(&conjunt, sckEsc);
    int descActiu;
    if (sel != -1) {
        int i = 0;
        for (i; i <= sckEsc; i++)
            if (FD_ISSET(i, &conjunt)) {
                descActiu = i;
            }
    } else {
        descActiu = -1;
    }
    return descActiu;
}

int LUMI_haArribatAlgunaCosaEnTemps(int sck, int temps) {
    fd_set conjunt;
    FD_ZERO(&conjunt); /* esborrem el contingut de la llista */
    FD_SET(0, &conjunt); /* afegim (“marquem”) el teclat a la llista */
    FD_SET(sck, &conjunt); /* afegim (“marquem”) el socket connectat a la llista */
    int sel = T_HaArribatAlgunaCosaEnTemps(&conjunt, sck,temps);
    int descActiu;
    if (sel != -1) {
        int i = 0;
        for (i; i <= sck; i++)
            if (FD_ISSET(i, &conjunt)) {
                descActiu = i;
            }
    } else {
        descActiu = -1;
    }
    return descActiu;
}

int LUMI_trobarSckNom(int sckUDP, char* ipLoc, int * portLoc) {
    return UDP_TrobaAdrSockLoc(sckUDP,ipLoc,&portLoc);
}