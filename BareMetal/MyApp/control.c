#include "control.h"

#include "SCSCL.h"
#include "info.h"

SCSData_t g_scsdata = {
    .ID[0] = 1,
    .ID[1] = 2,
    .position[0] = 2048,
    .position[1] = 2048,
    .time[0] = 0,
    .time[1] = 0,
    .speed[0] = 1500,
    .speed[1] = 1500,
};
// TODO SyncWritePos放在此处

void control_leggedbalance(void)
{
    SyncWritePos(g_scsdata.ID, 2, g_scsdata.position, g_scsdata.time, g_scsdata.speed);
}
