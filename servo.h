#ifndef SERVO_H_
#define SERVO_H_

#include <inc/tm4c123gh6pm.h>
#include <stdbool.h>
#include <stdint.h>
#include "driverlib/interrupt.h"



void servo_init(void);

void servo_move(uint16_t degrees);


#endif /* SERVO_H_ */
