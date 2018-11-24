/*  
 *  ---  4G_03 - Getting network information --- 
 *  
 *  Explanation: This example shows how to 
 *  
 *  Copyright (C) Libelium Comunicaciones Distribuidas S.L.
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
 *  along with this program.  If not, see http://www.gnu.org/licenses/.
 *
 *  Version:           1.2
 *  Design:            David Gascón
 *  Implementation:    Alejandro Gállego, Yuri Carmona, Luis Miguel Marti
 *  Port to Raspberry: Ruben Martin
 */

#include "arduPi4G.h"

// APN settings
///////////////////////////////////////
char apn[] = "";
char login[] = "";
char password[] = "";
///////////////////////////////////////

uint8_t connection_status;
char operator_name[20];

uint8_t error;


void setup()
{    
  printf("Start program\n");

  //////////////////////////////////////////////////
  // 1. sets operator parameters
  //////////////////////////////////////////////////
  _4G.set_APN(apn, login, password);

  //////////////////////////////////////////////////
  // 2. Show APN settings via USB port
  //////////////////////////////////////////////////
  _4G.show_APN();
}


void loop()
{

  //////////////////////////////////////////////////
  // 1. Switch ON the 4G module
  //////////////////////////////////////////////////
  error = _4G.ON();
  
  if (error == 0)
  {
    printf("1. 4G module ready\n"); 

    ////////////////////////////////////////////////
    // 1.1. Check connection to network and continue
    ////////////////////////////////////////////////
    connection_status = _4G.checkDataConnection(30);
    if (connection_status == 0)
    {
      printf("1.1. Module connected to network\n");

      //////////////////////////////////////////////
      // 1.2. Get RSSI
      //////////////////////////////////////////////
      error = _4G.getRSSI();
      if (error == 0)
      {
        printf("1.2. RSSI: %ddBm\n", _4G._rssi);
      }
      else
      {
        printf("1.2. Error calling 'getRSSI' function\n");
      }

      //////////////////////////////////////////////
      // 1.3. Get Network Type
      //////////////////////////////////////////////
      error = _4G.getNetworkType();

      if (error == 0)
      {
        printf("1.3. Network type: ");
        switch (_4G._networkType)
        {
        case 0:
          printf("GPRS\n");
          break;
        case 1:
          printf("EGPRS\n");
          break;
        case 2:
          printf("WCDMA\n");
          break;
        case 3:
          printf("HSDPA\n");
          break;
        case 4:
          printf("LTE\n");
          break;
        case 5:
          printf("Unknown or not registered\n");
          break;				
        }
      }
      else
      {
        printf("1.3. Error calling 'getNetworkType' function");
      }

      //////////////////////////////////////////////
      // 1.4. Get Operator name
      //////////////////////////////////////////////
      memset(operator_name, '\0', sizeof(operator_name));
      error = _4G.getOperator(operator_name);

      if (error == 0)
      {
        printf("1.4. Operator: %s\n", operator_name);
      }
      else
      {
        printf("1.4. Error calling 'getOperator' function\n");
      }

      //////////////////////////////////////////////
      // 1.5. Show RTC time
      //////////////////////////////////////////////
      printf("1.5. RTC Time: \n");
      
      error = _4G.showTimeFrom4G();

      if (error == 0)
      {
        printf("1.5. Time Show OK\n");
      }
      else
      {
        printf("1.5. Error calling 'showTimeFrom4G' function\n");
      }
    }
  }
  else
  {
    // Problem with the communication with the 4G module
    printf("4G module not started\n");
    printf("Error code: %d\n");
 }

  //////////////////////////////////////////////////
  // 2. Switch OFF the 4G module
  //////////////////////////////////////////////////
  _4G.OFF();
  printf("2. Switch OFF 4G module\n");

  //////////////////////////////////////////////////
  // 3. Sleep
  //////////////////////////////////////////////////
  printf("3. Enter deep sleep...\n");
  
  delay(10000);
   
  printf("4. Wake up!!\n\n");

}


//////////////////////////////////////////////
// Main loop setup() and loop() declarations
//////////////////////////////////////////////
int main()
{
    setup();
    while(1) loop();
    return (0);
}
//////////////////////////////////////////////
