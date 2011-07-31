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

//Fct JF
unsigned char get_flag(unsigned char flag);
char get_offset(unsigned char msb, unsigned char lsb, unsigned char ref);
void clean_buffer(unsigned char offset, char buf_length);
unsigned int get_rssi(void);

#endif
