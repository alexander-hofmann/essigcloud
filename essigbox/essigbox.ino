/***********************************/
/* essigbox code                   */
/* V0.0.1                          */
/* (c) 2016 hofmann engineering    */
/*                                 */
/***********************************/  
#include <DHT.h>      //include library for DHT22 temp/humd sensor  
#include "U8glib.h"   //include library for 128x64 ST7920 graphical display
#define DEBUG 1       //set debug mode
#define DHTPIN 2      //DHT22 data is connected to pin 2
#define DHTTYPE DHT22 //set type to DHT22 as the library supports 3 different sensors

// Connect pin 1 (on the left) of the sensor to +5V
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 4 (on the right) of the sensor to GROUND
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor
DHT dht(DHTPIN, DHTTYPE); //create instance of the sensor object

float tm;       //variable for the sensor value in °C
String tm_str;  //variable for the string, representing the temp

//define display connection
U8GLIB_ST7920_128X64 u8g(14, 13, 12, U8G_PIN_NONE); // display object, whereas SPI Com: SCK = en = 14, MOSI = rw = 13, CS = di = 12

/***********************************/
/* function draw                   */
/*   draws the temp on the display */
/* parameter none                  */
/* return none                     */
/***********************************/
void draw() {
    u8g.setFont(u8g_font_9x15);
    u8g.drawStr(10, 35, tm_str.c_str());
}
/***********************************/
/* function setup                  */
/*   initializes everything        */
/* parameter none                  */
/* return none                     */
/***********************************/
void setup() {
  #ifdef DEBUG
    Serial.begin(115200); //initialize RxD TxD with 115200 baud for debugging
  #endif
  tm = 0.0;   //set temp variable to 0.0 
  dht.begin(); //start DHT22 sensor to work
}
/***********************************/
/* function loop                   */
/*   main loop makes               */
/*      (1) read temp from DHT22   */
/*      (2) update display         */
/* parameter none                  */
/* return none                     */
/***********************************/
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
    u8g.firstPage();            //that's how display is updated in the u8g lib
    do {
      draw();
    } while ( u8g.nextPage() );
  }
  delay(2000);                  //every 2 seconds is enough
}
