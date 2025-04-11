#include <lcd.h>
#include <Timer.h>
#include <open_interface.h>
#include <uart-interrupt.h>
#include <stdlib.h>
#define PI 3.1415926



typedef struct pos {
    float x;
    float y;
} pos_t;

float move_forward(oi_t *sensor, int centimeters);

void turn_cclockwise(oi_t *sensor, double degrees);

void turn_clockwise(oi_t *sensor, double degrees);

float move_backward(oi_t *sensor, int degrees);

void encounter_obstacle_right(oi_t *sensor, pos_t *delta);

void encounter_obstacle_left(oi_t *sensor, pos_t *delta);




float move_forward(oi_t *sensor, int centimeters){
        double sum = 0;
        char str[20];
        float angle = 0;
        float radius = centimeters;

        pos_t delta;
        oi_setWheels(100, 100); // move forward; full speed
        while (sum < (centimeters) * 10 ) {
            delta.x = 0;
            delta.y = 0;
            oi_update(sensor);
            radius = centimeters - sum /10;
            sprintf(str, "\n\r%f", radius);
            uart_sendStr(str);
            if (sensor->bumpRight) {

                encounter_obstacle_right(sensor, &delta);
                oi_update(sensor);
                sum -= delta.y;
                radius = centimeters - sum /10;
                angle = (atan2(radius*10, delta.x)) * 180 / PI;
                turn_clockwise(sensor, 90 - (atan2(radius*10, delta.x)) * 180 / PI);

                sprintf(str, "\n\r%f, turning %f, delta.x %f", radius, angle, delta.x);
                uart_sendStr(str);

                oi_setWheels(100, 100);
            }

            if (sensor->bumpLeft) {
                encounter_obstacle_left(sensor, &delta);
                oi_update(sensor);
                sum -= delta.y;
                radius = centimeters - sum /10;
                turn_cclockwise(sensor, -90 + (atan2(radius*10, delta.x)) * 180 / PI);
                angle = (atan2(radius*10, delta.x)) * 180 / PI;
                sprintf(str, "\n\r%f, turning %f, delta.x %f", radius, angle, delta.x);
                uart_sendStr(str);
                oi_setWheels(100, 100);
            }

            sum += sensor->distance;
        }
        oi_setWheels(0, 0); // stop
        return sum;
}

//void autoMoveForward(oi_t *sensor, int object_angle, int centimeters, double x_pos, double y_pos){
//
//    double des_dx = cos((object_angle * 3.14) / 180) * centimeters;
//    double des_dy = sin((object_angle * 3.14) / 180) * centimeters;
//    double final_x = (des_dx + x_pos) * 10;
//    double final_y = (des_dy + y_pos) * 10;
//
//    char str[20];
//    float temp1;
//    float temp2;
//    int angle = (atan2((des_dy-y_pos), (des_dx-x_pos))*180/3.14);
//    int angleFrom90 = 90 - (atan2((des_dy-y_pos), (des_dx-x_pos))*180/3.14);
//
//    if (angleFrom90 >= 0) {
//        turn_clockwise(sensor, angleFrom90);
//    }
//    else {
//        turn_cclockwise(sensor, angleFrom90 * -1);
//    }
//
//    oi_setWheels(100, 100);
//    while ((abs((x_pos * 10) - final_x) > 100) && (abs((y_pos * 10) - final_y) > 100)) {
//        oi_update(sensor);
//
//
//
//
//
//
//
////
////        if ( ( abs( (x_pos * 10) - final_x) > 100) && ( abs( (y_pos * 10) - final_y) > 100)){
////            angle = (atan2((des_dy-y_pos)/(des_dx-x_pos))*180/3.14);
////            angleFrom90 = 90 - (atan2((des_dy-y_pos)/(des_dx-x_pos))*180/3.14);
////
////                if (angleFrom90 >= 0) {
////                        turn_clockwise(sensor, angleFrom90);
////                    }
////                    else {
////                        turn_cclockwise(sensor, angleFrom90 * -1);
////                    }
////        }
//
//
//
//                    if (sensor->bumpRight) {
//                        encounter_obstacle_right(sensor);
//                        oi_update(sensor);
//                        x_pos -= (15 * cos(angle)) /10;
//                        y_pos -= (15 * sin(angle)) /10;
//                        x_pos += (25 * cos(angle+90)) /10;
//                        y_pos += (25 * sin(angle+90)) /10;
//                        oi_setWheels(100, 100);
//                        sprintf(str, "\n\r%f, %f", temp1, temp2);
//                        uart_sendStr(str);
//                    }
//
//                    if (sensor->bumpLeft) {
//                        encounter_obstacle_left(sensor);
//                        oi_update(sensor);
//                        x_pos -= (15 * cos(angle)) /10;
//                        y_pos -= (15 * sin(angle)) /10;
//                        x_pos += (25 * cos(angle-90)) /10;
//                        y_pos += (25 * sin(angle-90)) /10;
//                        oi_setWheels(100, 100);
//                        sprintf(str, "\n\r%f, %f", temp1, temp2);
//                        uart_sendStr(str);
//                    }
//
//                    x_pos += (sensor->distance * cos((object_angle * 3.14) / 180)) /10;
//                    y_pos += (sensor->distance * sin((object_angle * 3.14) / 180)) /10;
//
//                    temp1 = ( abs( (x_pos * 10) - final_x))/10;
//                    temp2 = ( abs( (y_pos * 10) - final_y))/10;
//
//                    sprintf(str, "\n\r%f, %f", temp1, temp2);
//                    uart_sendStr(str);
//    }
//            if((abs((x_pos * 10) - final_x) > 100) || (abs((y_pos * 10) - final_y) > 100)){
//                if (abs((x_pos * 10) - final_x) <= 100) {
//                                        if (final_y > y_pos) {
//                                            turn_cclockwise(sensor, 90);
//                                        } else {
//                                            turn_clockwise(sensor, 90);
//                                        }
//                                    } else if (abs((y_pos * 10) - final_y) <= 100) {
//                                        if (final_x > x_pos) {
//                                            turn_clockwise(sensor, 90);
//                                        } else {
//                                            turn_cclockwise(sensor, 90);
//                                        }
//                                    }
//            }
//
//
//
//                    oi_setWheels(100, 100);
//                    while ((abs((x_pos * 10) - final_x) > 100) || (abs((y_pos * 10) - final_y) > 100)) {
//                        oi_update(sensor);
//
//                        // Update position
//                        x_pos += (sensor->distance * cos((object_angle * 3.14) / 180)) / 10;
//                        y_pos += (sensor->distance * sin((object_angle * 3.14) / 180)) / 10;
//
//                        temp1 = abs((x_pos * 10) - final_x) / 10;
//                        temp2 = abs((y_pos * 10) - final_y) / 10;
//
//                        sprintf(str, "\n\rFinal Move  %f,  %f", temp1, temp2);
//                        uart_sendStr(str);
//                    }
//
//
//        //sprintf(str, "\n\rAngle: %d, Distance: %d, Final X: %f, Final Y: %f", object_angle, centimeters, final_x/10, final_y/10);
//        //uart_sendStr(str);
////         while ( ( abs( (x_pos * 10) - final_x) > 100) || ( abs( (y_pos * 10) - final_y) > 100)){
////            oi_update(sensor);
////
////
////            if (sensor->bumpRight) {
////                encounter_obstacle_right(sensor);
////                oi_update(sensor);
////                centimeters += 15;
////                oi_setWheels(100, 100);
////            }
////
////            if (sensor->bumpLeft) {
////                encounter_obstacle_left(sensor);
////                oi_update(sensor);
////                centimeters += 15;
////                oi_setWheels(100, 100);
////            }
////
////            x_pos += (sensor->distance * cos((object_angle * 3.14) / 180)) /10;
////            y_pos += (sensor->distance * sin((object_angle * 3.14) / 180)) /10;
////
////            temp1 = ( abs( (x_pos * 10) - final_x))/10;
////                        temp2 = ( abs( (y_pos * 10) - final_y))/10;
////
////                                                sprintf(str, "\n\r%f, %f", temp1, temp2);
////                                            uart_sendStr(str);
////        }
//
//
//
//
//                    //    float des_dx = cos(angle/180*3.14) * dist;
//                    //    float des_dy = sin(angle/180*3.14) * dist;
//
//
//
//
//
//    oi_setWheels(0, 0);
//    sprintf(str, "\n\r%f, %f done", temp1, temp2);
//    uart_sendStr(str);// stop
//    }



float move_backward(oi_t *sensor, int centimeters){
        double sum = 0;
        oi_setWheels(-100, -100); // move forward; full speed
        while (sum > centimeters * -10) {
            oi_update(sensor);
            sum += sensor->distance;
        }
        oi_setWheels(0, 0); // stop
        return sum;
}

void turn_cclockwise(oi_t *sensor, double degrees){
        double sum = 0;

        oi_setWheels(100, -100); // turn; full speed

         while (sum < degrees - 15.0) {
             oi_update(sensor);
             sum += sensor->angle;
         }

         oi_setWheels(0, 0); // stop
}

void turn_clockwise(oi_t *sensor, double degrees){
        double sum = 0;

        oi_setWheels(-100, 100); // turn; full speed

         while (sum > -1 * degrees + 15.0) {
             oi_update(sensor);
             sum += sensor->angle;
         }

         oi_setWheels(0, 0); // stop
}

void encounter_obstacle_right(oi_t *sensor, pos_t *delta) {
     delta->y = -move_backward(sensor, 15);
     turn_cclockwise(sensor, 90);
     delta->x = move_forward(sensor, 35);
     turn_clockwise(sensor, 90);

}

void encounter_obstacle_left(oi_t *sensor,pos_t *delta) {
     delta->y = -move_backward(sensor, 15);
     turn_clockwise(sensor, 90);
     delta->x = -move_forward(sensor, 35);
     turn_cclockwise(sensor, 90);
}

