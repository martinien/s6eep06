#ifndef INC_COMMRF_H
#define INC_COMMRF_H

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//                         	Function prototypes                        //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

void construire_trame(char *trame, char *data8, char *num_seq);
void inc_seq(char *num_seq);
char rf_envoie(char *data8, char *num_seq);

#endif
