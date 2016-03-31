/*
 * File: wifi_printer.ino
 *
 * Auther: lambor
 * Date: 2016-03-27
 *
 *
 */

#include <Process.h>
#include "Adafruit_Thermal.h"
#include <Wire.h>
#include "SoftwareSerial.h"

#define TX_PIN 6 // Arduino transmit  YELLOW WIRE  labeled RX on printer
#define RX_PIN 5 // Arduino receive   GREEN WIRE   labeled TX on printer
SoftwareSerial mySerial(RX_PIN, TX_PIN); // Declare SoftwareSerial obj first
Adafruit_Thermal printer(&mySerial);     // Pass addr to printer constructor

#define BUF_SIZE  128
char *g_Buffer = NULL;
uint16_t g_BufferSize;

/**
 * @Fcuntion - printer
 * brief - call yun-bridge process to cat files from mt7688duo.
 */
void bridgeServe()
{  
  String data = "";
  Process p;
  int index = 0;

  Serial.println("bridgeServe");
  g_BufferSize = 0;
  g_Buffer = (char *)malloc(BUF_SIZE);
  memset(g_Buffer, '\0', BUF_SIZE);
  
  p.begin("cat");
  p.addParameter("/root/wifi_printer/files/printer_status.txt");
  p.run();
  while (p.available() > 0) {
    char c = p.read();
    g_Buffer[index++] += c;
  }
  g_Buffer[index++] += '\0';
  g_BufferSize = index;
  
  if(g_BufferSize > 0) {
    Serial.print("status: ");
    Serial.println(g_Buffer);
  }
  
  if(strstr("1", g_Buffer)) {
      index = 0;
      g_BufferSize = 0;      
      
      memset(g_Buffer, '\0', BUF_SIZE);
      
      // Clear status
      p.begin("/bin/ash");
      p.addParameter("/root/wifi_printer/script/clear_printer_status.sh");
      p.run();     
      delay(200);
      
      p.begin("cat");
      p.addParameter("/root/wifi_printer/files/message.txt");
      p.run();
      while (p.available() > 0) {
        char c = p.read();
        g_Buffer[index++] += c;
      }
      g_Buffer[index++] += '\0';
      g_BufferSize = index;
            
      if(g_BufferSize > 0) {   
        Serial.print("Message: ");
        Serial.println(g_Buffer); 
        printerServe("Message", g_Buffer);
      }
  }
  
  g_BufferSize = 0;
  free(g_Buffer);
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

void setup(){
  // Initialize Bridge00000
  Bridge.begin();
  // Initialize Printer
  mySerial.begin(19200);  // Initialize SoftwareSerial
  printer.begin();        // Init printer (same regardless of serial type)
  printer.setDefault(); // Restore printer to defaults
  // Initialize Serial
  Serial.begin(9600);
  //while (!Serial); 
  Serial.println("begin...");

  //printerServe("test", "hello");    
  }

void loop(){
    bridgeServe();    
    delay(1000);
}
