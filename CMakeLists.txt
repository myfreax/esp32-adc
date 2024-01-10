if("${IDF_VERSION_MAJOR}.${IDF_VERSION_MINOR}" VERSION_GREATER_EQUAL "5.0")
idf_component_register(
  SRCS "adc.c"
  INCLUDE_DIRS "include"
  REQUIRES esp_adc
  )
else()
idf_component_register(
  SRCS "adc.c"
  INCLUDE_DIRS "include"
  REQUIRES esp_adc_cal
  )
endif()
