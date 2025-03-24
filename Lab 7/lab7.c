///**
// * lab6-interrupt_template.c
// *
// * Template file for CprE 288 Lab 6
// *
// * @author Diane Rover, 2/15/2020
// *
// */
//
//#include "Timer.h"
//#include "lcd.h"
//#include "cyBot_Scan.h"  // For scan sensors
////#include "cyBot_uart.h"
//#include "uart-interrupt.h"
////#include "scan_functions.c"
//
//// Uncomment or add any include directives that you want to use
//// #include "open_interface.h"
//// #include "movement.h"
//// #include "button.h"
//
//// Your code can use the global variables defined in uart-interrupt.c
//// They are declared with the extern qualifier in uart-interrupt.h, which makes the variables visible to this file.
//
//#warning "Possible unimplemented functions"
//#define REPLACEME 0
//
//
//int main() {
//
//    timer_init(); // Must be called before lcd_init(), which uses timer functions
////    lcd_init();
////    uart_interrupt_init();
////    uart_init();
//    cyBOT_init_Scan(0b0111);
//    cyBOT_SERVO_cal();
//    right_calibration_value = 311500;
//    left_calibration_value = 1319500;
//    // OPTIONAL
//    //assign a value to command_byte if you want to know whether that ASCII code is received
//    //note that command_byte is global shared variable read by the ISR
//    //for example, try using a tab character as a command from PuTTY
//
////    while(1)
////    {
////        int count;
////        int start = 0;
////        char c = '\0';
//        cyBOT_Scan_t scan;
////            count = 0;
////
////            if (start_flag){
////                start = 1;
////                start_flag =0;
////            }
////            while (start && count <180) {
////                //cyBOT_Scan(count, &scan);
////                count++;
////
////                if (stop_flag){
////                    start = 0;
////                    stop_flag =0;
////                }
//////perform scan(0,180)
//////            }
////      // YOUR CODE HERE
////            //first, try leaving this loop empty and see what happens
////            //then add code for your application
////            // OPTIONAL
////            //test and reset command_flag if your ISR is updating it
////            //for example, if the flag is 1, do something, like send a message to PuTTY or LCD, or stop a sensor scan, etc.
////            //be sure to reset command_flag so you don't keep responding to an old flag
////
////    }
//
//    return 0;
//}
