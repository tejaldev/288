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
#define BUFFER_SIZE 5

typedef struct {
    float distance;
    int angle;
    int radial_width;
} Vector;

float filter_value(float filter_values[]) {
    int i;
    float min = 999;
    int min_index;
    float max = 0;
    int max_index;
    float average = 0;
    float total = 0;
    for (i=0;i<BUFFER_SIZE;i++){
        if (filter_values[i] < min){
            min = filter_values[i];
            min_index = i;
        }
        else if(filter_values[i] > max){
            max = filter_values[i];
            max_index = i;
        }
    }
    for (i = 0; i<BUFFER_SIZE;i++){
        if (i != min_index){
            if (i != max_index){
                total += filter_values[i];
            }
        }


    }
    average = total / (BUFFER_SIZE-2);
    return average;

}


    int detect_object(float curr_distance){
        int cutoff = 50;

        if(curr_distance < cutoff){
            return 1;
        }
        else{
            return 0;
        }
    }

    void locate_objects(Vector *rising, Vector *falling, Vector *center_object, int falling_count, int rising_count, int right_bound, int left_bound, int skip){
        int center;
        int i = 0;
        int j = 0;
        skip = 0;
        int radial_width=0;
        if (falling_count != 0){
            while (i < falling_count && j < rising_count) {
                if(rising[j].angle < 6 && falling[i].angle > 174){
                    skip++;
                }
                else if((rising[j].angle - falling[i].angle) < 10){
                      skip++;
                    }
                else{


                   center = (falling[i].angle + rising[j].angle) / 2;
                   center_object[i-skip].angle = center;
                   center_object[i-skip].distance =  falling[i-skip].distance;

                   //angular width:

                   radial_width= rising[j].angle-falling[i].angle;
                   center_object[i-skip].radial_width = radial_width;
                }


                   i++;
                   j++;

               }

               if (i < falling_count) {
                   center = (falling[i-1].angle + left_bound) / 2;
                   center_object[i-skip].angle = center;
                   center_object[i-skip].distance = rising[i-1].distance;
                   radial_width= left_bound-falling[i-1].angle;
                   center_object[i-skip].radial_width = radial_width;

               }
    }
    }

    oi_t *sensor_data;

    void scanning_initialize() {
        //timer_init(); // Initialize Timer, needed before any LCD screen functions can be called
            //                // and enables time functions (e.g. timer_waitMillis)
            //
                //lcd_init();   // Initialize the LCD screen.  This also clears the screen.
                cyBot_uart_init();
                //cyBOT_init_Scan(0b0111);
                //cyBOT_SERVO_cal();
                //9Right:253750
                //9Left:1251250
                //8Right: 332500
                //8Left: 1335250
                //2Right: 243250
                //2Left: 1225000
                right_calibration_value = 243250;
                left_calibration_value = 1225000;
                sensor_data = oi_alloc();

                oi_init(sensor_data);
                oi_setWheels(0,0);
    }

    void perform_scan(int right_bound, int left_bound, Vector *falling_edge, Vector *rising_edge, Vector *center_object) {
    cyBOT_Scan_t TEMP = {0, 0};
    float curr_value = 0.0;
    float value_filtered = 0.0;
    float buffer_values[BUFFER_SIZE];
    int value_index = 0;
    int prev_detect = 0;
    int curr_detect = 0;
    char message[50];
    int falling_index = 0;
    int rising_index = 0;
    //int skip = 0;

    // Initialize the buffer with values from the starting position
    int initial;
    for (initial = 0; initial < BUFFER_SIZE; initial++) {
        cyBOT_Scan(right_bound, &TEMP);
        buffer_values[initial] = TEMP.sound_dist;
    }

    // Perform the scan
    int angle;
    int i;
    for (angle = right_bound; angle <= left_bound; angle += 1) {
        cyBOT_Scan(angle, &TEMP);

        // Update buffer with current reading
        curr_value = TEMP.sound_dist;
        value_index = (angle - right_bound) % BUFFER_SIZE;
        buffer_values[value_index] = curr_value;

        // Filter the value
        value_filtered = filter_value(buffer_values);

        // Log scan data
        sprintf(message, "%f cm at %d deg | %f\n\r", value_filtered, angle, curr_value);
        for (i = 0; message[i] != '\0'; i++) {
            cyBot_sendByte(message[i]);
        }

        // Detect objects (rising and falling edges)
        prev_detect = curr_detect;
        curr_detect = detect_object(value_filtered);

        if (curr_detect && !prev_detect) {
            falling_edge[falling_index].angle = angle;
            falling_edge[falling_index].distance = value_filtered;
            falling_index++;
        } else if (!curr_detect && prev_detect) {
            rising_edge[rising_index].angle = angle;
            rising_edge[rising_index].distance = value_filtered;
            rising_index++;
        }
    }
    }

int not_main (void) {

    scanning_initialize();


        //cyBOT_Scan_t TEMP = {0,0};
                int right_bound;
                int left_bound;

                //float curr_value = 0.0;
                //float value_filtered = 0.0;

                //float buffer_values[7];
                //int value_index = 0;
                //int prev_detect = 0;
                //int curr_detect = 0;
                char message[50];
                Vector falling_edge[10];
                int falling_index = 0;
                Vector rising_edge[10];
                int rising_index = 0;
                Vector center_object[10];
                int num_objects;
                //int buffer_size;
                int min;
                int min_index;
                int run = 1;
                int skip;




        while(run){
            skip = 0;
            min = 999;
            min_index = 0;
        //buffer_size = 5;
        right_bound = 0;
        left_bound = 180;

        //curr_value = 0.0;
        //value_filtered = 0.0;
        //prev_detect = 0;
        //curr_detect = 0;
        falling_index = 0;
        rising_index = 0;
        num_objects = 0;
        memset(center_object, 0, sizeof(center_object));

      char data = cyBot_getByte_blocking();


      //int angle;


      if (data == 'm'){
          data = '\0';

//          int initial;
//          for (initial = 0; initial < BUFFER_SIZE; initial++){
//                    cyBOT_Scan(right_bound, &TEMP);
//                    value_index = initial;
//                    buffer_values[value_index] = TEMP.sound_dist;
//                }


          perform_scan(0, 180, falling_edge, rising_edge, center_object);
//      for( angle=right_bound;angle<=left_bound;angle += 1){
//
//
//      cyBOT_Scan(angle, &TEMP);
//
//
//      curr_value = TEMP.sound_dist;
//      value_index = (angle-right_bound)%BUFFER_SIZE;
//      buffer_values[value_index] = TEMP.sound_dist;
//
//      value_filtered = filter_value(buffer_values);
//
//      sprintf(message, "%f cm at %d deg | %f\n\r", value_filtered, angle, curr_value);
//
//      prev_detect = curr_detect;
//      curr_detect = detect_object(value_filtered);
//      if (curr_detect && !prev_detect){
//         falling_edge[falling_index].angle = angle;
//         falling_edge[falling_index].distance = value_filtered;
//         falling_index++;
//      }
//      else if(!curr_detect && prev_detect){
//          rising_edge[rising_index].angle = angle;
//          rising_edge[rising_index].distance = value_filtered;
//          rising_index++;
//      }
//
//
//      int i;
//            for (i = 0; message[i] != '\0'; i++){
//                cyBot_sendByte(message[i]);
//
//            }
//
//      }

      locate_objects(rising_edge, falling_edge, center_object, falling_index, rising_index, right_bound, left_bound, skip);
      int i;
      int j;
      for(i=0;i<(sizeof(center_object) / sizeof(center_object[0]));i++){
      if (center_object[i].distance != 0){
          num_objects++;
      }
      }

      for (j=0;j<num_objects;j++){
      sprintf(message, "Object %d: %f cm at %d deg with angular width %d deg\n\r",j , center_object[j].distance, center_object[j].angle, center_object[j].radial_width);
                  for (i = 0; message[i] != '\0'; i++){
                      cyBot_sendByte(message[i]);

                  }
      }

      min = 999;
      min_index = 0;


      for (j=0;j<num_objects;j++){
          if (center_object[j].radial_width < min){
              min = center_object[j].radial_width;
              min_index = j;
          }
      }
      if (center_object[min_index].angle > 90){
          sprintf(message, "Turning  left %d degrees towards Object %d\n\r",(center_object[min_index].angle - 90), min_index);
                                          for (i = 0; message[i] != '\0'; i++){
                                              cyBot_sendByte(message[i]);
                                          }
          turn_left(sensor_data, (center_object[min_index].angle - 90));

      }
      else {
          sprintf(message, "Turning right %d degrees towards Object %d\n\r",(center_object[min_index].angle - 90), min_index);
                                                        for (i = 0; message[i] != '\0'; i++){
                                                            cyBot_sendByte(message[i]);
                                                        }
          turn_right(sensor_data, (center_object[min_index].angle - 90));

      }
      sprintf(message, "Moving %f mm towards Object %d\n\r",(center_object[min_index].distance - 10) * 10, min_index);
                                                              for (i = 0; message[i] != '\0'; i++){
                                                                  cyBot_sendByte(message[i]);
                                                              }
      move_forward(sensor_data, (center_object[min_index].distance - 10) * 10);

      }

      else if(data == 'q'){
          run = 0;
      }

      //oi_free(sensor_data);
      }
        oi_free(sensor_data);
return 1;
}

