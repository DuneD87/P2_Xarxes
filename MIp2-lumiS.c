/**************************************************************************/
/*                                                                        */
/* P2 - MI amb sockets TCP/IP - Part II                                   */
/* Fitxer lumiC.c que implementa la capa d'aplicació de MI, sobre la capa */
/* de transport UDP (fent crides a la "nova" interfície de la capa UDP o  */
/* "nova" interfície de sockets), però només la part servidora            */
/* Autors: Xavier Avivar                                                  */
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
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>


#define MAX 500
const char fitxNodeLumi[50] = "nodelumi.cfg";


int Log_CreaFitx(const char *NomFitxLog) {

    return open(NomFitxLog, O_CREAT | O_RDWR | O_APPEND, S_IRUSR | S_IWUSR | S_IROTH | S_IWOTH);
}

int Log_Escriu(int FitxLog, const char *MissLog) {

    return write(FitxLog, MissLog, strlen(MissLog));
}

int Log_TancaFitx(int FitxLog) {

    int c = close(FitxLog);
    if (c == 0) c++;
    return c;
}

int LUMIS_obtenirHost(const char* adrLumi, char* host) {
    int x = 0;
    int y = 0;
    while (adrLumi[x] != '@')
        x++; //Saltem el nom
    x++; //saltem @
    while (adrLumi[x] != '\0') {
        host[y] = adrLumi[x];
        x++;
        y++;
    }
    host[y] = '\0';
    return y;
}

int LUMIS_guardaAdr(char* adr, char*miss, int y, int x, char cmp) {

    int i = x;
    int j = y;
    while (miss[i] != cmp) {
        adr[j] = miss[i];
        j++;
        i++;
    }
    adr[j] = '\0';

    return j;
}

int LUMIS_Finalitza(int sckLoc) {
    UDP_TancaSock(sckLoc);
}

int LUMIS_IniciaSockEsc(const char* iploc, int portUDP) {
    int sck = UDP_CreaSock(iploc, portUDP);
    return sck;
}

int LUMIS_emplenaTaula(taulaClients *taulaCli, char *host) {
    taulaCli->size = 0;
    FILE *fpt;
    fpt = fopen(fitxNodeLumi, "r");
    if (fpt == NULL) {
        return -1;
    }
    while (fgets(taulaCli->taulaCli[taulaCli->size].adMi, 40, fpt) != NULL) {
        strcpy(taulaCli->taulaCli[taulaCli->size].sck.adIP, "0.0.0.0");
        taulaCli->taulaCli[taulaCli->size].sck.portUDP = 0;
        char * aux = strtok(taulaCli->taulaCli[taulaCli->size].adMi,"\n");
        if (aux != NULL)
            strcpy(taulaCli->taulaCli[taulaCli->size].adMi,aux);
        taulaCli->size++;
        
           
    }
    fclose(fpt);
    if (taulaCli->size > 0) {
        int nBytes = LUMIS_obtenirHost(taulaCli->taulaCli[0].adMi, host);
    }
    //printf("El nostre host es: %s\n", host);
    strcpy(taulaCli->domini, host);
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
        //printf("admi:%s\n", adMI);
    } else
        strcpy(adMI, client);


    int bytesAdMi = strlen(adMI);
    if (bytesAdMi > 1) {
        while (i < taulaCli->size && trobat != 1) {

            if (strncmp(adMI, taulaCli->taulaCli[i].adMi, bytesAdMi) == 0) {

                trobat = 1;
            } else
                i++;
        }
    }
    if (trobat == 1)
        return i;
    else
        return -1;
}

int LUMIS_Registre(taulaClients *taulaCli, int index, char *ip, int port) {


    strcpy(taulaCli->taulaCli[index].sck.adIP, ip);
    taulaCli->taulaCli[index].sck.portUDP = port;

    return 1;
}

int LUMIS_procesRegistre(int sck, taulaClients *taulaCli, char *miss, int bytes_llegits, int logFile, char *ipRem, int portRem, char* resp) {

    int index = LUMIS_cercarClient(taulaCli, miss, bytes_llegits);
    int bytesEnviats = -1;
    char miss2[500];
    //printf("Procedint a fer registre\n");
    if (index != -1) {//Client trobat a la taula, procedim a fer registre
        int k = LUMIS_Registre(taulaCli, index, ipRem, portRem);
        strcpy(resp, "C0"); //Tot ha anat be, enviem missatge de confirmacio amb registre OK
        sprintf(miss2, "Registre completat de '%s' amb @IP: %s i #port: %d\n", taulaCli->taulaCli[index].adMi, taulaCli->taulaCli[index].sck.adIP, taulaCli->taulaCli[index].sck.portUDP);
        Log_Escriu(logFile, miss2);
        
    } else {
        //Format d'usuari incorrecte ?
        //Tot lo de dalt es feina del metode cercar
        char host[40];
        int nBytes = LUMIS_obtenirHost(miss + 1, host);
        sprintf(miss2, "Error amb registre de '%s' amb @IP: %s i #port: %d: USUARI NO TROBAT\n", taulaCli->taulaCli[index].adMi, taulaCli->taulaCli[index].sck.adIP, taulaCli->taulaCli[index].sck.portUDP);
        Log_Escriu(logFile, miss2);
        if (strncmp(host, taulaCli->domini, nBytes) == 0) {
            //Enviem missatge de que usuari no trobat
            snprintf(resp, 3, "C1");
        } else {
            snprintf(resp, 3, "C2");
            ; //Domini incorrecte / format incorrecte
            sprintf(miss2, "Error amb registre de '%s' amb @IP: %s i #port: %d: FORMAT INCORRECTE\n", taulaCli->taulaCli[index].adMi, taulaCli->taulaCli[index].sck.adIP, taulaCli->taulaCli[index].sck.portUDP);
            Log_Escriu(logFile, miss2);
        }
    }
    //printf("Envian missatge de resposta: %s\n", resp);
    bytesEnviats = LUMIS_EnviaA(sck, ipRem, *(&portRem), resp, strlen(resp));
    return bytesEnviats;
}

int LUMIS_procesLocalitzacio(int sck, taulaClients *taulaCli, char *miss, int bytes_llegits, int logFile) {
    //MISSATGES S'envien amb informacio incorrecte, revisar !!!! 
    char adrLumiDest[40]; //El preguntat
    char adrLumiOrig[40]; //El preguntador
    LUMIS_guardaAdr(adrLumiDest, miss, 0, 1, ':');
    LUMIS_guardaAdr(adrLumiOrig, miss, 0, strlen(adrLumiDest) + 2, '\0');
    char ipDest[16];
    int index = LUMIS_cercarClient(taulaCli, adrLumiDest, bytes_llegits);
    int indexOrig = LUMIS_cercarClient(taulaCli, adrLumiOrig, strlen(adrLumiOrig));
    int portDest;
    int nBytes;
    //primer mirem si el host de l'adreça desti pertany al nostre domini
    char hostDest[40];
    int nBytesDest = LUMIS_obtenirHost(adrLumiDest, hostDest);
    int cmpDest = strncmp(hostDest, taulaCli->domini, nBytesDest);
    if (cmpDest == 0) { //Som del mateix domini, yay !!
        if (index != -1) { //Client existeix ?
            portDest = taulaCli->taulaCli[index].sck.portUDP;
            if (portDest != 0) { //Client online !
                strcpy(ipDest, taulaCli->taulaCli[index].sck.adIP);
            } else { //Client offline !
                //Enviem resposta de l4 al origen
                //Comprovem si l'origen es del nostre domini!
                char hostOrig[40];
                int nBytesOrig = LUMIS_obtenirHost(adrLumiOrig, hostOrig);
                int cmpOrig = strncmp(hostOrig, taulaCli->domini, nBytesOrig);
                if (cmpOrig == 0) { // Usuari es del nostre domini, reenviem !
                    strcpy(ipDest, taulaCli->taulaCli[indexOrig].sck.adIP);
                    portDest = taulaCli->taulaCli[indexOrig].sck.portUDP;
                } else {
                    //Usuari es d'un domini diferent, resolem !
                    DNSc_ResolDNSaIP(hostOrig, ipDest);
                    portDest = 2020;
                }
                sprintf(miss, "l4%s", adrLumiOrig);
            }
        } else { //Client no existeix!
            //Enviem resposta de l1 al origen
            if (indexOrig != -1) {
                strcpy(ipDest, taulaCli->taulaCli[indexOrig].sck.adIP);
                portDest = taulaCli->taulaCli[indexOrig].sck.portUDP;
            } else {
                char hostOrig[40];
                int nBytesOrig = LUMIS_obtenirHost(adrLumiOrig, hostOrig);
                DNSc_ResolDNSaIP(hostOrig, ipDest);
                portDest = 2020;
                //printf("Host origen: %s\n", hostOrig);
            }
            sprintf(miss, "l1%s", adrLumiOrig);
        }
    } else { //Client es d'un domini diferent!
        //Resolem i reenviem missatge al domini !
        DNSc_ResolDNSaIP(hostDest, ipDest);
        portDest = 2020;
    }
    nBytes = strlen(miss);
    //printf("Envian missatge de resposta: %s a %s:%d\n ", miss, ipDest, portDest);
    char miss2[500];
    sprintf(miss2, "Enviem resposta '%s' a @IP: %s i #port: %d\n", miss, ipDest, portDest);
    Log_Escriu(logFile, miss2);
    LUMIS_EnviaA(sck, ipDest, portDest, miss, nBytes);
    return bytes_llegits - 1;
}

int LUMIS_procesRespLoc(int sck, taulaClients *taulaCli, char *miss, int logFile, const char * ipRem) {
    int nBytes, portDest;
    char respLoc[500], ipDest[16];
    //ex l0duned@PC-b:0.0.0.0:3452
    //primer obtenim l'usuari al que hem de enviar
    if (miss[1] == '0') {
        char adrMiDest[40];
        int nBytesAdrMi = LUMIS_guardaAdr(adrMiDest, miss, 0, 2, ':');
        int index = LUMIS_cercarClient(taulaCli, adrMiDest, nBytesAdrMi);
        char host[40];
        int nBytesHost = LUMIS_obtenirHost(adrMiDest, host);

        //Construim el missatge

        nBytes = strlen(miss) + 2;

        if (strncmp(host, taulaCli->domini, nBytesHost) == 0) {
            sprintf(respLoc, "%s", miss);
            strcpy(ipDest, taulaCli->taulaCli[index].sck.adIP);
            portDest = taulaCli->taulaCli[index].sck.portUDP;
        } else {
            sprintf(respLoc, "%s", miss);
            DNSc_ResolDNSaIP(host, ipDest);
            portDest = 2020;
        }


    } else if (miss[1] == '3' || miss[1] == '4' || miss[1] == '1') {
        //printf("Usuari offline o ocupat\n");
        //primer obtenim l'usuari al que hem de enviar

        char adrMiDest[40];
        int i = 2; // ens situem a la segona posicio
        int nBytesAdrMi = 0;
        while (miss[i] != '\0') {
            adrMiDest[nBytesAdrMi] = miss[i];
            i++;
            nBytesAdrMi++;
        }
        adrMiDest[nBytesAdrMi] = '\0';
        char host[40];
        int nBytesHost = LUMIS_obtenirHost(adrMiDest, host);
        int index = LUMIS_cercarClient(taulaCli, adrMiDest, nBytesAdrMi);

        if (strncmp(host, taulaCli->domini, nBytesHost) == 0) {
            strcpy(ipDest, taulaCli->taulaCli[index].sck.adIP);
            portDest = taulaCli->taulaCli[index].sck.portUDP;
        } else {
            DNSc_ResolDNSaIP(host, ipDest);
            portDest = 2020;
        }

        if (miss[1] == '3')
            sprintf(respLoc, "l3%s", adrMiDest);
        else if (miss[1] == '4')
            sprintf(respLoc, "l4%s", adrMiDest);
        else if (miss[1] == '1')
            sprintf(respLoc, "l1%s", adrMiDest);
    }
    nBytes = strlen(respLoc);
    //printf("Missatge enviat: %s \t a : %s\n", respLoc, ipDest);
    char miss2[500];
    sprintf(miss2, "Enviem missatge de Resposta de Loc '%s' a @IP: %s i #port: %d\n", respLoc, ipDest, portDest);
    Log_Escriu(logFile, miss2);
    LUMIS_EnviaA(sck, ipDest, portDest, respLoc, nBytes);
    return nBytes;
}

int LUMIS_procesDesregistre(int sck, taulaClients *taulaCli, char *miss, int bytes_llegits, int logFile, char *ipRem, int portRem, char *resp) {


    int index = LUMIS_cercarClient(taulaCli, miss, bytes_llegits);
    //printf("Procedin al desregistre de l'usuari: %s\n", taulaCli->taulaCli[index].adMi);
    strcpy(taulaCli->taulaCli[index].sck.adIP, "0.0.0.0");
    taulaCli->taulaCli[index].sck.portUDP = 0;
    strcmp(resp, "C0");
    char miss2[500];
    sprintf(miss2, "Desregistre d'usuari '%s'\n", taulaCli->taulaCli[index].adMi);
    Log_Escriu(logFile, miss2);
    //printf("Nom:%s\nIP:%s\nPORT:%d\n", taulaCli->taulaCli[index].adMi, taulaCli->taulaCli[index].sck.adIP, taulaCli->taulaCli[index].sck.portUDP);


    return 1;
}

int LUMIS_ServeixPeticions(int sck, taulaClients *taulaCli, int logFile) {
    char miss[500];
    char resp[500];
    char ipRem[16];
    char miss2[500];
    int portRem;
    int pet = 0;
    int bytes_llegits = LUMIS_RepDe(sck, ipRem, &portRem, miss, sizeof (miss));
    if (bytes_llegits == -1) {
        perror("Error al rebre el missatge: ");
        exit(-1);
    }
    miss[bytes_llegits] = '\0';
    //printf("MISSATGE REBUT: %s\n", miss);
    if (miss[0] == 'R') { //Client demana registre
        sprintf(miss2, "Missatge de REGISTRE rebut de @IP: %s i #port: %d\n", ipRem, portRem);
        Log_Escriu(logFile, miss2);
        LUMIS_procesRegistre(sck, taulaCli, miss, bytes_llegits, logFile, ipRem, portRem, resp);
        pet = 1;

    } else if (miss[0] == 'L') { //Client demana localitzar un usuari
        sprintf(miss2, "Missatge de LOCALITZACIO rebut de @IP: %s i #port: %d\n", ipRem, portRem);
        Log_Escriu(logFile, miss2);
        LUMIS_procesLocalitzacio(sck, taulaCli, miss, bytes_llegits, logFile);
        pet = 2;

    } else if (miss[0] == 'l') { //Servidor rep resposta de localitzacio
        sprintf(miss2, "Missatge de RespLOC rebut de @IP: %s i #port: %d\n", ipRem, portRem);
        Log_Escriu(logFile, miss2);
        LUMIS_procesRespLoc(sck, taulaCli, miss, logFile, ipRem); //Servidor rep resposta de localitzacio
        pet = 3;

    } else if (miss[0] == 'D') {
        sprintf(miss2, "Missatge de DESREGISTRE rebut de @IP: %s i #port: %d\n", ipRem, portRem);
        Log_Escriu(logFile, miss2);
        LUMIS_procesDesregistre(sck, taulaCli, miss, bytes_llegits, logFile, ipRem, portRem, resp);
        pet = 4;
    }
    
    return pet;
}

/* MÉS FUNCIONS INTERNES */


int LUMIS_HaArribatAlgunaCosa(int sckEsc) {
    int conjunt[1];
    conjunt[0] = sckEsc;
    int descActiu = T_HaArribatAlgunaCosa(conjunt, sckEsc);

    return descActiu;
}

/* Si ho creieu convenient, feu altres funcions INTERNES                  */



