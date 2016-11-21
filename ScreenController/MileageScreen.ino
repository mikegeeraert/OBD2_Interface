#include <OneWire.h>
#include <DallasTemperature.h>

//for temp sensor
#define THERMOMETER_BUS_PIN 50
#define DS18B20_ID 0x28

//MileageScreen function prototypes
void scrollMileageGauge_minor(void);
void doneScrollMileageGauge(void);
void updateMileageGauge_minor(void);
void updateMileageGauge_major(void);
void updateTempGauge(void);
void writeTempSensor(void);
void readTempSensor(void);

//Mileagescreen Tasks
Task _updateMileageGauge_major(1000,TASK_FOREVER, &updateMileageGauge_major);
Task _scrollMileageGauge_minor(12,151,&scrollMileageGauge_minor);
Task _updateMileageGauge_minor(10,TASK_FOREVER, &updateMileageGauge_minor);
Task _updateTempGauge(3000, TASK_FOREVER, &updateTempGauge);
Task _writeTempSensor(5000, TASK_FOREVER, &writeTempSensor);
Task _readTempSensor(5000, TASK_ONCE, &readTempSensor);

//Gauges on Mileagescreen
Adafruit_GFX_PartyHornGauge mileageGauge;
Adafruit_GFX_RectTextGauge  tempGauge;

// Setup a oneWire instance to communicate with any OneWire devices
OneWire ds(THERMOMETER_BUS_PIN);


//Global Variables
float temp; //calculated temp value
byte addr[8]; //address of temp sensor

void setupMileageScreen(void){
  //Set Screen black, horizontal rotation and no text wrap
  tft.fillScreen(ILI9341_BLACK);
  tft.setRotation(1);
  tft.setTextWrap(false);
  
  //Draw Mileage Gauge
  char  upper[] = "AVG";
  char  lower[] = "L/100km";
  mileageGauge.initGauge(&tft, 60+10,tft.height()-60-10,60,tft.width()-60-10-10,ILI9341_WHITE, ILI9341_RED, ILI9341_BLACK, ILI9341_WHITE, upper, lower);
  mileageGauge.drawGauge();
  tft.setFont(&FreeSans9pt7b);
  mileageGauge.drawUpperLabel_major(12);
  mileageGauge.drawLowerLabel_major(12);
  //tft.drawCircle(mileage.x0,mileage.y0,mileage.r,ILI9341_WHITE);
  
  //Draw Temp Gauge
  char temp_upper[] = "Outside";
  char temp_lower[] = "Temp(°C)";
  char temp_label[] = "00";
  tempGauge.initGauge(&tft, tft.width()-10-100, 10, 100, 100, mileageGauge.getwidth(),ILI9341_WHITE, ILI9341_BLACK, ILI9341_WHITE, temp_upper, temp_lower);
  tempGauge.drawGauge();
  tempGauge.drawUpperLabel(5);
  tempGauge.drawLowerLabel(7);
  
  //Add and Enable Update Tasks
   scheduler.addTask(_scrollMileageGauge_minor);
    Serial.println("added scrollMileageGauge_minor");
  _scrollMileageGauge_minor.setOnDisable(&doneScrollMileageGauge);
  _scrollMileageGauge_minor.enable();
  
  
  //tft.setFont(&FreeSansBold24pt7b);
  //tempGauge.updateGauge(temp_label);
}

void scrollMileageGauge_minor(void){
  static double fill=0.00;
  static char label[]="";
  static bool increase = true;
    if(fill>=1.00)  increase=false;
  if(fill<=0.00)   increase=true;
  
  if(increase) fill+=0.02;
  else fill-= 0.01;
  mileageGauge.updateGauge_minor(fill, label);
}
void doneScrollMileageGauge(void){
  
   scheduler.addTask(_updateMileageGauge_major);
    Serial.println("added updateMileageGauge_major");
  scheduler.addTask(_updateMileageGauge_minor);
    Serial.println("added updateMileageGauge_minor");
  scheduler.addTask(_updateTempGauge);
    Serial.println("added updateTempGauge");
  scheduler.addTask(_writeTempSensor);
    Serial.println("added writeTempSensor");
    
  _updateMileageGauge_major.enable();
  _updateMileageGauge_minor.enable();
  _updateTempGauge.enable();
  _writeTempSensor.enable();
}
void updateMileageGauge_minor(void){
  static double fill=0.00;
  static char label[] = "00.0";
  static bool increase = true;
  
  if(fill>=1.00)  increase=false;
  if(fill<=0.00)   increase=true;
  
  if(increase) fill+=0.01;
  else fill-= 0.01;
  
  tft.setFont(&FreeSans9pt7b);
  mileageGauge.updateGauge_minor(fill, label);
  

  
}
void updateMileageGauge_major(void){
  static char label[] = "00.0";
  static char label2[] = "0.0";
  static char label3[] = "0";
  static int count=0;
  
  count = count%3;
  tft.setFont(&FreeSansBold24pt7b);
  if(count==0)
    mileageGauge.updateGauge_major(label);
  else if(count==1)
    mileageGauge.updateGauge_major(label2);
  else if(count==2)
    mileageGauge.updateGauge_major(label3);
    
  count++;
}
void updateTempGauge(void){
  static char temp_label[] = "00";
  tft.setFont(&FreeSansBold24pt7b);
  tempGauge.updateGauge(temp_label);
}

enum TempState{OUTSIDE, COOLANT};
void touchISR(void) {
  static enum TempState gauge=OUTSIDE;
  static char outside[] = "Outside";
  static char coolant[] = "Coolant";
  switch (gauge) {
    case OUTSIDE:
      tft.setFont(&FreeSans9pt7b);
      tempGauge.changeUpperLabel(coolant);
      tempGauge.drawUpperLabel(5);
      gauge=COOLANT;
      break;
    case COOLANT:
      tft.setFont(&FreeSans9pt7b);
      tempGauge.changeUpperLabel(outside);
      tempGauge.drawUpperLabel(5);      
      gauge=OUTSIDE;
      break;
    default: 
      // if nothing else matches, do the default
      // default is optional
    break;
  }
  
}

void writeTempSensor(void){
  sensors.requestTemperatures(); // Send the command to get temperatures
   
}

void readTempSensor(void){
  temp = sensors.getTempCByIndex(0);
}


