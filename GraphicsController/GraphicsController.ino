#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include "Fonts/FreeSansBold24pt7b.h"
#include "Fonts/FreeSans9pt7b.h"

// For the Adafruit shield, these are the default.
#define TFT_DC 9
#define TFT_CS 10
#define pi 3.14159265359


struct gauge{
	int16_t x0;
	int16_t y0;
	int16_t radius;
	int16_t width;
  };

  
// Use hardware SPI (on Uno, #13, #12, #11) and the above for CS/DC
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

//For classic Gauge
void drawGaugeLine(struct gauge g, double rad, int color){
    int x1 = (int)((g.radius-g.width+2) * cos(rad)) + g.x0;
    int y1 = (int)((g.radius-g.width+2) * sin(rad)) + g.y0;
    int x2 = (int)((g.radius-2) * cos(rad)) + g.x0;
    int y2 = (int)((g.radius-2) * sin(rad)) + g.y0;
    tft.drawLine(x1,y1,x2,y2,color);
}
void drawClassicGauge(void){
  int r = 100;
  int w = r/5;
  int x0 = tft.width()/2;
  int y0= tft.height()-10;
  tft.drawGauge(x0, y0, r, w, ILI9341_WHITE);
}
void updateClassicGauge(void){
  static int r = 100;
  static int w = r/5;
  static int x0 = tft.width()/2;
  static int y0= tft.height()-10;
  static struct gauge g={x0,y0,r,w};
  static bool increase=true;
  static double d=181;
  if(increase){
  //for(double d=181;d<359.5;d+=0.25){
   drawGaugeLine(g,d*pi/180,ILI9341_RED);
   d+=0.25;
   if(d>=359) increase=false;
  }
  else{
  //}
  //for(double d=359;d>181.5;d-=0.25){
   drawGaugeLine(g,d*pi/180,ILI9341_BLACK);
   d-=0.25;
   if(d<=181.5) increase=true;
  //}
  }
}
void drawCircleGauge(void) {
  static int r = 40;
  static int w = r/5;
  static int x0 = tft.width()-10-r;
  static int y0= tft.height()-10-r;
  tft.drawCircle(x0,y0,r,ILI9341_WHITE);
  tft.drawCircle(x0,y0,r+1,ILI9341_WHITE);
  tft.drawCircle(x0,y0,r+2,ILI9341_WHITE);
}
void updateCircleGauge(void){
  static int r = 60;
  static int w = r/5;
  static int x0 = tft.width()-10-r;
  static int y0= tft.height()-10-r;
  static int cir=r+5;
  static bool increase = true;
  static int lastx=0;
  static int lasty=0;
  static String lasttext;
   if(increase){
  //for(cir=mileageGauge.radius + 5; cir<120;cir++){
    tft.drawQ2Circle(x0, y0, cir, ILI9341_RED);
    tft.drawPixel(x0,y0-cir,ILI9341_WHITE);
    tft.drawPixel(x0-1,y0-cir,ILI9341_WHITE);
    tft.drawPixel(x0-2,y0-cir,ILI9341_WHITE);
    cir++;
    increase= cir&0x7f;
  //}
  }
  else{
  //for(cir;cir>mileageGauge.radius +5;cir--){
    tft.drawQ2Circle(x0, y0, cir, ILI9341_BLACK);
    cir--;
    if(cir==45) increase = true;
  //}
  }
  if(!cir%20){
    int16_t  x1, y1;
    uint16_t w, h;
    tft.getTextBounds((char*)cir, lastx, lasty, &x1, &y1, &w, &h);
    tft.fillRect(x1, y1, w, h, ILI9341_BLACK);
    lastx=tft.width()-10;
    lasty=y0+cir;
    tft.setCursor(lastx,lasty);
    tft.print((String)cir);
  }
}
void drawLineComboGauge(void){
  static int r = 50;
  static int w = r/10;
  static int x0 = r +10;
  static int linegaugestart_x=x0+r+1;
  static int y0= tft.height()-r - 10;
  static int length = tft.width()-x0-10;
  static int numticks = 5;
  static int tickspacing = length/(numticks+2);
  
  tft.fillCircle(x0, y0, r, ILI9341_WHITE);
  for(int i=0; i<w;i++) tft.drawFastHLine(x0, y0+r-i,length, ILI9341_WHITE);
  tft.fillRect(x0+1, y0+1, r, r,ILI9341_WHITE);
  tft.fillCircle(x0, y0, r-w, ILI9341_BLACK);
  tft.setFont(&FreeSans9pt7b);
  int scale=30;
  int tickcount=1;
  for(int i=1; i<length-40;i++){ 
    if(i%tickspacing==0){
      tft.fillCircle(i+linegaugestart_x+w/2,y0+(r/2)-w/2-2,w/2,ILI9341_WHITE);
      tft.setCursor(i+linegaugestart_x,y0+(r/2)-w-5);
      tft.print(scale/numticks * tickcount);
      tickcount++;
    }
  }
  tft.setFont(&FreeSansBold24pt7b);
  
  
}
void UpdateLineComboGauge(void){
  static int r = 50;
  static int w = r/10;
  static int x0 = r +10;
  static int y0= tft.height()-r - 10;
  static int length = tft.width()-10;
  static bool increase=true;
  static int start= x0+r+1;
  static int curx= start;
  static int y=y0+(r/2);
  static int linewidth=(r/2)-w;
  static int color = ILI9341_RED;
  static int16_t x1,y1;
  static uint16_t fillw, fillh;
  static char * teststring = "TE";
  static int textoffset = (int) sqrt(sq(2*(r-w))/2); //find the largest square that can fit in the circle
  tft.getTextBounds(teststring, x0, y0, &x1, &y1, &fillw, &fillh);
  static int cursorx= x0-textoffset/2+((textoffset-(fillw))/2);
  static int cursory= y0+textoffset/2-((textoffset-(fillh))/2);
  static bool write=true;
  if(write){
  tft.setCursor(cursorx,cursory);
  tft.print(teststring);
  write=false;
  }
  
  if(increase){
    if(curx>=start+w) tft.drawFastVLine(curx-w,y,linewidth,ILI9341_RED);
    tft.drawFastVLine(curx++,y,linewidth,ILI9341_WHITE);
    if(curx>=length) increase=false;
  }
  else{
    if(curx>=start+w)tft.drawFastVLine(curx-w,y,linewidth,ILI9341_WHITE);
    tft.drawFastVLine(curx--,y,linewidth,ILI9341_BLACK);
    if(curx<=start) increase=true;
  }
  String str;
  char * cstr;
  if(curx%10==0){
  tft.getTextBounds(teststring, cursorx, cursory, &x1, &y1, &fillw, &fillh);
  tft.fillRect(x1, y1, fillw, fillh,ILI9341_BLACK);
  tft.setCursor(cursorx,cursory);
  str = String(curx);
  str.toCharArray(cstr,16);
  tft.print("TE");
  }
  
}
// If using the breakout, change pins as desired
//Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);
void setup(void){
    Serial.begin(9600);
  Serial.println("ILI9341 Test!"); 
 
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
  
  tft.fillScreen(ILI9341_BLACK);
  tft.setRotation(1);
  tft.setFont(&FreeSansBold24pt7b);
  tft.setTextColor(ILI9341_WHITE);
  
  //drawClassicGauge();
  //drawCircleGauge();
  drawLineComboGauge();
}

void loop(void){

  //updateClassicGauge();
  //updateCircleGauge();
  UpdateLineComboGauge();
   
}
