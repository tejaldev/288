#include "Timer.h"
#include "uart-interrupt.h"
#include "open_interface.h"
#include "movement.h"
#include "servo.h"
#include "adc.h"
#include "ping.h"

//USE BOT 3 FOR GOOD RESULTS

float adcValues[92];
int right_bound = 0;
int left_bound = 180;
int num_degrees = 2;


/**
 * main.c
 */
int main(void)
{
    oi_t *sensor_data = oi_alloc(); // do this only once at start of main()
    oi_init(sensor_data);
    timer_init(); // Must be called before lcd_init(), which uses timer functions
    lcd_init();
    uart_interrupt_init();
    servo_init();
    ping_init();
    ADC0_Init();



    while (1) {
        char bit = uart_receive_nonblocking();
        oi_update(sensor_data); //Clear data - make sure no neg degrees

//        uint16_t sensor = sensor_data->cliffFrontLeftSignal;
//        char toSendToPutty1[50];
//        sprintf(toSendToPutty1, "Sensor Value: %d", sensor);
//        uart_sendStr(toSendToPutty1);

//         w to go forward, a and d for turns, s for scan, b for break (end), and q to stop any function
        if (bit == 'w') {
           //Drive forward while collision detecting
            double sum = 0; // distance member in oi_t struct is type double
            oi_setWheels(50, 50); //move forward
            while (uart_receive_nonblocking() != 'q') {
                oi_update(sensor_data);
                sum += sensor_data->distance; // use -> notation since pointer
                char toSendToPutty[50];
                sprintf(toSendToPutty, "Drove Forward: %.2f cm", sum/10);
                uart_sendStr(toSendToPutty);
                if (sensor_data->bumpLeft) {
                    sprintf(toSendToPutty, "Hit object on left");
                    uart_sendStr(toSendToPutty);
                    move_backward(sensor_data, 100);
                    sprintf(toSendToPutty, "Drove Back 10cm");
                    uart_sendStr(toSendToPutty);
                    break;
                }
                if (sensor_data->bumpRight) {
                     sprintf(toSendToPutty, "Hit object on right");
                     uart_sendStr(toSendToPutty);
                     move_backward(sensor_data, 100);
                     sprintf(toSendToPutty, "Drove Back 10cm");
                     uart_sendStr(toSendToPutty);
                     break;
                }
                if (sensor_data->cliffFrontLeftSignal < 1000) {
                    sprintf(toSendToPutty, "Hole Detected Front Left");
                    uart_sendStr(toSendToPutty);
                    move_backward(sensor_data, 100);
                    sprintf(toSendToPutty, "Drove Back 10cm");
                    uart_sendStr(toSendToPutty);
                    break;
                }
                if (sensor_data->cliffLeftSignal < 1000) {
                    sprintf(toSendToPutty, "Hole Detected Left");
                    uart_sendStr(toSendToPutty);
                    move_backward(sensor_data, 100);
                    sprintf(toSendToPutty, "Drove Back 10cm");
                    uart_sendStr(toSendToPutty);
                    break;
                }
                if (sensor_data->cliffFrontRightSignal < 1000) {
                    sprintf(toSendToPutty, "Hole Detected Front Right");
                    uart_sendStr(toSendToPutty);
                    move_backward(sensor_data, 100);
                    sprintf(toSendToPutty, "Drove Back 10cm");
                    uart_sendStr(toSendToPutty);
                    break;
                }
                if (sensor_data->cliffRightSignal < 1000) {
                    sprintf(toSendToPutty, "Hole Detected Right");
                    uart_sendStr(toSendToPutty);
                    move_backward(sensor_data, 100);
                    sprintf(toSendToPutty, "Drove Back 10cm");
                    uart_sendStr(toSendToPutty);
                    break;
                }
                if (sensor_data->cliffFrontLeftSignal > 2500) {
                    sprintf(toSendToPutty, "OB Detected Front Left");
                    uart_sendStr(toSendToPutty);
                    move_backward(sensor_data, 100);
                    sprintf(toSendToPutty, "Drove Back 10cm");
                    uart_sendStr(toSendToPutty);
                    break;
                }
                if (sensor_data->cliffLeftSignal > 2500) {
                    sprintf(toSendToPutty, "OB Detected Left");
                    uart_sendStr(toSendToPutty);
                    move_backward(sensor_data, 100);
                    sprintf(toSendToPutty, "Drove Back 10cm");
                    uart_sendStr(toSendToPutty);
                    break;
                }
                if (sensor_data->cliffFrontRightSignal > 2500) {
                    sprintf(toSendToPutty, "OB Detected Front Right");
                    uart_sendStr(toSendToPutty);
                    move_backward(sensor_data, 100);
                    sprintf(toSendToPutty, "Drove Back 10cm");
                    uart_sendStr(toSendToPutty);
                    break;
                }
                if (sensor_data->cliffRightSignal > 2500) {
                    sprintf(toSendToPutty, "OB Detected Right");
                    uart_sendStr(toSendToPutty);
                    move_backward(sensor_data, 100);
                    sprintf(toSendToPutty, "Drove Back 10cm");
                    uart_sendStr(toSendToPutty);
                    break;
                }
            }
            oi_setWheels(0, 0);

        } else if (bit == 'a') {
            //Turn left
            double sum = 0;
            oi_setWheels(25, -25);
            while (uart_receive_nonblocking() != 'q') {
                oi_update(sensor_data);
                sum += sensor_data->angle;
                char toSendToPutty[50];
                sprintf(toSendToPutty, "Turned Left %.2f degrees", sum);
                uart_sendStr(toSendToPutty);
            }
            oi_setWheels(0, 0);
        } else if (bit == 'd') {
            //Turn right
            double sum = 0;
            oi_setWheels(-25, 25);
            while (uart_receive_nonblocking() != 'q') {
                oi_update(sensor_data);
                sum -= sensor_data -> angle;
                char toSendToPutty[50];
                sprintf(toSendToPutty, "Turned Right %.2f degrees", sum);
                uart_sendStr(toSendToPutty);
            }
            oi_setWheels(0, 0);

        } else if (bit == 'b') {
            //Play sound before breaking
            //Wee woo wee woo - Song 0
            unsigned char notes[] = {71, 67, 71, 67, 71, 67, 71, 67, 71, 67};
            unsigned char durations[] = {32, 32, 32, 32, 32, 32, 32, 32, 32, 32};

            //Baby you're a firework - Song 1
            unsigned char notes1[] = {51, 63, 61, 60, 58, 58, 56, 60};
            unsigned char durations1[] = {16, 16, 16, 16, 16, 92, 16, 92};
            oi_loadSong(0, 10, notes, durations);
            oi_loadSong(1, 8, notes1, durations1);
            oi_play_song(1);
            break;
        }  else if (bit == 's') {
            //Scan environment
            int i;

            for (i = right_bound; i <= left_bound && uart_receive_nonblocking() != 'q'; i += num_degrees) {
                servo_move(i);
                float adc_distance = 0;
                int j;
                for (j = 0; j < 3; j++) {
                    uint32_t adc_data = ADC0_InSeq3();
                    adc_distance += pow((adc_data / 8843.5), -1.799);
                    timer_waitMillis(50);
                }
                adc_distance /= 3;
                adcValues[i / num_degrees] = adc_distance;

//                char toSendToPutty[50];
//                sprintf(toSendToPutty, "Degree: %d  ADC Distance: %.2f", i, adc_distance);
//                uart_sendStr(toSendToPutty);
            }
            if (i > left_bound) {
                int i;
                    int startObj = -1;
                    int objectCount = 0;
                    int objectPositions[10];
                    int objectDegWidths[10];

                    char toSendToPutty[20];
                    sprintf(toSendToPutty, "Detecting Objects");
                    uart_sendStr(toSendToPutty);

                    for (i = right_bound + num_degrees; i < left_bound; i+=num_degrees) {
                        float value = adcValues[i/num_degrees];
                        float difference = value - adcValues[(i/num_degrees) - 1];
                        //detect start object
                        if (difference <= -15 && startObj == -1 && value < 100) {
                            startObj = i;
                        }
                        //detect end of object
                        else if (difference > 15 && startObj != -1) {
                            int endObj = (i-num_degrees);
                            int objectDegWidth = endObj - startObj;
                            if (objectDegWidth > 0) {
                                int middleObj = (startObj + endObj) / 2;
                                objectPositions[objectCount] = middleObj;
                                objectDegWidths[objectCount] = objectDegWidth;
                            objectCount++;
                            }
                            startObj = -1;
                        }
                        //detect if end scan on object
                        else if (i == left_bound - num_degrees && startObj != -1) {
                            int endObj = i;
                            if (endObj - startObj > num_degrees) {
                                int middleObj = (startObj + endObj) / 2;
                                objectPositions[objectCount] = middleObj;
                                objectCount++;
                            }
                        }
                    }
                    int j;
                    for (j = 0; j < objectCount; j++)
                    {
                        int objectPosition = objectPositions[j];
                        servo_move(objectPosition);
                        timer_waitMillis(500);
                        ping_trigger();
                        float ping_distance = ping_getDistance();
                        int degWidth = objectDegWidths[j];
                        float actualWidth = 2.0 * 3.14159265 *ping_distance * (degWidth / 360.0);
                        char toSendToPutty[50];
                        sprintf(toSendToPutty, "Object at %d degrees has a distance of %.2fcm and width of %.2fcm", objectPosition, ping_distance, actualWidth);
                        uart_sendStr(toSendToPutty);
                    }            }
            uart_sendStr("END");
        }
    }

    oi_free(sensor_data);
	return 0;
}
