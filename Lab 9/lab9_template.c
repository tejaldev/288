/**
 * @file lab9_template.c
 * @author
 * Template file for CprE 288 Lab 9
 */

#include "Timer.h"
#include "lcd.h"
#include "ping.h"
#include "uart-interrupt.h"

// Uncomment or add any include directives that are needed

#warning "Possible unimplemented functions"
#define REPLACEME 0

int main(void) {
	timer_init(); // Must be called before lcd_init(), which uses timer functions
	lcd_init();
	uart_interrupt_init();
	ping_init();



	while(1)
	{
//	    char str1[50];
//	    char str2[50];
//	    float dcc = 0;
//	    int overflow = 0;
////      ping_trigger();
//      timer_waitMillis(200);
//      dcc = ping_getDistance();
//      overflow = detect_overflow();
//      sprintf(str1, "Pulse width: %.0f | Overflow: %d\n\r", dcc, overflow);
//      sprintf(str2, "Pulse width: %.0f\nOverflow: %d\n", dcc, overflow);
//      uart_sendStr(str1);
//      lcd_printf(str2);


	    char str1[50];
	    char str2[50];
	    float dcc = 0;
	    int overflow = 0;
	        //        ping_trigger();
	    timer_waitMillis(200);
	    dcc = ping_getDistance();
	    overflow = detect_overflow();
	    sprintf(str1, "Distance: %.0f | Overflow: %d\n\r", dcc, overflow);
	    sprintf(str2, "Distance: %.0f\nOverflow: %d\n", dcc, overflow);
	    uart_sendStr(str1);
	    lcd_printf(str2);


	}

}
