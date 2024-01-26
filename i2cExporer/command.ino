
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
      } else if (command.startsWith("s")) {
        swapI2CPins();
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
