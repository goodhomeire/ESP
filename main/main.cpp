/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "oled.h"
#include "ec11.h"

extern "C" void app_main(void)
{
    char num[] = "1234567890";
    extern class Oled oled;
    extern class Skey ec11;
    printf("Hello WENXI!\n");
    oled.begin();
    oled.init();
    ec11.begin(SW, SA, SB, NULL);
    while (1)
    {
        for (size_t i = 0; i < sizeof(num)-1; i++)
        {
            oled.write_char(20,5,num[i],16,1);
            oled.refresh();
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            oled.clr();
        }
        oled.str(0, 0, "Hello World!", 16, 1, 1);
        oled.refresh();
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        oled.clr();
    }
    
    
}
