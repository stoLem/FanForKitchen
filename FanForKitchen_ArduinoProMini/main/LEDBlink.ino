// моргнём count раз
void LedBlink(uint8_t count) {
  for (uint8_t i = 0; i < count - 1; i++ ) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(300);
    digitalWrite(LED_BUILTIN, LOW);
    delay(400);
  }
  digitalWrite(LED_BUILTIN, HIGH);
  delay(300); 
  digitalWrite(LED_BUILTIN, LOW);
}


void BlinkRateSpeed(uint16_t nubmer) {
  uint16_t mask;
  uint8_t iByte;
  boolean isOne = false;

  delay(800);
  for (int8_t i = 15; i > -1; i--) {
    mask = 0;
    bitSet(mask, i);
    iByte = (nubmer & mask) >> i;
    if (isOne == false && iByte == 1) {
      isOne = true;
    }
    if (isOne == true) {
      if (iByte == 0) {
        digitalWrite(LED_BUILTIN, HIGH);
        delay(150);
        digitalWrite(LED_BUILTIN, LOW);
        delay(800);
      } else {
        digitalWrite(LED_BUILTIN, HIGH);
        delay(800);
        digitalWrite(LED_BUILTIN, LOW);
        delay(800);
      }
    }
  }
}
