/*
 * servo.c
 *
 *  Created on: Apr 10, 2025
 *      Author: collinvw
 */

#include <inc/tm4c123gh6pm.h>
#include "ping.h"
#include "Timer.h"
#include "driverlib/interrupt.h"
#include "uart-interrupt.h"

servo_init(void){


    //Configure portB
    SYSCTL_RCGCGPIO_R |= 0b000010; //Enable clock on port b
    while ((SYSCTL_PRGPIO_R & 0b000010) == 0) {};



    GPIO_PORTB_AFSEL_R |= 0x20;
    GPIO_PORTB_PCTL_R |= (GPIO_PORTB_PCTL_R & ~0xF00000) | 0x700000;

    GPIO_PORTB_DEN_R |= 0x20;

    SYSCTL_RCGCTIMER_R |= 0b000010;
    while((SYSCTL_RCGCTIMER_R & 0b000010) == 0){};



    //Configure the timer
    TIMER1_CTL_R &= ~0x100; // Disable

    TIMER1_CFG_R = 0x4; // Configuration
//    TIMER1_TBMR_R |= (TIMER1_TBMR_R & ~0x4) | 0xA; // Timer mode
    TIMER1_TBMR_R |= 0xA; // Timer mode
    TIMER1_TBMR_R &= ~0x10; // count down
    TIMER1_CTL_R &= ~0x4000; // CHECK THIS IF VALUES ARE ERRONEOUS
    TIMER1_TBPR_R = 0x04; // Prescaler
    TIMER1_TBILR_R = 0xE200; //Timer start value in INTERVAL LOAD
    TIMER1_TBPMR_R = 0x04; // Prescale match
    TIMER1_TBMATCHR_R = 0x8440; //Match value
    TIMER1_CTL_R |= 0x100; // Enable




    // Configure and enable the timer
    //FOR REFERENCE
//    TIMER3_CTL_R &= ~0x100;
//    TIMER3_CFG_R = 0x4;
//    TIMER3_TBMR_R |= 0x07;
//    TIMER3_TBMR_R &= ~0x10;
//    TIMER3_CTL_R |= 0xC00;
//    TIMER3_TBPR_R = 0xFF;
//
//    TIMER3_TBILR_R = 0xFFFF;
//    TIMER3_IMR_R |= 0x400;
//    TIMER3_ICR_R |= 0x400;
//
//    TIMER3_CTL_R |= 0x100;
}
servo_move(uint16_t degrees){
    uint32_t pulse_width = (((16000 / 90) * degrees)/2) + 16000;
    uint32_t clock = (320000 - (pulse_width + 2000));
    uint32_t clock_match = clock & 0x0FFFF;
    uint32_t clock_pmr = (clock & 0xFF0000) >> 16;

    TIMER1_TBMATCHR_R = clock_match; //Match value
    TIMER1_TBPMR_R = clock_pmr; //Prescale Match
}
