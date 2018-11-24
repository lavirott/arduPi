/*  
 *  TD P2P Code Example
 *
 *  NOTE: This example is only for TD1207 - Sigfox_868 modules.
 *  
 *  Explanation: This example shows how to transmit packets through the 
 *  LAN network created with other TD module
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

// ADDRESS: Define the LAN network address. 
// Range: From 0x000000 to 0xFFFFFF. 
// Default: 0x000000
//////////////////////////////////////////////
uint32_t address   = 0x000001;
//////////////////////////////////////////////

// define buffer for data transmission
uint8_t data[17];
uint8_t length;

uint8_t error;
uint32_t counter = 0;


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
  // Set LAN address
  //////////////////////////////////////////////
  error = Sigfox.setAddressLAN(address);
    
  // Check status
  if( error == 0 ) 
  {
    printf("Set LAN Address OK\n");
  }
  else 
  {
    printf("Set LAN Address ERROR\n");
  } 
}


void loop()
{
  
  //////////////////////////////////////////////
  // Create array of bytes to send
  //////////////////////////////////////////////  
  length = 0;
  data[length++] = 0x00;
  data[length++] = 0x00;
  data[length++] = 0x00;
  data[length++] = 0x00;
  data[length++] = 0x00;
  data[length++] = 0x00;
  data[length++] = 0x00;
  data[length++] = 0x00;
  data[length++] = 0x00;
  data[length++] = 0x00;
  data[length++] = 0x00;
  data[length++] = 0x00;
  data[length++] = 0x00;
  data[length++] = 0x00;
  data[length++] = 0x00;
  data[length++] = 0x00;
  data[length++] = counter++;
  
  printf("Data to send: %d\n", counter);
 
  //////////////////////////////////////////////
  // Send command
  //////////////////////////////////////////////  
  error = Sigfox.sendLAN(data, length);

  // Check status
  if( error == 0 ) 
  {
    printf("Send packet OK\n");
  }
  else 
  {
    printf("Send packet ERROR\n");
  }

  delay(5000);
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
