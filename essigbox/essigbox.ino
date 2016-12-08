/************************************************************
 essigbox code                   
 V0.0.1                          
 (c) 2016 hofmann engineering    
                                 
 Each essigbox will be connected to an essigcloud access 
 point through wifi. So this code will run on an Adafruit 
 HUZZAH ESP8266 Board. To program the board do:
   (1) Hold down the GPIO0 button, the red LED will be lit
   (2) While holding down GPIO0, click the RESET button
   (3) Release RESET, then release GPIO0
   (4) When you release the RESET button, the red LED will 
      be lit dimly, this means its ready to bootload
 Upload Speed is 115200 baud.
*************************************************************/  
#include <DHT.h>      //include library for DHT22 temp/humd sensor  
#include "U8glib.h"   //include library for the display

#define DEBUG 1          //set debug mode
#define DHTPIN 12        //DHT22 data is connected to pin 2
#define DHTTYPE DHT22    //set type to DHT22 as the library supports 3 different sensors
#define DISPLAY_RS  2    //RS pin of the SPI ST7920 display
#define DISPLAY_RW  3    //RW pin of the SPI ST7920 display
#define DISPLAY_E   4    //E pin of the SPI ST7920 display
#define DISPLAY_LIGHT 13 //backlight of the display

// Connect pin 1 (on the left) of the sensor to +5V
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 4 (on the right) of the sensor to GROUND
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor
DHT dht(DHTPIN, DHTTYPE); //create instance of the sensor object
float tm;       //variable for the sensor value in °C
String tm_string;
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
  u8g.setFont(u8g_font_6x12);
  tm_string = String(tm);
  u8g.drawStr(0, 40, "Temp: ");
  u8g.drawStr(60, 40, tm_string.c_str());
  u8g.drawStr(110, 40, unit);
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
  dht.begin(); //start DHT22 sensor to work
  //display init
  pinMode(DISPLAY_LIGHT, OUTPUT); 
  digitalWrite(DISPLAY_LIGHT, HIGH);
  
  if ( u8g.getMode() == U8G_MODE_R3G3B2 )
    u8g.setColorIndex(255); // white
  else if ( u8g.getMode() == U8G_MODE_GRAY2BIT )
    u8g.setColorIndex(3); // max intensity
  else if ( u8g.getMode() == U8G_MODE_BW )
    u8g.setColorIndex(1); // pixel on  
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
  u8g.firstPage();
  do {
    draw();
  } while ( u8g.nextPage() );
  delay(2000);                  //every 2 seconds is enough
}
