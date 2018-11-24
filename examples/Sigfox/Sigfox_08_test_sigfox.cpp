/*  
 *  Sigfox Code Example
 *  
 *  Explanation: This example shows how perform a Sigfox TX test.
 *  The possible settings are:
 *    count = 0..65535: Count of SIGFOX test RF messages
 *    period = 1..255 Period in seconds between SIGFOX test RF messages
 *    channel = 0 to 180 or 220 to 400 or -1 (random channel) 
 *  
 *  Copyright (C) 2015 Libelium Comunicaciones Distribuidas S.L. 
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

uint8_t error;

// define TEST settings
/////////////////////////////////////////////
uint16_t  counter = 10;
uint16_t  period  = 10;
int16_t   channel = -1;
/////////////////////////////////////////////


void setup() 
{
  //////////////////////////////////////////////
  // switch on
  //////////////////////////////////////////////
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
  
  //////////////////////////////////////////////
  // Perform SIGFOX TX test
  //////////////////////////////////////////////
  printf("Testing...\n");

  Sigfox.testTransmit(counter,period,channel);
   
  // Check sending status
  if( error == 0 ) 
  {
    printf("Sigfox TX test: OK\n");
  }
  else 
  {
    printf("Sigfox TX test: ERROR\n");
  }       
  
  //////////////////////////////////////////////
  // Sleep
  //////////////////////////////////////////////
  printf("Sleep...\n");
}


void loop() 
{
  delay(10000);
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
