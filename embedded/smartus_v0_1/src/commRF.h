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
char rf_recherche_positif(char *RX);
char rf_decalage_masque(char *decalage);
void rf_detection_trame(char *RX, char *decalage, char *trame_complete, char *trame, char *cnt);

#endif
