/*  
 *  ------ LoRaWAN Code Example -------- 
 *  
 *  Explanation: This example shows how to configure the channels settings.
 *  There are 16 channels. The first three of them are always preconfigured 
 *  and cannot be changed: 868.1 MHz, 868.3 MHz and 868.5 MHz. Besides other
 *  parameters can be changed in all channels: Duty Cycle, Data rate range
 *  and channel status
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
  printf("LoRaWAN example - Channels configuration\n");

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
    printf("1. Switch ON error = %d\n",error); 
  }


  //////////////////////////////////////////////
  // 2. Set frequency for specific channel
  //////////////////////////////////////////////

  error = LoRaWAN.setChannelFreq(3, 865000000);

  // Check status
  if( error == 0 ) 
  {
    printf("2. Frequency set OK. ");    
    printf("Frequency: %u\n",LoRaWAN._freq[3]);
  }
  else 
  {
    printf("2. Enable data rate error = %d\n", error); 
  }


  //////////////////////////////////////////////
  // 3. Set Duty Cycle for specific channel
  //////////////////////////////////////////////

  error = LoRaWAN.setChannelDutyCycle(3, 999);

  // Check status
  if( error == 0 ) 
  {
    printf("3. Adaptive Data Rate disabled OK. ");    
    printf("Duty Cycle: %d\n", LoRaWAN._dCycle[3]);
  }
  else 
  {
    printf("3. Data rate set error = %d\n", error); 
  }


  //////////////////////////////////////////////
  // 4. Set Data rate range for specific channel
  //////////////////////////////////////////////

  error = LoRaWAN.setChannelDRRange(3, 0, 2);

  // Check status
  if( error == 0 ) 
  {
    printf("4. Data Rate range set OK. ");    
    printf("Data Rate min: %d",LoRaWAN._drrMin[3]);
    printf(". Data Rate max: %d\n", LoRaWAN._drrMax[3]);
  }
  else 
  {
    printf("4. Data rate set error = %d\n", error);
  }


  //////////////////////////////////////////////
  // 5. Set Data rate range for specific channel
  //////////////////////////////////////////////

  error = LoRaWAN.setChannelStatus(3, (char*)"on");

  // Check status
  if( error == 0 ) 
  {
    printf("5. Channel status set OK: %d\n", LoRaWAN._status[3]);
  }
  else 
  {
    printf("5. Data rate set error = %d\n", error); 
  }


  //////////////////////////////////////////////
  // 6. Save configuration
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


  ///////////////////////////////////////////////////////////
  // show configuration for all channels available
  ///////////////////////////////////////////////////////////

  printf("\n----------------------------\n");

  for( int i=0; i<16; i++)
  {
    LoRaWAN.getChannelFreq(i);
    LoRaWAN.getChannelDutyCycle(i);
    LoRaWAN.getChannelDRRange(i);
    LoRaWAN.getChannelStatus(i);

    printf("Channel: %d\n", i);
    printf("  Freq: %d\n",LoRaWAN._freq[i]);
    printf("  Duty cycle: %d\n",LoRaWAN._dCycle[i]);
    printf("  DR min: %d\n",LoRaWAN._drrMin[i]);
    printf("  DR max: %d\n",LoRaWAN._drrMax[i]);
    printf("  Status: %d\n",LoRaWAN._status[i]);
    printf("----------------------------\n");
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





