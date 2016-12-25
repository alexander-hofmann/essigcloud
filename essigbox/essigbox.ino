/************************************************************
 essigbox code                   
 V0.2.0                          
 (c) 2016 hofmann engineering    

This code runs on Arduino NANO.
                                 
*************************************************************/  
#include <DHT.h>      //include library for DHT22 temp/humd sensor  
#include "U8glib.h"   //include library for the display
#include <SoftwareSerial.h>  //include library for SoftwareSerial communication to mqtt via rxtx2mqtt project

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
#define DEBUG_RX  10            //Software Serial for MQTT communication via rxtx2mqtt
#define DEBUG_TX  11            //Software Serial for MQTT communication via rxtx2mqtt
#define READY_MESSAGE "READY\n"     //Welcome message when rxtx2mqtt module is ready - can be parsed at other side of the rxtx connection

//Software Serial variable declaration
//connect pin rx to pin TX of ESP8266 module, programmed with rxtx2mqtt project
//connect pin tx to pin RX of ESP8266 module, programmed with rxtx2mqtt project
//connect the computer to DEBUG_TX and DEBUG_RX for Debugging messages
SoftwareSerial debugSerial(DEBUG_TX, DEBUG_RX); // RX, TX
float targetTemp = 0.0;

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
  tm_string = String(tm);   
  u8g.setFont(u8g_font_6x12);
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
 function processMQTTSerial                   
   data available from rxtx2mqtt module 
   meaning that MQTT Server sends data.
 parameter none                  
 return none                     
************************************/
void processMQTTSerial(void) {
  String s;
  s = Serial.readString();  //read from rxtx2mqtt project module
  #ifdef DEBUG
    debugSerial.print(s);
  #endif  
  if (s.startsWith("home/essigcloud/essigbox01/config/temp")) {   //if message is a new target temp message
    int equalsign = s.lastIndexOf("=");                           //search from equal sign
    if (equalsign > -1) {                                         //equal sign not found, exit
      String t = s.substring(equalsign + 1);                      //extract substring, first parameter is inclusive, so add 1
      targetTemp = t.toFloat();                                   //convert the string to a float and store it into the target temp variable
      #ifdef DEBUG
        debugSerial.print("new target temp received from MQTT = ");
        debugSerial.println(t.c_str()); 
      #endif
    }
  }
}
/************************************
 function heatingCable                   
   manage heating cables 1,2 and 3
 parameter boolean cable1, cable2, cable3
   HIGH means turn on, LOW means off                  
 return none                     
************************************/
void heatingCable(boolean cable1, boolean cable2, boolean cable3) {
    if (cable1) {
      digitalWrite(PLUGIN_1, PLUGIN_1_ON);
      Serial.println("home/essigcloud/essigbox01/heating1=1");   
    } else {
      digitalWrite(PLUGIN_1, PLUGIN_1_OFF); 
      Serial.println("home/essigcloud/essigbox01/heating1=0");       
    }
    if (cable2) {
      digitalWrite(PLUGIN_2, PLUGIN_2_ON);
      Serial.println("home/essigcloud/essigbox01/heating2=1");
    } else {
      digitalWrite(PLUGIN_2, PLUGIN_2_OFF);
      Serial.println("home/essigcloud/essigbox01/heating2=0");      
    }
    if (cable3) {
      digitalWrite(PLUGIN_3, PLUGIN_3_ON);
      Serial.println("home/essigcloud/essigbox01/heating3=1");
    } else {
      digitalWrite(PLUGIN_3, PLUGIN_3_OFF);
      Serial.println("home/essigcloud/essigbox01/heating3=0");      
    }
}
/************************************
 function controlTemp                   
   manage heating cables 1,2 and 3
   according to the desired target
   temperature.
 parameter none                  
 return none                     
************************************/
void controlTemp() {
  float error = tm - targetTemp;
  if (error <= -1) {
    heatingCable(HIGH, HIGH, HIGH);
    return;
  }
  if ((error > -1) && (error < 1)) {
      heatingCable(HIGH, HIGH, LOW);
  }
  if ((error >= 1) && (error < 2)) {
    heatingCable(HIGH, LOW, LOW);
    return;
  }
  if (error >= 2) {
    heatingCable(LOW, LOW, LOW);
    return;
  }

}
/************************************
 function setup                  
   initializes everything        
 parameter none                  
 return none                     
************************************/
void setup() {
  #ifdef DEBUG
    debugSerial.begin(115200); //initialize RxD TxD with 115200 baud for debugging
  #endif
  //Software Serial for MQTT communication via rxtx2mqtt project
  #ifdef DEBUG
    debugSerial.println("starting SoftwareSerial for MQTT communication via rxtx2mqtt project"); 
  #endif
  Serial.begin(115200);
  #ifdef DEBUG
    debugSerial.println("Software Serial started."); 
  #endif
  bool passed = false;
  String s = "";
  while (!passed) {
    if (Serial.available() >=6 ) {
      s = Serial.readString();
      #ifdef DEBUG
        debugSerial.println(s); 
      #endif      
      if (s.endsWith(READY_MESSAGE)) {
        passed = true;
      }
    }
  }
  //delete rx stack
  while (Serial.available()) {
    char c = Serial.read();
  }
  #ifdef DEBUG
    debugSerial.println("READY received."); 
  #endif
  delay(500);
  Serial.println("home/essigcloud/essigbox01/config/#?");
  if (Serial.available()) {
    processMQTTSerial();
  }
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
  heatingCable(LOW, LOW, LOW);
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
  Serial.print("home/essigcloud/essigbox01/temp=");
  Serial.println(tm); 
  if (isnan(tm)) {              //if value couldn't be read, send error in DEBUG mode
    #ifdef DEBUG
      debugSerial.println("read temp failed!");  
    #endif  
  } else {
    #ifdef DEBUG
      debugSerial.print("temp = ");  //in DEBUG mode, send current temp 
      debugSerial.println(tm);  
    #endif 
  }
  hm = dht.readHumidity();
  Serial.print("home/essigcloud/essigbox01/humidity=");
  Serial.println(hm); 
  if (isnan(hm)) {              //if value couldn't be read, send error in DEBUG mode
    #ifdef DEBUG
      debugSerial.println("read humidity failed!");  
    #endif  
  } else {
    #ifdef DEBUG
      //debugSerial.print("hum = ");  //in DEBUG mode, send current temp 
      //debugSerial.println(hm);  
    #endif 
  }
  //process incoming messages from MQTT
  if (Serial.available()) {
    processMQTTSerial();
  }
  //control the temperature
  controlTemp();
  //render display
  u8g.firstPage();
  do {
    draw();
  } while ( u8g.nextPage() );
  delay(2000);                  //every 2 seconds is enough
}
