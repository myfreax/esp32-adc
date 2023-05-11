## ESP32 ADC Component
Analog to Digital Converter (ADC) Component For ESP32

## Usage
```
git submodule add git@github.com:myfreax/esp32-adc.git components/adc
```
```c
#include "adc.h"

void app_main(){
  esp_adc_cal_characteristics_t* adc_chars =
      adc_config(ADC1_CHANNEL_0, ADC_WIDTH_BIT_DEFAULT, 1100,
                 ADC_ATTEN_DB_11);               
  uint32_t voltage = adc_voltage(ADC1_CHANNEL_0, adc_chars);               
}
```