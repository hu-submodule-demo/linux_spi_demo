/**
 * @file      : icm20608.c
 * @brief     : ICM20608驱动源文件
 * @author    : huenrong (huenrong1028@outlook.com)
 * @date      : 2023-02-05 17:49:10
 *
 * @copyright : Copyright (c) 2023 huenrong
 *
 * @history   : date       author          description
 *              2023-02-05 huenrong        创建文件
 *
 */

#include <stdio.h>
#include <unistd.h>

#include "./icm20608.h"
#include "../linux_gpio/gpio.h"
#include "../linux_spi/spi.h"

// ICM20608设备路径
#define ICM20608_DEV_PATH "/dev/spidev2.0"

// ICM20608设备片选脚(GPIO1_IO20)
#define GPIO_ICM20608_CS 20

// ICM20608设备片选脚控制
#define ICM20608_CS_L() gpio_set_value(GPIO_ICM20608_CS, E_GPIO_LOW)
#define ICM20608_CS_H() gpio_set_value(GPIO_ICM20608_CS, E_GPIO_HIGH)

// 陀螺仪和加速度自测(出厂时设置, 用于与用户的自检输出值比较)
#define ICM20_SELF_TEST_X_GYRO 0x00
#define ICM20_SELF_TEST_Y_GYRO 0x01
#define ICM20_SELF_TEST_Z_GYRO 0x02
#define ICM20_SELF_TEST_X_ACCEL 0x0D
#define ICM20_SELF_TEST_Y_ACCEL 0x0E
#define ICM20_SELF_TEST_Z_ACCEL 0x0F

// 陀螺仪静态偏移
#define ICM20_XG_OFFS_USRH 0x13
#define ICM20_XG_OFFS_USRL 0x14
#define ICM20_YG_OFFS_USRH 0x15
#define ICM20_YG_OFFS_USRL 0x16
#define ICM20_ZG_OFFS_USRH 0x17
#define ICM20_ZG_OFFS_USRL 0x18

#define ICM20_SMPLRT_DIV 0x19
#define ICM20_CONFIG 0x1A
#define ICM20_GYRO_CONFIG 0x1B
#define ICM20_ACCEL_CONFIG 0x1C
#define ICM20_ACCEL_CONFIG2 0x1D
#define ICM20_LP_MODE_CFG 0x1E
#define ICM20_ACCEL_WOM_THR 0x1F
#define ICM20_FIFO_EN 0x23
#define ICM20_FSYNC_INT 0x36
#define ICM20_INT_PIN_CFG 0x37
#define ICM20_INT_ENABLE 0x38
#define ICM20_INT_STATUS 0x3A

// 加速度输出
#define ICM20_ACCEL_XOUT_H 0x3B
#define ICM20_ACCEL_XOUT_L 0x3C
#define ICM20_ACCEL_YOUT_H 0x3D
#define ICM20_ACCEL_YOUT_L 0x3E
#define ICM20_ACCEL_ZOUT_H 0x3F
#define ICM20_ACCEL_ZOUT_L 0x40

// 温度输出
#define ICM20_TEMP_OUT_H 0x41
#define ICM20_TEMP_OUT_L 0x42

// 陀螺仪输出
#define ICM20_GYRO_XOUT_H 0x43
#define ICM20_GYRO_XOUT_L 0x44
#define ICM20_GYRO_YOUT_H 0x45
#define ICM20_GYRO_YOUT_L 0x46
#define ICM20_GYRO_ZOUT_H 0x47
#define ICM20_GYRO_ZOUT_L 0x48

#define ICM20_SIGNAL_PATH_RESET 0x68
#define ICM20_ACCEL_INTEL_CTRL 0x69
#define ICM20_USER_CTRL 0x6A
#define ICM20_PWR_MGMT_1 0x6B
#define ICM20_PWR_MGMT_2 0x6C
#define ICM20_FIFO_COUNTH 0x72
#define ICM20_FIFO_COUNTL 0x73
#define ICM20_FIFO_R_W 0x74
#define ICM20_WHO_AM_I 0x75

// 加速度静态偏移
#define ICM20_XA_OFFSET_H 0x77
#define ICM20_XA_OFFSET_L 0x78
#define ICM20_YA_OFFSET_H 0x7A
#define ICM20_YA_OFFSET_L 0x7B
#define ICM20_ZA_OFFSET_H 0x7D
#define ICM20_ZA_OFFSET_L 0x7E

/**
 * @brief  ICM20608初始化
 * @return true : 成功
 * @return false: 失败
 */
bool icm20608_init(void)
{
    // 打开设备
    if (!spi_open(ICM20608_DEV_PATH, E_SPI_MODE_0, 8000000, 8))
    {
        return false;
    }

    // 导出片选引脚
    if (!gpio_export(GPIO_ICM20608_CS))
    {
        return false;
    }

    // 设置片选引脚方向
    if (!gpio_set_direction(GPIO_ICM20608_CS, E_GPIO_OUT))
    {
        return false;
    }

    // 设置片选引脚电平
    if (!gpio_set_value(GPIO_ICM20608_CS, E_GPIO_LOW))
    {
        return false;
    }

    // 寄存器初始化
    ICM20608_CS_L();
    // usleep(1 * 1000);

    // 重置内部寄存器, 并恢复默认设置
    if (!spi_write_byte_sub(ICM20608_DEV_PATH, (ICM20_PWR_MGMT_1 & 0x7F), 0x80))
    {
        ICM20608_CS_H();

        return false;
    }

    ICM20608_CS_H();
    usleep(50 * 1000);

    // 自动选择最佳的可用时钟源–PLL(如果已准备好), 否则使用内部振荡器
    ICM20608_CS_L();
    if (!spi_write_byte_sub(ICM20608_DEV_PATH, (ICM20_PWR_MGMT_1 & 0x7F), 0x01))
    {
        ICM20608_CS_H();

        return false;
    }
    ICM20608_CS_H();
    usleep(50 * 1000);

    // 输出速率是内部采样率
    ICM20608_CS_L();
    if (!spi_write_byte_sub(ICM20608_DEV_PATH, (ICM20_SMPLRT_DIV & 0x7F), 0x00))
    {
        ICM20608_CS_H();

        return false;
    }
    ICM20608_CS_H();

    // 陀螺仪±2000dps量程
    ICM20608_CS_L();
    if (!spi_write_byte_sub(ICM20608_DEV_PATH, (ICM20_GYRO_CONFIG & 0x7F), 0x18))
    {
        ICM20608_CS_H();

        return false;
    }
    ICM20608_CS_H();

    // 加速度计±16G量程
    ICM20608_CS_L();
    if (!spi_write_byte_sub(ICM20608_DEV_PATH, (ICM20_ACCEL_CONFIG & 0x7F), 0x18))
    {
        ICM20608_CS_H();

        return false;
    }
    ICM20608_CS_H();

    // 陀螺仪低通滤波BW=20Hz
    ICM20608_CS_L();
    if (!spi_write_byte_sub(ICM20608_DEV_PATH, (ICM20_CONFIG & 0x7F), 0x04))
    {
        ICM20608_CS_H();

        return false;
    }
    ICM20608_CS_H();

    // 加速度计低通滤波BW=21.2Hz
    ICM20608_CS_L();
    if (!spi_write_byte_sub(ICM20608_DEV_PATH, (ICM20_ACCEL_CONFIG2 & 0x7F), 0x04))
    {
        ICM20608_CS_H();

        return false;
    }
    ICM20608_CS_H();

    // 打开加速度计和陀螺仪所有轴
    ICM20608_CS_L();
    if (!spi_write_byte_sub(ICM20608_DEV_PATH, (ICM20_PWR_MGMT_2 & 0x7F), 0x00))
    {
        ICM20608_CS_H();

        return false;
    }
    ICM20608_CS_H();

    // 关闭低功耗
    ICM20608_CS_L();
    if (!spi_write_byte_sub(ICM20608_DEV_PATH, (ICM20_LP_MODE_CFG & 0x7F), 0x00))
    {
        ICM20608_CS_H();

        return false;
    }
    ICM20608_CS_H();

    // 关闭FIFO
    ICM20608_CS_L();
    if (!spi_write_byte_sub(ICM20608_DEV_PATH, (ICM20_FIFO_EN & 0x7F), 0x00))
    {
        ICM20608_CS_H();

        return false;
    }
    ICM20608_CS_H();

    return true;
}

/**
 * @brief  ICM20608关闭设备
 * @return true : 成功
 * @return false: 失败
 */
bool icm20608_close(void)
{
    return spi_close(ICM20608_DEV_PATH);
}

/**
 * @brief  ICM20608读取设备ID
 * @param  id: 输出参数, 读取到的设备id
 * @return true : 成功
 * @return false: 失败
 */
bool icm20608_read_id(uint8_t *id)
{
    bool ret = false;

    // 读取设备id
    ICM20608_CS_L();
    ret = spi_read_byte_sub(id, ICM20608_DEV_PATH, (ICM20_WHO_AM_I | 0x80));
    ICM20608_CS_H();

    return ret;
}

/**
 * @brief  ICM20608读取加速度值
 * @param  accel_x: 输出参数, 加速度x轴实际数据(单位: g)
 * @param  accel_y: 输出参数, 加速度y轴实际数据(单位: g)
 * @param  accel_z: 输出参数, 加速度z轴实际数据(单位: g)
 * @return true : 成功
 * @return false: 失败
 */
bool icm20608_read_accel(float *accel_x, float *accel_y, float *accel_z)
{
    uint8_t read_data[6] = {0};

    // 读取加速度数据
    ICM20608_CS_L();
    usleep(50 * 1000);
    if (!spi_read_nbyte_sub(read_data, ICM20608_DEV_PATH, (ICM20_ACCEL_XOUT_H | 0x80), 6))
    {
        ICM20608_CS_H();

        return false;
    }
    ICM20608_CS_H();

    // 原始值
    *accel_x = (short)((read_data[0] << 8) | read_data[1]);
    *accel_y = (short)((read_data[2] << 8) | read_data[3]);
    *accel_z = (short)((read_data[4] << 8) | read_data[5]);

    // 原始值转实际值
    *accel_x /= 2048.0;
    *accel_y /= 2048.0;
    *accel_z /= 2048.0;

    return true;
}

/**
 * @brief  ICM20608读取温度值
 * @param  temp: 输出参数, 温度实际值(单位: °C)
 * @return true : 成功
 * @return false: 失败
 */
bool icm20608_read_temp(float *temp)
{
    uint8_t read_data[2] = {0};

    // 读取温度
    ICM20608_CS_L();
    if (!spi_read_nbyte_sub(read_data, ICM20608_DEV_PATH, (ICM20_TEMP_OUT_H | 0x80), 2))
    {
        ICM20608_CS_H();

        return false;
    }
    ICM20608_CS_H();

    // 原始值
    *temp = (short)((read_data[0] << 8) | read_data[1]);

    // 原始值转实际值
    *temp = (((*temp - 25) / 326.8) + 25);

    return true;
}

/**
 * @brief  ICM20608读取陀螺仪值
 * @param  gyro_x: 输出参数, 陀螺仪x轴实际数据(单位: °/S)
 * @param  gyro_y: 输出参数, 陀螺仪y轴实际数据(单位: °/S)
 * @param  gyro_z: 输出参数, 陀螺仪z轴实际数据(单位: °/S)
 * @return true : 成功
 * @return false: 失败
 */
bool icm20608_read_gyro(float *gyro_x, float *gyro_y, float *gyro_z)
{
    uint8_t read_data[6] = {0};

    // 读取陀螺仪数据
    ICM20608_CS_L();
    if (!spi_read_nbyte_sub(read_data, ICM20608_DEV_PATH, (ICM20_GYRO_XOUT_H | 0x80), 6))
    {
        ICM20608_CS_H();

        return false;
    }

    // 原始值
    *gyro_x = (short)((read_data[0] << 8) | read_data[1]);
    *gyro_y = (short)((read_data[2] << 8) | read_data[3]);
    *gyro_z = (short)((read_data[4] << 8) | read_data[5]);

    // 原始值转实际值
    *gyro_x /= 16.4;
    *gyro_y /= 16.4;
    *gyro_z /= 16.4;

    return true;
}
