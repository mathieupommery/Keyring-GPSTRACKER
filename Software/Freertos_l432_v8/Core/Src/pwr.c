#include "pwr.h"

void PWR_StartupCheckButton(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    uint32_t start, now;
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_PWR_CLK_ENABLE();

    GPIO_InitStruct.Pin  = PWR_BTN_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(PWR_BTN_GPIO_Port, &GPIO_InitStruct);

    if (HAL_GPIO_ReadPin(PWR_BTN_GPIO_Port, PWR_BTN_Pin)!= GPIO_PIN_RESET)
    {
        HAL_SuspendTick();
        HAL_PWR_DisableWakeUpPin(PWR_WAKEUP_PIN1);
        __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
        HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1_LOW);
        HAL_PWREx_EnableGPIOPullDown(PWR_GPIO_A, PWR_GPIO_BIT_1);
        HAL_PWREx_EnableGPIOPullDown(PWR_GPIO_A, PWR_GPIO_BIT_4);
        HAL_PWREx_EnablePullUpPullDownConfig();
        __disable_irq();
        HAL_PWREx_EnterSHUTDOWNMode();
        while (1)
        {
            /* NOP */
        }
    }
    start = HAL_GetTick();
    while ((now - start) <= PWR_STARTUP_LONG_PRESS_MS)
    {
        now = HAL_GetTick();
        if (HAL_GPIO_ReadPin(PWR_BTN_GPIO_Port, PWR_BTN_Pin)== GPIO_PIN_SET)
        {

            HAL_SuspendTick();
            HAL_PWR_DisableWakeUpPin(PWR_WAKEUP_PIN1);
            __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
            HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1_LOW);
            HAL_PWREx_EnableGPIOPullDown(PWR_GPIO_A, PWR_GPIO_BIT_1);
            HAL_PWREx_EnableGPIOPullDown(PWR_GPIO_A, PWR_GPIO_BIT_4);
            HAL_PWREx_EnablePullUpPullDownConfig();
            __disable_irq();
            HAL_PWREx_EnterSHUTDOWNMode();
            while (1)
            {
                /* NOP */
            }
        }
    }
}


void PWR_ProcessPWButton(Buttons_t *btn)
{
    uint32_t now = HAL_GetTick();
    if (btn->PW_BTN_FALLING_FLAG)
    {
    	btn->PW_BTN_FALLING_FLAG = 0;
        btn->pressStart_PW_ms = now;
        btn->time_PW_ms = 0;
        btn->BTN_PW_LONG = 0;
        btn->BTN_PW=0;
    }
    if (btn->PW_BTN_RISING_FLAG)
    {
    	btn->PW_BTN_RISING_FLAG = 0;

        if (btn->pressStart_PW_ms != 0)
        {
            btn->time_PW_ms = now - btn->pressStart_PW_ms;
            btn->pressStart_PW_ms = 0;

            if ((btn->time_PW_ms >= 50) && (btn->time_PW_ms <  400))
            {
            	btn->BTN_PW=1;
            	btn->BTN_PW_LONG=0;
            }
            else if ((btn->time_PW_ms >= 400) && (btn->time_PW_ms <  1500))
            {
            	btn->BTN_PW=0;
            	btn->BTN_PW_LONG=1;
            }
            else if (btn->time_PW_ms >= PWR_RUNTIME_LONG_PRESS_MS)
            {
                HAL_SuspendTick();
                HAL_PWR_DisableWakeUpPin(PWR_WAKEUP_PIN1);
                __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
                HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1_LOW);
                HAL_PWREx_EnableGPIOPullDown(PWR_GPIO_A, PWR_GPIO_BIT_1);
                HAL_PWREx_EnableGPIOPullDown(PWR_GPIO_A, PWR_GPIO_BIT_4);
                HAL_PWREx_EnablePullUpPullDownConfig();
                __disable_irq();
                HAL_PWREx_EnterSHUTDOWNMode();
                while (1)
                {
                }
            }
            else
            {

            }
        }
    }
}

