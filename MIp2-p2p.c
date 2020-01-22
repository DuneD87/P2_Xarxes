/**************************************************************************/
/*                                                                        */
/* P2 - MI amb sockets TCP/IP - Part II                                   */
/* Fitxer p2p.c que implementa la interfície aplicació-usuari de          */
/* l'aplicació de MI amb l'agent de LUMI integrat, sobre les capes        */
/* d'aplicació de MI i LUMI (fent crides a les interfícies de les capes   */
/* MI -fitxers mi.c i mi.h- i LUMI -lumi.c i lumi.h- ).                   */
/* Autors: Xavier Avivar                                                          */
/*                                                                        */
/**************************************************************************/



#include "MIp2-mi.h"
#include "MIp2-lumiC.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


const char exitAp[2] = "!q";

void mostrarError() {

    MI_MostraError();
    exit(-1);

}

/**
 * @brief Llegeix el nick
 * @param nicLoc Vector de caracaters a ser modificat dins la funcio, tornara amb \0 al final
 * @param LogFile Descriptor del fitxer LOG
 */
void llegirNick(char * nicLoc, int LogFile) {
    printf("Quin es el teu nickname?\n");
    scanf("%s", nicLoc);
    int nBytes = strlen(nicLoc);
    nicLoc[nBytes] = '\0';
    char miss[500];
    sprintf(miss, "Hem llegit el nick '%s'\n", nicLoc);
    if (Log_Escriu(LogFile, miss) == -1) {
        perror("Error en escriure al log");
        exit(-1);
    }
}

/**
 * @brief Mostra informacio sobre la conversa
 * @param ipLoc Vector de caracaters acabat en \0 que ens diu la IP del client local
 * @param ipRem Vector de caracaters acabat en \0 que ens diu la IP del client desti
 * @param portTCPloc Ens diu el port TCP del client local
 * @param portTCPrem Ens diu el port TCP del client desti
 * @param nicLoc Nick del client local
 * @param nicRem Nick del client desti
 */
void mostraInfoConversa(const char * ipLoc, const char * ipRem, int portTCPloc, int portTCPrem, const char * nicLoc, const char * nicRem) {
    printf("Sock LOC: @IP %s,TCP, #port %d\n", ipLoc, portTCPloc);
    printf("Sock REM: @IP %s,TCP,#port %d\n", ipRem, portTCPrem);

    /*Mostrem els nicks dels usuaris conectats                            */
    printf("Nick local:%s\nNick remot:%s\n", nicLoc, nicRem);
}

/**
 * @brief Estableix una conversa entre 2 clients
 * @param ipLoc Vector de caracaters acabat en \0 que ens diu la IP del client local
 * @param ipDesti Vector de caracaters acabat en \0 que ens diu la IP del client desti
 * @param portTCPloc Port TCP del client local
 * @param portTCP Port TCP del client desti
 * @param nicLoc Nick del client local
 * @param nicRem Nick del client desti
 * @param portUdp Port UDP del servidor
 * @param sckUdp Socket UDP per on rebrem/enviem missatges del/al servidor
 * @param scon Socket de conexio entre 2 clients
 * @param LogFile Descriptor del fitxer LOG
 * @param ipRem Vector de caracaters acabat en \0 que ens diu la IP remota del servidor
 * @return Un caracter (S/N) que ens diu si el client vol continuar o finalitzar
 */
char conversa(const char * ipLoc, char * ipDesti, int portTCPloc, int portTCP, const char * nicLoc, const char * nicRem, int * portUdp, int sckUdp, int scon, int LogFile, char * ipRem) {
    Log_Escriu(LogFile, "Inici conversa\n");
    mostraInfoConversa(ipLoc, ipDesti, portTCPloc, portTCP, nicLoc, nicRem);
    int bytesLlegits = LUMI_conversa(scon, sckUdp, ipRem, &(*portUdp), LogFile, exitAp);
    char respObtg = 'S';
    while (bytesLlegits > 0) {
        bytesLlegits = LUMI_conversa(scon, sckUdp, ipRem, &(*portUdp), LogFile, exitAp);
    }
    int aux = MI_AcabaConv(scon);
    if (aux == -1) mostrarError();
    Log_Escriu(LogFile, "Fi conversa\n");
    printf("Vols continuar? S/N\n");

    scanf(" %c", &respObtg);
    return respObtg;
}

/**
 * @brief Demana a la capa LUMI registrarse al seu domini
 * @param adrLumiLoc Adreça LUMI del usuari local
 * @param sckUdp Socket UDP per enviar/rebre missatge a/del servidor
 * @param portUdp Port UDP per el qual el servidor rep missatges (def.2020)
 * @param ipRem Vector de caracaters acabat en \0 que ens diu la IP remota del servidor
 * @param portUdpLoc Port UDP local del client
 * @param LogFile Descricptor del fitxer LOG
 * @param nBytesLoc Nombre de bytes de l'adreça LUMI
 */
void realitzarRegistre(char * adrLumiLoc, int sckUdp, int portUdp, char *ipRem, int portUdpLoc, int LogFile, int nBytesLoc) {
    char missResposta[500];
    int resultat = LUMI_registre(sckUdp, portUdp, missResposta, adrLumiLoc, ipRem, nBytesLoc, portUdpLoc, LogFile);
    while (resultat != 1) {
        if (resultat == -1)
            printf("Servidor inaccessible, torna a introduir l'adreca LUMI o prova més tard.\n");
        else if (resultat == 1)
            printf("No existeix usuari (user / domain name)\n");
        else if (resultat == 2)
            printf("Format incorrecte\n");
        scanf("%s", adrLumiLoc);
        int nBytesLoc = strlen(adrLumiLoc);
        adrLumiLoc[nBytesLoc] = '\0';
        resultat = LUMI_registre(sckUdp, portUdp, missResposta, adrLumiLoc, ipRem, nBytesLoc, portUdpLoc, LogFile);
    }
    printf("Registre Completat de forme correcte per usuari %s.\n", adrLumiLoc);
}

/**
 * @brief Rep/envia missatges del servidor acord amb l'input del usuari
 * @param portTCPloc Ens diu el portTCP local
 * @param ipLoc Vector de characaters acabat en \0 que ens diu l'ip que fara servir el client per establir conexio
 * @param sckTCP Socket d'escolta TCP per detectar si rebem peticions via TCP
 * @param sckUdp Socket UDP per rebre/enviar missatges del/a servidor
 * @param nicLoc Nick local del client
 * @param ipInterface Vector de caracters acabat en \0 que ens diu l'ip que ha escollit l'usuari al iniciar l'aplicacio
 * @param LogFile Descriptor del fitxer log
 * @param adrLumiLoc Adreça LUMI del usuari local
 * @param ipRem Vector de caracaters acabat en \0 que ens diu l'ip remota del servidor / client
 * @param portUdpLoc Port UDP escollit per l'usuari
 */
void parlarAmbServidor(int portTCPloc, char * ipLoc, int sckTCP, int sckUdp, const char * nicLoc, const char * ipInterface, int LogFile, const char *adrLumiLoc, char * ipRem, int portUdpLoc) {
    char missResposta[500], nicRem[303], missLoc[500];
    char ipDesti[16];
    int portTCP, scon, nBytesCreats;
    int portUdp = 2020;
    int nBytesLoc = strlen(adrLumiLoc);
    int enConversa = 0;
    int nIntents = 0;
    int descTemps = 0;
    char respObtg = 'S';
    printf("Introdueix el nom@domini del client amb el que vols contactar o espera a que algu es conecti\n");
    int descActiu = LUMI_haArribatAlgunaCosa(sckUdp, sckTCP);
    while (respObtg == 'S') {
        if (enConversa == 0) {
            if (descActiu == sckUdp) {
                int nBytes = LUMI_repMissatge(sckUdp, ipRem, &portUdpLoc, missResposta, sizeof (missResposta), LogFile);
                missResposta[nBytes] = '\0';
                int codiResposta = LUMI_gestionaMissatge(missResposta, nBytes, sckUdp, sckTCP, ipInterface, ipRem, portUdp, portTCPloc, LogFile);
                if (codiResposta >= 0 && codiResposta < 5) {
                    descTemps = 0;
                    nIntents = 0;
                    if (codiResposta == 0) {
                        scon = LUMI_demanaConv(missResposta, ipDesti, &portTCP, sckTCP, ipLoc, nicLoc, nicRem, &portTCPloc);
                        enConversa = 1;
                    } else if (codiResposta == 1) {
                        printf("Client no existeix!\nIntrodueix el nom@domini del client amb el que vols contactar o espera a que algu es conecti\n");
                    } else if (codiResposta == 2) {
                        printf("Format incorrecte!\nIntrodueix el nom@domini del client amb el que vols contactar o espera a que algu es conecti\n");
                    } else if (codiResposta == 3) {
                        printf("Client Ocupat !\nIntrodueix el nom@domini del client amb el que vols contactar o espera a que algu es conecti\n");
                    } else if (codiResposta == 4) {
                        printf("Client offline!\nIntrodueix el nom@domini del client amb el que vols contactar o espera a que algu es conecti\n");
                    }
                }
            } else if (descActiu == 0 && descTemps == 0) {
                char adrLumiRem[40];
                scanf("%s", adrLumiRem);
                int nBytesRem = strlen(adrLumiRem);
                if (strncmp(exitAp, adrLumiRem, 2) == 0) {
                    respObtg = 'N';
                    descActiu = 2;
                } else {
                    nBytesCreats = LUMI_construirMissatgeLoc(missResposta, adrLumiLoc, nBytesLoc, missLoc, LogFile, adrLumiRem, nBytesRem);
                    if (nBytesCreats != -1) {
                        LUMI_enviaMissatge(sckUdp, ipRem, portUdp, missLoc, nBytesCreats, LogFile);
                        descTemps = 1;
                        nIntents = 0;
                    } else {
                        printf("Host incorrecte!!\nIntrodueix el nom@domini del client amb el que vols contactar o espera a que algu es conecti\n");
                    }
                }

            } else if (descActiu == sckTCP) {
                scon = LUMI_connexio(sckTCP, ipDesti, ipLoc, nicLoc, nicRem, &portTCPloc, &portTCP, 1);
                printf("Peticio de connexio rebuda ! \n");
                enConversa = 1;
            }
        } else if (enConversa == 1) {
            respObtg = conversa(ipLoc, ipDesti, portTCPloc, portTCP, nicLoc, nicRem, &portUdp, sckUdp, scon, LogFile, ipRem);
            if (respObtg == 'S') {
                enConversa = 0;
                printf("Introdueix el nom@domini del client amb el que vols contactar o espera a que algu es conecti\n");
            }
        }
        if (enConversa == 0 && descTemps == 0)
            descActiu = LUMI_haArribatAlgunaCosa(sckUdp, sckTCP);
        else if (enConversa == 0 && descTemps == 1) {
            if (nIntents > 0 && nIntents < 5) {
                printf("Numero d'intents: %d. Reintentan...\n", nIntents + 1);
                LUMI_enviaMissatge(sckUdp, ipRem, portUdp, missLoc, nBytesCreats, LogFile);

            } else if (nIntents > 0) {
                descTemps = 0;
                printf("Introdueix el nom@domini del client amb el que vols contactar o espera a que algu es conecti\n");
            }
        }
        if (nIntents < 6) {
            nIntents++;
            descActiu = LUMI_haArribatAlgunaCosaEnTemps(sckUdp, -1, 2);
        }
    }
    if (respObtg == 'N') {
        printf("Finalitzan sessio\n");
        LUMI_ferDesregistre(adrLumiLoc, ipRem, portUdp, sckUdp, LogFile);
    }
}

int main(int argc, char *argv[]) {

    int portTCPloc, aux, portUdpLoc; //Aux serveix pel control d'errors
    char nicLoc[303], ipLoc[16], ipInterface[16], adrLumiLoc[40], ipRem[16];
    char fitxLog[40] = "Client.log";
    strcpy(ipRem, "0.0.0.0");
    int LogFile;
    MI_seleccionaInterficie(ipInterface);

    /* INICIALITZEM EL LOG FILE */
    LogFile = Log_CreaFitx(fitxLog);

    llegirNick(nicLoc, LogFile);

    int sesc = MI_IniciaEscPetiRemConv(portTCPloc);
    aux = MI_getsockname(sesc, ipLoc, &portTCPloc);
    if (aux == -1) mostrarError();
    printf("Port d'escolta escollit pel SO: %d\n", portTCPloc);

    printf("Introdueix el port udp\n");
    scanf("%d", &portUdpLoc);
    int sckUdp = LUMI_IniciaSckEscolta(ipRem, portUdpLoc);
    printf("Introdueix l'adreca LUMI\n");
    scanf("%s", adrLumiLoc);
    int nBytesLoc = strlen(adrLumiLoc);
    adrLumiLoc[nBytesLoc] = '\0';
    realitzarRegistre(adrLumiLoc, sckUdp, 2020, ipRem, portUdpLoc, LogFile, nBytesLoc);
    parlarAmbServidor(portTCPloc, ipInterface, sesc, sckUdp, nicLoc, ipInterface, LogFile, adrLumiLoc, ipRem, portUdpLoc);
    Log_TancaFitx(LogFile);
    return 0;
}

/* Definició de funcions INTERNES, és a dir, d'aquelles que es faran      */
/* servir només en aquest mateix fitxer. Les seves declaracions es troben */
/* a l'inici d'aquest fitxer.                                             */
