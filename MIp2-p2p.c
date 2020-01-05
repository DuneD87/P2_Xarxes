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
    int descActiu = LUMI_haArribatAlgunaCosa(sckUdp, scon);
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


void parlarAmbServidor(int portTCPloc, char * ipLoc, int sckTCP, const char * nicLoc) {
    char adrLumiLoc[40], ipRem[16], missResposta[500], nicRem[303];
    char ipDesti[16];
    int portTCP;
    int portUdp = 2020;
    int enConversa = 0;
    int scon, bytesLlegits, aux;
    strcpy(ipRem, "0.0.0.0");
    int sckUdp = LUMI_IniciaSckEscolta(ipRem, portUdp);
    printf("Introdueix l'adreca LUMI\n");
    int nBytesLoc = read(0, adrLumiLoc, sizeof ((adrLumiLoc)));
    adrLumiLoc[nBytesLoc - 1] = '\0';
    int resultat = LUMI_registre(sckUdp, portUdp, missResposta, adrLumiLoc, ipRem, nBytesLoc);
    while (resultat != 1) {
        printf("Introdueix l'adreca LUMI\n");
        int nBytesLoc = read(0, adrLumiLoc, sizeof ((adrLumiLoc)));
        adrLumiLoc[nBytesLoc - 1] = '\0';
        resultat = LUMI_registre(sckUdp, portUdp, missResposta, adrLumiLoc, ipRem, nBytesLoc);
    }
    char respObtg = 'S';
    printf("Introdueix el nom@domini del client amb el que vols contactar o espera a que algu es conecti\n");
    int descActiu = LUMI_haArribatAlgunaCosa(sckUdp, sckTCP);
    while (respObtg == 'S') {
        if (enConversa == 0) {
            if (descActiu == sckUdp) {
                //Ens arriba un missatge per UDP, com actuem ??
                //Potser que sigui un missatge de localitzacio L, haurem de construir un missatge l
                //Potser que sigui un missatge de resposta de localitzacio l, si es correcte, procedirem a fer la conexio tcp
                int nBytes = LUMI_repMissatge(sckUdp, ipRem, &portUdp, missResposta, sizeof (missResposta));
                if (missResposta[0] == 'L') {
                    //Ens arriba una peticio de localitzacio, construim missatge de resposta
                    missResposta[nBytes] = '\0';
                    int k = LUMI_construirMissatgeLocResp(sckUdp, sckTCP, missResposta, ipLoc, portTCPloc, ipRem, portUdp);
                } else if (missResposta[0] == 'l') {
                    //Extreim adreça i port del missatge i fem conexio TCP
                    missResposta[nBytes] = '\0';
                    LUMI_extreureIpPort(missResposta, ipDesti, &portTCP);
                    //Fem de servidor i li demanem conversa al client.. MIRAR IPLOC I IPREM, ESTEM ESCCRIBIN EN MEMORIA PROTEGIDA! JO DEL FUTUR
                    scon = connexio(sckTCP, ipDesti, ipLoc, nicLoc, nicRem, &portTCPloc, &portTCP, 0);
                    enConversa = 1;
                }

            } else if (descActiu == 0) {
                aux = LUMI_construirMissatgeLoc(sckUdp, ipRem, portUdp, missResposta, adrLumiLoc, nBytesLoc);

            } else if (descActiu == sckTCP) {
                //fem de client, ens arriba peticio de connexio de un servidor
                scon = connexio(sckTCP, ipDesti, ipLoc, nicLoc, nicRem, &portTCPloc, &portTCP, 1);
                printf("Peticio de connexio rebuda ! \n");
                enConversa = 1;
            }

        } else if (enConversa == 1) {
            mostraInfoConversa(ipLoc, ipDesti, portTCPloc, portTCP, nicLoc, nicRem);
            bytesLlegits = conversa(scon, sckUdp);
            respObtg = 1;
            while (bytesLlegits > 0) {
                bytesLlegits = conversa(scon, sckUdp);
            }
            aux = MI_AcabaConv(scon);
            if (aux == -1) mostrarError();
            printf("Vols continuar? S/N\n");
            
            scanf(" %c",&respObtg);
            
            if (respObtg == 'S') {
                enConversa = 0;
                printf("Introdueix el nom@domini del client amb el que vols contactar o espera a que algu es conecti\n");
            }
        }
        if (enConversa == 0)
            descActiu = LUMI_haArribatAlgunaCosa(sckUdp, sckTCP);
    }
    if (respObtg == 'N') {
        printf("Finalitzan sessio\n");
        LUMI_ferDesregistre(adrLumiLoc,ipRem,portUdp,sckUdp);
    }
}

int main(int argc, char *argv[]) {
    /* Declaració de variables, p.e., int n;                                 */

    int  portTCPloc,aux; //Aux serveix pel control d'errors
    char nicLoc[303],ipLoc[16];
    MI_seleccionaInterficie(ipLoc);

    /* Expressions, estructures de control, crides a funcions, etc.          */
    /* LLegim el nom, i port d'escolta i esperem a que l'usuari decideixi    */
    llegirNick(nicLoc);

    int sesc = MI_IniciaEscPetiRemConv(portTCPloc);
    aux = MI_getsockname(sesc, ipLoc, &portTCPloc);
    if (aux == -1) mostrarError();
    printf("Port d'escolta escollit pel SO: %d\n", portTCPloc);
    parlarAmbServidor(portTCPloc, ipLoc, sesc, nicLoc);
    return 0;
}

/* Definició de funcions INTERNES, és a dir, d'aquelles que es faran      */
/* servir només en aquest mateix fitxer. Les seves declaracions es troben */
/* a l'inici d'aquest fitxer.                                             */
