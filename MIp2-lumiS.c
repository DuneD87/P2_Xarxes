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
#include <stdio.h>

/* Definició de constants, p.e., #define XYZ       1500                   */

/* Declaració de funcions INTERNES que es fan servir en aquest fitxer     */
/* (les  definicions d'aquestes funcions es troben més avall) per així    */
/* fer-les conegudes des d'aquí fins al final d'aquest fitxer, p.e.,      */
/* int FuncioInterna(arg1, arg2...);                                      */
/* Com a mínim heu de fer les següents funcions INTERNES:                 */

int Log_CreaFitx(const char *NomFitxLog);
int Log_Escriu(int FitxLog, const char *MissLog);
int Log_TancaFitx(int FitxLog);

/* Definició de funcions EXTERNES, és a dir, d'aquelles que es cridaran   */
/* des d'altres fitxers, p.e., int LUMIs_FuncioExterna(arg1, arg2...) { } */
/* En termes de capes de l'aplicació, aquest conjunt de funcions externes */
/* formen la interfície de la capa LUMI, la part del servidor             */

int LUMI_HaArribatAlgunaCosa(int sck)
{
    fd_set conjunt;
    FD_ZERO(&conjunt); /* esborrem el contingut de la llista */
    FD_SET(0, &conjunt); /* afegim (“marquem”) el teclat a la llista */
    FD_SET(sck, &conjunt); /* afegim (“marquem”) el socket connectat a la llista */
    int sel = T_HaArribatAlgunaCosa(&conjunt, sck);
    int descActiu;
    if (sel != -1) {
        int i = 0;
        for (i; i <= sck; i++)
            if (FD_ISSET(i, &conjunt))
                descActiu = i;
    } else {
        descActiu = -1;
    }
    return descActiu;
}

sckAdd LUMIS_TrobaAdreca(char* adMI)
{
    int trobat = 0;
    int i = 0;
    while(i!=clientsTotal && trobat == 0){
        if(taulaClients[i].adMi == adMI){
            trobat = 1;
        }
        else i++;
    }
    if(trobat == 0){ //ERROR O AVIS?
        perror("User is not available");
        exit(-1);
    }
    
    return taulaClients[i].sck;
}

int LUMIS_Registre(char* adMI)
{
    
    
}

/* Definició de funcions INTERNES, és a dir, d'aquelles que es faran      */
/* servir només en aquest mateix fitxer. Les seves declaracions es troben */
/* a l'inici d'aquest fitxer.                                             */

/* Crea un fitxer de "log" de nom "NomFitxLog".                           */
/* "NomFitxLog" és un "string" de C (vector de chars imprimibles acabat   */
/* en '\0') d'una longitud qualsevol.                                     */
/* Retorna -1 si hi ha error; l'identificador del fitxer creat si tot va  */
/* bé.                                                                    */
int Log_CreaFitx(const char *NomFitxLog)
{
    FILE fit;
    fit = fopen(NomFitxLog, "w");
    if(fit == NULL){
        perror("Unable to open file");
        exit(-1);
    }
    return fit;
}

/* Escriu al fitxer de "log" d'identificador "FitxLog" el missatge de     */
/* "log" "MissLog".                                                       */
/* "MissLog" és un "string" de C (vector de chars imprimibles acabat      */
/* en '\0') d'una longitud qualsevol.                                     */
/* Retorna -1 si hi ha error; el nombre de caràcters del missatge de      */
/* "log" (sense el '\0') si tot va bé                                     */
int Log_Escriu(int FitxLog, const char *MissLog)
{
    int mida;
    if(mida = fputs(FitxLog,MissLog) == EOF){
        perror("Unable to write in file");
        exit(-1);
    }
    
    return strlen(MissLog);
}

/* Tanca el fitxer de "log" d'identificador "FitxLog".                    */
/* Retorna -1 si hi ha error; un valor positiu qualsevol si tot va bé.    */
int Log_TancaFitx(int FitxLog)
{
    if(fclose(FILE FitxLog) == EOF){
        perror("Unable to close file");
        exit(-1);
    }
    return 1;
}

/* Si ho creieu convenient, feu altres funcions INTERNES                  */

