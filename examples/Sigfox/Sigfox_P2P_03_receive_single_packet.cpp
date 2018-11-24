/*  
 *  TD P2P Code Example
 *
 *  NOTE: This example is only for TD1207 - Sigfox_868 modules.
 *  
 *  Explanation: This example shows how to receive single packets
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
 *  Version:              1.2
 *  Design:               David Gascon 
 *  Implementation:       Yuri Carmona, Luis Miguel Marti  
 *  Ported to Rasspberry: Ruben Martin  
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

uint8_t error;


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
  // Receive data
  printf("Waiting for data...\n");

  error = Sigfox.receive(10);

  if (error == 0) 
  {
    printf("Packet received: %s\n", Sigfox._packet);
  }
  else
  {
    printf("Packet not received (timeout)\n");
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
