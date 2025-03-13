///**
// * lab6_template.c
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
//#include "uart.h"
//
//
//// Uncomment or add any include directives that are needed
//// #include "open_interface.h"
//// #include "movement.h"
//// #include "button.h"
//
//
////#warning "Possible unimplemented functions"
//#define REPLACEME 0
//
//
//int main(void) {
//	timer_init(); // Must be called before lcd_init(), which uses timer functions
//	lcd_init();
//	uart_init();
//   cyBOT_init_Scan(0b0111);
//
//	// YOUR CODE HERE
//
//   int count = 0;
//   char c;
////   char output[21] = "";
//   int start = 0;
//   cyBOT_Scan_t scan;
//	while(1)
//	{
//	    count = 0;
//	    c = uart_receive_nonblocking();
//	    if (c == 'g'){
//	        start = 1;
//	    }
//	    while (start && count <180) {
//	        cyBOT_Scan(count, &scan);
//	        count++;
//	        c = uart_receive_nonblocking();
//	        if (c=='s'){
//	            start = 0;
//	        }
//
//	    }
//
//
//
//
//
//
//	}
//
//}
