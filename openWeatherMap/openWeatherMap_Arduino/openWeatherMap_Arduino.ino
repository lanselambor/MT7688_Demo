/* 
 * File: openweathermap.ino
 * 
 * Auther: lambor
 * Date: 2016-03-09
 * 
 * 
 */

#include <Process.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "Adafruit_Thermal.h"
// #include "Adafruit_NeoPixel.h"
#include <Wire.h>
#include <SeeedGrayOLED.h>
#include <avr/pgmspace.h>
#include <Arduino.h>
#include "SoftwareSerial.h"

#define TX_PIN 6 // Arduino transmit  YELLOW WIRE  labeled RX on printer
#define RX_PIN 5 // Arduino receive   GREEN WIRE   labeled TX on printer
SoftwareSerial mySerial(RX_PIN, TX_PIN); // Declare SoftwareSerial obj first
Adafruit_Thermal printer(&mySerial);     // Pass addr to printer constructor

int BT = 4;
volatile bool BT_State = false;
enum weather{current = 0, a_week};
char *title = "Weather Report";

volatile uint32_t g_timeCnt;

#define BUF_SIZE  512
char *g_Buffer;
uint8_t g_BufferSize;

// #define WS2812_PIN     5
// #define NUMPIXELS      4

// Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, WS2812_PIN, NEO_GRB + NEO_KHZ800);

/**
 * @Fcuntion - getWeather
 * brief - call yun-bridge process to cat weather forcast files,
 * the report including a week forcast and the current one.
 */
void getWeather(int period)
{
  String data = "";
  Process p;
  int index = 0;
  
  g_Buffer = (char *)malloc(BUF_SIZE);
  memset(g_Buffer, '\0', BUF_SIZE);
  
  p.begin("cat");
  if(period == current){
    p.addParameter("/tmp/current_weather");
  } else if (period == a_week) {
    p.addParameter("/tmp/a_week_weather");
  }
  p.run();  

  while (p.available() > 0) {
    char c = p.read();
    g_Buffer[index++] += c;
  }
  g_Buffer[index++] += '\0';  
  g_BufferSize = index;
  
  Serial.flush();
}

// Printer server
void printerServe(char *title, String data){
  printer.setSize('L');        // Set type size, accepts 'S', 'M', 'L'  
  printer.justify('C');
  printer.println(title);
  printer.setDefault(); // Restore printer to defaults
  
  printer.print(data);
  printer.feed(4);  
}


// RGB LED Strip
// void ledStrip(int led_index, uint32_t color)
// {
  // pixels.setPixelColor(led_index, color); // Moderately bright green color.
  // pixels.show();
// }

void oled96x96Display()
{
  int index;
  char *p = NULL;
  char buffer[128];
  char city[16], date[5], temp[8], humid[4], weather[16];
  
  g_timeCnt = millis();
  
  getWeather(current);
  Serial.println(g_Buffer);
  
  Serial.print("g_BufferSize: "); 
  Serial.println(g_BufferSize);
      
  // Get City 
  if(NULL != (p = strstr(g_Buffer, "City:")))
  {
    p += 6;      
    while(*p != '\n')
    {        
      city[index++] = *p;
      p++;
    }
    city[index] = '\0';
    Serial.print("city: ");
    Serial.println(city);
  }
  
  // Get Date
  if(NULL != (p = strstr(g_Buffer, "Date:")))
  {
    index = 0, p += 5;       
    while(*p != ';')
    {      
      date[index++] = *p;
      p++;
    } 
    date[index] = '\0';
    Serial.print("date: ");
    Serial.println(date);
  }
  
  if(NULL != (p = strstr(g_Buffer, "temp:")))
  {  
    // Get temp
    index = 0, p+=5;
    while(*p != ';')
    {      
      temp[index++] = *p;
      p++;
    } 
    temp[index] = '\0';
    Serial.print("temp: ");
    Serial.println(temp);
  }
  
  // Get humidity
  if(NULL != (p = strstr(g_Buffer, "humidity:")))
  {      
    index = 0, p+=9;
    while(*p != ';')
    {      
      humid[index++] = *p;
      p++;
    } 
    humid[index] = '\0';
    Serial.print("humid: ");
    Serial.println(humid);
  }
  
  // Get weather
  if(NULL != (p = strstr(g_Buffer, "weather:")))
  {      
    index = 0, p+=8;
    while(*p != '\n')
    {      
      weather[index++] = *p;
      p++;
    } 
    weather[index] = '\0';
    Serial.print("weather: ");
    Serial.println(weather);           
  }

  SeeedGrayOled.init();             //initialize SEEED OLED display
  SeeedGrayOled.clearDisplay();     //Clear Display.
  SeeedGrayOled.setNormalDisplay(); //Set Normal Display Mode
  SeeedGrayOled.setVerticalMode();  // Set to vertical mode for displaying text
  SeeedGrayOled.setTextXY(0,0);  //set Cursor to ith line, 0th column
  SeeedGrayOled.deactivateScroll();
  SeeedGrayOled.setGrayLevel(12); //Set Grayscale level. Any number between 0  - 15.
  
  SeeedGrayOled.putString("Now Weather");
  
  SeeedGrayOled.setTextXY(2, 0);
  SeeedGrayOled.putString(city);
  
  SeeedGrayOled.setTextXY(4, 0);
  SeeedGrayOled.putString(temp);
  
  SeeedGrayOled.setTextXY(6, 0);
  SeeedGrayOled.putString("humid: ");
  SeeedGrayOled.setTextXY(6, 7);
  SeeedGrayOled.putString(humid);
  
  SeeedGrayOled.setTextXY(8, 0);
  for(int i = 0; i < strlen(weather); i++)
  {
    if(i%12 == 0)
    {
      SeeedGrayOled.setTextXY(8+(i/12), 0);
    }
    SeeedGrayOled.putChar(weather[i]);
  } 
  
  // Free g_Buffer 
  free(g_Buffer);
}

void setup() {
  
  g_timeCnt = millis();
  
  // Initialize Printer  
  mySerial.begin(19200);  // Initialize SoftwareSerial  
  printer.begin();        // Init printer (same regardless of serial type)
  printer.setDefault(); // Restore printer to defaults  
  
  // Initialize Button 
  pinMode(BT, INPUT);
  
  // Initialize Bridge
  Bridge.begin();
  
  // Initialize led strip
  // pixels.begin();
  
  // Initialize Serial 
  Serial.begin(9600);
  Serial.println("begin...");

  // Initialize oled 
  Wire.begin();
  SeeedGrayOled.init();             //initialize SEEED OLED display
  SeeedGrayOled.clearDisplay();     //Clear Display.
  SeeedGrayOled.setNormalDisplay(); //Set Normal Display Mode
  SeeedGrayOled.setVerticalMode();  // Set to vertical mode for displaying text
  SeeedGrayOled.setTextXY(0,0);  //set Cursor to ith line, 0th column
  SeeedGrayOled.deactivateScroll();
  SeeedGrayOled.setGrayLevel(12); //Set Grayscale level. Any number between 0  - 15.
  
  
  // ledStrip(0, 0x0000ff);   
}

void loop() {
  
  // Printer print weather report
  if(digitalRead(BT) == HIGH)
  {
    delay(5);
    while(digitalRead(BT) == HIGH);    
    Serial.println("Printer working!");           
    
    getWeather(a_week);
    Serial.println(g_Buffer);
    printerServe(title, g_Buffer);
    
    // Free g_Buffer
    free(g_Buffer);
  }         
  // Display current weather to OLED  
  if(millis() - g_timeCnt > 10000)
  {
    g_timeCnt = millis();
    oled96x96Display();           
  }     
}