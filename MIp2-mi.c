/**************************************************************************/
/*                                                                        */
/* P1 - MI amb sockets TCP/IP - Part I                                    */
/* Fitxer mi.c que implementa la capa d'aplicació de MI, sobre la capa de */
/* transport TCP (fent crides a la "nova" interfície de la capa TCP o     */
/* "nova" interfície de sockets)                                          */
/* Autors: X, Y                                                           */
/*                                                                        */
/**************************************************************************/

/* Inclusió de llibreries, p.e. #include <sys/types.h> o #include "meu.h" */
/*  (si les funcions externes es cridessin entre elles, faria falta fer   */
/*   un #include del propi fitxer .h)                                     */
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "MIp2-t.h"

void MI_EliminarProtocol(char * text, int tipus)
{
	memmove(text,text+4,strlen(text));
}

void MI_AplicarProtocol(const char * textOrg,char * text, int tipus)
{
	char miLinia[303];
	int nBytes = strlen(textOrg);
	if (tipus == 0)
	{
		//Llegim nickname
		snprintf(miLinia,nBytes+4,"N%.3d%s",nBytes - 1,textOrg);
	} else
	{
		sprintf(miLinia,"L%.3d%s",nBytes - 1,textOrg);
	}
	strcpy(text,miLinia);
}

int MI_getsockname(int sck, char *ipLoc, int *portTCPloc){
	
	TCP_TrobaAdrSockLoc(sck,ipLoc,portTCPloc);
}

/* Definició de constants, p.e., #define XYZ       1500                   */

/* Declaració de funcions INTERNES que es fan servir en aquest fitxer     */
/* (les seves definicions es troben més avall) per així fer-les conegudes */
/* des d'aqui fins al final de fitxer.                                    */

/* Definicio de funcions EXTERNES, és a dir, d'aquelles que en altres     */
/* fitxers externs es faran servir.                                       */
/* En termes de capes de l'aplicació, aquest conjunt de funcions externes */
/* formen la interfície de la capa MI.                                    */

/* Inicia l’escolta de peticions remotes de conversa a través d’un nou    */
/* socket TCP, que té una @IP local qualsevol i el #port “portTCPloc”     */
/* (és a dir, crea un socket “servidor” o en estat d’escolta – listen –). */
/* Retorna -1 si hi ha error; l’identificador del socket d’escolta de MI  */
/* creat si tot va bé.                                                    */
int MI_IniciaEscPetiRemConv(int portTCPloc)
{
	char ipLoc[16] = "0.0.0.0";
	int sesc = TCP_CreaSockServidor(ipLoc,portTCPloc);
	return sesc;
}

/* Escolta indefinidament fins que arriba una petició local de conversa   */
/* a través del teclat o bé una petició remota de conversa a través del   */
/* socket d’escolta de MI d’identificador “SckEscMI” (un socket           */
/* “servidor”).                                                           */
/* Retorna -1 si hi ha error; 0 si arriba una petició local; SckEscMI si  */
/* arriba una petició remota.                                             */
int MI_HaArribatPetiConv(int SckEscMI)
{
	fd_set conjunt;
	FD_ZERO(&conjunt); /* esborrem el contingut de la llista */
	FD_SET(0,&conjunt); /* afegim (“marquem”) el teclat a la llista */
	FD_SET(SckEscMI,&conjunt); /* afegim (“marquem”) el socket connectat a la llista */
	int sel = T_HaArribatAlgunaCosa(&conjunt,SckEscMI);
	int descActiu;
	if (sel != -1)
	{
		int i = 0;
		for (i;i<=SckEscMI;i++)
			if (FD_ISSET(i,&conjunt)) {
				descActiu = i;
			}
	} else
	{
		descActiu = -1;
	}
	return descActiu;
}

/* Crea una conversa iniciada per una petició local que arriba a través   */
/* del teclat: crea un socket TCP “client” (en un #port i @IP local       */
/* qualsevol), a través del qual fa una petició de conversa a un procés   */
/* remot, el qual les escolta a través del socket TCP ("servidor") d'@IP  */
/* “IPrem” i #port “portTCPrem” (és a dir, crea un socket “connectat” o   */
/* en estat establert – established –). Aquest socket serà el que es farà */
/* servir durant la conversa.                                             */
/* Omple “IPloc*” i “portTCPloc*” amb, respectivament, l’@IP i el #port   */
/* TCP del socket del procés local.                                       */
/* El nickname local “NicLoc” i el nickname remot són intercanviats amb   */
/* el procés remot, i s’omple “NickRem*” amb el nickname remot. El procés */
/* local és qui inicia aquest intercanvi (és a dir, primer s’envia el     */
/* nickname local i després es rep el nickname remot).                    */
/* "IPrem" i "IPloc*" són "strings" de C (vectors de chars imprimibles    */
/* acabats en '\0') d'una longitud màxima de 16 chars (incloent '\0').    */
/* "NicLoc" i "NicRem*" són "strings" de C (vectors de chars imprimibles  */
/* acabats en '\0') d'una longitud màxima de 300 chars (incloent '\0').   */
/* Retorna -1 si hi ha error; l’identificador del socket de conversa de   */
/* MI creat si tot va bé.                                                 */
int MI_DemanaConv(const char *IPrem, int portTCPrem, char *IPloc, int *portTCPloc, const char *NicLoc, char *NicRem)
{
	int scon = TCP_CreaSockClient(IPloc,0);
	char aux[303];
	MI_AplicarProtocol(NicLoc,aux,0);
	if (TCP_DemanaConnexio(scon,IPrem,portTCPrem) == -1)
		return -1;
	 else {
		TCP_TrobaAdrSockLoc(scon,IPloc,portTCPloc);
		TCP_Envia(scon,aux,strlen(aux));
		int bytesRebuts = TCP_Rep(scon,NicRem,303);
		NicRem[bytesRebuts] = '\0';
		MI_EliminarProtocol(NicRem,0);
		return scon;
	}
}

/* Crea una conversa iniciada per una petició remota que arriba a través  */
/* del socket d’escolta de MI d’identificador “SckEscMI” (un socket       */
/* “servidor”): accepta la petició i crea un socket (un socket            */
/* “connectat” o en estat establert – established –), que serà el que es  */
/* farà servir durant la conversa.                                        */
/* Omple “IPrem*”, “portTCPrem*”, “IPloc*” i “portTCPloc*” amb,           */
/* respectivament, l’@IP i el #port TCP del socket del procés remot i del */
/* socket del procés local.                                               */
/* El nickname local “NicLoc” i el nickname remot són intercanviats amb   */
/* el procés remot, i s’omple “NickRem*” amb el nickname remot. El procés */
/* remot és qui inicia aquest intercanvi (és a dir, primer es rep el      */
/* nickname remot i després s’envia el nickname local).                   */
/* "IPrem*" i "IPloc*" són "strings" de C (vectors de chars imprimibles   */
/* acabats en '\0') d'una longitud màxima de 16 chars (incloent '\0').    */
/* "NicLoc" i "NicRem*" són "strings" de C (vectors de chars imprimibles  */
/* acabats en '\0') d'una longitud màxima de 300 chars (incloent '\0').   */
/* Retorna -1 si hi ha error; l’identificador del socket de conversa      */
/* de MI creat si tot va bé.                                              */
int MI_AcceptaConv(int SckEscMI, char *IPrem, int *portTCPrem, char *IPloc, int *portTCPloc, const char *NicLoc, char *NicRem)
{
	int scon = TCP_AcceptaConnexio(SckEscMI,IPrem,portTCPrem);
	char aux[303];
	MI_AplicarProtocol(NicLoc,aux,0);
	if (scon != -1)
	{
		TCP_TrobaAdrSockLoc(scon,IPloc,portTCPloc);
		TCP_Envia(scon,aux,strlen(aux));
	  int bytesRebuts = TCP_Rep(scon,NicRem,303);
		NicRem[bytesRebuts] = '\0';
		MI_EliminarProtocol(NicRem,0);
	}
	return scon;
}

/* Escolta indefinidament fins que arriba una línia local de conversa a   */
/* través del teclat o bé una línia remota de conversa a través del       */
/* socket de conversa de MI d’identificador “SckConvMI” (un socket        */
/* "connectat”).                                                          */
/* Retorna -1 si hi ha error; 0 si arriba una línia local; SckConvMI si   */
/* arriba una línia remota.                                               */
int MI_HaArribatLinia(int SckConvMI)
{
	fd_set conjunt;
	FD_ZERO(&conjunt); /* esborrem el contingut de la llista */
	FD_SET(0,&conjunt); /* afegim (“marquem”) el teclat a la llista */
	FD_SET(SckConvMI,&conjunt); /* afegim (“marquem”) el socket connectat a la llista */
	int sel = T_HaArribatAlgunaCosa(&conjunt,SckConvMI);
	int descActiu;
	if (sel != -1)
	{
		int i = 0;
		for (i;i<=SckConvMI;i++)
			if (FD_ISSET(i,&conjunt))
				descActiu = i;
	} else
	{
		descActiu = -1;
	}
	return descActiu;
}

/* Envia a través del socket de conversa de MI d’identificador            */
/* “SckConvMI” (un socket “connectat”) la línia “Linia” escrita per       */
/* l’usuari local.                                                        */
/* "Linia" és un "string" de C (vector de chars imprimibles acabat en     */
/* '\0'), no conté el caràcter fi de línia ('\n') i té una longitud       */
/* màxima de 300 chars (incloent '\0').                                   */
/* Retorna -1 si hi ha error; el nombre de caràcters n de la línia        */
/* enviada (sense el ‘\0’) si tot va bé (0 <= n <= 299).                  */
int MI_EnviaLinia(int SckConvMI, const char *Linia)
{
	char aux[303];
	MI_AplicarProtocol(Linia,aux,1);
	int nBytes = TCP_Envia(SckConvMI,aux,strlen(aux) - 1);
	if (nBytes == -1)
		return nBytes;
	else if (nBytes == 0)
		return -2;
	else
		return nBytes;
}

/* Rep a través del socket de conversa de MI d’identificador “SckConvMI”  */
/* (un socket “connectat”) una línia escrita per l’usuari remot, amb la   */
/* qual omple “Linia”, o bé detecta l’acabament de la conversa per part   */
/* de l’usuari remot.                                                     */
/* "Linia*" és un "string" de C (vector de chars imprimibles acabat en    */
/* '\0'), no conté el caràcter fi de línia ('\n') i té una longitud       */
/* màxima de 300 chars (incloent '\0').                                   */
/* Retorna -1 si hi ha error; -2 si l’usuari remot acaba la conversa; el  */
/* nombre de caràcters n de la línia rebuda (sense el ‘\0’) si tot va bé  */
/* (0 <= n <= 299).                                                       */
int MI_RepLinia(int SckConvMI, char *Linia)
{
	int nBytes = TCP_Rep(SckConvMI,Linia,300);
	Linia[nBytes] = '\0';
	MI_EliminarProtocol(Linia,1);
	if (nBytes == -1)
		return nBytes - 4;
	else if (nBytes == 0)
		return -2;
	else
		return nBytes - 4;
}

/* Acaba la conversa associada al socket de conversa de MI                */
/* d’identificador “SckConvMI” (un socket “connectat”).                   */
/* Retorna -1 si hi ha error; un valor positiu qualsevol si tot va bé.    */
int MI_AcabaConv(int SckConvMI)
{
	TCP_TancaSock(SckConvMI);
}

/* Acaba l’escolta de peticions remotes de conversa que arriben a través  */
/* del socket d’escolta de MI d’identificador “SckEscMI” (un socket       */
/* “servidor”).                                                           */
/* Retorna -1 si hi ha error; un valor positiu qualsevol si tot va bé.    */
int MI_AcabaEscPetiRemConv(int SckEscMI)
{
	TCP_TancaSock(SckEscMI);
}

int MI_seleccionaInterficie(char * ipLoc)
{
	return TCP_seleccionaInterficie(ipLoc);
}

void MI_MostraError(){
	
	char *err = T_MostraError();
	printf("Error: %s\n",err);
}
/* Si ho creieu convenient, feu altres funcions EXTERNES                  */

/* Definicio de funcions INTERNES, és a dir, d'aquelles que es faran      */
/* servir només en aquest mateix fitxer.                                  */
