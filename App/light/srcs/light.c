
#include "main.h"
#include "delay.h"

void light_task(void)
{
    HAL_GPIO_WritePin(User_LED_GPIO_Port, User_LED_Pin, GPIO_PIN_SET);
    delay_1();
    HAL_GPIO_WritePin(User_LED_GPIO_Port, User_LED_Pin, GPIO_PIN_RESET);
    delay_2();
}
