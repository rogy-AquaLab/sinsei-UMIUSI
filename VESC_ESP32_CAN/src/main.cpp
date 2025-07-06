#include <Arduino.h>
#include "vesc_can_esp32.hpp"
constexpr uint32_t vesc_id = 74; //VESC-toolだと10進数表記

twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(GPIO_NUM_13, GPIO_NUM_12, TWAI_MODE_NORMAL);
twai_timing_config_t t_config = TWAI_TIMING_CONFIG_500KBITS();

aqua::VESC vesc(g_config, t_config, vesc_id);

void setup(void) 
{
  Serial.begin(115200);
}

void loop() {
  vesc.setServo(0);
  delay(1000);
  vesc.setServo(0.5);
  delay(1000);
  vesc.setServo(1);
  delay(1000);
}


