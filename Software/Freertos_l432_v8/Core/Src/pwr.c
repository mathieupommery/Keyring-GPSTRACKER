#include "pwr.h"

extern SDCard_struct sdcard;

void PWR_EnterShutdown(void)
{
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_PWR_CLK_ENABLE();
    HAL_SuspendTick();

    HAL_GPIO_WritePin(AUX_EN_GPIO_Port, AUX_EN_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPS_EN_GPIO_Port, GPS_EN_Pin, GPIO_PIN_RESET);
    HAL_PWR_DisableWakeUpPin(PWR_WAKEUP_PIN1);
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
    HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1_LOW);
    HAL_PWREx_EnableGPIOPullDown(PWR_GPIO_B, PWR_GPIO_BIT_0);
    HAL_PWREx_EnableGPIOPullDown(PWR_GPIO_B, PWR_GPIO_BIT_1);
    HAL_PWREx_EnablePullUpPullDownConfig();

    /* 6. Désactiver toutes les interruptions globales (sécurité finale) */
    __disable_irq();

    /* 7. Entrer en mode Shutdown */
    HAL_PWREx_EnterSHUTDOWNMode();

    /* 8. Sécurité anti-retour : Le MCU ne devrait jamais atteindre ce point */
    while (1)
    {
        /* NOP */
    }
}


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

    /* Vérification de sécurité immédiate (Bouton non enfoncé ?) */
    if (HAL_GPIO_ReadPin(PWR_BTN_GPIO_Port, PWR_BTN_Pin) != GPIO_PIN_RESET)
    {
        PWR_EnterShutdown();
    }

    start = HAL_GetTick();
    now = start;

    /* Attente du maintien du bouton pour le démarrage complet */
    while ((now - start) <= PWR_STARTUP_LONG_PRESS_MS)
    {
        now = HAL_GetTick();
        if (HAL_GPIO_ReadPin(PWR_BTN_GPIO_Port, PWR_BTN_Pin) == GPIO_PIN_SET)
        {
            // Le bouton a été relâché trop tôt
            PWR_EnterShutdown();
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
        btn->BTN_A_LONG = 0;
        btn->BTN_A = 0;
    }

    if (btn->PW_BTN_RISING_FLAG)
    {
        btn->PW_BTN_RISING_FLAG = 0;

        if (btn->pressStart_PW_ms != 0)
        {
            btn->time_PW_ms = now - btn->pressStart_PW_ms;
            btn->pressStart_PW_ms = 0;

            if ((btn->time_PW_ms >= 50) && (btn->time_PW_ms < 400))
            {
                btn->BTN_A = 1;
                btn->BTN_A_LONG = 0;
            }
            else if ((btn->time_PW_ms >= 400) && (btn->time_PW_ms < 1500))
            {
                btn->BTN_A_LONG = 1;
                btn->BTN_A = 0;
            }
            else
            {
                btn->BTN_A = 0;
                btn->BTN_A_LONG = 0;
            }
        }
    }

    /* Condition de Shutdown en cours d'exécution (Maintien prolongé) */
    if(btn->pressStart_PW_ms != 0)
    {
        if ((now - btn->pressStart_PW_ms) >= PWR_RUNTIME_LONG_PRESS_MS)
        {

        	if (sdcard.file_is_open == 1)
        	        {
        	            sdcard.is_recording = 0;
        	            uint16_t timeout = 100; // 100 * 10 ms = 1 seconde maximum
        	            while (sdcard.file_is_open == 1 && timeout > 0)
        	            {
        	                osDelay(10); // Cède le CPU pour laisser la TrackerTask s'exécuter !
        	                timeout--;
        	            }

        	        }
#ifndef DEBUG1
            PWR_EnterShutdown();
#endif
        }
    }
}

