/// Simple 'Hello, world' program
/**
 * This program prints "Hello, world" to the LCD screen
 * @author Chad Nelson
 * @date 06/26/2012
 *
 * updated: phjones 9/3/2019
 * Description: Added timer_init call, and including Timer.h
 */



Detect when there are at least 2 detections in a row. If so then all in a row are part of the same object and only print one object


#include "Timer.h"
#include "lcd.h"
#include <string.h>
#include "movement.h"
#include "open_interface.h"
#include "cyBot_uart.h"
#include "cyBot_Scan.h"

//#define SMOOTH_FACTOR 0.1;

typedef struct {
    float distance;
    int angle;
} Object;

float filter_value(float new_value, float previous_filtered_value) {
    if (previous_filtered_value == 0.0){
       return new_value;
    }
    else{
    float num = (.4 * new_value) + ((1 - .4) * previous_filtered_value);
    return num;
}
}
float calculate_average_distance(float distances[], int num_readings) {
    float sum = 0.0;
    int i;
    for (i = 0; i < num_readings; i++) {
        sum += distances[i];
    }
    return sum / num_readings;
}
    void detect_objects(float distances[], int angles[], int num_readings, Object detected_objects[], int *num_detected) {
        *num_detected = 0;
        char message[50];
        int j = 0;
        float average_distance = calculate_average_distance(distances, num_readings);
        sprintf(message, "Average distance is %f\n\r", average_distance);
        for (j = 0; message[j] != '\0'; j++){
                                    cyBot_sendByte(message[j]);
                                }
        for (j = 0; j < num_readings; j++) {
            if(distances[j] != 0.0){
            if (distances[j] < average_distance) {

                detected_objects[*num_detected].distance = distances[j];
                detected_objects[*num_detected].angle = angles[j];
                (*num_detected)++;
            }
            }
        }

}
    void print_detected_objects(Object detected_objects[], int num_detected) {
        int i = 0;

        char message[100];
        //printf("Detected Objects:\n");
        for (i = 0; i < num_detected; i++) {
            sprintf(message, "Object %d: Distance = %.2f cm, Angle = %d deg\n\r", i + 1, detected_objects[i].distance, detected_objects[i].angle);
            int j;
                        for (j = 0; message[j] != '\0'; j++){
                            cyBot_sendByte(message[j]);
                        }
        }
    }

int main (void) {

    timer_init(); // Initialize Timer, needed before any LCD screen functions can be called
    //                // and enables time functions (e.g. timer_waitMillis)
    //
        lcd_init();   // Initialize the LCD screen.  This also clears the screen.
        cyBot_uart_init();
        cyBOT_init_Scan(0b0111);
        right_calibration_value = 332500;
        left_calibration_value = 1335250;
        //cyBOT_SERVO_cal();
        // right: 332500
        //left: 1335250
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

//        cyBOT_Scan(90, &TEMP);
//
        while(1){
        cyBOT_Scan_t TEMP = {0,0};
        int right_bound = 30;
        int left_bound = 150;
        int number_readings = 0;
        number_readings = left_bound - right_bound;
        float filtered_distances[number_readings];
        float previous_value = 0.0;
        float curr_value = 0.0;
        float value_filtered = 0.0;
        //filtered_distances[right_bound] = 0.0;


      char data = cyBot_getByte();
      char message[50];
      //lcd_putc(data);
      //lcd_printf("%c", &data);
//      sprintf(message, "We got a %f\n\r", TEMP.sound_dist);



      int j;
//      int number_readings = 0;
//      number_readings = left_bound - right_bound +1;
//      float filtered_distances[number_readings];
      int angles[number_readings];
      Object detected_objects[200];
      int num_detected;
//      int i;
//        for (i = 0; i < number_readings; i++) {
//             filtered_distances[i] = 0.0;
//             angles[i] = 0;
//        }

      if (data == 'm'){
          data = '\0';

      for( j=right_bound;j<=left_bound;j += 1){
          //float total = 0;
//          float average = 0;
      previous_value = value_filtered;

          cyBOT_Scan(j, &TEMP);
      //total += TEMP.sound_dist;
      //cyBOT_Scan(j, &TEMP);
      //total += TEMP.sound_dist;
//      cyBOT_Scan(j, &TEMP);
//      total += TEMP.sound_dist;
//
      //average = total / 2;

      curr_value = TEMP.sound_dist;
//      curr_value = average;
      value_filtered = filter_value(curr_value, previous_value);

      sprintf(message, "%f cm at %d deg | %f\n\r", value_filtered, j, curr_value);
      filtered_distances[j-right_bound] = value_filtered;
      angles[j-right_bound] = j;
      int i;
            for (i = 0; message[i] != '\0'; i++){
                cyBot_sendByte(message[i]);

            }

      }
      detect_objects(filtered_distances, angles, number_readings, detected_objects, &num_detected);
      print_detected_objects(detected_objects, num_detected);
      }
//      int i;
//      for (i = 0; message[i] != '\0'; i++){
//          cyBot_sendByte(message[i]);
//
//      }


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
