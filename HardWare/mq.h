#ifndef __MQ_H
#define __MQ_H

#include "adc.h"
#include "Delay.h"
#include "math.h"

float mq_get_value_mq4(void);
uint32_t mq_get_value_adc_mq4();
float mq_get_value_vrl_mq4(void);

#endif
