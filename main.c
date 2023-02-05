/**
 * @file      : main.c
 * @brief     : file brief
 * @author    : huenrong (huenrong1028@outlook.com)
 * @date      : 2023-02-05 18:16:39
 *
 * @copyright : Copyright (c) 2023 huenrong
 *
 * @history   : date       author          description
 *              2023-02-05 huenrong        创建文件
 *
 */

#include <stdio.h>
#include <unistd.h>

#include "./icm20608/icm20608.h"

/**
 * @brief  : 程序入口
 * @param  : argc: 输入参数, 参数个数
 * @param  : argv: 输入参数, 参数列表
 * @return : 成功: 0
 *           失败: 其它
 */
int main(int argc, char *argv[])
{
    // ICM20608初始化
    if (!icm20608_init())
    {
        perror("init icm20608 fail");

        return -1;
    }

    // ICM20608读取设备ID
    uint8_t icm20608_id = 0;
    if (!icm20608_read_id(&icm20608_id))
    {
        printf("read icm20608 id fail\n");

        return -1;
    }

    printf("icm20608 id: 0x%02X\n", icm20608_id);

    while (true)
    {
        // ICM20608读取加速度值
        float accel_x = 0;
        float accel_y = 0;
        float accel_z = 0;
        if (!icm20608_read_accel(&accel_x, &accel_y, &accel_z))
        {
            printf("read icm20608 accel fail\n");
        }

        // ICM20608读取温度值
        float temp = 0;
        if (!icm20608_read_temp(&temp))
        {
            printf("read icm20608 temp fail\n");
        }

        // icm20608读取陀螺仪值
        float gyro_x = 0;
        float gyro_y = 0;
        float gyro_z = 0;
        if (!icm20608_read_gyro(&gyro_x, &gyro_y, &gyro_z))
        {
            printf("read icm20608 gyro fail\n");
        }

        // 打印各参数
        printf("accel_x = %.2f (g), accel_x = %.2f (g), accel_x = %.2f (g)\n", accel_x, accel_y, accel_z);
        printf("gyro_x = %.2f (°/S), gyro_x = %.2f (°/S), gyro_x = %.2f (°/S)\n", gyro_x, gyro_y, gyro_z);
        printf("temp = %.2f (°C)\n\n", temp);
        printf("==========================================================\n\n");

        sleep(1);
    }

    icm20608_close();

    return 0;
}
