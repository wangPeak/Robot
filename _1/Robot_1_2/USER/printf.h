
#ifndef __printf_H

#define __printf_H

 

#include "stm32f10x.h"

#include <stdio.h>

void printf_init(void);

int fputc(int ch,FILE *f);

 

#endif
