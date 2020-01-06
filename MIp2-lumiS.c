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
    int sck = UDP_CreaSock(iploc, portUDP);
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

int LUMIS_EnviaA(int Sck, const char *IPrem, int portUDPrem, const char *SeqBytes, int LongSeqBytes) {
    return UDP_EnviaA(Sck, IPrem, portUDPrem, SeqBytes, LongSeqBytes);
}

int LUMIS_RepDe(int Sck, char *IPrem, int *portUDPrem, char *SeqBytes, int LongSeqBytes) {
    return UDP_RepDe(Sck, IPrem, portUDPrem, SeqBytes, LongSeqBytes);
}

int LUMIS_cercarClient(taulaClients* taulaCli, char * client, int nBytes) {
    int trobat = -1;
    int i = 0;
    char adMI[40];
    if (client[0] == 'R' || client[0] == 'D') {
        client[nBytes - 1] = '\0';
        strcpy(adMI, client + 1);
    } else
        strcpy(adMI, client);


    int bytesAdMi = strlen(adMI);
    while (i < taulaCli->size && trobat != 1) {
        if (strncmp(adMI, taulaCli->taulaCli[i].adMi, bytesAdMi) == 0) {

            trobat = 1;
        } else i++;
    }
    if (trobat == 1) return i;
    else return -1;
}

int LUMIS_Registre(taulaClients *taulaCli, int fitxLog, int index, char *ip, int port) {


    strcpy(taulaCli->taulaCli[index].sck.adIP, ip);
    taulaCli->taulaCli[index].sck.portUDP = port;

    return 1;
}



int LUMIS_ServeixPeticions(int sck, taulaClients *taulaCli, int logFile) {
    char miss[500];
    char resp[500];
    char ipRem[16];
    int portRem;
    int bytes_llegits = LUMIS_RepDe(sck, ipRem, &portRem, miss, sizeof (miss));
    if (bytes_llegits == -1) {
        perror("Error al rebre el missatge: ");
        exit(-1);
    }
    miss[bytes_llegits - 1] = '\0';
    printf("MISSATGE REBUT: %s\n", miss);
    if (miss[0] == 'R') LUMIS_procesRegistre(sck,taulaCli,miss,bytes_llegits,logFile,ipRem,portRem,resp);//Client demana registre

    else if (miss[0] == 'L') LUMIS_ProcesLocalitzacio(sck,taulaCli,miss,bytes_llegits,logFile);//Client demana localitzar un usuari
    
    else if (miss[0] == 'l') LUMIS_ProcesRespLoc(sck,taulaCli,miss,logFile); //Servidor rep resposta de localitzacio
    
    else if (miss[0] == 'D') LUMIS_ProcesDesregistre(sck,taulaCli,miss,bytes_llegits,logFile),ipRem,portRem,resp);
}

/* MÉS FUNCIONS INTERNES */

int LUMIS_ProcesRegistre(int sck,taulaClients *taulaCli, char *miss, char *bytes_llegits, int logFile, char *ipRem, int portRem, char* resp){
	
	int index = LUMIS_cercarClient(taulaCli, miss, bytes_llegits);
        if (index != -1) {//Client trobat a la taula, procedim a fer registre
            int k = LUMIS_Registre(taulaCli, logFile, index, ipRem, portRem);
            strcpy(resp, "C0"); //Tot ha anat be, enviem missatge de confirmacio amb registre OK
            if (k == 1)
                printf("Nom:%s\nIP:%s\nPORT:%d\n", taulaCli->taulaCli[index].adMi, taulaCli->taulaCli[index].sck.adIP, taulaCli->taulaCli[index].sck.portUDP);
            int bytes_enviats = LUMIS_EnviaA(sck, ipRem, *(&portRem), resp, sizeof (resp));
            if (bytes_enviats == -1) {
                perror("Error sending message");
                exit(-1);
            }
        } else {
            //Buscar en un altre domini ?
            //Usuari del nostre domini pero no trobat ?
            //Format d'usuari incorrecte ?
            //Tot lo de dalt es feina del metode cercar
        }
        
        return bytes_enviats;
}

int LUMIS_ProcesLocalitzacio(int sck,taulaClients *taulaCli, char *miss, char *bytes_llegits, int logFile){
	
	char adrMiDest[40]; //El preguntat

        int c = LUMIS_guardaAdr(adrMiDest,miss,0,1,':');
        int index = LUMIS_cercarClient(taulaCli, adrMiDest, bytes_llegits);
        if (index != -1) {//Client trobat a la taula. Li enviem missatge de localitzacio
            char ipDest[16];
            strcpy(ipDest, taulaCli->taulaCli[index].sck.adIP);
            int portDest = taulaCli->taulaCli[index].sck.portUDP;
            LUMIS_EnviaA(sck, ipDest, *(&portDest), miss, bytes_llegits - 1);
        }
        
        return bytes_llegits-1;
}

int LUMIS_ProcesRespLoc(int sck,taulaClients *taulaCli, char *miss, int logFile){
	
	//ex l0duned@PC-b:0.0.0.0:3452
        //primer obtenim l'usuari al que hem de enviar
        char adrMiDest[40];
        int nBytesAdrMi = LUMIS_guardaAdr(adrMiDest,miss,0,2,':');
        int index = LUMIS_cercarClient(taulaCli, adrMiDest, nBytesAdrMi);
        
        //Seguidament obtenim el port
        int i = nBytesAdrMi + 3; //saltem : + les 2 primeres posicions d'abans (a l'inici)
        while (miss[i] != ':') {
            i++; //saltem camp IP
        }
        i++; //saltem :
        char portTCP[10];
        int c = LUMIS_guardaAdr(portTCP,miss,0,i,'\0');
        
        //Construim el missatge
        char respLoc[500];
        int nBytes = nBytesAdrMi + j + strlen(ipRem) + 5;
        snprintf(respLoc, nBytes, "l0%s:%s:%s", adrMiDest, ipRem, portTCP);
        printf("%s\n",respLoc);
        char ipDest[16];
        strcpy(ipDest, taulaCli->taulaCli[index].sck.adIP);
        int portDest = taulaCli->taulaCli[index].sck.portUDP;
        LUMIS_EnviaA(sck, ipDest, *(&portDest), respLoc, nBytes);
        
        return nBytes;
}

int LUMIS_ProcesDesregistre(int sck,taulaClients *taulaCli, char *miss, char *bytes_llegits, int logFile, char *ipRem, int portRem, char *resp){
	
		printf("Desregistre!\n");
        int index = LUMIS_cercarClient(taulaCli, miss, bytes_llegits);
        strcpy(taulaCli->taulaCli[index].sck.adIP,"0.0.0.0");
        taulaCli->taulaCli[index].sck.portUDP = 0;
        strcmp(resp,"C0");
        printf("Nom:%s\nIP:%s\nPORT:%d\n", taulaCli->taulaCli[index].adMi, taulaCli->taulaCli[index].sck.adIP, taulaCli->taulaCli[index].sck.portUDP);
        int bytes_enviats(sck, ipRem, *(&portRem), resp, sizeof (resp)); 
        
        return bytes_enviats;
}

int LUMIS_guardaAdr(char* adr, char*miss, int y, int x, char cmp){
	
	int i = x;
	int j = y;
	while(miss[i] != cmp){
		adr[j] = miss[i];
		j++;
		i++;
	}
	adr[j] = '\0';
	
	return j;
}


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



