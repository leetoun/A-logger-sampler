#ifndef __LIS3MDL_REG_H
#define __LIS3MDL_REG_H


//  CTRL_REG 1
#define TEMP_Pos            7
#define TEMP_EN             (1 << TEMP_Pos)
#define TEMP_DIS            (0 << TEMP_Pos)

#define OMXY_MODE_Pos       5
#define LOW_MODE            (0 << OMXY_MODE_Pos)
#define MED_MODE            (1 << OMXY_MODE_Pos)
#define HIGH_MODE           (2 << OMXY_MODE_Pos)
#define ULTRA_MODE          (3 << OMXY_MODE_Pos)

#define ODR_Pos             2
#define ODR_0_625           (0 << ODR_Pos)
#define ODR_1_25            (1 << ODR_Pos)
#define ODR_2_5             (2 << ODR_Pos)
#define ODR_5               (3 << ODR_Pos)
#define ODR_10              (4 << ODR_Pos)
#define ODR_20              (5 << ODR_Pos)
#define ODR_40              (6 << ODR_Pos)
#define ODR_80              (7 << ODR_Pos)

#define FAST_MODE_Pos       1
#define FAST_MODE           (1 << FAST_MODE_Pos)

//  CTRL_REG 2
#define FS_Pos              5
#define FS_4G               (0 << FS_Pos)
#define FS_8G               (1 << FS_Pos)
#define FS_12G              (2 << FS_Pos)
#define FS_16G              (3 << FS_Pos)

#define REBOOT              0x08
#define SOFT_RST            0x04

//  CTRL_REG 3
#define SAMPLE_MODE_Pos     0
#define CONTINUOUS_MODE     (0 << SAMPLE_MODE_Pos)
#define SINGLE_MODE         (1 << SAMPLE_MODE_Pos)
#define STOP_MODE           (2 << SAMPLE_MODE_Pos)

//  CTRL_REG 4
#define OMZ_Pos             2
#define LOW_MODE_Z          (0 << OMZ_Pos)
#define MED_MODE_Z          (1 << OMZ_Pos)
#define HIGH_MODE_Z         (2 << OMZ_Pos)
#define ULTRA_MODE_Z        (3 << OMZ_Pos)


#define EN_Z_Pos            5
#define AXIS_Z_EN           (1 << EN_Z_Pos)
#define AXIS_Z_DIS          (0 << EN_Z_Pos)

#define EN_Y_Pos            6
#define AXIS_Y_EN           (1 << EN_Y_Pos)
#define AXIS_Y_DIS          (0 << EN_Y_Pos)

#define EN_X_Pos            7
#define AXIS_X_EN           (1 << EN_X_Pos)
#define AXIS_X_DIS          (0 << EN_X_Pos)

#define INTPolarity_Pos     2
#define INT_ACTIVE_H        (1 << INTPolarity_Pos)
#define INT_ACTIVE_L        (0 << INTPolarity_Pos)

#define INT_LATCH_Pos       1
#define INT_LATCH_EN        (0 << INT_LATCH_Pos)
#define INT_LATCH_DIS       (1 << INT_LATCH_Pos)

#define INTPin_Pos          0
#define INT_EN              (1 << INTPin_Pos)
#define INT_DIS             (0 << INTPin_Pos)


#endif
