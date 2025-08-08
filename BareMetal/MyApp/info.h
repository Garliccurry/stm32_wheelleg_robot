#ifndef __INFO_H__
#define __INFO_H__

/**********************    INCLUDE DIRECTIVES    *************************/
#include "main.h"

/**************    CONSTANTS, MACROS, & DATA STRUCTURES    ***************/
extern int g_flag_usartrec;

extern int   pos_left;
extern float g_vel;
extern int   g_hight;
extern int   wheel_run;

extern int i2cl;
extern int i2cr;
extern int i2cm;
/***********************    FUNCTION PROTOTYPES    ***********************/
void vSharedResources_Init(void);
#endif
