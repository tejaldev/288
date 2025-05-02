/*
 * adc.h
 *
 *  Created on: Apr 24, 2025
 *      Author: nrudrud
 */

#ifndef ADC_H_
#define ADC_H_

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "Timer.h"
#include <inc/tm4c123gh6pm.h>

void ADC0_Init(void);

uint32_t ADC0_InSeq3(void);


#endif /* ADC_H_ */
