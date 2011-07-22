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

#endif
