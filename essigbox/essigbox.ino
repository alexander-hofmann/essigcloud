/************************************************************
 essigbox code                   
 V0.2.0                          
 (c) 2016 hofmann engineering    

This code runs on Arduino NANO.
                                 
*************************************************************/  
#include <DHT.h>      //include library for DHT22 temp/humd sensor  
#include "U8glib.h"   //include library for the display

#define DEBUG         1             //set debug mode
#define DHTPIN        12            //DHT22 data is connected to pin 2
#define DHTTYPE       DHT22         //set type to DHT22 as the library supports 3 different sensors
#define DISPLAY_RS    2             //RS pin of the SPI ST7920 display
#define DISPLAY_RW    3             //RW pin of the SPI ST7920 display
#define DISPLAY_E     4             //E pin of the SPI ST7920 display
#define DISPLAY_LIGHT 13            //backlight of the display
#define PLUGIN_1      5             //relais1 of plugin1 is attached to pin5
#define PLUGIN_1_ON   LOW          //relais1 reacts on HIGH
#define PLUGIN_1_OFF  !PLUGIN_1_ON  //relais1 OFF is !ON
#define PLUGIN_2      6             //relais3 of plugin2 is attached to pin6
#define PLUGIN_2_ON   HIGH           //relais2 reacts on LOW
#define PLUGIN_2_OFF  !PLUGIN_2_ON  //relais2 OFF is !ON
#define PLUGIN_3      7             //relais3 of plugin3 is attached to pin7
#define PLUGIN_3_ON   HIGH          //relais3 reacts on HIGH
#define PLUGIN_3_OFF  !PLUGIN_3_ON  //relais3 OFF is !ON

// Connect pin 1 (on the left) of the sensor to +5V
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 4 (on the right) of the sensor to GROUND
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor
DHT dht(DHTPIN, DHTTYPE); //create instance of the sensor object
float tm;       //variable for the temperature sensor value in °C
String tm_string;
float hm;       //variable for the humidity sensor value in %
String hm_string;
char unit[3] = {(char)176, 'C', '\0'};

// Connect pin 1 (GND) to GND
// Connect pin 2 (VCC) to +5V
// Connect pin 4 (RS) to DISPLAY_RS
// Connect pin 5 (RW) to DISPLAY_RW
// Connect pin 6 (E) to DISPLAY_E
// Connect pin 15 (PSB) to GND (use SPI Mode)
// Connect pin 19 (A) to +5V
// Connect pin 20 (K) over BC537 to GND und base over 470Ohm to DISPLAY_LIGHT
U8GLIB_ST7920_128X64 u8g(DISPLAY_E, DISPLAY_RW, DISPLAY_RS, U8G_PIN_NONE);

/************************************
 function draw                   
   initializes draws the display 
   content        
 parameter none                  
 return none                     
************************************/
void draw(void) {
  //temperature
  u8g.setFont(u8g_font_6x12);
  tm_string = String(tm);
  u8g.drawStr(0, 15, "Temp: ");
  u8g.drawStr(60, 15, tm_string.c_str());
  u8g.drawStr(110, 15, unit);
  //humidity
  hm_string = String(hm);
  u8g.drawStr(0, 45, "Hum: ");
  u8g.drawStr(60, 45, hm_string.c_str());
  u8g.drawStr(110, 45, "%");
}
/************************************
 function setup                  
   initializes everything        
 parameter none                  
 return none                     
************************************/
void setup() {
  #ifdef DEBUG
    Serial.begin(115200); //initialize RxD TxD with 115200 baud for debugging
  #endif
  //DHT22 init
  tm = 0.0;   //set temp variable to 0.0 
  hm = 0.0;   //set hum variable to 0.0
  dht.begin(); //start DHT22 sensor to work
  //display init
  pinMode(DISPLAY_LIGHT, OUTPUT); 
  digitalWrite(DISPLAY_LIGHT, HIGH);
  pinMode(PLUGIN_1, OUTPUT);
  pinMode(PLUGIN_2, OUTPUT);
  pinMode(PLUGIN_3, OUTPUT);
  if ( u8g.getMode() == U8G_MODE_R3G3B2 )
    u8g.setColorIndex(255); // white
  else if ( u8g.getMode() == U8G_MODE_GRAY2BIT )
    u8g.setColorIndex(3); // max intensity
  else if ( u8g.getMode() == U8G_MODE_BW )
    u8g.setColorIndex(1); // pixel on 
  digitalWrite(PLUGIN_1, PLUGIN_1_ON);
  delay(500);
  digitalWrite(PLUGIN_2, PLUGIN_2_ON);
  delay(500);
  digitalWrite(PLUGIN_3, PLUGIN_3_ON);
  delay(500);
}
/************************************
 function loop                   
   main loop makes               
      (1) read temp from DHT22   
      (2) update display         
 parameter none                  
 return none                     
/************************************/
void loop() {
  tm = dht.readTemperature();   //read temp from DHT22
  if (isnan(tm)) {              //if value couldn't be read, send error in DEBUG mode
    #ifdef DEBUG
      Serial.println("read temp failed!");  
    #endif  
  } else {
    #ifdef DEBUG
      Serial.print("temp = ");  //in DEBUG mode, send current temp 
      Serial.println(tm);  
    #endif 
  }
  hm = dht.readHumidity();
  if (isnan(hm)) {              //if value couldn't be read, send error in DEBUG mode
    #ifdef DEBUG
      Serial.println("read humidity failed!");  
    #endif  
  } else {
    #ifdef DEBUG
      Serial.print("hum = ");  //in DEBUG mode, send current temp 
      Serial.println(hm);  
    #endif 
  }  
  
  u8g.firstPage();
  do {
    draw();
  } while ( u8g.nextPage() );
  delay(2000);                  //every 2 seconds is enough
}
