/*  
 *  ------ LoRaWAN Code Example -------- 
 *  
 *  Explanation: This example shows how to configure the module and
 *  send packets to a LoRaWAN gateway with acknowledgement. 
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

// Device parameters for Back-End registration
////////////////////////////////////////////////////////////
char DEVICE_EUI[]  = "0102030405060708";
char APP_EUI[] = "1112131415161718";
char APP_KEY[] = "01020304050607080910111213141516";
////////////////////////////////////////////////////////////

// Define port to use in Back-End: from 1 to 223
uint8_t PORT = 3;

// Define data payload to send (maximum is up to data rate)
char data[] = "0102030405060708090A0B0C0D0E0F";

// variable
uint8_t error;



void setup() 
{
  printf("LoRaWAN example - Send Confirmed packets (with ACK)\n");


  printf("------------------------------------\n");
  printf("Module configuration\n");
  printf("------------------------------------\n\n");


  //////////////////////////////////////////////
  // 1. Switch on
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
  // 2. Set Device EUI
  //////////////////////////////////////////////

  error = LoRaWAN.setDeviceEUI(DEVICE_EUI);

  // Check status
  if( error == 0 ) 
  {
    printf("2. Device EUI set OK\n");     
  }
  else 
  {
    printf("2. Device EUI set error = %d\n", error); 
  }


  //////////////////////////////////////////////
  // 3. Set Device Address
  //////////////////////////////////////////////

  error = LoRaWAN.setAppEUI(APP_EUI);

  // Check status
  if( error == 0 ) 
  {
    printf("3. Application EUI set OK\n");     
  }
  else 
  {
    printf("3. Application EUI set error = %d\n",error); 
  }


  //////////////////////////////////////////////
  // 4. Set Network Session Key
  //////////////////////////////////////////////
 
  error = LoRaWAN.setAppKey(APP_KEY);

  // Check status
  if( error == 0 ) 
  {
    printf("4. Application Key set OK\n");     
  }
  else 
  {
    printf("4. Application Key set error = %d\n", error); 
  }


  //////////////////////////////////////////////
  // 5. Save configuration
  //////////////////////////////////////////////
  
  error = LoRaWAN.saveConfig();

  // Check status
  if( error == 0 ) 
  {
    printf("5. Save configuration OK\n");     
  }
  else 
  {
    printf("5. Save configuration error = %d\n", error);
  }


  printf("\n------------------------------------\n");
  printf("Module configured\n");
  printf("------------------------------------\n\n");
  
  LoRaWAN.getDeviceEUI();
  printf("Device EUI: %s ",LoRaWAN._devEUI);
  
  LoRaWAN.getDeviceAddr();
  printf("Device Address: %s\n\n",LoRaWAN._devAddr);
}



void loop() 
{
  
  //////////////////////////////////////////////
  // 1. Switch on
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
  // 2. Join network
  //////////////////////////////////////////////

  error = LoRaWAN.joinOTAA();

  // Check status
  if( error == 0 ) 
  {
    printf("2. Join network OK\n");     

    //////////////////////////////////////////////
    // 3. Send confirmed packet 
    //////////////////////////////////////////////
  
    error = LoRaWAN.sendConfirmed(PORT, data);
  
    // Error messages:
    /*
     * '6' : Module hasn't joined a network
     * '5' : Sending error
     * '4' : Error with data length	  
     * '2' : Module didn't response
     * '1' : Module communication error   
     */
    // Check status
    if( error == 0 ) 
    {
      printf("3. Send Confirmed packet OK\n");     
      if (LoRaWAN._dataReceived == true)
      { 
        printf("   There's data on port number %d.\r\n", LoRaWAN._port);
        printf("   Data: %s\n", LoRaWAN._data);
      }
    }
    else 
    {
      printf("3. Send Unconfirmed packet error = %d\n", error); 
    }
  }
  else 
  {
    printf("2. Join network error = %d\n",error);
  }
  
    
  //////////////////////////////////////////////
  // 4. Clean channels
  //////////////////////////////////////////////
  error = LoRaWAN.reset();

  // Reset channels
  if( error == 0 ) 
  {
    printf("4. Clean channels OK\n");     
  }
  else 
  {
    printf("4. Clean channels error = %d\n", error); 
  }


  //////////////////////////////////////////////
  // 5. Switch off
  //////////////////////////////////////////////
  error = LoRaWAN.OFF(sock);

  // Check status
  if( error == 0 ) 
  {
    printf("5. Switch OFF OK\n");     
  }
  else 
  {
    printf("5. Switch OFF error = %d\n",error); 
  }
  
  
  printf("\n");
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



