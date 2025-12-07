#ifndef __PWR_H__
#define __PWR_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"


#define PWR_STARTUP_LONG_PRESS_MS   2000u
#define PWR_RUNTIME_LONG_PRESS_MS   3000u


void PWR_StartupCheckButton(void);
void PWR_ProcessPWButton(Buttons_t *btn);
#endif /* __PWR_H__ */
