#include "driver_mpu6050.h"
#include "usart.h"
#include <math.h>
#include <stdio.h>

extern I2C_HandleTypeDef hi2c3;
static I2C_Device MPU6050;
static LPF_TypeDef lpf_roll;

I2C_Device *xMPU6050_GetHandle(void)
{
    return &MPU6050;
}
/**********************************************************************
 * �������ƣ� MPU6050_WriteRegister
 * ���������� дMPU6050�Ĵ���
 * ��������� reg-�Ĵ�����ַ, data-Ҫд�������
 * ��������� ��
 * �� �� ֵ�� 0 - �ɹ�, ����ֵ - ʧ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2023/08/03	     V1.0	  Τ��ɽ	      ����
 ***********************************************************************/
static HAL_StatusTypeDef xMPU6050_WriteRegister(I2C_Device *dev, uint8_t reg, uint8_t data)
{
    uint8_t tmpbuf[2];

    tmpbuf[0] = reg;
    tmpbuf[1] = data;

    return HAL_I2C_Master_Transmit(dev->hi2c, dev->dev_addr, tmpbuf, 2, MPU6050_TIMEOUT);
}

/**********************************************************************
 * �������ƣ� MPU6050_ReadRegister
 * ���������� ��MPU6050�Ĵ���
 * ��������� reg-�Ĵ�����ַ
 * ��������� pdata-�����������������
 * �� �� ֵ�� 0 - �ɹ�, ����ֵ - ʧ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2023/08/03	     V1.0	  Τ��ɽ	      ����
 ***********************************************************************/
HAL_StatusTypeDef xMPU6050_ReadRegister(I2C_Device *dev, uint8_t reg, uint8_t *pdata, uint16_t size)
{
    return HAL_I2C_Mem_Read(dev->hi2c, dev->dev_addr,
                            reg, I2C_MEMADD_SIZE_8BIT,
                            pdata, size, MPU6050_TIMEOUT);
}

/**********************************************************************
 * �������ƣ� MPU6050_GetID
 * ���������� ��ȡMPU6050 ID
 * ��������� ��
 * ��������� ��
 * �� �� ֵ�� -1 - ʧ��, ����ֵ - ID
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2023/08/03	     V1.0	  Τ��ɽ	      ����
 ***********************************************************************/
int lMPU6050_GetID(void)
{
    uint8_t id;
    if (HAL_OK == xMPU6050_ReadRegister(&MPU6050, MPU6050_WHO_AM_I, &id, 1))
        return id;
    else
        return -1;
}

/**********************************************************************
 * ��������:  MPU6050_ReadData
 * ��������:  ��ȡMPU6050����
 * �������:  pAccX   X��ļ��ٶȵ�ַ
              pAccY   Y��ļ��ٶȵ�ַ
              pAccZ   Z��ļ��ٶȵ�ַ
 * �������: ��
 * �� �� ֵ: 0 - �ɹ�, ����ֵ - ʧ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2023/08/03	     V1.0	  Τ��ɽ	      ����
 ***********************************************************************/
void vMPU6050_ReadData(uint8_t *data)
{
    HAL_StatusTypeDef status = HAL_OK;
    if (data)
    {
        status |= xMPU6050_ReadRegister(&MPU6050, MPU6050_ACCEL_XOUT_H, &data[0], 6);
        status |= xMPU6050_ReadRegister(&MPU6050, MPU6050_GYRO_YOUT_H, &data[6], 4);
    }

    if (status != HAL_OK)
    {
        printf("IMU %d\r\n", status);
    }
}

/**********************************************************************
 * �������ƣ� MPU6050_Init
 * ���������� MPU6050��ʼ������,
 * ��������� ��
 * ��������� ��
 * �� �� ֵ�� 0 - �ɹ�, ����ֵ - ʧ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2023/08/03	     V1.0	  Τ��ɽ	      ����
 ***********************************************************************/
void vMPU6050_Init(void)
{
    Device_I2C_Register(&MPU6050, &hi2c3, MPU6050_ADDR, I2C_DEVICE_MPU6050);
    xMPU6050_WriteRegister(&MPU6050, MPU6050_PWR_MGMT_1, 0x00); // �������״̬
    xMPU6050_WriteRegister(&MPU6050, MPU6050_PWR_MGMT_2, 0x00);
    xMPU6050_WriteRegister(&MPU6050, MPU6050_SMPLRT_DIV, 0x09);
    xMPU6050_WriteRegister(&MPU6050, MPU6050_CONFIG, 0x06);
    xMPU6050_WriteRegister(&MPU6050, MPU6050_GYRO_CONFIG, 0x18);
    xMPU6050_WriteRegister(&MPU6050, MPU6050_ACCEL_CONFIG, 0x18);
    LPF_Init(&lpf_roll, 0.1);
    printf("mpu6050 init succ\r\n");
}

/**********************************************************************
 * �������ƣ� MPU6050_ParseData
 * ���������� ����MPU6050����
 * ��������� AccX/AccY/AccZ/GyroX/GyroY/GyroZ
 *            X/Y/Z��ļ��ٶ�,X/Y/Z��Ľ��ٶ�
 * ��������� result - �������������Ľ��,Ŀǰ��֧��X����ĽǶ�
 * �� �� ֵ�� ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2023/09/05	     V1.0	  Τ��ɽ	      ����
 ***********************************************************************/
void vMPU6050_ParseData(int16_t *data, float *Ang_roll, float *Ang_pitch)
{
    float roll, pitch;
    roll = atan2((double)(data[0] + MPU6050_X_ACCEL_OFFSET), (double)(data[2] + MPU6050_Z_ACCEL_OFFSET)) * 57.29577;
    pitch = atan2((double)(data[1] + MPU6050_Y_ACCEL_OFFSET), (double)(data[2] + MPU6050_Z_ACCEL_OFFSET)) * 57.29577;
    *Ang_roll = LowPassFilter(&lpf_roll, roll);
    *Ang_pitch = pitch;
}

/**********************************************************************
 * �������ƣ� mpu6050_task
 * ���������� MPU6050����,��ѭ����ȡMPU6050������ֵд�����
 * ��������� params - δʹ��
 * ��������� ��
 *            ��
 * �� �� ֵ�� 0 - �ɹ�, ����ֵ - ʧ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2023/09/05	     V1.0	  Τ��ɽ	      ����
 ***********************************************************************/

// void vMPU6050_Task(void *params)
//{
//     vMPU6050_Init();
//     uint8_t data[6];
//     while (1)
//     {
//		vMPU6050_ReadData(data);
////        vMPU6050_ParseData(data);
//
//
//
//
//		vTaskDelay(500);

//	}
//}
