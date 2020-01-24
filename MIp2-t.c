/**************************************************************************/
/*                                                                        */
/* P1 - MI amb sockets TCP/IP - Part I                                    */
/* Fitxer t.c que "implementa" la capa de transport, o més ben dit, que   */
/* encapsula les funcions de la interfície de sockets, en unes altres     */
/* funcions més simples i entenedores: la "nova" interfície de sockets.   */
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
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <unistd.h>
#include <netdb.h>

#include "MIp2-t.h"



/* Definició de constants, p.e., #define XYZ       1500                   */

/* Declaració de funcions INTERNES que es fan servir en aquest fitxer     */
/* (les seves definicions es troben més avall) per així fer-les conegudes */
/* des d'aqui fins al final de fitxer.                                    */

/* Definicio de funcions EXTERNES, és a dir, d'aquelles que en altres     */
/* fitxers externs es faran servir.                                       */
/* En termes de capes de l'aplicació, aquest conjunt de funcions externes */
/* són la "nova" interfície de la capa de transport (la "nova" interfície */
/* de sockets).                                                           */

/* Crea un socket TCP “client” a l’@IP “IPloc” i #port TCP “portTCPloc”   */
/* (si “IPloc” és “0.0.0.0” i/o “portTCPloc” és 0 es fa/farà una          */
/* assignació implícita de l’@IP i/o del #port TCP, respectivament).      */
/* "IPloc" és un "string" de C (vector de chars imprimibles acabat en     */
/* '\0') d'una longitud màxima de 16 chars (incloent '\0').               */
/* Retorna -1 si hi ha error; l’identificador del socket creat si tot     */

/* va bé.                                                                 */

int TCP_CreaSockClient(const char *IPloc, int portTCPloc) {
    struct sockaddr_in adrloc;
    int scon, i;

    if ((scon = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        scon = -1;
    }
    adrloc.sin_family = AF_INET;
    adrloc.sin_port = htons(portTCPloc);
    adrloc.sin_addr.s_addr = inet_addr(IPloc);
    for (i = 0; i < 8; i++) {
        adrloc.sin_zero[i] = '\0';
    }
    if ((bind(scon, (struct sockaddr*) &adrloc, sizeof (adrloc))) == -1) {
        return -1;
    }

    return scon;
}

/* Crea un socket TCP “servidor” (o en estat d’escolta – listen –) a      */
/* l’@IP “IPloc” i #port TCP “portTCPloc” (si “IPloc” és “0.0.0.0” i/o    */
/* “portTCPloc” és 0 es fa una assignació implícita de l’@IP i/o del      */
/* #port TCP, respectivament).                                            */
/* "IPloc" és un "string" de C (vector de chars imprimibles acabat en     */
/* '\0') d'una longitud màxima de 16 chars (incloent '\0').               */
/* Retorna -1 si hi ha error; l’identificador del socket creat si tot     */

/* va bé.                                                                 */
int TCP_CreaSockServidor(const char *IPloc, int portTCPloc) {
    struct sockaddr_in adrloc;
    int sesc, i;
    /* Es crea el socket TCP sesc del servidor (el socket "local"), que de moment no té */
    /* adreça (@IP i #port TCP) assignada. */
    if ((sesc = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        sesc = -1;
    }
    portTCPloc = 0;
    adrloc.sin_family = AF_INET;
    adrloc.sin_port = 0;
    adrloc.sin_addr.s_addr = inet_addr(IPloc); /* o bé: ...s_addr = INADDR_ANY */
    for (i = 0; i < 8; i++) {
        adrloc.sin_zero[i] = '\0';
    }
    if ((bind(sesc, (struct sockaddr*) &adrloc, sizeof (adrloc))) == -1) {

        sesc = -1;
    }
    /* Es crea una cua per emmagatzemar peticions de connexió pendents. */
    if ((listen(sesc, 3)) == -1) {
        sesc = -1;
    }
    return sesc;
}

/* El socket TCP “client” d’identificador “Sck” es connecta al socket     */
/* TCP “servidor” d’@IP “IPrem” i #port TCP “portTCPrem” (si tot va bé    */
/* es diu que el socket “Sck” passa a l’estat “connectat” o establert     */
/* – established –). Recordeu que això vol dir que s’estableix una        */
/* connexió TCP (les dues entitats TCP s’intercanvien missatges           */
/* d’establiment de la connexió).                                         */
/* "IPrem" és un "string" de C (vector de chars imprimibles acabat en     */
/* '\0') d'una longitud màxima de 16 chars (incloent '\0').               */

/* Retorna -1 si hi ha error; un valor positiu qualsevol si tot va bé.    */
int TCP_DemanaConnexio(int Sck, const char *IPrem, int portTCPrem) {
    int i;
    struct sockaddr_in adrrem;
    adrrem.sin_family = AF_INET;
    adrrem.sin_port = htons(portTCPrem);
    adrrem.sin_addr.s_addr = inet_addr(IPrem);
    for (i = 0; i < 8; i++) {
        adrrem.sin_zero[i] = '\0';
    }

    int connx = connect(Sck, (struct sockaddr*) &adrrem, sizeof (adrrem));

    return connx;
}

/* El socket TCP “servidor” d’identificador “Sck” accepta fer una         */
/* connexió amb un socket TCP “client” remot, i crea un “nou” socket,     */
/* que és el que es farà servir per enviar i rebre dades a través de la   */
/* connexió (es diu que aquest nou socket es troba en l’estat “connectat” */
/* o establert – established –; el nou socket té la mateixa adreça que    */
/* “Sck”).                                                                */
/* Omple “IPrem*” i “portTCPrem*” amb respectivament, l’@IP i el #port    */
/* TCP del socket remot amb qui s’ha establert la connexió.               */
/* "IPrem*" és un "string" de C (vector de chars imprimibles acabat en    */
/* '\0') d'una longitud màxima de 16 chars (incloent '\0').               */
/* Retorna -1 si hi ha error; l’identificador del socket connectat creat  */

/* si tot va bé.                                                          */
int TCP_AcceptaConnexio(int Sck, char *IPrem, int *portTCPrem) {
    struct sockaddr_in adrrem;
    int scon;
    int long_adrrem = sizeof (adrrem);
    if ((scon = accept(Sck, (struct sockaddr*) &adrrem, &long_adrrem)) == -1) {
        scon = -1;
    }
    strcpy(IPrem, inet_ntoa(adrrem.sin_addr));
    *portTCPrem = ntohs(adrrem.sin_port);
    return scon;
}

/* Envia a través del socket TCP “connectat” d’identificador “Sck” la     */
/* seqüència de bytes escrita a “SeqBytes” (de longitud “LongSeqBytes”    */
/* bytes) cap al socket TCP remot amb qui està connectat.                 */
/* "SeqBytes" és un vector de chars qualsevol (recordeu que en C, un      */
/* char és un enter de 8 bits) d'una longitud >= LongSeqBytes bytes.      */

/* Retorna -1 si hi ha error; el nombre de bytes enviats si tot va bé.    */
int TCP_Envia(int Sck, const char *SeqBytes, int LongSeqBytes) {
    int bytes_escrits;

    if ((bytes_escrits = write(Sck, SeqBytes, LongSeqBytes)) == -1) {
        bytes_escrits = -1;
    }

    return bytes_escrits;
}

/* Rep a través del socket TCP “connectat” d’identificador “Sck” una      */
/* seqüència de bytes que prové del socket remot amb qui està connectat,  */
/* i l’escriu a “SeqBytes*” (que té una longitud de “LongSeqBytes” bytes),*/
/* o bé detecta que la connexió amb el socket remot ha estat tancada.     */
/* "SeqBytes*" és un vector de chars qualsevol (recordeu que en C, un     */
/* char és un enter de 8 bits) d'una longitud <= LongSeqBytes bytes.      */
/* Retorna -1 si hi ha error; 0 si la connexió està tancada; el nombre de */

/* bytes rebuts si tot va bé.                                             */
int TCP_Rep(int Sck, char *SeqBytes, int LongSeqBytes) {
    int bytes_llegits;
    if ((bytes_llegits = read(Sck, SeqBytes, LongSeqBytes)) == -1) {
        bytes_llegits = -1;
    }
    return bytes_llegits;
}

/* S’allibera (s’esborra) el socket TCP d’identificador “Sck”; si “Sck”   */
/* està connectat es tanca la connexió TCP que té establerta.             */

/* Retorna -1 si hi ha error; un valor positiu qualsevol si tot va bé.    */
int TCP_TancaSock(int Sck) {
    close(Sck);
}

/* Donat el socket TCP d’identificador “Sck”, troba l’adreça d’aquest     */
/* socket, omplint “IPloc*” i “portTCPloc*” amb respectivament, la seva   */
/* @IP i #port TCP.                                                       */
/* "IPloc*" és un "string" de C (vector de chars imprimibles acabat en    */
/* '\0') d'una longitud màxima de 16 chars (incloent '\0').               */

/* Retorna -1 si hi ha error; un valor positiu qualsevol si tot va bé.    */
int TCP_TrobaAdrSockLoc(int Sck, char *IPloc, int *portTCPloc) {
    struct sockaddr_in adrloc;
    int err = 1;
    int long_adrl = sizeof (adrloc);
    if (getsockname(Sck, (struct sockaddr *) &adrloc, &long_adrl) == -1) {
        err = -1;
    }
    strcpy(IPloc, inet_ntoa(adrloc.sin_addr));
    *portTCPloc = ntohs(adrloc.sin_port);
    return err;
}

/* Donat el socket TCP “connectat” d’identificador “Sck”, troba l’adreça  */
/* del socket remot amb qui està connectat, omplint “IPrem*” i            */
/* “portTCPrem*” amb respectivament, la seva @IP i #port TCP.             */
/* "IPrem*" és un "string" de C (vector de chars imprimibles acabat en    */
/* '\0') d'una longitud màxima de 16 chars (incloent '\0').               */

/* Retorna -1 si hi ha error; un valor positiu qualsevol si tot va bé.    */
int TCP_TrobaAdrSockRem(int Sck, char *IPrem, int *portTCPrem) {
    struct sockaddr_in adrrem;
    int long_adrr2 = sizeof (adrrem);
    int err = 1;
    if (getpeername(Sck, (struct sockaddr *) &adrrem, &long_adrr2) == -1) {
        err = -1;
    }
    strcpy(IPrem, inet_ntoa(adrrem.sin_addr));
    *portTCPrem = ntohs(adrrem.sin_port);
    return err;
}

/* Crea un socket UDP a l’@IP “IPloc” i #port UDP “portUDPloc”            */
/* (si “IPloc” és “0.0.0.0” i/o “portUDPloc” és 0 es fa/farà una          */
/* assignació implícita de l’@IP i/o del #port UDP, respectivament).      */
/* "IPloc" és un "string" de C (vector de chars imprimibles acabat en     */
/* '\0') d'una longitud màxima de 16 chars (incloent '\0')                */
/* Retorna -1 si hi ha error; l’identificador del socket creat si tot     */

/* va bé.                                                                 */
int UDP_CreaSock(const char *IPloc, int portUDPloc) {
    struct sockaddr_in adrloc;
    int sck, i;
    /* Es crea el socket UDP sck del servidor */
    /* adreça (@IP i #port UDP) assignada. */
    if ((sck = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        sck = -1;
    }
    adrloc.sin_family = AF_INET;
    adrloc.sin_port = htons(portUDPloc);
    adrloc.sin_addr.s_addr = inet_addr(IPloc); //INADR_ANY?
    for (i = 0; i < 8; i++) {
        adrloc.sin_zero[i] = '\0';
    }
    if ((bind(sck, (struct sockaddr*) &adrloc, sizeof (adrloc))) == -1) {

        sck = -1;
    }
    /* Es crea una cua per emmagatzemar peticions de connexió pendents.
    if ((listen(sck, 3)) == -1) {
                    sck = -1;
    }*/
    return sck;
}

/* Envia a través del socket UDP d’identificador “Sck” la seqüència de    */
/* bytes escrita a “SeqBytes” (de longitud “LongSeqBytes” bytes) cap al   */
/* socket remot que té @IP “IPrem” i #port UDP “portUDPrem”.              */
/* "IPrem" és un "string" de C (vector de chars imprimibles acabat en     */
/* '\0') d'una longitud màxima de 16 chars (incloent '\0')                */
/* "SeqBytes" és un vector de chars qualsevol (recordeu que en C, un      */
/* char és un enter de 8 bits) d'una longitud >= LongSeqBytes bytes       */

/* Retorna -1 si hi ha error; el nombre de bytes enviats si tot va bé.    */
int UDP_EnviaA(int Sck, const char *IPrem, int portUDPrem, const char *SeqBytes, int LongSeqBytes) {
    struct sockaddr_in adrrem;
    adrrem.sin_family = AF_INET;
    adrrem.sin_port = htons(portUDPrem);
    adrrem.sin_addr.s_addr = inet_addr(IPrem);
    int nBytes = sendto(Sck, SeqBytes, LongSeqBytes, 0, (struct sockaddr*) &adrrem, sizeof (adrrem));
    return nBytes;
}

/* Rep a través del socket UDP d’identificador “Sck” una seqüència de     */
/* bytes que prové d'un socket remot i l’escriu a “SeqBytes*” (que té     */
/* una longitud de “LongSeqBytes” bytes).                                 */
/* Omple "IPrem*" i "portUDPrem*" amb respectivament, l'@IP i el #port    */
/* UDP del socket remot.                                                  */
/* "IPrem*" és un "string" de C (vector de chars imprimibles acabat en    */
/* '\0') d'una longitud màxima de 16 chars (incloent '\0')                */
/* "SeqBytes*" és un vector de chars qualsevol (recordeu que en C, un     */
/* char és un enter de 8 bits) d'una longitud <= LongSeqBytes bytes       */

/* Retorna -1 si hi ha error; el nombre de bytes rebuts si tot va bé.     */
int UDP_RepDe(int Sck, char *IPrem, int *portUDPrem, char *SeqBytes, int LongSeqBytes) {
    struct sockaddr_in addrrem;
    int laddRem;
    laddRem = sizeof (addrrem);
    int nBytes = recvfrom(Sck, SeqBytes, LongSeqBytes, 0, (struct sockaddr *) &addrrem, &laddRem);
    //UDP_TrobaAdrSockRem(Sck,IPrem,&(*portUDPrem));
    strcpy(IPrem, inet_ntoa(addrrem.sin_addr));
    *portUDPrem = htons(addrrem.sin_port);

    return nBytes;
}

/* S’allibera (s’esborra) el socket UDP d’identificador “Sck”.            */

/* Retorna -1 si hi ha error; un valor positiu qualsevol si tot va bé.    */
int UDP_TancaSock(int Sck) {
    close(Sck);
}

/* Donat el socket UDP d’identificador “Sck”, troba l’adreça d’aquest     */
/* socket, omplint “IPloc*” i “portUDPloc*” amb respectivament, la seva   */
/* @IP i #port UDP.                                                       */
/* "IPloc*" és un "string" de C (vector de chars imprimibles acabat en    */
/* '\0') d'una longitud màxima de 16 chars (incloent '\0')                */

/* Retorna -1 si hi ha error; un valor positiu qualsevol si tot va bé.    */
int UDP_TrobaAdrSockLoc(int Sck, char *IPloc, int *portUDPloc) {
    return TCP_TrobaAdrSockLoc(Sck, IPloc, *&portUDPloc);
}

/* El socket UDP d’identificador “Sck” es connecta al socket UDP d’@IP    */
/* “IPrem” i #port UDP “portUDPrem” (si tot va bé es diu que el socket    */
/* “Sck” passa a l’estat “connectat” o establert – established –).        */
/* Recordeu que a UDP no hi ha connexions com a TCP, i que això només     */
/* vol dir que es guarda localment l’adreça “remota” i així no cal        */
/* especificar-la cada cop per enviar i rebre. Llavors quan un socket     */
/* UDP està “connectat” es pot fer servir UDP_Envia() i UDP_Rep() (a més  */
/* de les anteriors UDP_EnviaA() i UDP_RepDe()) i UDP_TrobaAdrSockRem()). */

/* Retorna -1 si hi ha error; un valor positiu qualsevol si tot va bé.    */
int UDP_DemanaConnexio(int Sck, const char *IPrem, int portUDPrem) {
    struct sockaddr_in addrem;
    addrem.sin_family = AF_INET;
    addrem.sin_port = htons(portUDPrem);
    addrem.sin_addr.s_addr = inet_addr(IPrem);
    int i = 0;
    for (i; i < 8; i++)
        addrem.sin_zero[i] = '\0';
    int conx = connect(Sck, (struct sockaddr *) &addrem, sizeof (addrem));
    return conx;
}

/* Envia a través del socket UDP “connectat” d’identificador “Sck” la     */
/* seqüència de bytes escrita a “SeqBytes” (de longitud “LongSeqBytes”    */
/* bytes) cap al socket UDP remot amb qui està connectat.                 */
/* "SeqBytes" és un vector de chars qualsevol (recordeu que en C, un      */
/* char és un enter de 8 bits) d'una longitud >= LongSeqBytes bytes.      */

/* Retorna -1 si hi ha error; el nombre de bytes enviats si tot va bé.    */
int UDP_Envia(int Sck, const char *SeqBytes, int LongSeqBytes) {
    return TCP_Envia(Sck, SeqBytes, LongSeqBytes); //ES LEGAL ?
}

/* Rep a través del socket UDP “connectat” d’identificador “Sck” una      */
/* seqüència de bytes que prové del socket remot amb qui està connectat,  */
/* i l’escriu a “SeqBytes*” (que té una longitud de “LongSeqBytes” bytes).*/
/* "SeqBytes*" és un vector de chars qualsevol (recordeu que en C, un     */
/* char és un enter de 8 bits) d'una longitud <= LongSeqBytes bytes.      */

/* Retorna -1 si hi ha error; el nombre de bytes rebuts si tot va bé.     */
int UDP_Rep(int Sck, char *SeqBytes, int LongSeqBytes) {
    return TCP_Rep(Sck, SeqBytes, LongSeqBytes);
}

/* Donat el socket UDP “connectat” d’identificador “Sck”, troba l’adreça  */
/* del socket remot amb qui està connectat, omplint “IPrem*” i            */
/* “portUDPrem*” amb respectivament, la seva @IP i #port UDP.             */
/* "IPrem*" és un "string" de C (vector de chars imprimibles acabat en    */
/* '\0') d'una longitud màxima de 16 chars (incloent '\0').               */

/* Retorna -1 si hi ha error; un valor positiu qualsevol si tot va bé.    */
int UDP_TrobaAdrSockRem(int Sck, char *IPrem, int *portUDPrem) {
    return TCP_TrobaAdrSockRem(Sck, IPrem, *&portUDPrem);
}

/* Examina simultàniament i sense límit de temps (una espera indefinida)  */
/* els sockets (poden ser TCP, UDP i stdin) amb identificadors en la      */
/* llista “LlistaSck” (de longitud “LongLlistaSck” sockets) per saber si  */
/* hi ha arribat alguna cosa per ser llegida.                             */
/* "LlistaSck" és un vector d'enters d'una longitud >= LongLlistaSck      */
/* Retorna -1 si hi ha error; si arriba alguna cosa per algun dels        */

/* sockets, retorna l’identificador d’aquest socket.                      */
int T_HaArribatAlgunaCosa(const int *LlistaSck, int LongLlistaSck) {
    fd_set conjunt;
    FD_ZERO(&conjunt); /* esborrem el contingut de la llista */
    FD_SET(0, &conjunt); /* afegim (“marquem”) el teclat a la llista */
    int i;
    int max = LlistaSck[0];
    for (i = 0; i < LongLlistaSck;i++) {
        FD_SET(LlistaSck[i], &conjunt); /* afegim (“marquem”) el socket connectat a la llista */
        if (LlistaSck[i] > max) max = LlistaSck[i];
    }
    int sel = select(max + 1, &conjunt, NULL, NULL, NULL);
    int descActiu;
    if (sel != -1) {
        i = 0;
        for (i; i <= max; i++)
            if (FD_ISSET(i, &conjunt)) {
                descActiu = i;
            }
    } else {
        descActiu = -1;
    }

    return descActiu;
}

/* Examina simultàniament i sense límit de temps (una espera indefinida)  */
/* els sockets (poden ser TCP, UDP i stdin) amb identificadors en la      */
/* llista “LlistaSck” (de longitud “LongLlistaSck” sockets) per saber si  */
/* hi ha arribat alguna cosa per ser llegida.                             */
/* "LlistaSck" és un vector d'enters d'una longitud >= LongLlistaSck      */
/* Retorna -1 si hi ha error; si arriba alguna cosa per algun dels        */

/* sockets, retorna l’identificador d’aquest socket.                      */
int T_HaArribatAlgunaCosaEnTemps(const int *LlistaSck, int LongLlistaSck, int temps) {
    struct timeval tv;
    tv.tv_sec = temps;
    tv.tv_usec = 0;
    fd_set conjunt;
    FD_ZERO(&conjunt); /* esborrem el contingut de la llista */
    FD_SET(0, &conjunt); /* afegim (“marquem”) el teclat a la llista */
    int i;
    int max = LlistaSck[0];
    for (i = 0; i < LongLlistaSck;i++) {
        FD_SET(LlistaSck[i], &conjunt); /* afegim (“marquem”) el socket connectat a la llista */
        if (LlistaSck[i] > max) max = LlistaSck[i];
    }
    int sel = select(max + 1, &conjunt, NULL, NULL, &tv);
    int descActiu;
    if (sel != -1) {
        i = 0;
        for (i; i <= max; i++)
            if (FD_ISSET(i, &conjunt)) {
                descActiu = i;
            }
    } else {
        descActiu = -1;
    }

    return descActiu;
}

/* Obté un missatge de text que descriu l'error produït en la darrera     */
/* crida de sockets.                                                      */
/* Retorna aquest missatge de text en un "string" de C (vector de chars   */

/* imprimibles acabat en '\0')                                            */
char* T_MostraError(void) {
    return strerror(errno);
}

int TCP_seleccionaInterficie(char * ipLoc) {
    printf("Selecciona una interficie de les seguents:\n");
    struct ifaddrs *ifa;
    struct sockaddr_in adLoc;
    int s, family;
    getifaddrs(&ifa);
    int i = 0;
    char host[16];
    while (ifa) {
        s = getnameinfo(ifa->ifa_addr, sizeof (struct sockaddr_in), host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
        if (ifa->ifa_addr && ifa->ifa_addr->sa_family == AF_INET) {
            printf("[%d]Interface : <%s>\n", i, ifa->ifa_name);
            printf("\t  Address : <%s>\n", host);
            i++;
        }

        ifa = ifa->ifa_next;
    }
    freeifaddrs(ifa);
    int res;
    scanf("%d", &res);
    getifaddrs(&ifa);
    i = 0;
    while (ifa) {
        s = getnameinfo(ifa->ifa_addr, sizeof (struct sockaddr_in), host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
        if (ifa->ifa_addr && ifa->ifa_addr->sa_family == AF_INET) {
            if (i == res) {
                strcpy(ipLoc, host);

            }
            i++;
        }

        ifa = ifa->ifa_next;
    }
    freeifaddrs(ifa);
}

/* Si ho creieu convenient, feu altres funcions EXTERNES                  */

/* Definicio de funcions INTERNES, és a dir, d'aquelles que es faran      */
/* servir només en aquest mateix fitxer.                                  */
