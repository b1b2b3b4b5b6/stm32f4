#ifndef ADC_SAMPLE_H
#define ADC_SAMPLE_H

#include "init.h"

#define     ADC1_DR_Address     ((uint32_t)ADC1_BASE+0x4c)

extern u32 frequency;
extern __IO uint16_t ADC1OscConver[320];
void Tim2Config(void);
void Adc1Config(void);
void Tim4Config();
void Tim12Config();
void StartSample();

#endif