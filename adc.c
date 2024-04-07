#include "adc.h"
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)
#include "esp_log.h"
static const char* TAG = "ADC";

static adc_oneshot_unit_handle_t adc_oneshot_handle;

static bool adc_calibration_init(adc_unit_t unit, adc_channel_t channel,
                                 adc_atten_t atten,
                                 adc_cali_handle_t* out_handle);
esp_err_t adc_init(adc_unit_t unit) {
  adc_oneshot_unit_init_cfg_t unit_config = {
      .unit_id = unit,
  };
  return adc_oneshot_new_unit(&unit_config, &adc_oneshot_handle);
}

adc_chan_t* adc_chan_config(adc_channel_t channel, adc_bitwidth_t width_bit,
                            adc_atten_t atten) {
  adc_chan_t* adc = malloc(sizeof(adc_chan_t));
  adc->atten = atten;
  adc->channel = channel;
  adc->adc_cali_handle = malloc(sizeof(adc_cali_handle_t));
  adc->adc_oneshot_handle = &adc_oneshot_handle;
  adc_oneshot_chan_cfg_t chan_config = {
      .bitwidth = width_bit,
      .atten = atten,
  };
  ESP_ERROR_CHECK(
      adc_oneshot_config_channel(adc_oneshot_handle, channel, &chan_config));
  bool calibration =
      adc_calibration_init(ADC_UNIT_1, channel, atten, &adc->adc_cali_handle);
  adc->calibration = calibration;
  return adc;
}

uint32_t adc_voltage(adc_chan_t* adc) {
  static uint32_t calibrat_voltage = 0;
  int adc_raw;
  int voltage;
  esp_err_t err =
      adc_oneshot_read(*adc->adc_oneshot_handle, adc->channel, &adc_raw);
  if (adc->calibration && adc_raw > 0 && err == ESP_OK) {
    err = adc_cali_raw_to_voltage(adc->adc_cali_handle, adc_raw, &voltage);
    if (voltage > 0 && err == ESP_OK) {
      calibrat_voltage = (uint32_t)voltage;
      return calibrat_voltage;
    }
  }
  return calibrat_voltage;
}

static bool adc_calibration_init(adc_unit_t unit, adc_channel_t channel,
                                 adc_atten_t atten,
                                 adc_cali_handle_t* out_handle) {
  adc_cali_handle_t handle = NULL;
  esp_err_t ret = ESP_FAIL;
  bool calibrated = false;

#if ADC_CALI_SCHEME_CURVE_FITTING_SUPPORTED
  if (!calibrated) {
    adc_cali_curve_fitting_config_t cali_config = {
        .unit_id = unit,
        .chan = channel,
        .atten = atten,
        .bitwidth = ADC_BITWIDTH_DEFAULT,
    };
    ret = adc_cali_create_scheme_curve_fitting(&cali_config, &handle);
    if (ret == ESP_OK) {
      calibrated = true;
    }
  }
#endif

#if ADC_CALI_SCHEME_LINE_FITTING_SUPPORTED
  if (!calibrated) {
    adc_cali_line_fitting_config_t cali_config = {
        .unit_id = unit,
        .atten = atten,
        .bitwidth = ADC_BITWIDTH_DEFAULT,
    };
    ret = adc_cali_create_scheme_line_fitting(&cali_config, &handle);
    if (ret == ESP_OK) {
      calibrated = true;
    }
  }
#endif

  *out_handle = handle;
  if (ret == ESP_ERR_NOT_SUPPORTED || !calibrated) {
    ESP_LOGW(TAG, "eFuse not burnt, skip software calibration");
  }

  return calibrated;
}

#else
esp_adc_cal_characteristics_t* adc_config(adc1_channel_t adc_channel,
                                          adc_bits_width_t width_bit,
                                          uint32_t default_vref,
                                          adc_atten_t atten) {
  ESP_ERROR_CHECK(adc1_config_width(width_bit));
  ESP_ERROR_CHECK(adc1_config_channel_atten(adc_channel, atten));

  esp_adc_cal_characteristics_t* adc_chars =
      calloc(1, sizeof(esp_adc_cal_characteristics_t));
  esp_adc_cal_characterize(ADC_UNIT_1, atten, width_bit, default_vref,
                           adc_chars);
  return adc_chars;
}

uint32_t adc_voltage(adc1_channel_t adc_channel,
                     esp_adc_cal_characteristics_t* adc_chars) {
  uint32_t adc_reading = 0;
  for (int i = 0; i < 64; i++) {
    adc_reading += adc1_get_raw((adc1_channel_t)adc_channel);
  }
  adc_reading /= 64;
  uint32_t voltage = esp_adc_cal_raw_to_voltage(adc_reading, adc_chars);
  return voltage;
}
#endif
