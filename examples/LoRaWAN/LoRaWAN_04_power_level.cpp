/*  
 *  ------ LoRaWAN Code Example -------- 
 *  
 *  Explanation: This example shows how to configure the power level
 *  LoRaWAN interface:
 *       868 MHz     433 MHz
 *    0:  N/A        10 dBm
 *    1:  14 dBm      7 dBm
 *    2:  11 dBm      4 dBm 
 *    3:   8 dBm      1 dBm
 *    4:   5 dBm     -2 dBm 
 *    5:   2 dBm     -5 dBm
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
 *  Implementation:    Luismi Marti, Ruben Martin  
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
  printf("LoRaWAN example - Power configuration\n");

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
  // 2. Set Power level
  //////////////////////////////////////////////

  error = LoRaWAN.setPower(1);

  // Check status
  if( error == 0 ) 
  {
    printf("2. Power level set OK\n");     
  }
  else 
  {
    printf("2. Power level set error = %d\n", error); 
  }


  //////////////////////////////////////////////
  // 3. Get Device EUI
  //////////////////////////////////////////////

  error = LoRaWAN.getPower();

  // Check status
  if( error == 0 ) 
  {
    printf("3. Power level get OK.\n");    
    printf("Power index: %d\n",LoRaWAN._powerIndex);
  }
  else 
  {
    printf("3. Power level set error = %d\n", error); 
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
//Do nothing
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



