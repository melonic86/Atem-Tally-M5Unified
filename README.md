**Note:** I use this code for M5StickCPlus2 tally directly connected to a ATEM mini, it should work an all M5 stick and all ATEM.

**Installation**

- Install Arduino IDE
- Install M5Stack Board by following this tutorial: https://docs.m5stack.com/en/arduino/arduino_board
- Add to the library: MfUnified, Wifi
- Add manuelly in the library folder of Arduino: SkaarhojPgmspace, ATEMbase and ATEMstd
- Open custom_tally_loswald_rotation.ino and copy that code in your Arduino.
- Change the wifi name, password and Ip of your ATEM
- Select your board in Arduino IDE
- Compile and upload your code

**How to use de M5:**

- Button A (short press)
  Change camera number of the tally module
- Buttin B (short press)
  Auto change orientation with the gyro
- Button B (Long press)
  Force to change the Atem programm to the camera

