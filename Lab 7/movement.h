/*
 * movement.h
 *
 *  Created on: Feb 6, 2025
 *      Author: tejal
 */

#ifndef MOVEMENT_H_
#define MOVEMENT_H_
#include "open_interface.h"

double move_forward(oi_t *sensor_data, double distance_mm);
double turn_right(oi_t *sensor_data, double degrees);
double turn_left(oi_t *sensor_data, double degrees);
double go_around_right(oi_t *sensor_data);
double go_around_left(oi_t *sensor_data);
double move_backward(oi_t *sensor_data, double distance_mm);

#endif /* MOVEMENT_H_ */
