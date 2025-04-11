/*
 * lab10.c
 *
 *  Created on: Apr 10, 2025
 *      Author: collinvw
 */
#include "Timer.h"
#include "lcd.h"
#include "servo.h"
#include "uart-interrupt.h"

 void main(){
    timer_init(); // Must be called before lcd_init(), which uses timer functions
    lcd_init();
    uart_interrupt_init();
    //ping_init();
    servo_init();
    servo_move(0);
    while(1){
        servo_move(180);
//        timer_waitMicros(1000);
//        servo_move(40);
//        timer_waitMicros(1000);
//        servo_move(50);
//        timer_waitMicros(1000);
//        servo_move(00);
//        timer_waitMicros(1000);

    }
}
