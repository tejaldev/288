#include <lcd.h>
#include <Timer.h>
#include <open_interface.h>
#include <uart-interrupt.h>

void move_forward(oi_t *sensor, int centimeters);

void turn_cclockwise(oi_t *sensor, double degrees);

void turn_clockwise(oi_t *sensor, double degrees);

void move_backward(oi_t *sensor, int degrees);

void encounter_obstacle_right(oi_t *sensor);

void encounter_obstacle_left(oi_t *sensor);

void move_forward(oi_t *sensor, int centimeters){
        double sum = 0;
        oi_setWheels(300, 300); // move forward; full speed
        while (sum < centimeters * 10) {
            oi_update(sensor);

            if (sensor->bumpRight) {
                encounter_obstacle_right(sensor);
                oi_update(sensor);
                centimeters += 15;
                oi_setWheels(300, 300);
            }

            if (sensor->bumpLeft) {
                encounter_obstacle_left(sensor);
                oi_update(sensor);
                centimeters += 15;
                oi_setWheels(300, 300);
            }

            sum += sensor->distance;
        }
        oi_setWheels(0, 0); // stop
}

void move_backward(oi_t *sensor, int centimeters){
        double sum = 0;
        oi_setWheels(-100, -100); // move forward; full speed
        while (sum > centimeters * -10) {
            oi_update(sensor);
            sum += sensor->distance;
        }
        oi_setWheels(0, 0); // stop
}

void turn_cclockwise(oi_t *sensor, double degrees){
        double sum = 0;

        oi_setWheels(200, -200); // turn; full speed

         while (sum < degrees - 15.0) {
             oi_update(sensor);
             sum += sensor->angle;
         }

         oi_setWheels(0, 0); // stop
}

void turn_clockwise(oi_t *sensor, double degrees){
        double sum = 0;

        oi_setWheels(-200, 200); // turn; full speed

         while (sum > -1 * degrees + 15.0) {
             oi_update(sensor);
             sum += sensor->angle;
         }

         oi_setWheels(0, 0); // stop
}

void encounter_obstacle_right(oi_t *sensor) {
     move_backward(sensor, 15);
     turn_cclockwise(sensor, 90);
     move_forward(sensor, 25);
     turn_clockwise(sensor, 90);
}

void encounter_obstacle_left(oi_t *sensor) {
     move_backward(sensor, 15);
     turn_clockwise(sensor, 90);
     move_forward(sensor, 25);
     turn_cclockwise(sensor, 90);
}

