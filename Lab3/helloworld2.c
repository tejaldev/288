/// Simple 'Hello, world' program
/**
 * This program prints "Hello, world" to the LCD screen
 * @author Chad Nelson
 * @date 06/26/2012
 *
 * updated: phjones 9/3/2019
 * Description: Added timer_init call, and including Timer.h
 */

#include "Timer.h"
#include "lcd.h"
#include <string.h>
#include "movement.h"
#include "open_interface.h"
#include "cyBot_uart.h"
#include "cyBot_Scan.h"
int main (void) {

    timer_init(); // Initialize Timer, needed before any LCD screen functions can be called
    //                // and enables time functions (e.g. timer_waitMillis)
    //
        lcd_init();   // Initialize the LCD screen.  This also clears the screen.
        cyBot_uart_init();
        cyBOT_init_Scan(0b0111);
    //oi_t *sensor_data = oi_alloc();
    //oi_init(sensor_data);
//
//    int i;
//    for(i=0;i<=4;i++){
//    move_forward(sensor_data, 500);
//    turn_right(sensor_data, -90);
//    }
    //oi_setWheels(0,0);
//    move_forward(sensor_data, 500);
//    turn_right(sensor_data, -90);
//    move_forward(sensor_data, 500);
//        turn_right(sensor_data, -90);
//        move_forward(sensor_data, 500);
//            turn_right(sensor_data, -90);
//            move_forward(sensor_data, 500);
//                turn_right(sensor_data, -90);
      //double des_drv_dist = 2000;


      //move_forward(sensor_data, des_drv_dist);
        cyBOT_Scan_t TEMP = {0,0};
        cyBOT_Scan(90, &TEMP);



      while(1){
      //char data = cyBot_getByte();
      char message[20];
      //lcd_putc(data);
      //lcd_printf("%c", &data);
      sprintf(message, "We got a %f\n\r", TEMP.sound_dist);
      cyBOT_Scan(90, &TEMP);
      int i;
      for (i = 0; message[i] != '\0'; i++){
          cyBot_sendByte(message[i]);
      }


      }
    //oi_free(sensor_data);

}
//    for(i=0;i<=2;i++){
//        move_forward(sensor_data, 500);
//        turn_left(sensor_data, 90);
//        }














////    int i;
////
////    char str[] = "                   Microcontrollers are lots of fun!";
////    char buffStr[21];
//////    int h;
//////    for(h=0;h<10;h++){
//////        lcd_sendCommand(0x1C);
//////    }
//////
//////    lcd_putc('a');
////
////
////	for (i=20;i>=0;i--){
//////	lcd_setCursorPos(i,0);
////	// Print "Hello, world" on the LCD
////
//////	lcd_putc(str[index]);
//////	buffStr = '\0'
//////	strncpy(buffStr, ' ')
////	strncpy(buffStr, str, 20-i);
////	lcd_printf(buffStr);
////	timer_waitMicros(300);
//
////	int i=0;
////    char str[] = "                   Microcontrollers are lots of fun!";
//
//	char message[] = "                   Microcontrollers are lots of fun!";
//
//
//	    while(1){
//	        lcd_rotatingBanner(message);
//	    }
//
//
//
//
//
//	// lcd_puts("Hello, world"); // Replace lcd_printf with lcd_puts
//        // step through in debug mode and explain to TA how it works
//
//	// NOTE: It is recommended that you use only lcd_init(), lcd_printf(), lcd_putc, and lcd_puts from lcd.h.
//       // NOTE: For time functions, see Timer.h
//
//
//	return 0;
//}
//void lcd_rotatingBanner(char *message){
//
//    int messageLength = strlen(message);
//            char displayBuffer[21];
//            int i,j;
//for ( i = 0; i < messageLength; i++) {
//
//              for ( j = 0; j < 20; j++) {
//                  int messageIndex = (i + j) % messageLength;
//                  if (i + j < messageLength) {
//                      displayBuffer[j] = message[messageIndex];
//                  } else {
//                      displayBuffer[j] = ' ';
//                  }
//              }
//              displayBuffer[20] = '\0';
//
//
//              lcd_printf("%s", displayBuffer);
//
//
//              timer_waitMillis(300);
//          }
//
//
//          lcd_clear();
//          timer_waitMillis(300);
//
//}
