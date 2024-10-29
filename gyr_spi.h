/*
 * gyr_spi.h
 *
 * Header file for gyr_spi.c
 *
 * This file is part of the libopencm3 project.
 *
 * Provides SPI setup and functions for interfacing with the gyroscope.
 *
 * Authors: Uwe Hermann, Stephen Caudle, Fernando Cortes, Guillermo Rivera
 * License: GNU Lesser General Public License v3.0
 */

#ifndef GYR_SPI_H
#define GYR_SPI_H

#include <stdint.h>

// Define gyroscope control registers and settings
#define GYR_CTRL_REG1      0x20
#define GYR_CTRL_REG4      0x23
#define GYR_OUT_X_L        0x28
#define GYR_OUT_X_H        0x29
#define GYR_OUT_Y_L        0x2A
#define GYR_OUT_Y_H        0x2B
#define GYR_OUT_Z_L        0x2C
#define GYR_OUT_Z_H        0x2D
#define GYR_RNW            0x80  // Read/Not Write bit

// Gyroscope control register settings
#define GYR_CTRL_REG1_PD       (1 << 3)   // Power down mode
#define GYR_CTRL_REG1_XEN      (1 << 1)   // Enable X-axis
#define GYR_CTRL_REG1_YEN      (1 << 0)   // Enable Y-axis
#define GYR_CTRL_REG1_ZEN      (1 << 2)   // Enable Z-axis
#define GYR_CTRL_REG1_BW_SHIFT 4          // Bandwidth shift value
#define GYR_CTRL_REG4_FS_SHIFT 4          // Full-scale shift value

// Gyroscope sensitivity
#define SENSITIVITY_2000DPS    70         // Sensitivity for 2000 dps

// Function prototypes
void gyr_setup(void);
int16_t gyr_readX(void);
int16_t gyr_readY(void);
int16_t gyr_readZ(void);

#endif // GYR_SPI_H

