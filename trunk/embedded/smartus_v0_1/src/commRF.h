#ifndef INC_COMMRF_H
#define INC_COMMRF_H

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//                         	Function prototypes                        //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

void construire_trame_envoie(char *trame, char *donnee8);
//void inc_seq(char *num_seq);
char rf_envoie(char *donnee8);
char rf_valider_confirmation(char *trame);
unsigned rf_recherche_positif(char *RX);
unsigned rf_decalage_masque(unsigned decalage);
void rf_detection_trame(char *RX, unsigned *decalage, char *trame_complete, char *trame, unsigned *cnt);

#endif
