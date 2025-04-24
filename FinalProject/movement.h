/*
 * movement.h
 *
 *  Created on: Feb 6, 2025
 *      Author: nrudrud
 */

#ifndef HEADER_FILE
#define HEADER_FILE



void move_forward(oi_t *sensor_data, double distance_mm);
void move_backward(oi_t *sensor_data, double distance_mm);
void turn_right(oi_t *sensor, double degrees);
void turn_left(oi_t *sensor, double degrees);
double collision_detection(oi_t *sensor, double distance_mm);

#endif
