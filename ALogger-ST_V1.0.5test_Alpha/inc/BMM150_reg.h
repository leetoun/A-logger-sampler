#ifndef __BMM150_REG_H
#define __BMM150_REG_H

#define SOFT_RESET          0x82
#define SUSPEND             0
#define SLEEP               1

//  BMM_CTRL
#define OPMODE_Pos          1
#define NORMAL_MODE         (0 << OPMODE_Pos)
#define FORCE_MODE          (1 << OPMODE_Pos)
#define SLEEP_MODE          (3 << OPMODE_Pos)

#define ODR_Pos             3
#define ODR_10              (0 << ODR_Pos)
#define ODR_2               (1 << ODR_Pos)
#define ODR_6               (2 << ODR_Pos)
#define ODR_8               (3 << ODR_Pos)
#define ODR_15              (4 << ODR_Pos)
#define ODR_20              (5 << ODR_Pos)
#define ODR_25              (6 << ODR_Pos)
#define ODR_30              (7 << ODR_Pos)

//  BMM_SENS_CTRL
#define DRPin_Pos           7
#define DR_EN               (1 << DRPin_Pos)
#define DR_DIS              (0 << DRPin_Pos)

#define INTPin_Pos          6
#define INT_EN              (1 << INTPin_Pos)
#define INT_DIS             (0 << INTPin_Pos)

#define EN_Z_Pos            5
#define AXIS_Z_EN           (0 << EN_Z_Pos)
#define AXIS_Z_DIS          (1 << EN_Z_Pos)

#define EN_Y_Pos            4
#define AXIS_Y_EN           (0 << EN_Y_Pos)
#define AXIS_Y_DIS          (1 << EN_Y_Pos)

#define EN_X_Pos            3
#define AXIS_X_EN           (0 << EN_X_Pos)
#define AXIS_X_DIS          (1 << EN_X_Pos)

#define DRPolarity_Pos      2
#define DR_ACTIVE_H         (1 << DRPolarity_Pos)
#define DR_ACTIVE_L         (0 << DRPolarity_Pos)

#define INTPolarity_Pos     0
#define INT_ACTIVE_H        (1 << INTPolarity_Pos)
#define INT_ACTIVE_L        (0 << INTPolarity_Pos)

#define INT_LATCH_Pos       1
#define INT_LATCH_EN        (1 << INT_LATCH_Pos)
#define INT_LATCH_DIS       (0 << INT_LATCH_Pos)

#endif
