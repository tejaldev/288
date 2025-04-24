#include "open_interface.h"

double move_forward(oi_t *sensor_data, double distance_mm) {


    double sum = 0; // distance member in oi_t struct is type double
    oi_setWheels(150, 150); //move forward at full speed
    while (sum < distance_mm) {
        oi_update(sensor_data);
        sum += sensor_data -> distance; // use -> notation since pointer
        lcd_printf("Forward: %.1f mm", sum);
        }
    oi_setWheels(0,0);
    return sum;
}

void move_backward(oi_t *sensor_data, double distance_mm) {


    double sum = 0; // distance member in oi_t struct is type double
    oi_setWheels(-50, -50); //move forward at full speed
    while (sum < distance_mm) {
        oi_update(sensor_data);
        sum -= sensor_data -> distance; // use -> notation since pointer
        lcd_printf("Backwards: %.1f mm", sum);
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
        lcd_printf("Total: %.1f mm", sum);
        if (sensor -> bumpLeft || sensor -> bumpRight) {
            oi_setWheels(0,0);
            return sum;
        }
    }
    oi_setWheels(0,0);
    return sum;
}
