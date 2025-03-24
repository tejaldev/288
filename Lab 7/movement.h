#ifndef MOVEMENT_H_
#define MOVEMENT_H_

#include <lcd.h>
#include <Timer.h>
#include <open_interface.h>

void move_forward(oi_t *sensor, int centimeters);

void turn_cclockwise(oi_t *sensor, double degrees);

void turn_clockwise(oi_t *sensor, double degrees);

void move_backward(oi_t *sensor, int degrees);

void encounter_obstacle_right(oi_t *sensor);

void encounter_obstacle_left(oi_t *sensor);

#endif /* MOVEMENT_H_ */
