#include "Timer.h"
#include "uart-interrupt.h"
#include "open_interface.h"
#include "movement.h"
#include "servo.h"
#include "adc.h"

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
    ADC0_Init();

    int num_degrees = 2;

    while (1) {
        char bit = uart_receive_nonblocking();
        oi_update(sensor_data); //Clear data - make sure no neg degrees

//        uint16_t sensor = sensor_data->cliffFrontLeftSignal;
//        char toSendToPutty1[50];
//        sprintf(toSendToPutty1, "Sensor Value: %d", sensor);
//        uart_sendStr(toSendToPutty1);

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
            for (i = 0; i <= 180 && uart_receive_nonblocking() != 'q'; i += num_degrees) {
                servo_move(i);
                uint32_t adc_data = ADC0_InSeq3();
                float distance = pow((adc_data / 8843.5), -1.799);
                char toSendToPutty[50];
                sprintf(toSendToPutty, "Distance: %.2fcm", distance);
                uart_sendStr(toSendToPutty);
                timer_waitMillis(100);
            }
        }

    }

    oi_free(sensor_data);
	return 0;
}
