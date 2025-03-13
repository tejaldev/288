/*
 * movement.c
 *
 *  Created on: Feb 6, 2025
 *      Author: tejal
 */
#include "movement.h"
#include "open_interface.h"


double move_forward(oi_t *sensor_data, double distance_mm){
    double sum = 0;
    oi_setWheels(100,100);

        while (sum < distance_mm){
            oi_setWheels(100,100);

            if(sensor_data -> bumpLeft){
                sum += go_around_right(sensor_data);
            }
            else if(sensor_data -> bumpRight){
                sum += go_around_left(sensor_data);
            }
            else{
            oi_update(sensor_data);
            sum += sensor_data -> distance;
        }}

        oi_setWheels(0,0);
return sum;
}
double move_backward(oi_t *sensor_data, double distance_mm){
    double sum = 0;
    oi_setWheels(-100,-100);
        while (sum > distance_mm){
            oi_update(sensor_data);
            sum += sensor_data -> distance;
        }
     oi_setWheels(0,0);

return sum;
}


double turn_right(oi_t *sensor_data, double degrees){
    double sum = 0;
    oi_setWheels(-100,100);
        while(sum > degrees){
            oi_update(sensor_data);
            sum += sensor_data -> angle;

        }
        oi_setWheels(0,0);

return sum;
}
double turn_left(oi_t *sensor_data, double degrees){
    double sum = 0;
    oi_setWheels(100,-100);
        while(sum < degrees){
            oi_update(sensor_data);
            sum += sensor_data -> angle;

        }
        oi_setWheels(0,0);

return sum;
}
double go_around_right(oi_t *sensor_data){
    double sum = 0;
    sum += move_backward(sensor_data, -150);
    turn_right(sensor_data, -85);
    move_forward(sensor_data, 250);
    turn_left(sensor_data, 85);
    return sum;
}
double go_around_left(oi_t *sensor_data){
    double sum = 0;
    sum += move_backward(sensor_data, -150);
    turn_left(sensor_data, 85);
    move_forward(sensor_data, 250);
    turn_right(sensor_data, -85);
    return sum;
}
