#ifndef INC_COMMRF_H
#define INC_COMMRF_H

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//                         	Function prototypes                        //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

void construire_trame_envoie(char *trame, char *donnee8);
char rf_envoie(char *donnee8, char *tampon);
char rf_extraction(char *tampon, char *donnee);
char rf_gerer_RX(char *tampon, char *donnee);

#endif
