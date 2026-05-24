#ifndef __PWR_H__
#define __PWR_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "sd_app.h"

#define PWR_STARTUP_LONG_PRESS_MS   1500u
#define PWR_RUNTIME_LONG_PRESS_MS   2500u


void PWR_StartupCheckButton(void);
void PWR_ProcessPWButton(Buttons_t *btn);
void PWR_EnterShutdown(void);


#endif /* __PWR_H__ */
