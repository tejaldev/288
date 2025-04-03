/*
 * lab8.c
 *
 *  Created on: Apr 2, 2025
 *      Author: tejal
 */




#include <lcd.h>
#include <open_interface.h>
#include <Timer.h>
#include <cyBot_uart.h>
#include <cyBot_Scan.h>
#include <movement.h>
#include <adc.h>
#include <math.h>
#include "uart-interrupt.h"





void calibrateSensor(){
    char val;
    char str[50];
    int adcval[10];
    int count = 0;

    sprintf(str, "\n\rMeasure at 10cm");
    uart_sendStr(str);
    val = uart_receive();
    if (val=='w'){
        adcval[count] = read();
        sprintf(str, "\n\r%d", adcval[count]);
        uart_sendStr(str);
        count++;
    }

    sprintf(str, "\n\rMeasure at 13 cm");
        uart_sendStr(str);
        val = uart_receive();
        if (val=='w'){
            adcval[count] = read();
            sprintf(str, "\n\r%d", adcval[count]);
            uart_sendStr(str);
            count++;
        }

        sprintf(str, "\n\rMeasure at 16cm");
            uart_sendStr(str);
            val = uart_receive();
            if (val=='w'){
                adcval[count] = read();
                           sprintf(str, "\n\r%d", adcval[count]);
                           uart_sendStr(str);
                           count++;
            }
    sprintf(str, "\n\rMeasure at 20cm");
    uart_sendStr(str);
    val = uart_receive();
    if (val=='w'){
        adcval[count] = read();
        sprintf(str, "\n\r%d", adcval[count]);
        uart_sendStr(str);
        count++;
    }

    sprintf(str, "\n\rMeasure at 30cm");
    uart_sendStr(str);
    val = uart_receive();
    if (val=='w'){
        adcval[count] = read();
        sprintf(str, "\n\r%d", adcval[count]);
        uart_sendStr(str);
        count++;
    }

    sprintf(str, "\n\rMeasure at 40cm");
    uart_sendStr(str);
    val = uart_receive();
    if (val=='w'){
        adcval[count] = read();
        sprintf(str, "\n\r%d", adcval[count]);
        uart_sendStr(str);
        count++;
    }

    sprintf(str, "\n\rMeasure at 50cm");
    uart_sendStr(str);
    val = uart_receive();
    if (val=='w'){
        adcval[count] = read();
        sprintf(str, "\n\r%d", adcval[count]);
        uart_sendStr(str);
        count++;
    }

    sprintf(str, "\n\rMeasure at 60cm");
    uart_sendStr(str);
    val = uart_receive();
    if (val=='w'){
        adcval[count] = read();
        sprintf(str, "\n\r%d", adcval[count]);
        uart_sendStr(str);
        count++;
    }

    sprintf(str, "\n\rMeasure at 70cm");
    uart_sendStr(str);
    val = uart_receive();
    if (val=='w'){
        adcval[count] = read();
        sprintf(str, "\n\r%d", adcval[count]);
        uart_sendStr(str);
        count++;
    }

    sprintf(str, "\n\rMeasure at 80cm");
    uart_sendStr(str);
    val = uart_receive();
    if (val=='w'){
        adcval[count] = read();
        sprintf(str, "\n\r%d", adcval[count]);
        uart_sendStr(str);
        count++;
    }


}

int main(void){
    timer_init();
    lcd_init();
    cyBOT_init_Scan(0b0011);
    ADC0_Init();

    //cyBOT_SERVO_cal();

           right_calibration_value = 290500;
           left_calibration_value = 1267000;

    //initialize communication
    cyBot_uart_init();

    //initialize lcd screen
    lcd_init();

    timer_init();

    oi_t *sensor_data = oi_alloc(); // do this only once at start of main()

    oi_init(sensor_data); // do this only once at start of main()

    cyBOT_Scan_t getScan;

    cyBOT_Scan(90, &getScan);

    float average = 0;
    int sum = 0;
    float result = 0;

    float coef = 217421.0027;
    float exp = -1.2948;

    //int count;
//    for(count=0;count<10;count++){
//        returned = read();
//        //returned = 15;
//        lcd_printf("%d", returned);
//    }
    int val;
    int distance;
    while(1){
        sum =0;
        average = 0;
//        calibrateSensor();
        val = uart_receive();
//               // lcd_putc(val);
//
                int num[10];

                int x;
                if (val == 'w')
                {
//
//
                    char str[20];
                    int i;
//                    x = read();
//                    distance = coef * (pow(average, exp));
//                    sprintf(str, "Raw: %d,\n\rDistance: %d\n\r", x, distance);
//                    uart_sendStr(str);
//
//                    lcd_printf(str);

                    for (i =0; i<10; i++){
                        num[i] = read();
                        sum += num[i];
//                        sprintf(str, "\n\r%d", num[i]);
//                        uart_sendStr(str);
                    }
//
//
//
                       // float result = 0.0000001;
//                        for (i = 0; i<10; i++){
//                            sum += num[i];
//
//                        }
                        average = sum/10;

                        distance = (coef * (pow(average, exp))) + 3;
                        sprintf(str, "Raw: %f,\n\rDistance: %d\n\r", average, distance);
                        uart_sendStr(str);

                        lcd_printf(str);
//
////                        lcd_printf("%d", returned);
////                        returned = 1-returned;
////                        result = 1/returned;
////                        result = -37962.9*returned;
//
//                        result = 122.935*(pow(0.999, average));
//                        sprintf(str, "\n\r%f", result);
//                        uart_sendStr(str);
//                        //lcd_printf("%d --- %.2lf", , average);
//
                }
    }

//    int num1 = read();
//    int num2 = read();
//    int num3 = read();
//    int num4 = read();
//    int num5 = read();
//    int num6 = read();
//    int num7 = read();
//    int num8 = read();
//    int num9 = read();
//    int num10 = read();
//
//    returned = (num1+num2+num3+num4+num5+num6+num7+num8+num9+num10)/10;
//    lcd_printf("%d", returned);


    //2600-(returned - 500)*0.0161

}
