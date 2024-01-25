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
void loop() {
  breathLED();
  readBTN();
  rotatePoint();
  cmdParse();
}
