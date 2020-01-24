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
#include "MIp2-mi.h"
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

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

void LUMI_extreureIpPort(const char* miss, char* ipDesti, int* portTcp) {
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

int LUMI_obtenirHost(const char* adrLumi, char* host) {
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

int LUMI_connexio(int sesc, char* ipRem, char* ipLoc, const char* nicLoc, char* nicRem, int* portTCPloc, int* portTCPrem, int tipus) {
    int scon;
    if (tipus == 0) {
        /*Ens arriba input del teclat, fem de client                          */
        scon = MI_DemanaConv(ipRem, *portTCPrem, ipLoc, &*portTCPloc, nicLoc, nicRem);
    } else if (tipus == 1) {
        /*Ens arriba input per el socket de escolta, femd e servidor*/
        scon = MI_AcceptaConv(sesc, ipRem, &*portTCPrem, ipLoc, &*portTCPloc, nicLoc, nicRem);    
    }
    return scon;
}

int LUMI_conversa(int scon, int sckUdp, char* ipRem, int* portUdp, int LogFile, const char * exitAp) {
    char buffer[303];
    int bytesLlegits = 0;
    char missResposta[500];
    int bytesEnviats;
    int descActiu = LUMI_haArribatAlgunaCosa(sckUdp, scon);
    if (descActiu == 0) {
        bytesLlegits = read(0, buffer, sizeof (buffer));
        buffer[bytesLlegits] = '\0';
        bytesEnviats = MI_EnviaLinia(scon, buffer);

    } else if (descActiu == scon) {
        //Ens arriba informacio, la guardem i mostrem per pantalla
        bytesLlegits = MI_RepLinia(scon, buffer);
        buffer[bytesLlegits] = '\n';
        buffer[bytesLlegits + 1] = '\0';
        write(1, buffer, bytesLlegits + 1);
    } else if (descActiu == sckUdp) {
        //Gestionem els diferents casos d'estar en conversa i que ens arribin peticions via UDP, que ens pot arribar ? 
        //Peticio de localitzacio i prou, per tant contestem l3.
        //Qualsevol altre missatge el tractem com a missatge erroni
        bytesLlegits = LUMI_repMissatge(sckUdp, ipRem, &(*portUdp), missResposta, sizeof (missResposta), LogFile);
        missResposta[bytesLlegits] = '\0';
        if (missResposta[0] == 'L') {
            //Hem d'obtenir la preguntadora, per tant..
            char adrLumiPreguntadora[40];

            int i = 0;
            while (missResposta[i] != ':') {
                i++; //saltem la primera entitat 
            }
            i++; //saltem :
            int j = 0;
            while (missResposta[i] != '\0') {
                adrLumiPreguntadora[j] = missResposta[i];
                i++;
                j++;
            }
            adrLumiPreguntadora[j] = '\0';
            snprintf(missResposta, j + 3, "l3%s", adrLumiPreguntadora);
            LUMI_enviaMissatge(sckUdp, ipRem, *portUdp, missResposta, j + 3, LogFile);
        } else {
            bytesLlegits = sprintf(missResposta, "Error: Missatge amb format inesperat");
            LUMI_enviaMissatge(sckUdp, ipRem, *portUdp, missResposta, bytesLlegits + 1, LogFile);
        }
    }
    if (strncmp(buffer, exitAp, 2) == 0)
        bytesLlegits = 0;
    return bytesLlegits;
}

int LUMI_demanaConv(const char* missResposta, char* ipDesti, int* portTCP, int sckTCP, char * ipLoc, const char * nicLoc, char * nicRem, int * portTCPloc) {
    LUMI_extreureIpPort(missResposta, ipDesti, &(*portTCP));
    //Fem de servidor i li demanem conversa al client.. MIRAR IPLOC I IPREM, ESTEM ESCCRIBIN EN MEMORIA PROTEGIDA! JO DEL FUTUR
    int scon = LUMI_connexio(sckTCP, ipDesti, ipLoc, nicLoc, nicRem, &(*portTCPloc), &(*portTCP), 0);
    return scon;
}

int LUMI_gestionaMissatge(const char * missResposta, int nBytes, int sckUdp, int sckTCP, const char * ipInterface, char * ipRem, int portUdp, int portTCPloc, int LogFile) {
    int resposta = -1;
    //Ens arriba un missatge per UDP, com actuem ??
    //Potser que sigui un missatge de localitzacio L, haurem de construir un missatge l
    //Potser que sigui un missatge de resposta de localitzacio l, si es correcte, procedirem a fer la conexio tcp

    if (missResposta[0] == 'L') {
        //Ens arriba una peticio de localitzacio, construim missatge de resposta

        int k = LUMI_construirMissatgeLocResp(sckUdp, sckTCP, missResposta, ipInterface, portTCPloc, ipRem, portUdp, LogFile);
        resposta = 5;
    } else if (missResposta[0] == 'l') {

        if (missResposta[1] == '0') {
            resposta = 0;
        } else if (missResposta[1] == '1') {
            resposta = 1;
        } else if (missResposta[1] == '2') {
            resposta = 2;
        } else if (missResposta[1] == '3') {
            resposta = 3;
        } else if (missResposta[1] == '4') {
            resposta = 4;
        }
    }
    return resposta;
}

int LUMI_ferRegistre(int sck, char *ipRem, int portUDP, const char* adrMiLoc, int LogFile) {
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
    printf("%s\n",host);
    int n = DNSc_ResolDNSaIP(host, ipRem);
    char miss[500];
    sprintf(miss, "Inicialitzem Registre de %s\n", adrMiLoc);
    Log_Escriu(LogFile, miss);

    return n;
}

int LUMI_enviaMissatge(int Sck, const char* IPrem, int portUDPrem, const char* SeqBytes, int LongSeqBytes, int LogFile) {
    int n = UDP_EnviaA(Sck, IPrem, portUDPrem, SeqBytes, LongSeqBytes);
    char miss[500];
    sprintf(miss, "Enviem '%s' de %d bytes a @IP: %s i #port: %d\n", SeqBytes, LongSeqBytes, IPrem, portUDPrem);
    Log_Escriu(LogFile, miss);
    return n;
}

int LUMI_repMissatge(int Sck, char* IPrem, int* portUDPrem, char* seqBytes, int LongSeqBytes, int LogFile) {

    char miss2[500];
    int bytesLlegits = UDP_RepDe(Sck, IPrem, &(*portUDPrem), seqBytes, LongSeqBytes);
    seqBytes[bytesLlegits] = '\0';
    sprintf(miss2, "Rebem '%s' de %d bytes de @IP: %s i #port: %d\n", seqBytes, bytesLlegits, IPrem, *portUDPrem);
    Log_Escriu(LogFile, miss2);

    return bytesLlegits;

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

int LUMI_haArribatAlgunaCosa(int sckEsc, int sckTCP) {
   
    
    int conjunt[2];
    conjunt[0] = sckTCP;
    conjunt[1] = sckEsc;
    int descActiu = T_HaArribatAlgunaCosa(conjunt, 2);
    
    return descActiu;
}

int LUMI_haArribatAlgunaCosaEnTemps(int sck, int sckTCP, int temps) {
    
    int conjunt[2];
    conjunt[0] = sckTCP;
    conjunt[1] = sck;
    int descActiu = T_HaArribatAlgunaCosaEnTemps(conjunt, 2, temps);
    return descActiu;
}

int LUMI_trobarSckNom(int sckUDP, char* ipLoc, int * portLoc) {
    return UDP_TrobaAdrSockLoc(sckUDP, ipLoc, &(*portLoc));
}

int LUMI_registre(int sckUdp, int portUdp, char* miss, const char* adrLumiLoc,char* ipRem, int nBytesLoc, int portUdpLoc, int LogFile) {
    char petRegistre[500];
    int resultat = LUMI_ferRegistre(sckUdp, ipRem, portUdp, adrLumiLoc, LogFile);
    char miss2[500];
    LUMI_construeixProtocolLUMI(adrLumiLoc, petRegistre);
    LUMI_enviaMissatge(sckUdp, ipRem, portUdp, petRegistre, nBytesLoc, LogFile);
    if (resultat == 0) {
        int nIntents = 0;
        int descActiu = LUMI_haArribatAlgunaCosaEnTemps(sckUdp, -1, 5);
        while (resultat == 0) {
            if (descActiu == sckUdp) {
                int nBytes = LUMI_repMissatge(sckUdp, ipRem, &portUdpLoc, miss, sizeof (miss), LogFile);
                if (miss[0] == 'C' && miss[1] == '0') {
                    
                    resultat = 1;
                    sprintf(miss2, "S'ha completat registre per usuari %s\n", adrLumiLoc);
                    Log_Escriu(LogFile, miss2);
                } else if (miss[0] == 'C' && miss[1] == '1') {
                    
                    resultat = 1;
                    sprintf(miss2, "Error: no s'ha trobat l'usuari %s\n", adrLumiLoc);
                    Log_Escriu(LogFile, miss2);
                } else if (miss[0] == 'C' && miss[1] == '2') {
                    
                    resultat = 2;
                    sprintf(miss2, "Error: L'usuari %s ha entrat un format incorrecte d'@MI\n", adrLumiLoc);
                    Log_Escriu(LogFile, miss2);
                }
            } else {
                if (nIntents < 5) {
                    printf("Numero d'intents: %d. Reintentan...\n", nIntents + 1);
                    nIntents++;
                    LUMI_enviaMissatge(sckUdp, ipRem, portUdp, petRegistre, nBytesLoc, LogFile);
                    descActiu = LUMI_haArribatAlgunaCosaEnTemps(sckUdp, -1, 5);
                } else {
                    resultat = 3;
                    sprintf(miss2, "Error: timeout reached. No s'ha pogut completar el registre de l'usuari %s\n", adrLumiLoc);
                    Log_Escriu(LogFile, miss2);
                }
            }

        }
    } else {
        sprintf(miss2, "L'usuari %s ha entrat un domini incorrecte\n", adrLumiLoc);
        Log_Escriu(LogFile, miss2);
    }
    return resultat;
}

int LUMI_construirMissatgeLoc(const char* miss, const char* adrLumiLoc, int nBytesLoc, char * missLoc, int LogFile, char * adrLumiRem, int nBytesRem) {
   
    if (nBytesRem < 2) return 0;
    char host[40];
    char ipDest[16];
    int nBytesMissLoc;
    LUMI_obtenirHost(adrLumiRem, host);
    if (DNSc_ResolDNSaIP(host, ipDest) == 0) {
        nBytesMissLoc = nBytesLoc + nBytesRem + 2;
        
        sprintf(missLoc,"L%s:%s", adrLumiRem, adrLumiLoc);
        //printf("Missatge al client: %s\nNombre de bytes: %d\n", missLoc, nBytesRem);
        char miss2[500];
        sprintf(miss2, "Construim missatge L: %s\n", missLoc);
        Log_Escriu(LogFile, miss2);
    } else {
        nBytesMissLoc = -1;
    }

    return nBytesMissLoc;
}



int LUMI_construirMissatgeLocResp(int sckUdp, int sckTCP, const char* miss, const char* ipLoc, int portTcp, const char* ipRem, int portUdp, int LogFile) {
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
    int nBytes = strlen(adrMiDest) + strlen(portTCParray) + strlen(ipLoc) + 4;
    sprintf(missLocResp, "l0%s:%s:%s", adrMiDest, ipLoc, portTCParray);
    //printf("Missatge a enviar: %s amb %d bytes\n",missLocResp,n);
    int k = LUMI_enviaMissatge(sckUdp, ipRem, portUdp, missLocResp, nBytes, LogFile);
    char miss2[500];
    sprintf(miss2, "Enviat missatge '%s' de Resposta de Localització a @IP: %s i #port: %d\n", missLocResp, ipRem, portUdp);
    Log_Escriu(LogFile, miss2);

    return k;
}

int LUMI_ferDesregistre(const char* adrLumiLoc, const char* ipRem, int portUDP, int sckUDP, int LogFile) {
    char miss[500];
    //Primer construim el nostre missatge que tindra un format Dduned@PC-b
    int midaAdr = strlen(adrLumiLoc);
   
    sprintf(miss, "D%s", adrLumiLoc);

    int k = LUMI_enviaMissatge(sckUDP, ipRem, portUDP, miss, midaAdr + 1, LogFile);
    char miss2[500];
    sprintf(miss2, "Enviat missatge de desregistre '%s' a @IP: %s i #port: %d\n", miss, ipRem, portUDP);
    Log_Escriu(LogFile, miss2);

    return k;
}
