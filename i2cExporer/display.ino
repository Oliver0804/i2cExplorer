
void breathLED() {
  unsigned long currentTime = millis();
  float elapsed = (currentTime - startTime) % interval; // 计算经过时间的一部分
  float phase = (elapsed / interval) * TWO_PI; // 将时间映射到0到2π之间

  // 使用正弦波调整亮度
  float brightness = (sin(phase) + 1) / 2; // 将正弦波结果从-1~1映射到0~1
  brightness = lowerLimit + brightness * (upperLimit - lowerLimit); // 映射到亮度范围

  analogWrite(ledPin, int(brightness));
}

#define BLACK   0x0000
#define BROWN   0xA145 // 近似棕色
#define RED     0xF800
#define ORANGE  0xFD20 // 近似橙色
#define YELLOW  0xFFE0
#define GREEN   0x07E0
#define BLUE    0x001F
#define PURPLE  0x780F // 近似紫色
#define GRAY    0x8410 // 近似灰色
#define WHITE   0xFFFF
#define CYAN     0x07FF
#define MAGENTA  0xF81F

void gfxInit() {
  Serial.println("Arduino_GFX");
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


}
void gfxShowLogo() {
  gfx->setRotation(0);
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
  int startY = ((gfx->height() - totalHeight) / 2)-5; // 垂直居中

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
  unsigned long currentTime = millis();
  
  float elapsed = currentTime - startTime;

  // 根据经过的时间计算旋转角度
  float angle = elapsed * rotationSpeed;
  angle = fmod(angle, TWO_PI); // 保持角度在0到2π之间

  // 计算点的新位置
  int x = centerX + radius * cos(angle);
  int y = centerY + radius * sin(angle);

  // 清除旧点和绘制新点的逻辑保持不变...
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
void rotatePoint_old() {
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


void gfxDisplayI2cPin(int sdaPin, int sclPin) {
  gfxShowLogo() ;
  //gfx->fillScreen(0x0000); // 清空屏幕

  // 设置文本旋转为90度
  gfx->setRotation(1);
  int textX = 190;
  int textY = 80;
  int textYplus = 17;
  int y = 0;

  // 显示VCC和GND
  gfx->setTextSize(2); // 设置文字大小
  gfx->setTextColor(RED, GREEN);
  gfx->setCursor(textX, textY + (textYplus * y++)); // 设置文字位置
  gfx->println("VCC");

  gfx->setTextColor(BROWN, GREEN);
  gfx->setCursor(textX, textY + (textYplus * y++));
  gfx->println("GND");

  // 显示SDA和SCL，颜色根据引脚号变化
  uint16_t sdaColor = (sdaPin == 2 ? CYAN : MAGENTA);
  uint16_t sclColor = (sclPin == 3 ? MAGENTA : CYAN);
  uint16_t sdaBgColor = (sdaPin == 2 ? BLACK : 0xFD20);
  uint16_t sclBgColor = (sclPin == 3 ? 0xFD20 : BLACK);
  gfx->setTextColor(sdaColor, sdaBgColor);
  gfx->setCursor(textX, textY + (textYplus * y++));
  gfx->println(sdaPin == 2 ? "SDA" : "SCL");

  gfx->setTextColor(sclColor, sclBgColor);
  gfx->setCursor(textX, textY + (textYplus * y++));
  gfx->println(sclPin == 3 ? "SCL" : "SDA");

  // 重置文本旋转为正常
  gfx->setRotation(0);
}
