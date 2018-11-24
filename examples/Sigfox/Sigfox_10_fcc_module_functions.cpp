/*
 *  Sigfox Code Example
 *
 *  NOTE: This example is only for TD1508 - Sigfox_900 modules.
 *
 *  Explanation: This example shows how to set/get different FCC parameters
 *
 *  Copyright (C) 2016 Libelium Comunicaciones Distribuidas S.L.
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
uint8_t sock = SOCKET0;
//////////////////////////////////////////////

uint8_t error;

// "**********************************************"
// "| This code only works with Sigfox900 module |"
// "**********************************************"

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
  // show Region
  //////////////////////////////////////////////
  error = Sigfox.getRegion();

  // Check status
  if (error == 0)
  {
    printf("Get region OK: ");

    switch (Sigfox._region)
    {
      case SIGFOX_REGION_ETSI:    printf("ETSI region\n");    break;
      case SIGFOX_REGION_FCC:     printf("FCC region\n");     break;
      case SIGFOX_REGION_ARIB:    printf("ARIB region\n");    break;
      case SIGFOX_REGION_UNKNOWN:
      default:                    printf("Unknown region\n"); break;
    }
  }
  else
  {
    printf("Get region: ERROR\n");
  }

  //////////////////////////////////////////////
  // set Macro Channel Bitmask
  //////////////////////////////////////////////
  error = Sigfox.setMacroChannelBitmask((char *)"000001FF0000000000000000");

  // Check status
  if (error == 0)
  {
    printf("Set Macro channel bitmask OK\n");
  }
  else
  {
    printf("Set Macro channel bitmask: ERROR\n");
  }

  //////////////////////////////////////////////
  // get Macro Channel Bitmask
  //////////////////////////////////////////////
  error = Sigfox.getMacroChannelBitmask();

  // Check status
  if (error == 0)
  {
    printf("Get Macro channel bitmask OK: %s\n", Sigfox._macroChannelBitmask);
  }
  else
  {
    printf("Get Macro channel bitmask: ERROR\n");
  }

  //////////////////////////////////////////////
  // set Macro Channel 
  //////////////////////////////////////////////
  error = Sigfox.setMacroChannel(1);

  // Check status
  if (error == 0)
  {
    printf("Set Macro channel OK\n");
  }
  else
  {
    printf("Set Macro channel: ERROR\n");
  }

  //////////////////////////////////////////////
  // get Macro Channel
  //////////////////////////////////////////////
  error = Sigfox.getMacroChannel();

  // Check status
  if (error == 0)
  {
    printf("Get Macro channel OK: %u\n", Sigfox._macroChannel);
  }
  else
  {
    printf("Get Macro channel: ERROR\n");
  }

  //////////////////////////////////////////////
  // set Downlink Frequency Offset
  //////////////////////////////////////////////
  error = Sigfox.setDownFreqOffset(3000000);

  // Check status
  if (error == 0)
  {
    printf("Set Downlink Frequency Offset OK\n");
  }
  else
  {
    printf("Set Downlink Frequency Offset: ERROR\n");
  }

  //////////////////////////////////////////////
  // get Downlink Frequency Offset
  //////////////////////////////////////////////
  error = Sigfox.getDownFreqOffset();

  // Check status
  if (error == 0)
  {
    printf("Get Downlink Frequency Offset: %u\n", Sigfox._downFreqOffset);
  }
  else
  {
    printf("Get Downlink Frequency Offset: ERROR\n");
  }

  printf("-------------------------------\n");

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
