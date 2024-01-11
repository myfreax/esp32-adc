#pragma once
#include "esp_idf_version.h"

#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)
#include <stdbool.h>

#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "esp_adc/adc_oneshot.h"

typedef struct {
  adc_channel_t channel;
  adc_atten_t atten;
  adc_oneshot_unit_handle_t* adc_oneshot_handle;
  bool calibration;
  adc_cali_handle_t adc_cali_handle;
} adc_chan_t;

esp_err_t adc_init(adc_unit_t unit);

adc_chan_t* adc_chan_config(adc_channel_t channel, adc_bitwidth_t width_bit,
                            adc_atten_t atten);
int adc_voltage(adc_chan_t* adc);
#else
#include "driver/adc.h"
#include "esp_adc_cal.h"
esp_adc_cal_characteristics_t* adc_config(adc1_channel_t adc_channel,
                                          adc_bits_width_t width_bit,
                                          uint32_t default_vref,
                                          adc_atten_t atten);
uint32_t adc_voltage(adc1_channel_t adc_channel,
                     esp_adc_cal_characteristics_t* adc_chars);
#endif