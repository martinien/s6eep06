#ifndef INC_PERIPH_H
#define INC_PERIPH_H


/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//                         	Function prototypes                            //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

void define_io(void);
void peripheral_pin_select(void);
void setup_adc(void);
void setup_timer1(void);
void radio_dir(unsigned char dir);
void delay_us(unsigned int t);

#endif
