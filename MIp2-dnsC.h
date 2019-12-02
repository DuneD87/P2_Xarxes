/**************************************************************************/
/*                                                                        */
/* P2 - MI amb sockets TCP/IP - Part II                                   */
/* Fitxer capçalera de dnsC.c                                             */
/*                                                                        */
/* Autors: X, Y                                                           */
/*                                                                        */
/**************************************************************************/

/* Declaració de funcions EXTERNES de dnsC.c, és a dir, d'aquelles        */
/* funcions que es faran servir en un altre fitxer extern a dnsC.c,       */
/* p.e., int DNSc_FuncioExterna(arg1, arg2...) { }                        */
/* El fitxer extern farà un #include del fitxer dnsC.h a l'inici, i       */
/* així les funcions seran conegudes en ell.                              */
/* En termes de capes de l'aplicació, aquest conjunt de funcions          */
/* EXTERNES formen la interfície de la capa DNS, la part del client       */

int DNSc_ResolDNSaIP(const char *NomDNS, char *IP);