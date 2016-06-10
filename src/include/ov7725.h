#ifndef OV7725_H_
#define OV7725_H_

#include <stdint.h>
#include "settings.h"

/* Constants */
#define TIMEOUT_MAX      				10000
#define MAX_IMAGE_HEIGHT				64
#define MAX_IMAGE_WIDTH					64

/*
 * Resolution:
 * ROW_SIZE * BINNING_ROW <= MAX_IMAGE_WIDTH
 * COLUMN_SIZE * BINNING_COLUMN <= MAX_IMAGE_HEIGHT
 */

#define FULL_IMAGE_SIZE (144*108*2)
#define FULL_IMAGE_ROW_SIZE (144)
#define FULL_IMAGE_COLUMN_SIZE (108)

/* Functions */

/**
  * @brief  init sensor.
  */
uint8_t ov7725_init(void);

/**
  * @brief  Changes sensor context based on settings
  */
void ov7725_set_context(void);

#endif /* OV7725_H_ */
