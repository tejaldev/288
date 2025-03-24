#include <Timer.h>
#include <uart-interrupt.h>
#include <stdio.h>
#include <movement.h>
#include <cyBot_Scan.h>
#include <math.h>
#include <stdlib.h>

/**
 * main.c
 */

typedef struct Object_Struct {
    float distance; //Centimeters
    int angle; //Degrees Start
    int middle; //Middle Angle
    int width; //Radial Degree
    int linearWidth; //cm
    int caliberatedIRMin; //cm
    int caliberatedIRFirst; //cm
    int caliberatedIRLast; //cm
} Object_t;

void printStringToPutty(char str[]);
float min(float a, float b);
float max(float a, float b);
int rawToCm(int raw);
int avgIR(cyBOT_Scan_t* scan);
int getDistAtAngle(int angle, cyBOT_Scan_t* scan);
void goToObject(int angle, int dist, oi_t* sensor);

int main(void)
{
    int const MAX_DISTANCE = 75;
    int const MAX_MARGIN = 10;

    int angle;
    int start = 0x00000007;
    int getByte = 0;
    char sensorData[100];
    char outcomeData[100];
    int itemInView = 0;
    int numItems = 0;
    float lastDistanceScan = 0;
    int switchManualAuto = 0; // 1: Manual, 0: Auto

    oi_t *sensor_data = oi_alloc();
    oi_init(sensor_data);
//    uart_init(115200);
    uart_interrupt_init();
    timer_init();
    cyBOT_init_Scan(start);
    //cyBOT_SERVO_cal();

        right_calibration_value = 311500;
        left_calibration_value = 1319500;

    cyBOT_Scan_t scan;
    uart_sendChar('S');

    cyBOT_Scan(0, &scan);

    while (1) {
        Object_t objects[10];
        getByte = uart_receive();

        if (getByte == 'n') {
            if (switchManualAuto == 1)
                switchManualAuto = 0;
            else
                switchManualAuto = 1;

            uart_sendChar('n');
        }

        if (getByte == 'w') {
            move_forward(sensor_data, 2);
            uart_sendChar('w');
        }

        else if (getByte == 's') {
            move_backward(sensor_data, 2);
            uart_sendChar('s');
        }

        else if (getByte == 'a') {
            turn_cclockwise(sensor_data, 90);
            move_forward(sensor_data, 2);
            uart_sendChar('a');
        }

        else if (getByte == 'd') {
            turn_clockwise(sensor_data, 90);
            move_forward(sensor_data, 2);
            uart_sendChar('d');
        }

        if (getByte == 'q') {
            turn_cclockwise(sensor_data, 20);
            uart_sendChar('q');
        }
        else if (getByte == 'e') {
            turn_clockwise(sensor_data, 20);
            uart_sendChar('e');
        }

        if (getByte == 'm') {
            cyBOT_Scan(0, &scan);
            int minRaw;

            printStringToPutty("\n\r    Angle  Distance(cm) CaliberatedIR");
            for (angle = 0; angle <= 180; angle += 2) {
                cyBOT_Scan(angle, &scan);
                int rawCM = rawToCm(avgIR(&scan));

                if (rawCM + MAX_MARGIN < min(lastDistanceScan, MAX_DISTANCE) && itemInView == 0) {
                    uart_sendChar('V');
                    itemInView = 1;

                    objects[numItems].angle = angle;
                    objects[numItems].caliberatedIRFirst = rawCM;

                    minRaw = 0;
                    minRaw = rawCM;
                }

                minRaw = min(rawCM, minRaw);

                if (rawCM > min(lastDistanceScan, MAX_DISTANCE) + MAX_MARGIN && angle - objects[numItems].angle > 2 && itemInView == 1) {
                    uart_sendChar('^');
                    itemInView = 0;

                    objects[numItems].caliberatedIRMin = minRaw;
                    objects[numItems].caliberatedIRLast = rawCM;
                    objects[numItems].width = angle - objects[numItems].angle;

                    int middle = (angle + objects[numItems].angle) / 2;
                    objects[numItems].middle = middle;
                    objects[numItems].distance = getDistAtAngle(middle, &scan);

                    int avgIR = (objects[numItems].caliberatedIRMin + objects[numItems].caliberatedIRFirst) / 2;
                    objects[numItems].linearWidth = avgIR * sin((objects[numItems].width / 2) * M_PI / 180) * 2;

                    numItems++;
                }
                else if (rawCM > min(lastDistanceScan, MAX_DISTANCE) + MAX_MARGIN && angle - objects[numItems].angle <= 2 && itemInView == 1) {
                    itemInView = 0;
                }

                sprintf(sensorData, "\n\r%7d %7.0f %12d", angle, min(scan.sound_dist, MAX_DISTANCE), rawCM);
                printStringToPutty(sensorData);

                lastDistanceScan = rawCM;
            }

            if (itemInView == 1 && angle - objects[numItems].angle > 8) {
                itemInView = 0;

                objects[numItems].caliberatedIRMin = minRaw;
                objects[numItems].width = angle - objects[numItems].angle;

                int middle = (angle + objects[numItems].angle) / 2;
                objects[numItems].middle = middle;
                objects[numItems].distance = getDistAtAngle(middle, &scan);

                int avgIR = (objects[numItems].caliberatedIRMin + objects[numItems].caliberatedIRFirst) / 2;
                objects[numItems].linearWidth = avgIR * sin((objects[numItems].width / 2) * M_PI / 180) * 2;

                numItems++;
            }

            printStringToPutty("\n\r Object#  Angle  Distance  Width  LinearWidth");

            int i;
            for (i = 0; i < numItems; i++) {
                sprintf(outcomeData, "\n\r%4d %8d %8.0f %8d %10d", i + 1, objects[i].middle, objects[i].distance, objects[i].width, objects[i].linearWidth);
                printStringToPutty(outcomeData);
            }

            int j = 0;
            int smallestWidth = 100;
            Object_t smallestObject;

            for (j = 0; j < numItems; j++) {
                if (objects[j].linearWidth < smallestWidth) {
                    smallestObject = objects[j];
                    smallestWidth = objects[j].linearWidth;
                }
            }

            cyBOT_Scan(smallestObject.middle, &scan);

            if(switchManualAuto == 0)
                goToObject(smallestObject.middle, smallestObject.distance, sensor_data);
            else {
                sprintf(outcomeData, "\n\rSmallest Object: %4d %8d %8.0f %8d %10d", i + 1, smallestObject.middle, smallestObject.distance, smallestObject.width, smallestObject.linearWidth);
                printStringToPutty(outcomeData);
            }
        }
    }
}

void printStringToPutty(char str[]) {
    int i;

    for (i = 0; i < strlen(str); i++) {
        uart_sendChar(str[i]);
    }
}

float min(float a, float b) {
    return (a < b) ? a : b;
}

float max(float a, float b) {
    return (a > b) ? a : b;
}

int rawToCm(int raw) {
    return (double)8000000 * pow((double)raw, -1.709);
}

int avgIR(cyBOT_Scan_t* scan) {
    int i = 0;
    int totalIR = 0;

    for (i = 0; i < 5; i++) {
        totalIR += scan->IR_raw_val;
    }

    return totalIR / 5;
}

int getDistAtAngle(int angle, cyBOT_Scan_t* scan) {
    int totalDist = 0;
    int i = 0;
    cyBOT_Scan(angle, scan);

    for (i = 0; i < 5; i++) {
        totalDist += scan->sound_dist;
        timer_waitMillis(100);
    }

    return totalDist / 5;
}

void goToObject(int angle, int dist, oi_t* sensor) {
    int angleFrom90 = 90 - angle;
    if (angleFrom90 > 0) {
        turn_clockwise(sensor, max(angleFrom90, 16));
        move_forward(sensor, max(dist - 15, 5));
    }
    else {
        turn_cclockwise(sensor, max(angleFrom90 * -1, 16));
        move_forward(sensor, max(dist - 15, 5));
    }
}
