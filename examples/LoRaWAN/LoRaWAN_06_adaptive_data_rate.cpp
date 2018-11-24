/*  
 *  ------ LoRaWAN Code Example -------- 
 *  
 *  Explanation: This example shows how to enable / disable the 
 *  adaptive data rate.
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
 *  Version:           0.4
 *  Design:            David Gascon 
 *  Implementation:    Luismi Marti, Rubén Martín  
 */

#include "arduPiLoRaWAN.h"

// socket to use
//////////////////////////////////////////////
uint8_t sock = SOCKET0;
//////////////////////////////////////////////

// variable
uint8_t error;

void setup() 
{
  printf("LoRaWAN example - Data Rate configuration\n");

  //////////////////////////////////////////////
  // 1. switch on
  //////////////////////////////////////////////

  error = LoRaWAN.ON(sock);

  // Check status
  if( error == 0 ) 
  {
    printf("1. Switch ON OK\n");     
  }
  else 
  {
    printf("1. Switch ON error = %d\n", error); 
  }


  //////////////////////////////////////////////
  // 2. Enable Adaptive Data Rate (ADR)
  //////////////////////////////////////////////

  error = LoRaWAN.setADR((char*)"on");

  // Check status
  if( error == 0 ) 
  {
    printf("2. Adaptive Data Rate enabled OK. ");    
    printf("ADR: %d\n", LoRaWAN._adr);
  }
  else 
  {
    printf("2. Enable data rate error = %d\n", error); 
  }


  //////////////////////////////////////////////
  // 3. Disable Adaptive Data Rate (ADR)
  //////////////////////////////////////////////

  error = LoRaWAN.setADR((char*)"off");

  // Check status
  if( error == 0 ) 
  {
    printf("3. Adaptive Data Rate disabled OK. ");    
    printf("ADR: %d\n", LoRaWAN._adr);
  }
  else 
  {
    printf("3. Data rate set error = %d\n", error); 
  }


  //////////////////////////////////////////////
  // 4. Save configuration
  //////////////////////////////////////////////

  error = LoRaWAN.saveConfig();

  // Check status
  if( error == 0 ) 
  {
    printf("4. Save configuration OK\n");
  }
  else 
  {
    printf("4. Save configuration error = %d\n", error); 
  }

}


void loop() 
{

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




