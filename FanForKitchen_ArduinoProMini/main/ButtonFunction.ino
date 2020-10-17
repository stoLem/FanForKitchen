void SetSpeed(uint8_t setMode) {
  SetSpeed(setMode, false);
}

void SetSpeed(uint8_t setMode, boolean force) {
  if (force == false) {
    switch (setMode) {
      case SLOW:
        if (speedMode != SLOW) {
          speedMode = SLOW;
          fanSpeed = FAN_SPEED_SLOW;
          bladeSpeed = (uint16_t) ((uint32_t)fanSpeed * blades * periodCounter / 30000);
          regulator.setpoint = bladeSpeed;
        }
        break;
      case MEDIUM:
        if (speedMode != MEDIUM) {
          speedMode = MEDIUM;
          fanSpeed = FAN_SPEED_MEDIUM;
          bladeSpeed = (uint16_t) ((uint32_t)fanSpeed * blades * periodCounter / 30000);
          regulator.setpoint = bladeSpeed;
        }
        break;
      case FAST:
        if (speedMode != FAST) {
          speedMode = FAST;
          fanSpeed = FAN_SPEED_FAST;
          bladeSpeed = (uint16_t) ((uint32_t)fanSpeed * blades * periodCounter / 30000);
          regulator.setpoint = bladeSpeed;
        }
        break;
    }
  } else {
    switch (setMode) {
      case SLOW:
        speedMode = SLOW;
        fanSpeed = FAN_SPEED_SLOW;
        break;
      case MEDIUM:
        speedMode = MEDIUM;
        fanSpeed = FAN_SPEED_MEDIUM;
        break;
      case FAST:
        speedMode = FAST;
        fanSpeed = FAN_SPEED_FAST;
        break;
    }
    bladeSpeed = (uint16_t) ((uint32_t)fanSpeed * blades * periodCounter / 30000);
    regulator.setpoint = bladeSpeed;
  }
}
