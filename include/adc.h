#pragma once
#include "driver/adc.h"
#include "esp_adc_cal.h"

esp_adc_cal_characteristics_t* adc_config(adc1_channel_t adc_channel,
                                          adc_bits_width_t width_bit,
                                          uint32_t default_vref,
                                          adc_atten_t atten);
uint32_t adc_voltage(adc1_channel_t adc_channel,
                     esp_adc_cal_characteristics_t* adc_chars);