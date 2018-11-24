/*  
 *  Sigfox Code Example
 *  
 *  Explanation: This example shows how to send a Sigfox packet waiting 
 *  for a ACK (acknowledgement) to arrive.
 *  In this example, the message to be sent is defined as a string.
 *  This string defines the diferente HEX values of the bytes to be sent
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


void setup() 
{
  //////////////////////////////////////////////
  // 1. switch on
  //////////////////////////////////////////////
  printf("Sigfox - Sending example\n");

  error = Sigfox.ON(sock);
  
  // Check status
  if( error == 0 ) 
  {
    printf("Switch ON OK\n");
  }
  else 
  {
    printf("Switch ON ERROR\n");
  } 

  //////////////////////////////////////////////
  // 2. send data
  //////////////////////////////////////////////
  printf("Sending a packet...\n");

  // Send 12 bytes at most
  error = Sigfox.sendACK((char*)"000102030405060708090A0B");
  
  // Check sending status
  if( error == 0 ) 
  {
    printf("Sigfox packet sent OK\n");
    printf("Back-End response: 0x%X\n", Sigfox._buffer);
  }
  else 
  {
    printf("Sigfox packet sent ERROR\n"); 
  } 

  printf("Enter Sleep\n");
}


void loop() 
{  
  //////////////////////////////////////////////
  // 3. sleep
  //////////////////////////////////////////////
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
