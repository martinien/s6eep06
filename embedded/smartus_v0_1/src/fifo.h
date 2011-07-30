#ifndef INC_FIFO_H
#define INC_FIFO_H

#define FIFO_LENGTH	32

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//                         	Function prototypes                            //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

void fifo_init(void);
void fifo_add(unsigned char data, unsigned char reset);

#endif
