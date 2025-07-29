#ifndef __SHARED_H
#define __SHARED_H

/**********************    INCLUDE DIRECTIVES    *************************/
#include "main.h"

/**************    CONSTANTS, MACROS, & DATA STRUCTURES    ***************/
extern int g_flag_usartrec;

extern int pos_left;
extern float g_vel;
extern int g_hight;
extern int wheel_run;
/***********************    FUNCTION PROTOTYPES    ***********************/
void vSharedResources_Init(void);
#endif
