#include "driver_mpu6050.h"
#include "usart.h"
#include <math.h>
#include <stdio.h>

extern I2C_HandleTypeDef hi2c3;
static I2C_Device MPU6050;
static LPF_TypeDef lpf_roll;

I2C_Device* xMPU6050_GetHandle(void)
{
    return &MPU6050;
}
/**********************************************************************
 * 函数名称： MPU6050_WriteRegister
 * 功能描述： 写MPU6050寄存器
 * 输入参数： reg-寄存器地址, data-要写入的数据
 * 输出参数： 无
 * 返 回 值： 0 - 成功, 其他值 - 失败
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/03	     V1.0	  韦东山	      创建
 ***********************************************************************/
static HAL_StatusTypeDef xMPU6050_WriteRegister(I2C_Device* dev, uint8_t reg, uint8_t data)
{
    uint8_t tmpbuf[2];

    tmpbuf[0] = reg;
    tmpbuf[1] = data;
    
    return HAL_I2C_Master_Transmit(dev->hi2c, dev->dev_addr, tmpbuf, 2, MPU6050_TIMEOUT);
}

/**********************************************************************
 * 函数名称： MPU6050_ReadRegister
 * 功能描述： 读MPU6050寄存器
 * 输入参数： reg-寄存器地址
 * 输出参数： pdata-用来保存读出的数据
 * 返 回 值： 0 - 成功, 其他值 - 失败
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/03	     V1.0	  韦东山	      创建
 ***********************************************************************/
HAL_StatusTypeDef xMPU6050_ReadRegister(I2C_Device* dev, uint8_t reg, uint8_t *pdata, uint16_t size)
{
	return HAL_I2C_Mem_Read(dev->hi2c, dev->dev_addr,
                            reg, I2C_MEMADD_SIZE_8BIT,
                            pdata, size, MPU6050_TIMEOUT);
}

/**********************************************************************
 * 函数名称： MPU6050_GetID
 * 功能描述： 读取MPU6050 ID
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： -1 - 失败, 其他值 - ID
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/03	     V1.0	  韦东山	      创建
 ***********************************************************************/
int lMPU6050_GetID(void)
{
    uint8_t id;
	if(HAL_OK == xMPU6050_ReadRegister(&MPU6050, MPU6050_WHO_AM_I, &id, 1))
        return id;
    else
        return -1;
}


/**********************************************************************
 * 函数名称:  MPU6050_ReadData
 * 功能描述:  读取MPU6050数据
 * 输入参数:  pAccX   X轴的加速度地址
              pAccY   Y轴的加速度地址
              pAccZ   Z轴的加速度地址
 * 输出参数: 无
 * 返 回 值: 0 - 成功, 其他值 - 失败
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/03	     V1.0	  韦东山	      创建
 ***********************************************************************/
void vMPU6050_ReadData(uint8_t* data)
{
    HAL_StatusTypeDef status = HAL_OK;
	if(data)
	{
		status |= xMPU6050_ReadRegister(&MPU6050, MPU6050_ACCEL_XOUT_H, &data[0], 6);
        status |= xMPU6050_ReadRegister(&MPU6050, MPU6050_GYRO_YOUT_H, &data[6], 4);
	}

    if(status != HAL_OK) {
        printf("IMU %d\r\n",status);
    }
}



/**********************************************************************
 * 函数名称： MPU6050_Init
 * 功能描述： MPU6050初始化函数,
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 0 - 成功, 其他值 - 失败
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/03	     V1.0	  韦东山	      创建
 ***********************************************************************/
void vMPU6050_Init(void)
{
    xI2CDevice_Register(&MPU6050, &hi2c3, MPU6050_ADDR, I2C_DEVICE_MPU6050);
	xMPU6050_WriteRegister(&MPU6050, MPU6050_PWR_MGMT_1 , 0x00);	//解除休眠状态
	xMPU6050_WriteRegister(&MPU6050, MPU6050_PWR_MGMT_2 , 0x00);
	xMPU6050_WriteRegister(&MPU6050, MPU6050_SMPLRT_DIV , 0x09);
	xMPU6050_WriteRegister(&MPU6050, MPU6050_CONFIG     , 0x06);
	xMPU6050_WriteRegister(&MPU6050, MPU6050_GYRO_CONFIG, 0x18);
	xMPU6050_WriteRegister(&MPU6050, MPU6050_ACCEL_CONFIG,0x18);
    vLPF_Init(&lpf_roll, 0.1);
    printf("mpu6050 init succ\r\n");
}


/**********************************************************************
 * 函数名称： MPU6050_ParseData
 * 功能描述： 解析MPU6050数据
 * 输入参数： AccX/AccY/AccZ/GyroX/GyroY/GyroZ
 *            X/Y/Z轴的加速度,X/Y/Z轴的角速度
 * 输出参数： result - 用来保存计算出的结果,目前仅支持X方向的角度
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/09/05	     V1.0	  韦东山	      创建
 ***********************************************************************/
void vMPU6050_ParseData(int16_t* data, float *Ang_roll, float *Ang_pitch)
{
    float roll, pitch;
    roll = atan2((double)(data[0] + MPU6050_X_ACCEL_OFFSET), (double)(data[2] + MPU6050_Z_ACCEL_OFFSET)) * 57.29577;
    pitch = atan2((double)(data[1] + MPU6050_Y_ACCEL_OFFSET), (double)(data[2] + MPU6050_Z_ACCEL_OFFSET)) * 57.29577;
    *Ang_roll = fLowPassFilter(&lpf_roll, roll);
    *Ang_pitch = pitch;
    
}


/**********************************************************************
 * 函数名称： mpu6050_task
 * 功能描述： MPU6050任务,它循环读取MPU6050并把数值写入队列
 * 输入参数： params - 未使用
 * 输出参数： 无
 *            无
 * 返 回 值： 0 - 成功, 其他值 - 失败
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/09/05	     V1.0	  韦东山	      创建
 ***********************************************************************/

//void vMPU6050_Task(void *params)
//{	
//    vMPU6050_Init();
//    uint8_t data[6];
//    while (1)
//    {    
//		vMPU6050_ReadData(data);
////        vMPU6050_ParseData(data);
//        
//        
//        
//        
//		vTaskDelay(500);

//	}
//}

