/*
 * https://github.com/earlephilhower/arduino-pico/releases/download/global/package_rp2040_index.json
 */
#include <Wire.h>
#include <U8g2lib.h>
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R2, /* reset=*/ U8X8_PIN_NONE);

#define imgWidth 128
#define imgHeight 64 


int connectedI2CAddress = -1;
#define PICO_ONBORAD_LED 25

#define MAX_I2C_DEVICES 128
byte foundAddresses[MAX_I2C_DEVICES];
int foundCount = 0;


void setup() {
  Serial.begin(230400);

  Wire.setSDA(0);
  Wire.setSCL(1);
  Wire.begin();

  pinMode(PICO_ONBORAD_LED, OUTPUT);

  while (!Serial) continue;
  digitalWrite(PICO_ONBORAD_LED, HIGH);

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

  
  u8g2.begin();  
  u8g2.setFont(u8g2_font_HelvetiPixelOutline_te); //設定字型

}
int shfitX=0;
void loop() {
    displayI2CAddresses();

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

void displayI2CAddresses() {
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_HelvetiPixelOutline_te); // 设置字体
    u8g2.drawStr(shfitX, 12, "I2CExporer");  // 滚动文字
    u8g2.setFont(u8g2_font_ncenB08_tr); // 设置字体

    shfitX++;
    if(shfitX > 128) shfitX = -100;

    // 显示I2C地址
    int line = 2; // 行号，从第二行开始显示地址
    int col = 0;  // 列号
    for (int i = 0; i < foundCount; i++) {
      char buf[6];
      sprintf(buf, "0x%02X", foundAddresses[i]);
      u8g2.drawStr(30 * col + 2, 12 * line + 10, buf); // 显示I2C地址

      col++;
      if (col > 5) { // 控制每行显示的地址数量
        col = 0;
        line++;
      }
    }
  } while (u8g2.nextPage());
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

      Wire.beginTransmission(address);
      error = Wire.endTransmission();

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
}

void connectI2C(String command) {
  int address = strtol(&command.c_str()[2], NULL, 16);
  Wire.beginTransmission(address);
  if (Wire.endTransmission() == 0) {
    connectedI2CAddress = address;
    Serial.print("Connected to I2C device at 0x");
    Serial.println(address, HEX);
  } else {
    Serial.println("Error: Could not connect to I2C device");
  }
}

void readRegister(String command) {
  int reg = strtol(&command.c_str()[2], NULL, 16);
  Wire.beginTransmission(connectedI2CAddress);
  Wire.write(reg);
  Wire.endTransmission();

  Wire.requestFrom(connectedI2CAddress, 1);
  if (Wire.available()) {
    byte data = Wire.read();
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

  Wire.beginTransmission(connectedI2CAddress);
  Wire.write(reg);  // 寄存器地址
  Wire.write(data); // 寫數據
  byte error = Wire.endTransmission();

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

  Wire.beginTransmission(address);
  if (Wire.endTransmission() != 0) {
    Serial.println("Error: Could not connect to I2C device");
    return;
  }


  for (unsigned int reg = 0; reg <= 0xFF; reg++) {
    Wire.beginTransmission(address);
    Wire.write(reg);
    if (Wire.endTransmission() != 0) {
      continue;
    }

    Wire.requestFrom(address, 1);
    if (Wire.available()) {
      digitalWrite(PICO_ONBORAD_LED, HIGH);

      byte data = Wire.read();
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
    Wire.beginTransmission(connectedI2CAddress);
    Wire.write(reg);
    Wire.endTransmission();
    Wire.requestFrom(connectedI2CAddress, 1);
    if (Wire.available()) {
      byte data = Wire.read();
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
