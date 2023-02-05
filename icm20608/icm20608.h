/**
 * @file      : icm20608.h
 * @brief     : ICM20608驱动头文件
 * @author    : huenrong (huenrong1028@outlook.com)
 * @date      : 2023-02-05 17:48:52
 *
 * @copyright : Copyright (c) 2023 huenrong
 *
 * @history   : date       author          description
 *              2023-02-05 huenrong        创建文件
 *
 */

#ifndef __ICM20608_H
#define __ICM20608_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief  ICM20608初始化
 * @return true : 成功
 * @return false: 失败
 */
bool icm20608_init(void);

/**
 * @brief  ICM20608关闭设备
 * @return true : 成功
 * @return false: 失败
 */
bool icm20608_close(void);

/**
 * @brief  ICM20608读取设备ID
 * @param  id: 输出参数, 读取到的设备id
 * @return true : 成功
 * @return false: 失败
 */
bool icm20608_read_id(uint8_t *id);

/**
 * @brief  ICM20608读取加速度值
 * @param  accel_x: 输出参数, 加速度x轴实际数据(单位: g)
 * @param  accel_y: 输出参数, 加速度y轴实际数据(单位: g)
 * @param  accel_z: 输出参数, 加速度z轴实际数据(单位: g)
 * @return true : 成功
 * @return false: 失败
 */
bool icm20608_read_accel(float *accel_x, float *accel_y, float *accel_z);

/**
 * @brief  ICM20608读取温度值
 * @param  temp: 输出参数, 温度实际值(单位: °C)
 * @return true : 成功
 * @return false: 失败
 */
bool icm20608_read_temp(float *temp);

/**
 * @brief  ICM20608读取陀螺仪值
 * @param  gyro_x: 输出参数, 陀螺仪x轴实际数据(单位: °/S)
 * @param  gyro_y: 输出参数, 陀螺仪y轴实际数据(单位: °/S)
 * @param  gyro_z: 输出参数, 陀螺仪z轴实际数据(单位: °/S)
 * @return true : 成功
 * @return false: 失败
 */
bool icm20608_read_gyro(float *gyro_x, float *gyro_y, float *gyro_z);

#ifdef __cplusplus
}
#endif

#endif // __ICM20608_H
