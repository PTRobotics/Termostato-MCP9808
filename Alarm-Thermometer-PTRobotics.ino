/**************************************************************************/
/*!
   Simple alarm thermometer
   by: Pedro Costa Silva
   PTRobotics - http://www.ptrobotics.com
   date: 27 SET 2014
   license: This code is public domain and built from the following examples


  This is a demo for the Adafruit MCP9808 breakout
  ----> http://www.adafruit.com/products/1782
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!


  Serial 7-Segment Display Example Code
   I2C Mode Stopwatch
   by: Jim Lindblom
     SparkFun Electronics
   date: November 27, 2012
   license: This code is public domain.

   This example code shows how you could use the Arduino Wire 
   library to interface with a Serial 7-Segment Display.

   There are example functions for setting the display's
   brightness, decimals, clearing the display, and sending a 
   series of bytes via I2C.

   Each I2C transfer begins with a Wire.beginTransmission(address)
   where address is the 7-bit address of the device set to 
   receive the data. Wire.write() sends a byte of data. I2C
   communication is closed with Wire.endTransmission().

   Circuit:
   Arduino -------------- Serial 7-Segment
     5V   --------------------  VCC
     GND  --------------------  GND
     SDA  --------------------  SDA (A4 on older 'duino's)
     SCL  --------------------  SCL (A5 on older 'duino's)
*/
/**************************************************************************/

#include <Wire.h>
#include "Adafruit_MCP9808.h"
// Here we'll define the I2C address of our S7S. By default it
//  should be 0x71. This can be changed, though.
const byte s7sAddress = 0x71;
const float limit = 28.00; //Alarm limit
unsigned int temperature = 0;  // This variable will count up to 65k
char tempString[10];  // Will be used with sprintf to create strings

// Create the MCP9808 temperature sensor object
Adafruit_MCP9808 tempsensor = Adafruit_MCP9808();

void setup() {
  pinMode(2, OUTPUT);
  digitalWrite(2, LOW);
  Wire.begin();  // Initialize hardware I2C pins
  Serial.begin(9600);
  Serial.println("PTRobotics - Teste do MCP9808");
  
  // Make sure the sensor is found, you can also pass in a different i2c
  // address with tempsensor.begin(0x19) for example
  if (!tempsensor.begin()) {
    Serial.println("Couldn't find MCP9808!");
    while (1);
  }
    // Clear the display, and then turn on all segments and decimals
  clearDisplayI2C();  // Clears display, resets cursor

  // Custom function to send four bytes via I2C
  //  The I2C.write function only allows sending of a single
  //  byte at a time.
  s7sSendStringI2C("-HI-");
  setDecimalsI2C(0b1111111);  // Turn on all decimals, colon, apos

  setBrightnessI2C(255);  // High brightness
  delay(200);

  // Clear the display before jumping into loop
  clearDisplayI2C();  
}

void loop() {
  // Read and print out the temperature, then convert to *F
  float c = tempsensor.readTempC();
  float f = c * 9.0 / 5.0 + 32;
  Serial.print("Temp: "); Serial.print(c); Serial.print("*C\t"); 
  Serial.print(f); Serial.println("*F");
  dtostrf(c,4, 2, tempString); //Convert the float to string
  tempString[2]=tempString[3];//Remove the dot from the string
  tempString[3]=tempString[4];//Remove the dot from the string
  // This will output the tempString to the S7S
  s7sSendStringI2C(tempString);
 
  setDecimalsI2C(0b00000010);  // Sets digit 3 decimal on
  
  if (c>=limit){
    digitalWrite(2, HIGH);
  }else{
    digitalWrite(2, LOW);
  }
  delay(100);
}

// This custom function works somewhat like a serial.print.
//  You can send it an array of chars (string) and it'll print
//  the first 4 characters in the array.
void s7sSendStringI2C(String toSend)
{
  Wire.beginTransmission(s7sAddress);
  for (int i=0; i<4; i++)
  {
    Wire.write(toSend[i]);
  }
  Wire.endTransmission();
}

// Send the clear display command (0x76)
//  This will clear the display and reset the cursor
void clearDisplayI2C()
{
  Wire.beginTransmission(s7sAddress);
  Wire.write(0x76);  // Clear display command
  Wire.endTransmission();
}

// Set the displays brightness. Should receive byte with the value
//  to set the brightness to
//  dimmest------------->brightest
//     0--------127--------255
void setBrightnessI2C(byte value)
{
  Wire.beginTransmission(s7sAddress);
  Wire.write(0x7A);  // Set brightness command byte
  Wire.write(value);  // brightness data byte
  Wire.endTransmission();
}

// Turn on any, none, or all of the decimals.
//  The six lowest bits in the decimals parameter sets a decimal 
//  (or colon, or apostrophe) on or off. A 1 indicates on, 0 off.
//  [MSB] (X)(X)(Apos)(Colon)(Digit 4)(Digit 3)(Digit2)(Digit1)
void setDecimalsI2C(byte decimals)
{
  Wire.beginTransmission(s7sAddress);
  Wire.write(0x77);
  Wire.write(decimals);
  Wire.endTransmission();
}

