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

int connexio(int sesc, char * ipRem, char * ipLoc, char * nicLoc, char * nicRem, int * portTCPloc, int * portTCPrem) {
    printf("%s", "Introdueix IP del servidor o espera contacte\n");
    int descActiu = MI_HaArribatPetiConv(sesc);
    int scon;
    if (descActiu == 0) {
        /*Ens arriba input del teclat, fem de client                          */
        scon = MI_DemanaConv(ipRem, *portTCPrem, ipLoc, &*portTCPloc, nicLoc, nicRem);

        if (scon == -1) mostrarError();

    } else if (descActiu == sesc) {
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

int conversa(int scon) {
    char buffer[303];
    int bytesLlegits = 0;
    int descActiu = MI_HaArribatLinia(scon);
    int bytesEnviats;
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

void construirMissatgeLocResp(int sckUdp, int sckTCP,const char * miss, const char * ipLoc, int portTcp, const char * ipRem, int portUdp) {
    char adrMiDest[40];
    //Primer obtenim l'adreca LUMI del preguntador de L@Preguntada:@Preguntador
    int i = 1;
    while (miss[i] != ':')
    {
        i++;
    }
    i++;
    int j = 0;
    while (miss[i] != '\0')
    {
        adrMiDest[j] = miss[i];
        i++;
        j++;
    }
    adrMiDest[j] = '\0';
    char missLocResp[500];
    char portTCParray[10];
    int n = sprintf(portTCParray,"%d",portTcp);
    int nBytes = j + n + 12;
    snprintf(missLocResp,nBytes,"l0%s:0.0.0.0:%s",adrMiDest,portTCParray);
    //printf("Missatge a enviar: %s amb %d bytes",missLocResp,n);
    LUMI_enviaMissatge(sckUdp, ipRem, portUdp, missLocResp, nBytes);
}

void construirMissatgeLoc(int sckUdp, const char *ipRem, int portUdp, const char * miss, const char * adrLumiLoc, int nBytesLoc) {
    char adrLumiRem[40];
    printf("Introdueix el nom@domini del client amb el que vols contactar\n");
    int nBytesRem = read(0, adrLumiRem, 40);
    char missLoc[500];
    adrLumiRem[nBytesRem - 1] = '\0';
    int nBytesMissLoc = nBytesLoc + nBytesRem + 2;
    snprintf(missLoc, nBytesMissLoc, "L%s:%s", adrLumiRem, adrLumiLoc);
    printf("Missatge al client: %s\nNombre de bytes: %d\n",missLoc,nBytesRem);
    LUMI_enviaMissatge(sckUdp, ipRem, portUdp, missLoc, nBytesMissLoc - 1);
}

void parlarAmbServidor(char * ipDesti, int * portTCP, int portTCPLoc, const char * ipLoc,int sckTCP) {
    char adrLumiLoc[40], ipRem[16], petRegistre[500], missResposta[500];
    int portUdp = 2020;
    strcpy(ipRem, "0.0.0.0");
    int sckUdp = LUMI_IniciaSckEscolta(ipRem, portUdp);
    printf("Introdueix l'adreca LUMI\n");
    int nBytesLoc = read(0, adrLumiLoc, sizeof ((adrLumiLoc)));
    adrLumiLoc[nBytesLoc - 1] = '\0';
    LUMI_ferRegistre(sckUdp, ipRem, portUdp, adrLumiLoc);
    LUMI_construeixProtocolLUMI(adrLumiLoc, petRegistre);
    LUMI_enviaMissatge(sckUdp, ipRem, portUdp, petRegistre, nBytesLoc + 1);
    int respObtg = 0;
    int step = 0;
    int descActiu = LUMI_haArribatAlgunaCosa(sckUdp);
    while (respObtg == 0) {
        if (descActiu == sckUdp) {
            int nBytes = LUMI_repMissatge(sckUdp, ipRem, portUdp, missResposta, sizeof (petRegistre));
            
            if (step == 0) {
                if (missResposta[0] == 'C') {
                    if (missResposta[1] == '0') {
                        //Hem completat el registre, demanem l'usuari amb el que vol parlar
                        printf("Registre Completat de forme correcte per usuari %s.\n", adrLumiLoc);
                        //Hem finalitzat el registre, ara falta o demanar peticio de conversa, o esperar missatge de localitzacio
                        printf("Selecciona una opcio:\n[0]Localitzar un usuari\n[1]Esperar connexio\n");
                        int opcio;
                        scanf("%d", &opcio);
                        while (opcio != 0 && opcio != 1) {
                            printf("Opcio incorrecte!\n");
                            printf("Selecciona una opcio:\n[0]Localitzar un usuari\n[1]Esperar connexio\n");
                            scanf("%d", &opcio);
                        }
                        if (opcio == 0) step = 1;
                        else if (opcio == 1) step = 2;
                    }
                }
            }
            if (step == 1) {
                if (missResposta[0] != 'l') {//Encara no hem demanat al servidor
                    construirMissatgeLoc(sckUdp,ipRem,portUdp,missResposta,adrLumiLoc,nBytesLoc);
                } else {//Hem obtingut resposta del client..
                    printf("Missatge obtingut: %s\n",missResposta);
                }
            } else if (step == 2) {
                if (missResposta[0] == 'L') {
                    //Ens arriba una peticio de localitzacio, construim missatge de resposta
                    missResposta[nBytes] = '\0';
                    printf("Hola soc la varsha i tinc una peticio de localitzacio, que faig ??\n");
                    construirMissatgeLocResp(sckUdp,sckTCP,missResposta,ipLoc,portTCPLoc,ipRem,portUdp);
                }
            }


        }
        descActiu = LUMI_haArribatAlgunaCosa(sckUdp);
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
    parlarAmbServidor(ipRem, &portTCPrem, portTCPloc,ipLoc,sesc);
    while (continuar == 'S') {
        int scon = connexio(sesc, ipRem, ipLoc, nicLoc, nicRem, &portTCPloc, &portTCPrem);
        if (scon != -1) {
            /*Ens hem conectat, mostrem les adreces i ports del remot i el local  */

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
    }
    return 0;
}

/* Definició de funcions INTERNES, és a dir, d'aquelles que es faran      */
/* servir només en aquest mateix fitxer. Les seves declaracions es troben */
/* a l'inici d'aquest fitxer.                                             */
