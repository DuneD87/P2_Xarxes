/**************************************************************************/
/*                                                                        */
/* P2 - MI amb sockets TCP/IP - Part II                                   */
/* Fitxer lumiC.c que implementa la capa d'aplicació de MI, sobre la capa */
/* de transport UDP (fent crides a la "nova" interfície de la capa UDP o  */
/* "nova" interfície de sockets), però només la part servidora            */
/* Autors: X, Y                                                           */
/*                                                                        */
/**************************************************************************/

/* Inclusió de llibreries, p.e. #include <sys/types.h> o #include "meu.h" */
/*  (si les funcions EXTERNES es cridessin entre elles, faria falta fer   */
/*   un #include "lumi.h")                                                */

#include "MIp2-t.h"
#include "MIp2-dnsC.h"
#include "MIp2-lumiS.h"
#include <ifaddrs.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <search.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>

/* Definició de constants, p.e., #define XYZ       1500 */

#define MAX 500
const char fitxNodeLumi[50] = "nodelumi.cfg";

//sckAdd peticionsRem[max]; //Taula per guardar adreçes de les peticions d'altres servidors

/* Declaració de funcions INTERNES que es fan servir en aquest fitxer     */
/* (les  definicions d'aquestes funcions es troben més avall) per així    */
/* fer-les conegudes des d'aquí fins al final d'aquest fitxer, p.e.,      */
/* int FuncioInterna(arg1, arg2...);                                      */
/* Com a mínim heu de fer les següents funcions INTERNES:                 */


/******************************************************************FUNCIONS EXTERNES************************************************************/

/* Definició de funcions EXTERNES, és a dir, d'aquelles que es cridaran   */
/* des d'altres fitxers, p.e., int LUMIs_FuncioExterna(arg1, arg2...) { } */
/* En termes de capes de l'aplicació, aquest conjunt de funcions externes */

/* formen la interfície de la capa LUMI, la part del servidor             */

int LUMIS_Finalitza(int sckLoc) {
    UDP_TancaSock(sckLoc);
}

int LUMIS_IniciaSockEsc(const char* iploc, int portUDP) {
    int sck = UDP_CreaSock(iploc,portUDP);
    return sck;
}

int LUMIS_emplenaTaula(taulaClients *taulaCli) {
    taulaCli->size = 0;
    FILE *fpt;
    fpt = fopen(fitxNodeLumi, "r");
    if (fpt == NULL) {
        return -1;
    }
    while (fgets(taulaCli->taulaCli[taulaCli->size].adMi, 40, fpt) != NULL) {
        strcpy(taulaCli->taulaCli[taulaCli->size].sck.adIP, "0.0.0.0");
        taulaCli->taulaCli[taulaCli->size].sck.portUDP = 0;
        taulaCli->size++;
    }
    fclose(fpt);

    return taulaCli->size;
}

int LUMIS_EnviaA(int Sck, const char *IPrem, int portUDPrem, const char *SeqBytes, int LongSeqBytes)
{
    return UDP_EnviaA(Sck,IPrem,portUDPrem,SeqBytes,LongSeqBytes);
}

int LUMIS_RepDe(int Sck, char *IPrem, int *portUDPrem, char *SeqBytes, int LongSeqBytes)
{
    return UDP_RepDe(Sck,IPrem,portUDPrem,SeqBytes,LongSeqBytes);
}

int LUMIS_Registre(taulaClients *taulaCli, int fitxLog, char* linia, int mida, char* resp, char *ip, int port) {
    int trobat = -1;
    int i = 0;
    char adMI[40];
    strcpy(adMI, linia + 1);
    while (i < taulaCli->size && trobat != 1) {
        printf("%s\n",adMI);
        if (strcmp(adMI, taulaCli->taulaCli[i].adMi) == 0) trobat = 1;
        else i++;
    }
    if (trobat == 1) {
        strcpy(taulaCli->taulaCli[i].sck.adIP, ip);
        taulaCli->taulaCli[i].sck.portUDP = port;
        strcpy(resp, "C0"); //ok
    } else strcpy(resp, "C1"); //usuari no trobat, innexistent

    return trobat;
}

int LUMIS_Desregistre(taulaClients * taulaCli, int fitxLog, char* linia, int mida, char* resp) {
    /*int err = 1;
    data *find = malloc(sizeof(data));
    find->adMi = adMI;
    void *cli = tfind(find,&root,compare);
    if(cli != NULL)
    {
        resp = "C0";
        (*(data**)cli)->sck.adIP = "0";
        (*(data**)cli)->sck.portUDP = 0;
        
    }
    else if(cli == NULL){
        resp = "C1";
    }
    else err = -1;
    
    return err;*/
}

int LUMIS_Localitzacio(taulaClients *taulaCli, int fitxLog, char* linia, int mida, char* resp) //Falta el cas on el servidor és diferent!!
{
    /*int err;
    data *find = malloc(sizeof(data));
    find->adMi = adMI;
    void *cli = tfind(find,&root,compare); //Busca el client amb el qual es vol parlar
    if(cli != NULL) //trobat, existeix
    {
        int ip = (*(data**)cli)->sck.adIP;
        if(ip == 0){ //offline
            resp = "C4";
        }
        else{
            if(ip!= 0){
                int port = (*(data**)cli)->sck.portUDP; //Client x port UDP o TCP?
                resp = "C0"+ip+":"+port; 
            }
            //else {} -> online però ocupat? 
        }
    }
    else if(cli == NULL) resp = "C1";
    else err = -1;;
    
    return err; */
}



int LUMIS_ServeixPeticions(int sck, taulaClients *taulaCli, int logFile) {
    char miss[500];
    char resp[500];
    char ipRem[16];
    int portRem;
    int bytes_llegits = LUMIS_RepDe(sck,ipRem,&portRem,miss,sizeof(miss));
    if (bytes_llegits == -1)
    {
        perror("Error al rebre el missatge: ");
        exit(-1);
    }
    int k = LUMIS_Registre(taulaCli,logFile,miss,bytes_llegits,resp,ipRem,portRem);
    printf("Nom:%s\nIP:%s\nPORT:%d\n",taulaCli->taulaCli[0].adMi,taulaCli->taulaCli[0].sck.adIP,taulaCli->taulaCli[0].sck.portUDP);
    int bytes_enviats = LUMIS_EnviaA(sck, ipRem,portRem,resp,sizeof(resp));
    if (bytes_enviats == -1) {
        perror("Error sending message");
        exit(-1);
    }
    /*char miss[500];
    char resp[500];
    int bytes_llegits, bytes_enviats, portrem;
    char iprem[16];
    int err = 1;
    if ((bytes_llegits = LUMIS_RepDe(sck,iprem,&portrem,miss,sizeof(miss))) == -1) {
        perror("Error recieving message");
        exit(-1);
    }
    miss[bytes_llegits] = '\0';
    if (miss[0] == 'R'){
        printf("Missatge de registre rebut!\n");
        err = LUMIS_Registre(taulaCli, logFile, miss, bytes_llegits, resp, iprem, portrem);
    }
    else if (miss[0] == 'D') err = LUMIS_Desregistre(taulaCli, logFile, miss, bytes_llegits, resp);
    else if (miss[0] == 'L') err = LUMIS_Localitzacio(taulaCli, logFile, miss, bytes_llegits, resp);
    else strcpy(resp, "C2");

    if ((bytes_enviats = LUMIS_EnviaA(sck, iprem,portrem,resp,sizeof(resp))) == -1) {
        perror("Error sending message");
        exit(-1);
    }

    return err;*/
}

/***********************************************************FUNCIONS INTERNES***********************************************************************/

/* Definició de funcions INTERNES, és a dir, d'aquelles que es faran      */
/* servir només en aquest mateix fitxer. Les seves declaracions es troben */
/* a l'inici d'aquest fitxer.                                             */

/* Crea un fitxer de "log" de nom "NomFitxLog".                           */
/* "NomFitxLog" és un "string" de C (vector de chars imprimibles acabat   */
/* en '\0') d'una longitud qualsevol.                                     */
/* Retorna -1 si hi ha error; l'identificador del fitxer creat si tot va  */

/* bé.                                                                    */
int Log_CreaFitx(const char *NomFitxLog) {
    return open(NomFitxLog, O_CREAT | O_RDWR | O_APPEND);
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

/* MÉS FUNCIONS INTERNES */




int LUMIS_HaArribatAlgunaCosa(int sckEsc) {
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

/* Si ho creieu convenient, feu altres funcions INTERNES                  */



