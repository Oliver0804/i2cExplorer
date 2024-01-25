void readBTN() {
  if (digitalRead(PICO_ONBORAD_BTN) == LOW) {
    // 如果按钮A被按下，执行I2C扫描
    scanAddresses();
    delay(500); // 简单的防抖动延迟
  }
}
