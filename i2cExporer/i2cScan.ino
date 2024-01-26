void swapI2CPins() {
  // 交换SDA和SCL引脚
  int temp = sdaPin;
  sdaPin = sclPin;
  sclPin = temp;

  // 重新初始化SoftwareI2C
  softwarei2c.begin(sdaPin, sclPin); // 使用新的引脚重新初始化
  Serial.println("swapI2CPins");
  gfxDisplayI2cPin(sdaPin,sclPin);
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

      error = softwarei2c.beginTransmission(address);
      if (error == 1) {
        if (address < 16) {
          Serial.print("0");
        }
        Serial.print(address, HEX);
        foundAddresses[foundCount++] = address; // 保存找到的地址
      } else {
        Serial.print("--");
      }
      softwarei2c.endTransmission();
      Serial.print(" ");
    }
    digitalWrite(PICO_ONBORAD_LED, LOW);

    Serial.println();
  }

  gfxShowi2c();
}

void connectI2C(String command) {
  int address = strtol(&command.c_str()[2], NULL, 16);
  softwarei2c.beginTransmission(address);
  if (softwarei2c.endTransmission() == 0) {
    connectedI2CAddress = address;
    Serial.print("Connected to I2C device at 0x");
    Serial.println(address, HEX);
  } else {
    Serial.println("Error: Could not connect to I2C device");
  }
}

void readRegister(String command) {
  int reg = strtol(&command.c_str()[2], NULL, 16);
  softwarei2c.beginTransmission(connectedI2CAddress);
  softwarei2c.write(reg);
  softwarei2c.endTransmission();

  softwarei2c.requestFrom(connectedI2CAddress, 1);
 
    byte data = softwarei2c.read();
    Serial.print("Data at 0x");
    Serial.print(reg, HEX);
    Serial.print(": 0x");
    Serial.println(data, HEX);

}

void writeRegister(String command) {
  int spaceIndex = command.indexOf(' ');
  int reg = strtol(&command.c_str()[spaceIndex + 1], NULL, 16);
  int nextSpaceIndex = command.indexOf(' ', spaceIndex + 1);
  int data = strtol(&command.c_str()[nextSpaceIndex + 1], NULL, 16);

  softwarei2c.beginTransmission(connectedI2CAddress);
  softwarei2c.write(reg);  // 寄存器地址
  softwarei2c.write(data); // 寫數據
  byte error = softwarei2c.endTransmission();

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

  softwarei2c.beginTransmission(address);
  if (softwarei2c.endTransmission() != 0) {
    Serial.println("Error: Could not connect to I2C device");
    return;
  }


  for (unsigned int reg = 0; reg <= 0xFF; reg++) {
    softwarei2c.beginTransmission(address);
    softwarei2c.write(reg);
    if (softwarei2c.endTransmission() != 0) {
      continue;
    }

    softwarei2c.requestFrom(address, 1);

    digitalWrite(PICO_ONBORAD_LED, HIGH);

    byte data = softwarei2c.read();
    Serial.print("0x");
    Serial.print(reg, HEX);
    Serial.print(": 0x");
    Serial.println(data, HEX);
    digitalWrite(PICO_ONBORAD_LED, LOW);

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
    softwarei2c.beginTransmission(connectedI2CAddress);
    softwarei2c.write(reg);
    softwarei2c.endTransmission();
    softwarei2c.requestFrom(connectedI2CAddress, 1);

    byte data = softwarei2c.read();
    //Serial.print("0x");
    //Serial.print(reg, HEX);
    //Serial.print(": 0x");
    Serial.println(data, HEX);


    delay(interval);

    if (Serial.available()) {

      break;
    }
  }

  while (Serial.available()) {
    Serial.read();
  }
}
 
