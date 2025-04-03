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

    int returned = 10;
    int count;
//    for(count=0;count<10;count++){
//        returned = read();
//        //returned = 15;
//        lcd_printf("%d", returned);
//    }
    int val;
    while(1){
        val = uart_receive();
                lcd_putc(val);


                if (val == 'w')
                {
                    int num1 = read();
                        int num2 = read();
                        int num3 = read();
                        int num4 = read();
                        int num5 = read();
                        int num6 = read();
                        int num7 = read();
                        int num8 = read();
                        int num9 = read();
                        int num10 = read();

                        float result = 0.0000001;
                        returned = (num1+num2+num3+num4+num5+num6+num7+num8+num9+num10)/10;
//                        lcd_printf("%d", returned);
//                        returned = 1-returned;
//                        result = 1/returned;
//                        result = -37962.9*returned;

                        result = 122.935*(pow(0.999, returned));
                        lcd_printf("%d --- %.2lf", returned, result);
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
