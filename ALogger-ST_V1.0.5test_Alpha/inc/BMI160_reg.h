#ifndef __bmi160_H_
#define __bmi160_H_
#include <stdint.h>

#define REG_CHIPID                    0x00
#define REG_ERROR                     0x02
#define REG_PMU_STATUS                0x03
#define REG_DATA_MAG_X_L              0x04
#define REG_DATA_MAG_X_H              0x05
#define REG_DATA_MAG_Y_L              0x06
#define REG_DATA_MAG_Y_H              0x07
#define REG_DATA_MAG_Z_L              0x08
#define REG_DATA_MAG_Z_H              0x09
#define REG_DATA_RHALL_L              0x0A
#define REG_DATA_RHALL_H              0x0B
#define REG_DATA_GYR_X_L              0x0C
#define REG_DATA_GYR_X_H              0x0D
#define REG_DATA_GYR_Y_L              0x0E
#define REG_DATA_GYR_Y_H              0x0F
#define REG_DATA_GYR_Z_L              0x10
#define REG_DATA_GYR_Z_H              0x11
#define REG_DATA_ACC_X_L              0x12
#define REG_DATA_ACC_X_H              0x13
#define REG_DATA_ACC_Y_L              0x14
#define REG_DATA_ACC_Y_H              0x15
#define REG_DATA_ACC_Z_L              0x16
#define REG_DATA_ACC_Z_H              0x17
#define REG_SENSORTIME_0              0x18
#define REG_SENSORTIME_1              0x19
#define REG_SENSORTIME_2              0x1A
#define REG_STATUS                    0x1B
#define REG_INT_STATUS_0              0x1C
#define REG_INT_STATUS_1              0x1D
#define REG_INT_STATUS_2              0x1E
#define REG_INT_STATUS_3              0x1F
#define REG_TEMP_L                    0x20
#define REG_TEMP_H                    0x21
#define REG_FIFO_LEN_L                0x22
#define REG_FIFO_LEN_H                0x23
#define REG_FIFO_DATA                 0x24
#define REG_ACC_CONF                  0x40
#define REG_ACC_RANGE                 0x41
#define REG_GYR_CONF                  0x42
#define REG_GYR_RANGE                 0x43
#define REG_MAG_CONF                  0x44
#define REG_FIFO_DOWNS                0x45
#define REG_FIFO_WATERMARK            0x46
#define REG_FIFO_CONF                 0x47
#define REG_MAG_IF_0                  0x4B
#define REG_MAG_IF_1                  0x4C
#define REG_MAG_IF_2                  0x4D
#define REG_MAG_IF_3                  0x4E
#define REG_MAG_IF_4                  0x4F
#define REG_INT_EN_0                  0x50
#define REG_INT_EN_1                  0x51
#define REG_INT_EN_2                  0x52
#define REG_INT_OUT_CTRL              0x53
#define REG_INT_LATCH                 0x54
#define REG_INT_MAP_0                 0x55
#define REG_INT_MAP_1                 0x56
#define REG_INT_MAP_2                 0x57
#define REG_INT_DATA_0                0x58
#define REG_INT_DATA_1                0x59
#define REG_INT_LOWHIGH_0             0x5A
#define REG_INT_LOWHIGH_1             0x5B
#define REG_INT_LOWHIGH_2             0x5C
#define REG_INT_LOWHIGH_3             0x5D
#define REG_INT_LOWHIGH_4             0x5E
#define REG_INT_MOTION_0              0x5F
#define REG_INT_MOTION_1              0x60
#define REG_INT_MOTION_2              0x61
#define REG_INT_MOTION_3              0x62
#define REG_INT_TAP_0                 0x63
#define REG_INT_TAP_1                 0x64
#define REG_INT_ORIENT_0              0x65
#define REG_INT_ORIENT_1              0x66
#define REG_INT_FLAT_0                0x67
#define REG_INT_FLAT_1                0x68
#define REG_FOC_CONF                  0x69
#define REG_CONF                      0x6A
#define REG_IF_CONF                   0x6B
#define REG_PMU_TRIG                  0x6C
#define REG_SELF_TEST                 0x6D
#define REG_NV_CONF                   0x70
#define REG_OFFSET_0                  0x71
#define REG_OFFSET_1                  0x72
#define REG_OFFSET_2                  0x73
#define REG_OFFSET_3                  0x74
#define REG_OFFSET_4                  0x75
#define REG_OFFSET_5                  0x76
#define REG_OFFSET_6                  0x77
#define REG_STEP_CNT_0                0x78
#define REG_STEP_CNT_1                0x79
#define REG_STEP_CONF_0               0x7A
#define REG_STEP_CONF_1               0x7B
#define REG_CMD                       0x7E

// REG_CMD
#define CMD_SOFTRESET                 0xB6
#define CMD_FIFOFLUSH                 0xB0
#define CMD_ACC_SUSPEND               0x10
#define CMD_ACC_NORMAL                0x11
#define CMD_ACC_LOWPOWER              0x12
#define CMD_GYR_SUSPEND               0x14
#define CMD_GYR_NORMAL                0x15
#define CMD_GYR_LOWPOWER              0x17
#define CMD_MAG_SUSPEND               0x18
#define CMD_MAG_NORMAL                0x19
#define CMD_MAG_LOWPOWER              0x1A

// REG_INT_OUT_CTRL
#define INT2_EN_Pos                   7
#define INT2_EN                      (1<<INT2_EN_Pos)
#define INT2_DIS                     (0<<INT2_EN_Pos)
#define INT2_OD_Pos                   6
#define INT2_OD                      (1<<INT2_OD_Pos)
#define INT2_PP                      (0<<INT2_OD_Pos)
#define INT2_ACTIVE_Pos               5
#define INT2_ACTIVE_HIGN             (1<<INT2_ACTIVE_Pos)
#define INT2_ACTIVE_LOW              (0<<INT2_ACTIVE_Pos)
#define INT2_EDGE_Pos                 4
#define INT2_EDGE                    (1<<INT2_EDGE_Pos)
#define INT2_LEVEL                   (0<<INT2_EDGE_Pos)
#define INT1_EN_Pos                   3
#define INT1_EN                      (1<<INT1_EN_Pos)
#define INT1_DIS                     (0<<INT1_EN_Pos)
#define INT1_OD_Pos                   2
#define INT1_OD                      (1<<INT1_OD_Pos)
#define INT1_PP                      (0<<INT1_OD_Pos)
#define INT1_ACTIVE_Pos               1
#define INT1_ACTIVE_HIGN             (1<<INT1_ACTIVE_Pos)
#define INT1_ACTIVE_LOW              (0<<INT1_ACTIVE_Pos)
#define INT1_EDGE_Pos                 0
#define INT1_EDGE                    (1<<INT1_EDGE_Pos)
#define INT1_LEVEL                   (0<<INT1_EDGE_Pos)

// REG_ACC_CONF
#define ACC_US_Pos                    7
#define UNDERSAMPLE_EN               (1<<ACC_US_Pos)
#define UNDERSAMPLE_DIS              (0<<ACC_US_Pos)

#define ACC_BWP_Pos                   4
/*
 *  bwp | us = 0 | us=1
 *   00 | OSR4   | average    
 *   01 | OSR2   |  =
 *   10 | Normal |   2^bwp
 *   11 | CICOIS |   
 */
#define BWP(x)                       (x<<ACC_BWP_Pos)
/*
 *	ODR = 100 / 2 ^ (8 - val)
 */

#define ODR_25                        6
#define ODR_50                        7
#define ODR_100                       8
#define ODR_200                       9

// REG_ACC_RANGE
#define RANGE_2G                      3
#define RANGE_4G                      5
#define RANGE_8G                      8
#define RANGE_16G                     12
#endif
