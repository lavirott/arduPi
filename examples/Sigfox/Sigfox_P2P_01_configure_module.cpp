/*  
 *  TD P2P Code Example
 *
 *  NOTE: This example is only for TD1207 - Sigfox_868 modules.
 *  
 *  Explanation: This example shows how to configure the module for 
 *  LAN operation. This example shows how to set/get:
 *    - LAN address
 *    - LAN mask
 *    - LAN frequency channel
 *    - LAN power level
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

// ADDRESS: Define the LAN network address. Range: From 0x000000 to 0xFFFFFF. Default: 0x000000
// MASK: Define the Mask address. Range: From 0x000000 to 0xFFFFFF. Default: 0xFFFFFF
// FREQUENCY: Define the Frequency. Range: From 868000000 to 869700000. Default: 869700000
// POWER: Define the RF power level in dBm. Range: From -35 to 14. Default: 14
///////////////////////////////////////////////////////////////////////////////////////////
uint32_t address   = 0x000001;
uint32_t mask      = 0xFFFFFF;
uint32_t frequency = 869700000;
uint8_t  power     = 14;
///////////////////////////////////////////////////////////////////////////////////////////

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
}


void loop() 
{
  //////////////////////////////////////////////
  // 1. Set/Get LAN address
  //////////////////////////////////////////////
  
  // 1.1. Set LAN address
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
  
  // 1.2. Get LAN address
  error = Sigfox.getAddressLAN();
    
  // Check status
  if( error == 0 ) 
  {
    printf("Get LAN Address OK\n");
    printf("Module ID: 0x%X\n", Sigfox._address);
  }
  else 
  {
    printf("Get LAN Address ERROR\n");
  }
  

  //////////////////////////////////////////////
  // 2. Set/Get Mask address
  //////////////////////////////////////////////
 
  // 2.1. Set mask
  error = Sigfox.setMask(mask);
    
  // Check status
  if( error == 0 ) 
  {
    printf("Set Mask OK\n");
  }
  else 
  {
    printf("Set Mask ERROR\n");
  } 
  
  // 2.2. Get mask
  error = Sigfox.getMask();
    
  // Check status
  if( error == 0 ) 
  {
    printf("Get Mask OK\n");
    printf("Mask ID: 0x%X\n", Sigfox._mask);
  }
  else 
  {
    printf("Get Mask ERROR\n");
  } 

  
  //////////////////////////////////////////////
  // 3. Set/Get Frequency band
  //////////////////////////////////////////////
 
  // 3.1. Set frequency
  error = Sigfox.setFrequency(frequency);
    
  // Check status
  if( error == 0 ) 
  {
    printf("Set frequency OK\n");
  }
  else 
  {
    printf("Set frequency ERROR\n");
  } 
  
  // 3.2. Get frequency
  error = Sigfox.getFrequency();
    
  // Check status
  if( error == 0 ) 
  {
    printf("Get frequency OK\n");
    printf("Frequency: %d\n", Sigfox._frequency);
  }
  else 
  {
    printf("Get frequency ERROR\n");
  } 
      
  
  //////////////////////////////////////////////
  // 4. Set/Get Power level
  //////////////////////////////////////////////
 
  // 4.1. Set power level
  error = Sigfox.setPowerLAN(power);
    
  // Check status
  if( error == 0 ) 
  {
    printf("Set power level OK\n");
  }
  else 
  {
    printf("Set power level ERROR\n");
  } 
  
  // 4.2. Get power level
  error = Sigfox.getPowerLAN();
    
  // Check status
  if( error == 0 ) 
  {
    printf("Get power level OK\n");
    printf("Power Level (dBm): %d\n", Sigfox._powerLAN);
  }
  else 
  {
    printf("Get power level ERROR\n");
  }

  printf("---------------------------\n");
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
