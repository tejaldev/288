#include "open_interface.h"

void move_forward(oi_t *sensor_data, double distance_mm) {


    double sum = 0; // distance member in oi_t struct is type double
    oi_setWheels(150, 150); //move forward at full speed
    while (sum < distance_mm) {
        oi_update(sensor_data);
        sum += sensor_data -> distance; // use -> notation since pointer
        }
    oi_setWheels(0,0);
}

void move_backward(oi_t *sensor_data, double distance_mm) {


    double sum = 0; // distance member in oi_t struct is type double
    oi_setWheels(-50, -50); //move forward at full speed
    while (sum < distance_mm) {
        oi_update(sensor_data);
        sum -= sensor_data -> distance; // use -> notation since pointer
        }
    oi_setWheels(0,0);
}


void turn_right(oi_t *sensor, double degrees) {
    double sum = 0;
    oi_setWheels(-25, 25);
    while (sum < degrees) {
        oi_update(sensor);
        sum -= sensor -> angle;
    }
    oi_setWheels(0,0);
}


void turn_left(oi_t *sensor, double degrees) {
    double sum = 0;
    oi_setWheels(25, -25);
    while (sum < degrees) {
       oi_update(sensor);
       sum += sensor -> angle;
    }
    oi_setWheels(0,0);
}

double collision_detection(oi_t *sensor, double distance_mm) {
    double sum = 0; // distance member in oi_t struct is type double
        oi_setWheels(150, 150); //move forward
        while (sum < distance_mm) {
            oi_update(sensor);
            sum += sensor -> distance; // use -> notation since pointer
            if (sensor -> bumpLeft) {
                move_backward(sensor, 150);
                turn_right(sensor, 90);
                move_forward(sensor, 250);
                turn_left(sensor, 90);
                oi_setWheels(150, 150);
                sum -= 150;
            }
            else if (sensor -> bumpRight) {
                move_backward(sensor, 150);
                turn_left(sensor, 90);
                move_forward(sensor, 250);
                turn_right(sensor, 90);
                oi_setWheels(150, 150);
                sum -= 150;
            }
            }
        oi_setWheels(0,0);
        return 0;
}
