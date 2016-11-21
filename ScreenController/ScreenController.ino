

#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include "Adafruit_FT6206.h"
#include <Wire.h>      // this is needed for FT6206
#include "Fonts/FreeSansBold24pt7b.h"
#include "Fonts/FreeSans9pt7b.h"
#include "TaskScheduler.h"
#include <PinChangeInt.h>
//#include <PinChangeIntConfig.h>


// For the Adafruit shield, these are the default.
#define TFT_DC 9
#define TFT_CS 10
#define pi 3.14159265359
//For touch screen interrupt control
#define TOUCH_PIN 2
// For Brightness control
#define BACKLIGHT_PIN 5

// Create Scheduler 
Scheduler scheduler;
//Prototype method of intial task
void setupMileageScreen();
void brightnessControl(void);
Task _setuphomescreen(0,1, &setupMileageScreen);
Task _brightnesscontrol(10,TASK_FOREVER, &brightnessControl);


// Use hardware SPI (on Uno, #13, #12, #11) and the above for CS/DC
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

// The FT6206 uses hardware I2C (SCL/SDA)
Adafruit_FT6206 ctp = Adafruit_FT6206();



void setup(void){
    Serial.begin(9600);
  Serial.println("Running Information Display"); 
  
  //Test to make sure touchscreen controller starts
  if (! ctp.begin(40)) {  // pass in 'sensitivity' coefficient
    Serial.println("Couldn't start FT6206 touchscreen controller");
    while (1);
  }

  Serial.println("Capacitive touchscreen started");
 
  
  tft.begin();

  // read diagnostics (optional but can help debug problems)
  uint8_t x = tft.readcommand8(ILI9341_RDMODE);
  Serial.print("Display Power Mode: 0x"); Serial.println(x, HEX);
  x = tft.readcommand8(ILI9341_RDMADCTL);
  Serial.print("MADCTL Mode: 0x"); Serial.println(x, HEX);
  x = tft.readcommand8(ILI9341_RDPIXFMT);
  Serial.print("Pixel Format: 0x"); Serial.println(x, HEX);
  x = tft.readcommand8(ILI9341_RDIMGFMT);
  Serial.print("Image Format: 0x"); Serial.println(x, HEX);
  x = tft.readcommand8(ILI9341_RDSELFDIAG);
  Serial.print("Self Diagnostic: 0x"); Serial.println(x, HEX);
  analogWrite(BACKLIGHT_PIN , 0);
   
  //Intitialize the scheduler
  scheduler.init();
  Serial.println("Initialized scheduler"); 
  scheduler.addTask(_setuphomescreen);
  Serial.println("added setuphomescreen");
  scheduler.addTask(_brightnesscontrol);
  Serial.println("added brightnesscontrol");
  _brightnesscontrol.enable();
  _setuphomescreen.enable();
  
  
  pinMode(TOUCH_PIN, INPUT);     //set the pin to input
  digitalWrite(TOUCH_PIN, HIGH); //use the internal pullup resistor
  PCintPort::attachInterrupt(TOUCH_PIN, touchISR,RISING); // attach a PinChange Interrupt to our pin on the rising edge
  
}
void loop(void){
  //updateMileageScreen();
  //brightnessControl();
  scheduler.execute();
}

void brightnessControl(void){
  static int bright =0;
  static bool increase = true;
  if(bright>=255) increase=false;
  if(bright<=0) increase=true;
  analogWrite(BACKLIGHT_PIN , bright);
  
  if(increase)bright++;
  else bright--;

}
