/*  
 *  ------ P2P Code Example -------- 
 *  
 *  Explanation: This example shows how to configure the module
 *  for P2P mode and LoRaWAN mode too. In this code, the Waspmote
 *  waits for new incoming P2P packets. Then routes the information
 *  received to the LoRaWAN gateway.
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

// define radio settings (P2P interface)
//////////////////////////////////////////////
uint8_t power = 15;
uint32_t frequency = 868100000;
char spreading_factor[] = "sf12";
char coding_rate[] = "4/5";
uint16_t bandwidth = 125;
char crc_mode[] = "on";
//////////////////////////////////////////////

// Define LoRaWAN settings (LoRaWAN interface)
////////////////////////////////////////////////////////////
uint8_t PORT = 3;
char DEVICE_EUI[]  = "0102030405060708";
char DEVICE_ADDR[] = "05060708";
char NWK_SESSION_KEY[] = "01020304050607080910111213141516";
char APP_SESSION_KEY[] = "000102030405060708090A0B0C0D0E0F";
////////////////////////////////////////////////////////////

// define functions
uint8_t radioModuleSetup(void);
uint8_t lorawanModuleSetup(void);


// define
char packet[512];

// variable
uint8_t error;



void setup() 
{
  printf("P2P to LoRaWAN gateway\n\n");

  // module setup
  error = lorawanModuleSetup();

  // Check status
  if (error == 0)
  {
    printf("LoRaWAN interface configured OK\n");     
  }
  else 
  {
    printf("LoRaWAN interface configured ERROR\n");     
  }  


  // module setup
  error = radioModuleSetup();

  // Check status
  if (error == 0)
  {
    printf("Radio interface configured OK\n");     
  }
  else 
  {
    printf("Radio interface configured ERROR\n");     
  }
  printf("\n");

}


void loop() 
{

  printf("\n-------------------------------------------------------\n");
  printf("Listening to packets:\n");
  printf("-------------------------------------------------------\n");

  // Set receive mode 
  error = LoRaWAN.receiveRadio(10000);

  // Check status
  if (error == 0)
  {
    printf("--> Packet received\n");
    
    printf("data: %s\n",(char*) LoRaWAN._buffer);
    
    printf("length: %u\n", LoRaWAN._length);
        
    // copy packet contents into a global buffer
    strncpy( packet, (char*) LoRaWAN._buffer, sizeof(packet));
    
    printf("\n-------------------------------------------------------\n");
    printf("Route data to LoRaWAN gateway:\n");
    printf("-------------------------------------------------------\n");


    // switch to LoRaWAN mode
    LoRaWAN.macResume();


    // join 
    error = LoRaWAN.joinABP();

    // Check status
    if( error == 0 ) 
    {
      printf("Join network OK\n");     
    }
    else 
    {
      printf("Join network error = %d\n", error);
    }


    // send received packet to LoRaWAN gateway
    error = LoRaWAN.sendConfirmed( PORT, packet);

    // Check status
    if( error == 0 ) 
    {
      printf("Send Confirmed packet OK\n");     
    }
    else 
    {
      printf("Send Confirmed packet error = %d\n", error); 
    }

    // module interface
    error = lorawanModuleSetup();

    // Check status
    if (error == 0)
    {
      printf("LoRaWAN interface configured OK\n");     
    }
    else 
    {
      printf("LoRaWAN interface configured ERROR\n");     
    }  

    // module setup
    error = radioModuleSetup();

    // Check status
    if (error == 0)
    {
      printf("Radio interface configured OK\n");     
    }
    else 
    {
      printf("Radio interface configured ERROR\n");     
    }
    printf("\n"); 

  }
  else 
  {
    // error code
    //  1: error
    //  2: no incoming packet
    printf("Error waiting for packets. error = %d\n", error);  
  }  
}




/***********************************************************************************
*
* radioModuleSetup()
*
*   This function includes all functions related to the module setup and configuration
*   The user must keep in mind that each time the module powers on, all settings are set
*   to default values. So it is better to develop a specific function including all steps
*   for setup and call it everytime the module powers on.
*
*
***********************************************************************************/
uint8_t radioModuleSetup()
{ 
  uint8_t status = 0;
  uint8_t e = 0;

  printf("\n-------------------------------------------------------\n");
  printf("P2P mode configuration:\n");
  printf("-------------------------------------------------------\n");

  
  //////////////////////////////////////////////
  // 1. switch on
  //////////////////////////////////////////////

  e = LoRaWAN.ON(sock);

  // Check status
  if (e == 0)
  {
    printf("1. Switch ON OK\n");     
  }
  else 
  {
    printf("1. Switch ON error = %d\n",e); 
    status = 1;
  }
  printf("-------------------------------------------------------\n");



  //////////////////////////////////////////////
  // 2. Enable P2P mode
  //////////////////////////////////////////////

  e = LoRaWAN.macPause();

  // Check status
  if (e == 0)
  {
    printf("2. P2P mode enabled OK\n");
  }
  else 
  {
    printf("2. Enable P2P mode error = %d\n", e);
    status = 1;
  }
  printf("-------------------------------------------------------\n");



  //////////////////////////////////////////////
  // 3. Set/Get Radio Power
  //////////////////////////////////////////////

  // Set power
  e = LoRaWAN.setRadioPower(power);

  // Check status
  if (e == 0)
  {
    printf("3.1. Set Radio Power OK\n");
  }
  else 
  {
    printf("3.1. Set Radio Power error = %d\n", e);
    status = 1;
  }

  // Get power
  e = LoRaWAN.getRadioPower();

  // Check status
  if (e == 0) 
  {
    printf("3.2. Get Radio Power OK. "); 
    printf("Power: %d\n",LoRaWAN._radioPower);
  }
  else 
  {
    printf("3.2. Get Radio Power error = %d\n", e); 
    status = 1;
  }
  printf("-------------------------------------------------------\n");



  //////////////////////////////////////////////
  // 4. Set/Get Radio Frequency
  //////////////////////////////////////////////

  // Set frequency
  e = LoRaWAN.setRadioFreq(frequency);

  // Check status
  if (e == 0)
  {
    printf("4.1. Set Radio Frequency OK\n");
  }
  else 
  {
    printf("4.1. Set Radio Frequency error = %d\n", error);
    status = 1;
  }

  // Get frequency
  e = LoRaWAN.getRadioFreq();

  // Check status
  if (e == 0) 
  {
    printf("4.2. Get Radio Frequency OK. "); 
    printf("Frequency: %d\n", LoRaWAN._radioFreq);
  }
  else 
  {
    printf("4.2. Get Radio Frequency error = %d\n", e); 
    status = 1;
  }
  printf("-------------------------------------------------------\n");



  //////////////////////////////////////////////
  // 5. Set/Get Radio Spreading Factor (SF)
  //////////////////////////////////////////////

  // Set SF
  e = LoRaWAN.setRadioSF(spreading_factor);

  // Check status
  if (e == 0)
  {
    printf("5.1. Set Radio SF OK\n");
  }
  else 
  {
    printf("5.1. Set Radio SF error = %e\n", e);
    status = 1;
  }

  // Get SF
  e = LoRaWAN.getRadioSF();

  // Check status
  if (e == 0) 
  {
    printf("5.2. Get Radio SF OK. "); 
    printf("Spreading Factor: %s\n", LoRaWAN._radioSF);
  }
  else 
  {
    printf("5.2. Get Radio SF error = %d\n", e); 
    status = 1;
  }
  printf("-------------------------------------------------------\n");



  //////////////////////////////////////////////
  // 6. Set/Get Radio Coding Rate (CR)
  //////////////////////////////////////////////

  // Set CR
  e = LoRaWAN.setRadioCR(coding_rate);

  // Check status
  if (e == 0)
  {
    printf("6.1. Set Radio CR OK\n");
  }
  else 
  {
    printf("6.1. Set Radio CR error = %d\n", e);
    status = 1;
  }

  // Get CR
  e = LoRaWAN.getRadioCR();

  // Check status
  if (e == 0) 
  {
    printf("6.2. Get Radio CR OK. "); 
    printf("Coding Rate: %s\n",LoRaWAN._radioCR);
  }
  else 
  {
    printf("6.2. Get Radio CR error = %d\n", e); 
    status = 1;
  }
  printf("-------------------------------------------------------\n");



  //////////////////////////////////////////////
  // 7. Set/Get Radio Bandwidth (BW)
  //////////////////////////////////////////////

  // Set BW
  e = LoRaWAN.setRadioBW(bandwidth);

  // Check status
  if (e == 0)
  {
    printf("7.1. Set Radio BW OK\n");
  }
  else 
  {
    printf("7.1. Set Radio BW error = %d\n", e);
  }

  // Get BW
  e = LoRaWAN.getRadioBW();

  // Check status
  if (e == 0) 
  {
    printf("7.2. Get Radio BW OK. "); 
    printf("Bandwidth: %u\n", LoRaWAN._radioBW);
  }
  else 
  {
    printf("7.2. Get Radio BW error = %d\n", e); 
    status = 1;
  }
  printf("-------------------------------------------------------\n");



  //////////////////////////////////////////////
  // 8. Set/Get Radio CRC mode
  //////////////////////////////////////////////

  // Set CRC
  e = LoRaWAN.setRadioCRC(crc_mode);

  // Check status
  if (e == 0)
  {
    printf("8.1. Set Radio CRC mode OK\n");
  }
  else 
  {
    printf("8.1. Set Radio CRC mode error = %d\n", e);
    status = 1;
  }

  // Get CRC
  e = LoRaWAN.getRadioCRC();

  // Check status
  if (e == 0) 
  {
    printf("8.2. Get Radio CRC mode OK. "); 
    printf("CRC status: %u\n",LoRaWAN._crcStatus);
  }
  else 
  {
    printf("8.2. Get Radio CRC mode error = %e\n", e); 
    status = 1;
  }
  printf("-------------------------------------------------------\n");


  return status;
}

/***********************************************************************************
 *
 * lorawanModuleSetup()
 *
 *   This function includes all functions related to the module setup for LoRaWAN
 *   performance.
 *
 *
 ***********************************************************************************/
uint8_t lorawanModuleSetup()
{ 

  uint8_t status = 0;
  uint8_t e = 0;


  printf("\n-------------------------------------------------------\n");
  printf("LoRaWAN mode configuration:\n");
  printf("-------------------------------------------------------\n");

  //////////////////////////////////////////////
  // 1. Switch on
  //////////////////////////////////////////////

  e = LoRaWAN.ON(sock);

  // Check status
  if( e == 0 ) 
  {
    printf("1. Switch ON OK\n");     
  }
  else 
  {
    printf("1. Switch ON error = %d\n", e); 
    status = 1;
  }

  LoRaWAN.factoryReset();

  //////////////////////////////////////////////
  // 2. Set Device EUI
  //////////////////////////////////////////////

  e = LoRaWAN.setDeviceEUI(DEVICE_EUI);

  // Check status
  if( e == 0 ) 
  {
    printf("2.1. Device EUI set OK\n");     
  }
  else 
  {
    printf("2.1. Device EUI set error = %d\n", e); 
    status = 1;
  }


  e = LoRaWAN.getDeviceEUI();

  // Check status
  if( e == 0 ) 
  {
    printf("2.2. Device EUI get OK. ");   
    printf("Device EUI: %s\n", LoRaWAN._devEUI);
  }
  else 
  {
    printf("2.2. Device EUI get error = %d\n", e); 
    status = 1;
  }


  //////////////////////////////////////////////
  // 3. Set Device Address
  //////////////////////////////////////////////

  e = LoRaWAN.setDeviceAddr(DEVICE_ADDR);

  // Check status
  if( e == 0 ) 
  {
    printf("3.1. Device address set OK\n");     
  }
  else 
  {
    printf("3.1. Device address set error = %d\n", e); 
    status = 1;
  }

  e = LoRaWAN.getDeviceAddr();

  // Check status
  if( e == 0 ) 
  {
    printf("3.2. Device address get OK. ");   
    printf("Device address: %s\n", LoRaWAN._devAddr);
  }
  else 
  {
    printf("3.2. Device address get error = %d\n", e); 
    status = 1;
  }



  //////////////////////////////////////////////
  // 4. Set Network Session Key
  //////////////////////////////////////////////

  error = LoRaWAN.setNwkSessionKey(NWK_SESSION_KEY);

  // Check status
  if( error == 0 ) 
  {
    printf("4. Network Session Key set OK\n");     
  }
  else 
  {
    printf("4. Network Session Key set error = %d\n", error);
    status = 1;
  }


  //////////////////////////////////////////////
  // 5. Set Application Session Key
  //////////////////////////////////////////////

  error = LoRaWAN.setAppSessionKey(APP_SESSION_KEY);

  // Check status
  if( error == 0 ) 
  {
    printf("5. Application Session Key set OK\n");     
  }
  else 
  {
    printf("5. Application Session Key set error = %d\n", error); 
    status = 1;
  }


  //////////////////////////////////////////////
  // 6. Save configuration
  //////////////////////////////////////////////

  error = LoRaWAN.saveConfig();

  // Check status
  if( error == 0 ) 
  {
    printf("6. Save configuration OK\n");     
  }
  else 
  {
    printf("6. Save configuration error = %d\n", error); 
    status = 1;
  }

  printf("-------------------------------------------------------\n");

  return status;
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



