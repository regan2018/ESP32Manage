#include "PWMController.h"

PWMController::PWMController(uint8_t pin, uint32_t frequency, uint8_t resolution) {
  _pin = pin;
  _frequency = frequency;
  _resolution = resolution;

  ledcSetup(0, _frequency, _resolution);
  ledcAttachPin(_pin, 0);
}

void PWMController::setDutyCycle(uint8_t percent) {
  uint32_t duty = map(percent, 0, 100, 0, (1 << _resolution) - 1);
  ledcWrite(0, duty);
}
