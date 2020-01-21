/**************************************************************************/
/*                                                                        */
/* P2 - MI amb sockets TCP/IP - Part II                                   */
/* Fitxer nodelumi.c que implementa la interfície aplicació-administrador */
/* d'un node de LUMI, sobre la capa d'aplicació de LUMI (fent crides a la */
/* interfície de la capa LUMI -fitxers lumi.c i lumi.h-).                 */
/* Autors: X, Y                                                           */
/*                                                                        */
/**************************************************************************/

/* Inclusió de llibreries, p.e. #include <stdio.h> o #include "meu.h"     */

#include "MIp2-lumiS.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
/* Definició de constants, p.e., #define XYZ       1500                   */
#define PORTUDP 2020

/* Declaració de funcions INTERNES que es fan servir en aquest fitxer     */
/* (les  definicions d'aquestes funcions es troben més avall) per així    */
/* fer-les conegudes des d'aquí fins al final d'aquest fitxer, p.e.,      */

/* int FuncioInterna(arg1, arg2...);                                      */

int main(int argc, char *argv[]) {
    /* Declaració de variables, p.e., int n;                                 */
    char ipLoc[16];
    struct taulaClients taulaCli;
    char fitxLog[40] = "MIp2-loglumi-";
    int LogFile, descActiu, sckLoc;
    strcpy(ipLoc, "0.0.0.0");
    char host[40];
    /* Expressions, estructures de control, crides a funcions, etc.          */


    //Comencem inicialitzant el servidor: llegeix la taula de client, crea socket UDP i obre el fitxer
    if (LUMIS_emplenaTaula(&taulaCli, host) == -1) {
        printf("Error al emplenar taula\n");
        exit(-1);
    }

    /* INICIALITZEM EL LOG FILE */
    host[4] = '\0';
    strcat(fitxLog, host);
    strcat(fitxLog, ".log");
    LogFile = Log_CreaFitx(fitxLog);

    if ((sckLoc = LUMIS_IniciaSockEsc(ipLoc, 2020)) == -1) //perror?
    {
        printf("Error en inicia socket de escolta\n");
        exit(-1);
    }

    /*if ((LogFile = LUMIS_IniciaFitxer(fitxLog)) == -1) {
        printf("Error en iniciar el fitxer log\n");
        exit(-1);
    }*/
    printf("Escoltant...\n");
    descActiu = LUMIS_HaArribatAlgunaCosa(sckLoc);
    while (descActiu > 0) {
        LUMIS_mostraClients(&taulaCli);
        LUMIS_ServeixPeticions(sckLoc, &taulaCli, LogFile);
        descActiu = LUMIS_HaArribatAlgunaCosa(sckLoc);
    }
    printf("Finalitzan sessio..\n");
    Log_Escriu(LogFile, "FINALITZA LA SESSIÓ\n");
    LUMIS_Finalitza(sckLoc);
    Log_TancaFitx(LogFile);
    return 0;

}

/* Definició de funcions INTERNES, és a dir, d'aquelles que es faran      */
/* servir només en aquest mateix fitxer. Les seves declaracions es troben */
/* a l'inici d'aquest fitxer.                                             */
