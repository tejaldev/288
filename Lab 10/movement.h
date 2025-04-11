#ifndef MOVEMENT_H_
#define MOVEMENT_H_

#include <lcd.h>
#include <Timer.h>
#include <open_interface.h>

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

//void autoMoveForward(oi_t *sensor, int facing_angle, int centimeters, double x, double y);

#endif /* MOVEMENT_H_ */
