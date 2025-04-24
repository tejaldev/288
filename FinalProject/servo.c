
#include "servo.h"

unsigned long pwm_period;


void servo_init(void) {
    SYSCTL_RCGCGPIO_R |= 0b000010;
    while((SYSCTL_RCGCGPIO_R & 0b000010) == 0);
    //Configure GPIO alternate function to timer 1B
    GPIO_PORTB_AFSEL_R |= 0x20;
    GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R & 0xFFF0FFFFF) | 0x00700000;
    GPIO_PORTB_DEN_R |= 0x20;

        // Configure and enable the timer

    SYSCTL_RCGCTIMER_R |= 0b000010;
    while((SYSCTL_RCGCTIMER_R & 0b000010) == 0);

    TIMER1_CTL_R &= ~0x100; //Disable the timer while configuring

    TIMER1_CFG_R |= 0x4; //Set to 16 bit timer
    TIMER1_TBMR_R |= 0x0A; //Turns on PWM mode and Periodic Timer Mode
    TIMER1_TBMR_R &= ~0x10; //Enable count down
    TIMER1_CTL_R &= ~0x4000; //PWM Output level is not inverted
    TIMER1_TBPR_R |= 0x04; //Load PR - Prescaler (262144)
    TIMER1_TBILR_R |= 0xE200; //Load ILR - Clock starting count (57856)
    TIMER1_CFG_R |= 0x4; //Set to 16 bit timer
    servo_move(90);
    TIMER1_CTL_R |= 0x100; //Re-enable the timer
}

void servo_move(uint16_t degrees) {
    if (degrees > 180){
        degrees = 180;
    }
    float deg_to_fraction = 1.7 * (degrees/180.0); //reconfigure 1.7 based on cybot
    float deg_in_time = 0.02 - (deg_to_fraction * 0.001);
    uint16_t clock_cycle = 57856 - (320000 - (deg_in_time * 16000000));

    // Update the TIMER1_TBMATCHR_R and TIMER1_TBPMR_R registers
    TIMER1_TBMATCHR_R = clock_cycle;
    TIMER1_TBPMR_R = 0x04;
}
