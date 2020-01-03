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

int main(int argc,char *argv[])
{
    char adrLumiLoc[40];
    char adrLumiRem[40];
    char ipRem[16];
    int portUdp = 2020;
    strcpy(ipRem,"0.0.0.0");
    int sckUdp = LUMI_IniciaSckEscolta(ipRem,portUdp);
    printf("Socket de connexio %d\n",sckUdp);
    printf("Introdueix l'adreca LUMI\n");
    int nBytes = read(0,adrLumiLoc,sizeof((adrLumiLoc)));
    adrLumiLoc[nBytes] = '\0';
    LUMI_ferRegistre(sckUdp, ipRem,portUdp, adrLumiLoc);
    char petRegistre[500];
    LUMI_construeixProtocolLUMI(adrLumiLoc,petRegistre);
    printf("Envian missatge de registre a IP:%s\n amb Port UDP:%d\n",ipRem,portUdp);
    LUMI_enviaMissatge(sckUdp,ipRem,portUdp,petRegistre,nBytes + 1);
    int temps = 2;
    int nIntents = 0;
    int descActiu = LUMI_haArribatAlgunaCosaEnTemps(sckUdp,temps);
    while (1)
    {
        if (descActiu == sckUdp)
        {
            nBytes = LUMI_repMissatge(sckUdp,ipRem,portUdp,petRegistre,sizeof(petRegistre));
            if (petRegistre[0] == 'C')
            {
                if (petRegistre[1] == '0')
                {
                    printf("Registre Completat de forme correcte.\nIntrodueix l'adreca de la persona amb la que vols contactar\n");
                    nBytes = read(0,adrLumiRem,sizeof(adrLumiLoc));
                }
            }
        } else
        {
            if (nIntents < 5) {
                printf("Timeout: Reintentan. INTENT num:%d\n",nIntents);
                nIntents++;
            } else 
            {
                printf("Nombre d'intents sobrepassat, abortant\n");
                exit(-1);
            }
        }
        descActiu = LUMI_haArribatAlgunaCosaEnTemps(sckUdp,temps);
    }
}

/* Definició de funcions INTERNES, és a dir, d'aquelles que es faran      */
/* servir només en aquest mateix fitxer. Les seves declaracions es troben */
/* a l'inici d'aquest fitxer.                                             */
