void readBTN() {
  const unsigned long longPressTime = 1000; // 定义长按的时间阈值，例如1000毫秒
  unsigned long startTime;

  // 检查按钮是否被按下
  if (digitalRead(PICO_ONBORAD_BTN) == LOW) {
    startTime = millis(); // 记录按下的时间
    while (digitalRead(PICO_ONBORAD_BTN) == LOW) {
      // 如果按钮仍然被按住，等待直到释放或确认为长按
      if (millis() - startTime > longPressTime) {
        // 长按逻辑
        swapI2CPins();
        return; // 长按处理完成后返回
      }
    }
    // 短按逻辑
    scanAddresses();
    // delay(500); // 可以考虑添加防抖动延时
  }
}
