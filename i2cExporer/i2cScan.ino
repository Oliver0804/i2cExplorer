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
/*
void connectI2C(String command) {
  int address = strtol(&command.c_str()[2], NULL, 16);
  scanI2C.beginTransmission(address);
  if (scanI2C.endTransmission() == 0) {
    connectedI2CAddress = address;
    Serial.print("Connected to I2C device at 0x");
    Serial.println(address, HEX);
  } else {
    Serial.println("Error: Could not connect to I2C device");
  }
}

void readRegister(String command) {
  int reg = strtol(&command.c_str()[2], NULL, 16);
  scanI2C.beginTransmission(connectedI2CAddress);
  scanI2C.write(reg);
  scanI2C.endTransmission();

  scanI2C.requestFrom(connectedI2CAddress, 1);
  if (scanI2C.available()) {
    byte data = scanI2C.read();
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

  scanI2C.beginTransmission(connectedI2CAddress);
  scanI2C.write(reg);  // 寄存器地址
  scanI2C.write(data); // 寫數據
  byte error = scanI2C.endTransmission();

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

  scanI2C.beginTransmission(address);
  if (scanI2C.endTransmission() != 0) {
    Serial.println("Error: Could not connect to I2C device");
    return;
  }


  for (unsigned int reg = 0; reg <= 0xFF; reg++) {
    scanI2C.beginTransmission(address);
    scanI2C.write(reg);
    if (scanI2C.endTransmission() != 0) {
      continue;
    }

    scanI2C.requestFrom(address, 1);
    if (scanI2C.available()) {
      digitalWrite(PICO_ONBORAD_LED, HIGH);

      byte data = scanI2C.read();
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
    scanI2C.beginTransmission(connectedI2CAddress);
    scanI2C.write(reg);
    scanI2C.endTransmission();
    scanI2C.requestFrom(connectedI2CAddress, 1);
    if (scanI2C.available()) {
      byte data = scanI2C.read();
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
  */
