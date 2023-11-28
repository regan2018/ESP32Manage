#include <Arduino.h>

class PWMController {
public:
  PWMController(uint8_t pin, uint32_t frequency, uint8_t resolution);
  void setDutyCycle(uint8_t percent);

private:
  uint8_t _pin;
  uint32_t _frequency;
  uint8_t _resolution;
};
