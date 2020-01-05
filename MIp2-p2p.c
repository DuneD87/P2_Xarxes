/**************************************************************************/
/*                                                                        */
/* P2 - MI amb sockets TCP/IP - Part II                                   */
/* Fitxer p2p.c que implementa la interfície aplicació-usuari de          */
/* l'aplicació de MI amb l'agent de LUMI integrat, sobre les capes        */
/* d'aplicació de MI i LUMI (fent crides a les interfícies de les capes   */
/* MI -fitxers mi.c i mi.h- i LUMI -lumi.c i lumi.h- ).                   */
/* Autors: X, Y                                                           */
/*                                                                        */
/**************************************************************************/

/* Inclusió de llibreries, p.e. #include <stdio.h> o #include "meu.h"     */

#include "MIp2-mi.h"
#include "MIp2-lumiC.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
/* Definició de constants, p.e., #define XYZ       1500                   */

/* Declaració de funcions INTERNES que es fan servir en aquest fitxer     */
/* (les  definicions d'aquestes funcions es troben més avall) per així    */
/* fer-les conegudes des d'aquí fins al final d'aquest fitxer, p.e.,      */

/* int FuncioInterna(arg1, arg2...);                                      */

const char exitAp[2] = "!q";

void mostrarError() {

    MI_MostraError();
    exit(-1);

}

void llegirNick(char * nicLoc) {
    printf("Quin es el teu nickname?\n");
    int nBytes = read(0, nicLoc, 300);
    nicLoc[nBytes] = '\0';

}

int connexio(int sesc, char * ipRem, char * ipLoc, const char * nicLoc, char * nicRem, int * portTCPloc, int * portTCPrem, int tipus) {
    
    //int descActiu = MI_HaArribatPetiConv(sesc);
    int scon;
    if (tipus == 0) {
        /*Ens arriba input del teclat, fem de client                          */
        scon = MI_DemanaConv(ipRem, *portTCPrem, ipLoc, &*portTCPloc, nicLoc, nicRem);

        if (scon == -1) mostrarError();

    } else if (tipus == 1) {
        /*Ens arriba input per el socket de escolta, femd e servidor*/
        scon = MI_AcceptaConv(sesc, ipRem, &*portTCPrem, ipLoc, &*portTCPloc, nicLoc, nicRem);
        if (scon == -1) mostrarError();
    }
    return scon;
}

void mostraInfoConversa(const char * ipLoc, const char * ipRem, int portTCPloc, int portTCPrem, const char * nicLoc, const char * nicRem) {
    printf("Sock LOC: @IP %s,TCP, #port %d\n", ipLoc, portTCPloc);
    printf("Sock REM: @IP %s,TCP,#port %d\n", ipRem, portTCPrem);

    /*Mostrem els nicks dels usuaris conectats                            */
    printf("Nick local:%s\nNick remot:%s\n", nicLoc, nicRem);
}

int conversa(int scon, int sckUdp) {
    char buffer[303];
    int bytesLlegits = 0;
    int bytesEnviats;
    int descActiu = LUMI_haArribatAlgunaCosa(sckUdp,scon);
    if (descActiu == 0) {

        bytesLlegits = read(0, buffer, sizeof (buffer));
        buffer[bytesLlegits] = '\0';
        bytesEnviats = MI_EnviaLinia(scon, buffer);
        if (bytesEnviats == -1) mostrarError();

    } else if (descActiu == scon) {
        //Ens arriba informacio, la guardem i mostrem per pantalla
        bytesLlegits = MI_RepLinia(scon, buffer);
        if (bytesLlegits == -1) mostrarError();
        buffer[bytesLlegits] = '\n';
        buffer[bytesLlegits + 1] = '\0';
        write(1, buffer, bytesLlegits + 1);
    }
    if (strncmp(buffer, exitAp, 2) == 0)
        bytesLlegits = 0;
    return bytesLlegits;
}

void construirMissatgeLocResp(int sckUdp, int sckTCP, const char * miss, const char * ipLoc, int portTcp, const char * ipRem, int portUdp) {
    char adrMiDest[40];
    //Primer obtenim l'adreca LUMI del preguntador de L@Preguntada:@Preguntador
    int i = 1;
    while (miss[i] != ':') {
        i++;
    }
    i++;
    int j = 0;
    while (miss[i] != '\0') {
        adrMiDest[j] = miss[i];
        i++;
        j++;
    }
    adrMiDest[j] = '\0';
    char missLocResp[500];
    char portTCParray[10];
    int n = sprintf(portTCParray, "%d", portTcp);
    int nBytes = j + n + 12;
    snprintf(missLocResp, nBytes, "l0%s:0.0.0.0:%s", adrMiDest, portTCParray);
    //printf("Missatge a enviar: %s amb %d bytes",missLocResp,n);
    LUMI_enviaMissatge(sckUdp, ipRem, portUdp, missLocResp, nBytes);
}

void construirMissatgeLoc(int sckUdp, const char *ipRem, int portUdp, const char * miss, const char * adrLumiLoc, int nBytesLoc) {
    char adrLumiRem[40];

    int nBytesRem = read(0, adrLumiRem, 40);
    char missLoc[500];
    adrLumiRem[nBytesRem - 1] = '\0';
    int nBytesMissLoc = nBytesLoc + nBytesRem + 2;
    snprintf(missLoc, nBytesMissLoc, "L%s:%s", adrLumiRem, adrLumiLoc);
    printf("Missatge al client: %s\nNombre de bytes: %d\n", missLoc, nBytesRem);
    LUMI_enviaMissatge(sckUdp, ipRem, portUdp, missLoc, nBytesMissLoc - 1);
}

int ferRegistre(int sckUdp, int portUdp, char * miss, const char * adrLumiLoc, char * ipRem, int nBytesLoc) {
    char petRegistre[500];
    LUMI_ferRegistre(sckUdp, ipRem, portUdp, adrLumiLoc);
    LUMI_construeixProtocolLUMI(adrLumiLoc, petRegistre);
    LUMI_enviaMissatge(sckUdp, ipRem, portUdp, petRegistre, nBytesLoc + 1);
    int descActiu = LUMI_haArribatAlgunaCosaEnTemps(sckUdp, -1, 5);
    int resultat = 0;
    int nIntents = 0;
    while (resultat == 0) {
        if (descActiu == sckUdp) {
            int nBytes = LUMI_repMissatge(sckUdp, ipRem, portUdp, miss, sizeof (miss));
            if (miss[0] == 'C' && miss[1] == '0') {
                printf("Registre Completat de forme correcte per usuari %s.\n", adrLumiLoc);
                resultat = 1;
            }
        } else {
            if (nIntents < 5) {
                printf("Numero d'intents: %d. Reintentan...\n", nIntents);
                nIntents++;
                LUMI_enviaMissatge(sckUdp, ipRem, portUdp, petRegistre, nBytesLoc + 1);
                descActiu = LUMI_haArribatAlgunaCosaEnTemps(sckUdp, -1, 5);
            } else {
                resultat = -1;
            }
        }

    }
    return resultat;
}

void extreureIpPort(const char * miss, char * ipDesti, int * portTcp) {
    //ex l0duned@PC-b:100.10.10.103:45232
    int i = 0;
    while (miss[i] != ':') {
        i++; //saltem fins trobar IP
    }
    i++; //saltem :
    int j = 0;
    while (miss[i] != ':') {
        ipDesti[j] = miss[i];
        i++;
        j++;
    }
    ipDesti[j] = '\0';
    i++; //saltem :
    int k = 0;
    while (miss[i] != '\0') {
        k = ((miss[i]) - '0') + k * 10;
        i++;
    }
    *(portTcp) = k;
}

void parlarAmbServidor(int portTCPloc, char * ipLoc, int sckTCP, const char * nicLoc) {
    char adrLumiLoc[40], ipRem[16], missResposta[500], nicRem[303];
    char ipDesti[16];
    int portTCP;
    int portUdp = 2020;
    int enConversa = 0;
    int scon, bytesLlegits,aux;
    strcpy(ipRem, "0.0.0.0");
    int sckUdp = LUMI_IniciaSckEscolta(ipRem, portUdp);
    printf("Introdueix l'adreca LUMI\n");
    int nBytesLoc = read(0, adrLumiLoc, sizeof ((adrLumiLoc)));
    adrLumiLoc[nBytesLoc - 1] = '\0';
    int resultat = ferRegistre(sckUdp, portUdp, missResposta, adrLumiLoc, ipRem, nBytesLoc);
    while (resultat != 1) {
        printf("Introdueix l'adreca LUMI\n");
        int nBytesLoc = read(0, adrLumiLoc, sizeof ((adrLumiLoc)));
        adrLumiLoc[nBytesLoc - 1] = '\0';
        resultat = ferRegistre(sckUdp, portUdp, missResposta, adrLumiLoc, ipRem, nBytesLoc);
    }
    int respObtg = 0;
    int step = 0;
    printf("Introdueix el nom@domini del client amb el que vols contactar o espera a que algu es conecti\n");
    int descActiu = LUMI_haArribatAlgunaCosa(sckUdp, sckTCP);
    while (respObtg == 0) {
        if (enConversa == 0) {
            if (descActiu == sckUdp) {
                //Ens arriba un missatge per UDP, com actuem ??
                //Potser que sigui un missatge de localitzacio L, haurem de construir un missatge l
                //Potser que sigui un missatge de resposta de localitzacio l, si es correcte, procedirem a fer la conexio tcp
                int nBytes = LUMI_repMissatge(sckUdp, ipRem, &portUdp, missResposta, sizeof (missResposta));
                if (missResposta[0] == 'L') {
                    //Ens arriba una peticio de localitzacio, construim missatge de resposta
                    missResposta[nBytes] = '\0';
                    construirMissatgeLocResp(sckUdp, sckTCP, missResposta, ipLoc, portTCPloc, ipRem, portUdp);
                } else if (missResposta[0] == 'l') {
                    //Extreim adreça i port del missatge i fem conexio TCP
                    missResposta[nBytes] = '\0';
                    extreureIpPort(missResposta, ipDesti, &portTCP);
                    //Fem de servidor i li demanem conversa al client.. MIRAR IPLOC I IPREM, ESTEM ESCCRIBIN EN MEMORIA PROTEGIDA! JO DEL FUTUR
                    scon = connexio(sckTCP, ipDesti, ipLoc, nicLoc, nicRem, &portTCPloc, &portTCP, 0);
                    enConversa = 1;
                }

            } else if (descActiu == 0) {
                construirMissatgeLoc(sckUdp, ipRem, portUdp, missResposta, adrLumiLoc, nBytesLoc);

            } else if (descActiu == sckTCP) {
                //fem de client, ens arriba peticio de connexio de un servidor
                scon = connexio(sckTCP, ipDesti, ipLoc, nicLoc, nicRem, &portTCPloc, &portTCP, 1);
                printf("Peticio de connexio rebuda ! \n");
                enConversa = 1;
            }
            
        } else if (enConversa == 1) {
            mostraInfoConversa(ipLoc, ipDesti, portTCPloc, portTCP, nicLoc, nicRem);
            bytesLlegits = conversa(scon,sckUdp);
            while (bytesLlegits > 0) {
                bytesLlegits = conversa(scon,sckUdp);
            }
            aux = MI_AcabaConv(scon);
            if (aux == -1) mostrarError();
        } 
        if (enConversa == 0)
            descActiu = LUMI_haArribatAlgunaCosa(sckUdp, sckTCP);
    }
}

int main(int argc, char *argv[]) {
    /* Declaració de variables, p.e., int n;                                 */

    int bytesLlegits, bytesEscrits, descActiu, portTCPloc, portTCPrem, aux; //Aux serveix pel control d'errors
    int nVoltes = 0;
    char nicLoc[303], nicRem[303], ipRem[16], ipLoc[16];
    char continuar = 'S';
    MI_seleccionaInterficie(ipLoc);
    
    /* Expressions, estructures de control, crides a funcions, etc.          */
    /* LLegim el nom, i port d'escolta i esperem a que l'usuari decideixi    */
    llegirNick(nicLoc);

    int sesc = MI_IniciaEscPetiRemConv(portTCPloc);
    aux = MI_getsockname(sesc, ipLoc, &portTCPloc);
    if (aux == -1) mostrarError();
    printf("Port d'escolta escollit pel SO: %d\n", portTCPloc);
    parlarAmbServidor(portTCPloc, ipLoc, sesc,nicLoc);
    /*while (continuar == 'S') {
        int scon = connexio(sesc, ipRem, ipLoc, nicLoc, nicRem, &portTCPloc, &portTCPrem);
        if (scon != -1) {
           

            mostraInfoConversa(ipLoc, ipRem, portTCPloc, portTCPrem, nicLoc, nicRem);
            bytesLlegits = conversa(scon);
            while (bytesLlegits > 0) {
                bytesLlegits = conversa(scon);
            }
            aux = MI_AcabaConv(scon);
            if (aux == -1) mostrarError();

        } else mostrarError();

        printf("Desitja continuar ? S/N\n");
        scanf(" %c", &continuar);
    }*/
    return 0;
}

/* Definició de funcions INTERNES, és a dir, d'aquelles que es faran      */
/* servir només en aquest mateix fitxer. Les seves declaracions es troben */
/* a l'inici d'aquest fitxer.                                             */
