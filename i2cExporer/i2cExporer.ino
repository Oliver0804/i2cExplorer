/*
   https://github.com/earlephilhower/arduino-pico/releases/download/global/package_rp2040_index.json
*/
#include <Wire.h>


#include <Arduino_GFX_Library.h>
#define GFX_BL DF_GFX_BL // default backlight pin, you may replace DF_GFX_BL to actual backlight pin

Arduino_DataBus *bus = new Arduino_SWSPI(8 /* DC */, 9 /* CS */, 10 /* SCK */, 11 /* MOSI */, -1 /* MISO */);
Arduino_GFX *gfx = new Arduino_GC9A01(bus, 12 /* RST */, 0 /* rotation */, true /* IPS */);


int connectedI2CAddress = -1;
#define PICO_ONBORAD_LED 25
#define PICO_ONBORAD_SDA 12
#define PICO_ONBORAD_SCL 13
#define PICO_ONBORAD_BTN 22
#define MAX_I2C_DEVICES 128

byte foundAddresses[MAX_I2C_DEVICES];
int foundCount = 0;

float angle = 0.0; // 旋转角度
float radius = 118.0; // 旋转半径
int centerX = gfx->width() / 2; // 圆心X坐标
int centerY = gfx->height() / 2; // 圆心Y坐标
float speed = 0.02; // 旋转速度


const int ledPin = 16; // 使用GP16引脚
unsigned long startTime = millis(); // 记录开始时间
const int interval = 1500; // 完整呼吸周期（毫秒）
const int lowerLimit = 20;  // 亮度下限
const int upperLimit = 250; // 亮度上限

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
void gfxShowi2c(){
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
      gfx->setTextColor(0x07E0,0x0000); // 绿色字体
      gfx->setTextSize(random(1, 6), random(1, 6), random(2)); // 随机字体大小和间距
      char buf[6];
      sprintf(buf, "0x%02X", foundAddresses[i]);
      gfx->println(buf);
    }
  }
  }
void setup() {
  Serial.begin(230400);
  //while (!Serial) continue;
  gfxInit();
  gfxShowLogo();
  Wire1.setSDA(2);
  Wire1.setSCL(3);
  Wire1.begin();
  pinMode(PICO_ONBORAD_LED, OUTPUT);
  pinMode(PICO_ONBORAD_BTN, INPUT_PULLUP);
  digitalWrite(PICO_ONBORAD_LED, HIGH);
  pinMode(ledPin, OUTPUT);

  Serial.println("  _____ ___   _____ ______");
  Serial.println(" |_   _|__ \\ / ____|  ____|");
  Serial.println("   | |    ) | |    | |__  __  ___ __   ___  _ __ ___ _ __");
  Serial.println("   | |   / /| |    |  __| \\ \\/ / '_ \\ / _ \\| '__/ _ \\ '__|");
  Serial.println("  _| |_ / /_| |____| |____ >  <| |_) | (_) | | |  __/ |");
  Serial.println(" |_____|____|\\_____|______/_/\\_\\ .__/ \\___/|_|  \\___|_|");
  Serial.println("                               | |");
  Serial.println("                               |_|  Github : Oliver0804 ");
  showHelp(connectedI2CAddress != -1);
  showHelp(connectedI2CAddress != 1);
  digitalWrite(PICO_ONBORAD_LED, LOW);

  Serial.print("> ");

}
int shfitX = 0;
void loop() {
  //displayI2CAddresses();
  //gfxShowi2c();
  //gfxTest();
  breathLED();
  readBTN();
  rotatePoint();
  cmdParse();

}
void readBTN(){
    if (digitalRead(PICO_ONBORAD_BTN) == LOW) {
    // 如果按钮A被按下，执行I2C扫描
    scanAddresses();
    delay(500); // 简单的防抖动延迟
  }
  }
void cmdParse() {
  if (Serial.available()) {
    digitalWrite(PICO_ONBORAD_LED, HIGH);

    String command = Serial.readStringUntil('\n');
    command.trim();

    if (command.length() == 0) {
      Serial.println("");
    } else if (command.equals("help")) {
      showHelp(connectedI2CAddress != -1);
    } else if (command.equals("exit")) {
      connectedI2CAddress = -1;
      Serial.println("Disconnected from I2C device.");
    } else if (connectedI2CAddress == -1) {
      if (command.equals("scana")) {
        scanAddresses();
      } else if (command.startsWith("scanr")) {
        scanRegisters(command);
      } else if (command.startsWith("c ")) {
        connectI2C(command);
      } else {
        Serial.println("Error: Invalid command");
      }
    } else {
      if (command.startsWith("c ")) {
        connectI2C(command);
      } else if (command.startsWith("r ")) {
        readRegister(command);
      } else if (command.startsWith("w ")) {
        writeRegister(command);
      } else if (command.startsWith("m ")) {
        monitorRegister(command);
      } else {
        Serial.println("Error: Invalid command or not connected to I2C device");
      }
    }

    // print >
    if (connectedI2CAddress != -1) {
      Serial.print("0x");
      Serial.print(connectedI2CAddress, HEX);
      Serial.print("> ");
    } else {
      Serial.print("> ");
    }
  }
  digitalWrite(PICO_ONBORAD_LED, LOW);
}
void showHelp(bool isConnected) {
  if (isConnected) {
    Serial.println("");
    Serial.println("Available commands when connected to an I2C device:");
    Serial.println("  r [register] - Read from register [register] on connected I2C device");
    Serial.println("  w [register] [data] - Write [data] to register [register] on connected I2C device");
    Serial.println("  m [register] [interval] - Monitor a register at a regular interval (default 500ms, max 3000ms)");
    Serial.println("  exit - Disconnect from current I2C device");
  } else {
    Serial.println("");
    Serial.println("Available commands when no I2C device is connected:");
    Serial.println("  scana - Scan for all I2C devices");
    Serial.println("  scanr [address] - Scan registers at [address]");
    Serial.println("  c [address] - Connect to I2C device at [address]");
  }
}



void scanAddresses() {
  Serial.println("Scanning for I2C devices...");
  Serial.println("    0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f");
  byte error, address;

  foundCount = 0; // 重置找到的设备数量

  for (int row = 0; row < 8; ++row) {
    if (row == 0) {
      Serial.print("00: ");
    } else {
      Serial.print(row, HEX);
      Serial.print("0: ");
    }

    for (int col = 0; col < 16; ++col) {
      digitalWrite(PICO_ONBORAD_LED, HIGH);

      address = row * 16 + col;

      if (address < 3 || address > 0x77) {
        Serial.print("-- ");
        continue;
      }

      Wire1.beginTransmission(address);
      error = Wire1.endTransmission();

      if (error == 0) {
        if (address < 16) {
          Serial.print("0");
        }
        Serial.print(address, HEX);
        foundAddresses[foundCount++] = address; // 保存找到的地址
      } else {
        Serial.print("--");
      }
      Serial.print(" ");
    }
    digitalWrite(PICO_ONBORAD_LED, LOW);

    Serial.println();
  }
  gfxShowi2c();
}

void connectI2C(String command) {
  int address = strtol(&command.c_str()[2], NULL, 16);
  Wire1.beginTransmission(address);
  if (Wire1.endTransmission() == 0) {
    connectedI2CAddress = address;
    Serial.print("Connected to I2C device at 0x");
    Serial.println(address, HEX);
  } else {
    Serial.println("Error: Could not connect to I2C device");
  }
}

void readRegister(String command) {
  int reg = strtol(&command.c_str()[2], NULL, 16);
  Wire1.beginTransmission(connectedI2CAddress);
  Wire1.write(reg);
  Wire1.endTransmission();

  Wire1.requestFrom(connectedI2CAddress, 1);
  if (Wire1.available()) {
    byte data = Wire1.read();
    Serial.print("Data at 0x");
    Serial.print(reg, HEX);
    Serial.print(": 0x");
    Serial.println(data, HEX);
  } else {
    Serial.println("Error: No data received");
  }
}

void writeRegister(String command) {
  int spaceIndex = command.indexOf(' ');
  int reg = strtol(&command.c_str()[spaceIndex + 1], NULL, 16);
  int nextSpaceIndex = command.indexOf(' ', spaceIndex + 1);
  int data = strtol(&command.c_str()[nextSpaceIndex + 1], NULL, 16);

  Wire1.beginTransmission(connectedI2CAddress);
  Wire1.write(reg);  // 寄存器地址
  Wire1.write(data); // 寫數據
  byte error = Wire1.endTransmission();

  if (error == 0) {
    Serial.print("Data 0x");
    Serial.print(data, HEX);
    Serial.print(" written to register 0x");
    Serial.println(reg, HEX);
  } else {
    Serial.println("Error: Write operation failed");
  }
}


void scanRegisters(String command) {
  int address = strtol(&command.c_str()[5], NULL, 16);

  Serial.print("Scanning registers for device at 0x");
  Serial.println(address, HEX);

  Wire1.beginTransmission(address);
  if (Wire1.endTransmission() != 0) {
    Serial.println("Error: Could not connect to I2C device");
    return;
  }


  for (unsigned int reg = 0; reg <= 0xFF; reg++) {
    Wire1.beginTransmission(address);
    Wire1.write(reg);
    if (Wire1.endTransmission() != 0) {
      continue;
    }

    Wire1.requestFrom(address, 1);
    if (Wire1.available()) {
      digitalWrite(PICO_ONBORAD_LED, HIGH);

      byte data = Wire1.read();
      Serial.print("0x");
      Serial.print(reg, HEX);
      Serial.print(": 0x");
      Serial.println(data, HEX);
      digitalWrite(PICO_ONBORAD_LED, LOW);

    } else {

      break;
    }
  }
}


void monitorRegister(String command) {
  if (connectedI2CAddress == -1) {
    Serial.println("Error: No I2C device connected");
    return;
  }

  int index = command.indexOf(' ');
  int reg = strtol(&command.c_str()[index + 1], NULL, 16);
  index = command.indexOf(' ', index + 1);
  long interval = (index == -1) ? 500 : strtol(&command.c_str()[index + 1], NULL, 10);
  interval = constrain(interval, 1, 3000); //max 3000ms

  Serial.print("Monitoring register 0x");
  Serial.print(reg, HEX);
  Serial.print(" on device 0x");
  Serial.print(connectedI2CAddress, HEX);
  Serial.print(" every ");
  Serial.print(interval);
  Serial.println(" ms. Enter any character to stop.");

  while (!Serial.available()) {
    Wire1.beginTransmission(connectedI2CAddress);
    Wire1.write(reg);
    Wire1.endTransmission();
    Wire1.requestFrom(connectedI2CAddress, 1);
    if (Wire1.available()) {
      byte data = Wire1.read();
      //Serial.print("0x");
      //Serial.print(reg, HEX);
      //Serial.print(": 0x");
      Serial.println(data, HEX);
    }

    delay(interval);

    if (Serial.available()) {

      break;
    }
  }

  while (Serial.available()) {
    Serial.read();
  }
}
