
void breathLED() {
  unsigned long currentTime = millis();
  float elapsed = (currentTime - startTime) % interval; // 计算经过时间的一部分
  float phase = (elapsed / interval) * TWO_PI; // 将时间映射到0到2π之间

  // 使用正弦波调整亮度
  float brightness = (sin(phase) + 1) / 2; // 将正弦波结果从-1~1映射到0~1
  brightness = lowerLimit + brightness * (upperLimit - lowerLimit); // 映射到亮度范围

  analogWrite(ledPin, int(brightness));
}

void gfxInit() {
  Serial.println("Arduino_GFX Hello World example");

#ifdef GFX_EXTRA_PRE_INIT
  GFX_EXTRA_PRE_INIT();
#endif
  if (!gfx->begin())
  {
    Serial.println("gfx->begin() failed!");
  }
  gfx->fillScreen(BLACK);

#ifdef GFX_BL
  pinMode(GFX_BL, OUTPUT);
  digitalWrite(GFX_BL, HIGH);
#endif

  gfx->setCursor(10, 10);
  gfx->setTextColor(RED);
  gfx->println("Hello World!");
  gfx->fillScreen(0x0000); // 清空屏幕
  delay(500); // 5 seconds
}
void gfxShowLogo() {
  gfx->fillScreen(0x0000);  // 清屏
  gfx->setTextColor(0x07E0); // 设置文本颜色
  gfx->setTextSize(1); // 根据需要调整文本大小
  gfx->setTextWrap(false);  // 关闭文字自动换行

  const char *lines[] = {
    "        _____ ___   _____              ",
    "       |_   _|__ \\ / ____|             ",
    "         | |    ) | |                  ",
    " ______  | |   / /| |                  ",
    "|  ____|_| |_ / /_| |____              ",
    "| |__  |_____|____|\\_____| __ ___ _ __ ",
    "|  __|  \\ \\/ / '_ \\/ _ \\| '__/ _ \\ '__|",
    "| |____ >  <| |_) | (_) | | |  __/ |   ",
    "|______/_/\\_\\ .__/ \\___/|_|  \\___|_|   ",
    "            | |                        ",
    "         |_| Github : Oliver0804 "
  };

  int linesCount = sizeof(lines) / sizeof(lines[0]);
  int lineSpacing = 8; // 行间距
  int totalHeight = linesCount * lineSpacing; // 文本块总高度
  int startY = (gfx->height() - totalHeight) / 2; // 垂直居中

  for (int i = 0; i < linesCount; i++) {
    int lineLength = strlen(lines[i]) * 6; // 估算每行长度（每个字符宽度假设为6像素）
    int startX = (gfx->width() - lineLength) / 2; // 水平居中
    gfx->setCursor(startX, startY + i * lineSpacing);
    gfx->println(lines[i]);
  }
}

void rotatePoint() {
  static int oldX = 0;
  static int oldY = 0;
  static int oldFadeX = 0;
  static int oldFadeY = 0;

  // 更新角度
  angle += speed;
  if (angle > 2 * PI) {
    angle -= 2 * PI;
  }

  // 计算点的新位置
  int x = centerX + radius * cos(angle);
  int y = centerY + radius * sin(angle);

  // 清除旧点
  gfx->fillCircle(oldX, oldY, 2, 0x0000); // 使用背景颜色覆盖旧点
  for (float t = 0; t < 1; t += 0.1) {
    oldFadeX = centerX + radius * cos(angle - t);
    oldFadeY = centerY + radius * sin(angle - t);
    gfx->fillCircle(oldFadeX, oldFadeY, 1, 0x0000); // 清除拖尾
  }

  // 绘制新的旋转点
  gfx->fillCircle(x, y, 1, 0x07E0); // 绘制绿色点，点半径为2

  // 绘制拖尾效果
  for (float t = 0; t < 1; t += 0.1) {
    int fadeX = centerX + radius * cos(angle - t);
    int fadeY = centerY + radius * sin(angle - t);
    uint16_t fadeColor = gfx->color565(0, 255 * (1 - t), 0); // 颜色逐渐减淡
    gfx->fillCircle(fadeX, fadeY, 1, fadeColor);
  }

  // 更新旧点位置
  oldX = x;
  oldY = y;
}

void gfxTest() {
  gfx->setCursor(random(gfx->width()), random(gfx->height()));
  gfx->setTextColor(random(0xffff), random(0xffff));
  gfx->setTextSize(random(6) /* x scale */, random(6) /* y scale */, random(2) /* pixel_margin */);
  gfx->println("Hello World!");
}
void gfxShowi2c() {
  gfx->fillScreen(0x0000); // 清空屏幕
  if (foundCount == 0) {
    // 如果没有找到I2C设备
    gfx->setTextColor(0x0000, 0xF800); // 黑色字体，红色背景
    gfx->setTextSize(4);
    gfx->setCursor(10, (gfx->height() - 28) / 2); // 居中显示
    gfx->println("Not Found I2C");
  } else {
    // 如果找到了I2C设备
    for (int i = 0; i < foundCount; i++) {
      gfx->setCursor(random(gfx->width()), random(gfx->height()));
      gfx->setTextColor(0x07E0, 0x0000); // 绿色字体
      gfx->setTextSize(random(1, 6), random(1, 6), random(2)); // 随机字体大小和间距
      char buf[6];
      sprintf(buf, "0x%02X", foundAddresses[i]);
      gfx->println(buf);
    }
  }
}
