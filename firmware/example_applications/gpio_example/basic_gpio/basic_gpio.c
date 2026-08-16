// SPDX-License-Identifier: Apache-2.0

#include "hal_gpio.h"
#include "stdlib.h"


int main(void)
{
    gpio_init();

    gpio_set_direction(2, GPIO_OUTPUT);

    gpio_write(2, GPIO_HIGH);

    while (1)
    {
        gpio_write(2,GPIO_LOW);
        printf("\nWelcome to Vega-nest!!!\n");		
        udelay(10000); 
        gpio_write(2, GPIO_HIGH);
        printf("\nWelcome to Vega-nest!!!\n");		
    }

    return 0;
}