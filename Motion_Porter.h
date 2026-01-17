// ----- USED BUTTONS ----- //
void dumpGamepad(ControllerPtr ctl) {
  Serial.printf(
  "idx=%d, dpad: 0x%02x, buttons: 0x%04x, axis L: %4d, %4d, axis R: %4d, %4d\n",
  ctl->index(),        // Controller Index
  ctl->dpad(),         // D-pad
  ctl->buttons(),      // bitmask of pressed buttons
  ctl->axisX(),        // (-511 - 512) left X Axis
  ctl->axisY(),        // (-511 - 512) left Y axis
  ctl->axisRX(),       // (-511 - 512) right X axis
  ctl->axisRY()        // (-511 - 512) right Y axis
  );
}

// Function to convert from rawValue to dynamicValue //
float dynamicValue(int16_t rawValue, float exponent) {
  if (abs(rawValue) < 25) return 0;

  float normalize = (float)rawValue / 512.0;

  float sign = (normalize > 0) ? 1.0 : -1.0;

  return pow(fabs(normalize), exponent) * sign;
}

// Function for applying the PWM value to the motor //
void applyPWM(int motorIdx, float speed) {
  // Control the direction of the wheel rotate
  if (speed >= 0) {
    digitalWrite(pinDIR[motorIdx], HIGH);
  } else {
    digitalWrite(pinDIR[motorIdx], LOW);
  }

  // Getting PWM value which set the motor speed
  int pwmValue = (int)fabs(speed);

  if (pwmValue > 255) pwmValue = 255;

  // Sending signal to the motor
  analogWrite(pinPWM[motorIdx], pwmValue);
}

// --------------- MAIN --------------- //
void processGamepad(ControllerPtr ctl) {
  // Controller buttons mapping
  int joystickY = ctl->axisY(); // left Y Axis
  int joystickX = ctl->axisX(); // left X Axis
  int joystickRY = ctl->axisRY(); // right Y Axis
  int joystickRX = ctl->axisRX(); // right X Axis
  bool L1Pressed = ctl->buttons() == 0x0010; // L1
  bool R1Pressed = ctl->buttons() == 0x0020; // R1
  bool L2Pressed = ctl->buttons() == 0x0040; // L2

  // Calling dynamicValue functions for joystick axis
  float moveY = dynamicValue(joystickY, 2.0);
  float moveX = dynamicValue(joystickX, 2.0);
  float liftY = dynamicValue(joystickRY, 2.0);
  float rotX = dynamicValue(joystickRX, 2.0);

  // ----- L2 PRESSED ----- //
  float speedMultiplier = 1.0;
  if (L2Pressed == 1) {
    speedMultiplier = 0.5; // Reduce the speed by half
  }

  // ----- Mecanum Kinematic ----- //
  float frontLeft = (moveY + moveX + rotX) * speedMultiplier;
  float frontRight = (moveY - moveX - rotX) * speedMultiplier;
  float backLeft = (moveY - moveX + rotX) * speedMultiplier;
  float backRight = (moveY + moveX - rotX) * speedMultiplier;

  // Normalize the value in case the value exceed one (> 1)
  float maxValue = fmax(1.0, fmax(fabs(frontLeft), fmax(fabs(frontRight), fmax(fabs(backLeft), fabs(backRight)))));
  frontLeft /= maxValue;
  frontRight /= maxValue;
  backLeft /= maxValue;
  backRight /= maxValue;

  // Calling the applyPWM function
  applyPWM(0, frontLeft * 255);
  applyPWM(1, frontRight * 255);
  applyPWM(2, backLeft * 255);
  applyPWM(3, backRight * 255);
  
  // Printing PWM value from each motor to serial monitor
  Serial.printf("FL : %.2f\n", frontLeft);
  Serial.printf("FR : %.2f\n", frontRight);
  Serial.printf("BL : %.2f\n", backLeft);
  Serial.printf("BR : %.2f\n", backRight);

  // ----- Lifter Up & Down ----- //
  if (liftY != 0) {
    int finalLifterSpeed = liftY * 255 * speedMultiplier;
    if (liftY < 0) {
      digitalWrite(pinLifter2, HIGH);
    } else {
      digitalWrite(pinLifter2, LOW);
    }

    int speedLifter = (int)(fabs(liftY) * 255);

    if (speedLifter > 255) speedLifter = 255;

    analogWrite(pinLifter1, speedLifter);

    Serial.printf("Lifter = %.2f\n", finalLifterSpeed);
  }

  // ----- R1 Pressed ----- //
  if (R1Pressed == 1) {
    // Tighten up the gripper
  }

  // ----- L1 Pressed ----- //
  if (L1Pressed == 1) {
    // Loosen up the griper
  }
  
  // Call this function if you want to see
  // dumpGamepad(ctl);
}