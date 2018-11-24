/*  
 *  Sigfox Code Example
 *  
 *  Explanation: This example shows how to send a Sigfox packet.
 *  In this example, the message to be sent is defined as an array 
 *  of bytes. It is necessary to indicate the length of this array.
 *  
 *  Copyright (C) 2016 Libelium Comunicaciones Distribuidas S.L. 
 *  http://www.libelium.com 
 *  
 *  This program is free software: you can redistribute it and/or modify  
 *  it under the terms of the GNU General Public License as published by  
 *  the Free Software Foundation, either version 3 of the License, or  
 *  (at your option) any later version.  
 *   
 *  This program is distributed in the hope that it will be useful,  
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of  
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the  
 *  GNU General Public License for more details.  
 *   
 *  You should have received a copy of the GNU General Public License  
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *  
 *  Version:             1.2
 *  Design:              David Gascon 
 *  Implementation:      Yuri Carmona, Luis Miguel Marti  
 *  Ported to Raspberry: Ruben Martin  
 */

#include "arduPiSigfox.h"

//////////////////////////////////////////////
uint8_t sock = SOCKET0;     //Asign to UART0
//////////////////////////////////////////////

#define second 1000
#define minute 60*second
unsigned long pausetime = 1 * minute;  // pause in minutes
unsigned long previousMillis = 0; 

// define variable to create a speficic frame to be sent
uint8_t data[12];
uint8_t size;

// define error variable
uint8_t error;

// define analogs pins
const int temperature_pin = 0;
const int battery_pin = 1;

// define digitals pins
const int DIGITAL1 = 2;
const int DIGITAL2 = 3;
const int DIGITAL3 = 4;
const int DIGITAL4 = 5;
const int DIGITAL5 = 6;
const int DIGITAL6 = 7;
const int DIGITAL7 = 8;
const int DIGITAL8 = 9;

// define vars for sensors
float temperature;
uint8_t battery;
uint8_t digitalPins;
uint8_t digital1;
uint8_t digital2;
uint8_t digital3;
uint8_t digital4;
uint8_t digital5;
uint8_t digital6;
uint8_t digital7;
uint8_t digital8;

// define union-type variables
union temp_union_u
{
  uint8_t  value1[4];
  float    value2;
}temp_union;


void setup() 
{
  //Initialize pins
  for (int i=2; i==9; i++) pinMode(i, INPUT);

  //////////////////////////////////////////////
  // 1. switch on              
  //////////////////////////////////////////////  
  printf("1. Switch on the module:\n");

  error = Sigfox.ON(sock);
  
  // Check sending status
  if( error == 0 ) 
  {
    printf("Switch ON OK\n");
  }
  else 
  {
    printf("Switch ON ERROR\n"); 
  } 
}


void loop() 
{
  unsigned long currentMillis = millis();

  //Wait "pausetime" in minutes to send data array
  if (currentMillis - previousMillis >= pausetime) 
  {
    // save the last time in send data
    previousMillis = currentMillis;
    
    //////////////////////////////////////////////
    // 2. create array with sensor data
    // Sigfox Back-END device "Display type" must be: 
    // Temp::float:32 Battery::uint:8 Digital8::bool:7 Digital7::bool:6
    //       Digital6::bool:5 Digital5::bool:4 Digital4::bool:3
    //       Digital3::bool:2 Digital2::bool:1 Digital1::bool:0
    //////////////////////////////////////////////
  
    // 2. Reading sensors:
    printf("2. Reading sensors:\n");
 
    // 2.1. simulate temperature raw reading.
    temperature = (float)analogRead(temperature_pin);  
    printf("RAW temperature: %f\n", temperature);  
   
    // 2.2. simulate battery level raw reading
    battery = (uint8_t)analogRead(battery_pin);  
    printf("RAW battery: %d\n", battery);
 
    // 2.3. Digital pins reading
    digital1 = digitalRead(DIGITAL1);
    digital2 = digitalRead(DIGITAL2);
    digital3 = digitalRead(DIGITAL3);
    digital4 = digitalRead(DIGITAL4);
    digital5 = digitalRead(DIGITAL5);
    digital6 = digitalRead(DIGITAL6);
    digital7 = digitalRead(DIGITAL7);
    digital8 = digitalRead(DIGITAL8);
  
    //Digital Pins (bitmap):
    digitalPins |= digital1;
    digitalPins |= digital2 << 1;
    digitalPins |= digital3 << 2;
    digitalPins |= digital4 << 3;
    digitalPins |= digital5 << 4;
    digitalPins |= digital6 << 5;
    digitalPins |= digital7 << 6;
    digitalPins |= digital8 << 7; 
    
    printf("Digital Pins (bitmap HEX): 0x%X\n", digitalPins);
  
    // 2.4. Fill structure fields
    temp_union.value2 = temperature;  
  
    // fill 'data' buffer with data
    data[0] = temp_union.value1[3]; // big-endian
    data[1] = temp_union.value1[2];
    data[2] = temp_union.value1[1];
    data[3] = temp_union.value1[0];
    data[4] = battery;
    data[5] = digitalPins;
    size = 6;
         
    //Final Frame to send in "data"
    printf("Final Frame to send: 0x%X\n", data);
  
    //////////////////////////////////////////////
    // 3. send data
    //////////////////////////////////////////////
    
    printf("3. Sending packet:\n");
   
    //3. Sending packet:
    error = Sigfox.send(data,size);
  
    // Check sending status
    if( error == 0 ) 
    {
      printf("Sigfox packet sent OK\n");
    }
    else 
    {
      printf("Sigfox packet sent ERROR\n");
    } 
  //////////////////////////////////////////////
  // 4. sleep pause_time (minutes)
  //////////////////////////////////////////////
  printf("4. Sleep %d minutes\n", (pausetime/(60*second)));
  printf("-----------------------------\n");
  }
}


//////////////////////////////////////////////
// Main loop setup() and loop() declarations
//////////////////////////////////////////////
int main (){
    setup();
    while(1){
        loop();
    }
    return (0);
}                 
//////////////////////////////////////////////
