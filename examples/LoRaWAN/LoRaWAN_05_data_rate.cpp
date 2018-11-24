/*  
 *  ------ LoRaWAN Code Example -------- 
 *  
 *  Explanation: This example shows how to configure the data rate.
 *  The possibilities are:
 *    0: SF = 12, BW = 125 kHz, BitRate = 250 bps
 *    1: SF = 11, BW = 125 kHz, BitRate = 440 bps
 *    2: SF = 10, BW = 125 kHz, BitRate = 980 bps
 *    3: SF =  9, BW = 125 kHz, BitRate = 1760 bps
 *    4: SF =  8, BW = 125 kHz, BitRate = 3125 bps
 *    5: SF =  7, BW = 125 kHz, BitRate = 5470 bps
 *    6: SF =  7, BW = 250 kHz, BitRate = 11000 bps
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
 *  Implementation:    Luis M. Martinez, Ruben Martin 
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
  printf("\nData Rate options:\n");
  printf("------------------------------------------------------\n");
  printf("  0: SF = 12, BW = 125 kHz, BitRate =   250 bps\n");
  printf("  1: SF = 11, BW = 125 kHz, BitRate =   440 bps\n");
  printf("  2: SF = 10, BW = 125 kHz, BitRate =   980 bps\n");
  printf("  3: SF =  9, BW = 125 kHz, BitRate =  1760 bps\n");
  printf("  4: SF =  8, BW = 125 kHz, BitRate =  3125 bps\n");
  printf("  5: SF =  7, BW = 125 kHz, BitRate =  5470 bps\n");
  printf("  6: SF =  7, BW = 250 kHz, BitRate = 11000 bps\n");  
  printf("------------------------------------------------------\n\n");

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

  error = LoRaWAN.setDataRate(0);

  // Check status
  if( error == 0 ) 
  {
    printf("2. Data rate set OK\n");     
  }
  else 
  {
    printf("2. Data rate set error = %d\n",error); 
  }


  //////////////////////////////////////////////
  // 3. Get Device EUI
  //////////////////////////////////////////////

  error = LoRaWAN.getDataRate();

  // Check status
  if( error == 0 ) 
  {
    printf("3. Data rate get OK.\n");    
    printf("Data rate index: %d\n",LoRaWAN._dataRate);
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




