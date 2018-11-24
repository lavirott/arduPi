/*  
 *  ------ Sigfox Code Example -------- 
 *  
 *  Explanation: This example shows how to send a Keep-Alive packet
 *  to the Sigfox network
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
  // switch on
  //////////////////////////////////////////////
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
  // Send Keep-Alive packet
  //////////////////////////////////////////////
  Sigfox.sendKeepAlive();
   
  // Check sending status
  if( error == 0 ) 
  {
    printf("Send Keep-Alive: OK\n");
  }
  else 
  {
    printf("Send Keep-Alive: ERROR\n");
  }       
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
