/*  
 *  ------ LoRaWAN Code Example -------- 
 *  
 *  Explanation: This example shows how to configure the module
 *  and all general settings related to back-end registration
 *  process.
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

//////////////////////////////////////////////
uint8_t sock = SOCKET0;
//////////////////////////////////////////////

// Device parameters for Back-End registration
////////////////////////////////////////////////////////////
char DEVICE_EUI[]  = "0102030405060708";
char DEVICE_ADDR[] = "05060708";
char NWK_SESSION_KEY[] = "01020304050607080910111213141516";
char APP_SESSION_KEY[] = "000102030405060708090A0B0C0D0E0F";
char APP_KEY[] = "000102030405060708090A0B0C0D0E0F";
////////////////////////////////////////////////////////////

// variable
uint8_t error;



void setup() 
{
  printf("LoRaWAN example - Module configuration\n");


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
  // 2. Reset to factory default values
  //////////////////////////////////////////////
  
  error = LoRaWAN.factoryReset();

  // Check status
  if( error == 0 ) 
  {
    printf("2. Reset to factory default values OK\n");
  }
  else 
  {
    printf("2. Reset to factory error = %d\n", error);
  }


  //////////////////////////////////////////////
  // 3. Set/Get Device EUI
  //////////////////////////////////////////////

  // Set Device EUI
  error = LoRaWAN.setDeviceEUI(DEVICE_EUI);

  // Check status
  if( error == 0 ) 
  {
    printf("3.1. Set Device EUI OK\n");
  }
  else 
  {
    printf("3.1. Set Device EUI error = %d\n", error);
  }

  // Get Device EUI
  error = LoRaWAN.getDeviceEUI();

  // Check status
  if( error == 0 ) 
  {
    printf("3.2. Get Device EUI OK. ");
    printf("Device EUI: %s\n",LoRaWAN._devEUI);
  }
  else 
  {
    printf("3.2. Get Device EUI error = %d\n", error);
  }


  //////////////////////////////////////////////
  // 4. Set/Get Device Address
  //////////////////////////////////////////////

  // Set Device Address
  error = LoRaWAN.setDeviceAddr(DEVICE_ADDR);

  // Check status
  if( error == 0 ) 
  {
    printf("4.1. Set Device address OK\n");
  }
  else 
  {
    printf("4.1. Set Device address error = %d\n", error);
  }
  
  // Get Device Address
  error = LoRaWAN.getDeviceAddr();

  // Check status
  if( error == 0 ) 
  {
    printf("4.2. Get Device address OK. ");
    printf("Device address: %s\n", LoRaWAN._devAddr);
  }
  else 
  {
    printf("4.2. Get Device address error = %d\n", error);
  }


  //////////////////////////////////////////////
  // 5. Set Network Session Key
  //////////////////////////////////////////////
 
  error = LoRaWAN.setNwkSessionKey(NWK_SESSION_KEY);

  // Check status
  if( error == 0 ) 
  {
    printf("5. Set Network Session Key OK\n");
  }
  else 
  {
    printf("5. Set Network Session Key error = %d\n", error);
  }


  //////////////////////////////////////////////
  // 6. Set Application Session Key
  //////////////////////////////////////////////

  error = LoRaWAN.setAppSessionKey(APP_SESSION_KEY);

  // Check status
  if( error == 0 ) 
  {
    printf("6. Set Application Session Key OK\n");
  }
  else 
  {
    printf("6. Set Application Session Key error = %d\n", error);
  }


  //////////////////////////////////////////////
  // 7. Set retransmissions for uplink confirmed packet
  //////////////////////////////////////////////

  // set retries
  error = LoRaWAN.setRetries(7);

  // Check status
  if( error == 0 ) 
  {
    printf("7.1. Set Retransmissions for uplink confirmed packet OK\n");
  }
  else 
  {
    printf("7.1. Set Retransmissions for uplink confirmed packet error = %d\n", error);
  }
  
  // Get retries
  error = LoRaWAN.getRetries();

  // Check status
  if( error == 0 ) 
  {
    printf("7.2. Get Retransmissions for uplink confirmed packet OK. \n");
    printf("TX retries: %d\n", LoRaWAN._retries);
  }
  else 
  {
    printf("7.2. Get Retransmissions for uplink confirmed packet error = %d\n", error);
  }


  //////////////////////////////////////////////
  // 8. Set application key
  //////////////////////////////////////////////

  error = LoRaWAN.setAppKey(APP_KEY);

  // Check status
  if( error == 0 ) 
  {
    printf("8. Application key set OK\n");
  }
  else 
  {
    printf("8. Application key set error = %d\n", error);
  }


  //////////////////////////////////////////////
  // 9. For 900MHz US bands with gateways limited 
  // to 8 channels, disable the unavailable channels
  //////////////////////////////////////////////

  for (int ch = 8; ch <= 64; ch++)
  {
    error = LoRaWAN.setChannelStatus(ch, (char*)"off");

    // Check status
    if( error == 0 ) 
    {
      printf("9. Channel %d status set OK\n", ch);
    }
    else 
    {
      printf("9. Channel %d status set error = %d\n", ch, error);
    }
  }


  //////////////////////////////////////////////
  // 10. Save configuration
  //////////////////////////////////////////////
  
  error = LoRaWAN.saveConfig();

  // Check status
  if( error == 0 ) 
  {
    printf("10. Save configuration OK\n");
  }
  else 
  {
    printf("10. Save configuration error = %d\n", error);
  }


  printf("------------------------------------\n");
  printf("Now the LoRaWAN module is ready for\n");
  printf("joining networks and send messages.\n");
  printf("Please check the next examples...\n");
  printf("------------------------------------\n");

}


void loop() 
{
  // do nothing
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
