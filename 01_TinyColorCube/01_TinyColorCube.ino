/**
 * TinyColorCube
 * 
 * created by techniccontroller 12.12.2020
 * 
 * components:
 * - Attiny85
 * - Neopixelstrip
 * - GY-521
 */
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#include <TinyWireM.h>                  // I2C Master lib for ATTinys which use USI
#include <math.h>

// Which pin on the Attiny85 is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1
#define NEOPIXELPIN            4

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS      6

// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
// Note that for older NeoPixel strips you might need to change the third parameter--see the strandtest
// example for more information on possible values.
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, NEOPIXELPIN, NEO_GRB + NEO_KHZ800);

const int MPU_ADDR = 0x68; // I2C address of the MPU-6050. If AD0 pin is set to HIGH, the I2C address will be 0x69.

int16_t accelerometer_x, accelerometer_y, accelerometer_z; // variables for accelerometer raw data
int16_t gyro_x, gyro_y, gyro_z; // variables for gyro raw data
int16_t temperature; // variables for temperature data


// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return pixels.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return pixels.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return pixels.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

void setup() {

  pixels.begin(); // This initializes the NeoPixel library.
  pixels.setBrightness(100);
  
  TinyWireM.begin();
  TinyWireM.beginTransmission(MPU_ADDR); // Begins a transmission to the I2C slave (GY-521 board)
  TinyWireM.send(0x6B); // PWR_MGMT_1 register
  TinyWireM.send(0); // set to zero (wakes up the MPU-6050)
  TinyWireM.endTransmission(true);
}

void loop() {

  TinyWireM.beginTransmission(MPU_ADDR);
  TinyWireM.send(0x3B); // starting with register 0x3B (ACCEL_XOUT_H) [MPU-6000 and MPU-6050 Register Map and Descriptions Revision 4.2, p.40]
  TinyWireM.endTransmission(false); // the parameter indicates that the Arduino will send a restart. As a result, the connection is kept active.
  TinyWireM.requestFrom(MPU_ADDR, 7*2); // request a total of 7*2=14 registers
  
  // "Wire.read()<<8 | Wire.read();" means two registers are read and stored in the same variable
  accelerometer_x = TinyWireM.receive()<<8 | TinyWireM.receive(); // reading registers: 0x3B (ACCEL_XOUT_H) and 0x3C (ACCEL_XOUT_L)
  accelerometer_y = TinyWireM.receive()<<8 | TinyWireM.receive(); // reading registers: 0x3D (ACCEL_YOUT_H) and 0x3E (ACCEL_YOUT_L)
  accelerometer_z = TinyWireM.receive()<<8 | TinyWireM.receive(); // reading registers: 0x3F (ACCEL_ZOUT_H) and 0x40 (ACCEL_ZOUT_L)
  temperature = TinyWireM.receive()<<8 | TinyWireM.receive(); // reading registers: 0x41 (TEMP_OUT_H) and 0x42 (TEMP_OUT_L)
  gyro_x = TinyWireM.receive()<<8 | TinyWireM.receive(); // reading registers: 0x43 (GYRO_XOUT_H) and 0x44 (GYRO_XOUT_L)
  gyro_y = TinyWireM.receive()<<8 | TinyWireM.receive(); // reading registers: 0x45 (GYRO_YOUT_H) and 0x46 (GYRO_YOUT_L)
  gyro_z = TinyWireM.receive()<<8 | TinyWireM.receive(); // reading registers: 0x47 (GYRO_ZOUT_H) and 0x48 (GYRO_ZOUT_L)

  double angle = atan(accelerometer_y*1.0/accelerometer_z) / M_PI * 180 + 90;
  
  //uint32_t color = Wheel(abs(accelerometer_z) / 90);
  uint32_t color = Wheel((int) (abs(angle*1.4)));

  for(int i = 0; i < NUMPIXELS; i++){
    pixels.setPixelColor(i, color); // Moderately bright green color.
  }
  pixels.show(); // This sends the updated pixel color to the hardware.
  
  /*for(int i=0;i<255;i++){
    pixels.setPixelColor(0, pixels.Color(0,0,i)); // Moderately bright green color.
    pixels.show(); // This sends the updated pixel color to the hardware.
    delay(10);
  }
  
  pixels.setPixelColor(0, pixels.Color(255,0,0)); // Moderately bright green color.
  pixels.show(); // This sends the updated pixel color to the hardware.*/
  delay(100);
}
