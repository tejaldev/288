/// Simple 'Hello, world' program
/**
 * This program prints "Hello, world" to the LCD screen
 * @author Chad Nelson
 * @date 06/26/2012
 *
 * updated: phjones 9/3/2019
 * Description: Added timer_init call, and including Timer.h
 */



//Detect when there are at least 2 detections in a row. If so then all in a row are part of the same object and only print one object


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
} Vector;

float filter_value(float filter_values[]) {
    int i;
    float min = 999;
    int min_index;
    float max = 0;
    int max_index;
    float average = 0;
    float total = 0;
    for (i=0;i<7;i++){
        if (filter_values[i] < min){
            min = filter_values[i];
            min_index = i;
        }
        else if(filter_values[i] > max){
            max = filter_values[i];
            max_index = i;
        }
    }
    for (i = 0; i<7;i++){
        if (i != min_index){
            if (i != max_index){
                total += filter_values[i];
            }
        }


    }
    average = total / 5;
    return average;

}

//float calculate_average_distance(float distances[], int num_readings) {
//    float sum = 0.0;
//    int i;
//    for (i = 0; i < num_readings; i++) {
//        sum += distances[i];
//    }
//    return sum / num_readings;
//}

    int detect_object(float curr_distance){
        int cutoff = 50;

        if(curr_distance < cutoff){
            return 1;
        }
        else{
            return 0;
        }
    }
//
//
//        *num_detected = 0;
//        char message[50];
//        int j = 0;
//        float average_distance = calculate_average_distance(distances, num_readings);
//        sprintf(message, "Average distance is %f\n\r", average_distance);
//        for (j = 0; message[j] != '\0'; j++){
//                                    cyBot_sendByte(message[j]);
//                                }
//        for (j = 0; j < num_readings; j++) {
//            if(distances[j] != 0.0){
//            if (distances[j] < average_distance) {
//
//                detected_objects[*num_detected].distance = distances[j];
//                detected_objects[*num_detected].angle = angles[j];
//                (*num_detected)++;
//            }
//            }
//        }
//
//}
//    void print_detected_objects(Object detected_objects[], int num_detected) {
//        int i = 0;
//
//        char message[100];
//        //printf("Detected Objects:\n");
//        for (i = 0; i < num_detected; i++) {
//            sprintf(message, "Object %d: Distance = %.2f cm, Angle = %d deg\n\r", i + 1, detected_objects[i].distance, detected_objects[i].angle);
//            int j;
//                        for (j = 0; message[j] != '\0'; j++){
//                            cyBot_sendByte(message[j]);
//                        }
//        }
//    }

    void locate_objects(Vector *rising, Vector *falling, Vector *center_object, int falling_count, int rising_count, int right_bound, int left_bound){
        int center;
        int i = 0;
        int j = 0;
        if (falling_count != 0){
            while (i <= falling_count && j <= rising_count) {
                   center = (falling[i].angle + rising[j].angle) / 2;
                   center_object[i].angle = center;
                   center_object[i].distance = (rising[i].distance + falling[i].distance)/2;
                   i++;
                   j++;
               }

               if (i <= falling_count) {
                   center = (falling[i].angle + 180) / 2;
                   center_object[i].angle = center;
                   center_object[i].distance = (rising[i].distance + falling[i].distance)/2;
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
        cyBOT_Scan_t TEMP = {0,0};
                int right_bound;
                int left_bound;
                //int number_readings = 0;
                //number_readings = left_bound - right_bound;
                //float filtered_distances[number_readings];
                //float previous_value;
                float curr_value = 0.0;
                float value_filtered = 0.0;
                //filtered_distances[right_bound] = 0.0;
                float seven_values[7];
                int value_index = 0;
                int prev_detect = 0;
                int curr_detect = 0;
                char message[50];
                Vector falling_edge[10];
                int falling_index = 0;
                Vector rising_edge[10];
                int rising_index = 0;
                Vector center_object[10];





        while(1){
        //cyBOT_Scan_t TEMP = {0,0};
        right_bound = 0;
        left_bound = 180;
        //number_readings = 0;
        //number_readings = left_bound - right_bound;
        //filtered_distances[number_readings];
        //previous_value = 0.0;
        curr_value = 0.0;
        value_filtered = 0.0;
        prev_detect = 0;
        curr_detect = 0;
        falling_index = 0;
        rising_index = 0;

        //filtered_distances[right_bound] = 0.0;



      char data = cyBot_getByte();

      //lcd_putc(data);
      //lcd_printf("%c", &data);
//      sprintf(message, "We got a %f\n\r", TEMP.sound_dist);



      int angle;
//      int number_readings = 0;
//      number_readings = left_bound - right_bound +1;
//      float filtered_distances[number_readings];
      //int angles[number_readings];
      //Object detected_objects[200];
      //int num_detected;
//      int i;
//        for (i = 0; i < number_readings; i++) {
//             filtered_distances[i] = 0.0;
//             angles[i] = 0;
//        }

      if (data == 'm'){
          data = '\0';

          int initial;
          for (initial = 0; initial < 7; initial++){
                    cyBOT_Scan(right_bound, &TEMP);
                    value_index = initial;
                    seven_values[value_index] = TEMP.sound_dist;
                }


      for( angle=right_bound;angle<=left_bound;angle += 1){
          //float total = 0;
//          float average = 0;

      //previous_value = value_filtered;

      cyBOT_Scan(angle, &TEMP);
      //total += TEMP.sound_dist;
      //cyBOT_Scan(j, &TEMP);
      //total += TEMP.sound_dist;
//      cyBOT_Scan(j, &TEMP);
//      total += TEMP.sound_dist;
//
      //average = total / 2;

      curr_value = TEMP.sound_dist;
      value_index = (angle-right_bound)%7;
      seven_values[value_index] = TEMP.sound_dist;
//      curr_value = average;
      value_filtered = filter_value(seven_values);

      sprintf(message, "%f cm at %d deg | %f\n\r", value_filtered, angle, curr_value);

      prev_detect = curr_detect;
      curr_detect = detect_object(value_filtered);
      if (curr_detect && !prev_detect){
         falling_edge[falling_index].angle = angle;
         falling_edge[falling_index].distance = value_filtered;
         falling_index++;
      }
      else if(!curr_detect && prev_detect){
          rising_edge[rising_index].angle = angle;
          rising_edge[rising_index].distance = value_filtered;
          rising_index++;
      }


      //filtered_distances[j-right_bound] = value_filtered;
      //angles[j-right_bound] = j;
      int i;
            for (i = 0; message[i] != '\0'; i++){
                cyBot_sendByte(message[i]);

            }

      }

      locate_objects(rising_edge, falling_edge, center_object, falling_index, rising_index, right_bound, left_bound);
      int i;
      int j;
      for (j=0;j<falling_index-1;j++){
      sprintf(message, "Object %d: %f cm at %d deg\n\r",j , center_object[j].distance, center_object[j].angle);
                  for (i = 0; message[i] != '\0'; i++){
                      cyBot_sendByte(message[i]);

                  }
      }
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
////    for (i=20;i>=0;i--){
//////  lcd_setCursorPos(i,0);
////    // Print "Hello, world" on the LCD
////
//////  lcd_putc(str[index]);
//////  buffStr = '\0'
//////  strncpy(buffStr, ' ')
////    strncpy(buffStr, str, 20-i);
////    lcd_printf(buffStr);
////    timer_waitMicros(300);
//
////    int i=0;
////    char str[] = "                   Microcontrollers are lots of fun!";
//
//  char message[] = "                   Microcontrollers are lots of fun!";
//
//
//      while(1){
//          lcd_rotatingBanner(message);
//      }
//
//
//
//
//
//  // lcd_puts("Hello, world"); // Replace lcd_printf with lcd_puts
//        // step through in debug mode and explain to TA how it works
//
//  // NOTE: It is recommended that you use only lcd_init(), lcd_printf(), lcd_putc, and lcd_puts from lcd.h.
//       // NOTE: For time functions, see Timer.h
//
//
//  return 0;
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
