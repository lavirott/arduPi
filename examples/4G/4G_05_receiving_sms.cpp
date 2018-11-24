/*  
 *  --------------- 4G_03 - Receiving SMS  --------------- 
 *  
 *  Explanation: This example shows how to set up the module to use
 *  SMS and receive text messages
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
 *  Version:           1.1
 *  Design:            David Gascón
 *  Implementation:    Alejandro Gállego, Yuri Carmona, Luis Miguel Marti
 *  Port to Raspberry: Ruben Martin
 */

#include "arduPi4G.h"

// define variables
uint8_t error;
char sms_received[150];


void setup()
{    
  
  printf("Start program\n");
  
  //////////////////////////////////////////////////
  // 1. Switch on the 4G module
  //////////////////////////////////////////////////
  error = _4G.ON();

  if (error == 0)
  {
    printf("1. 4G module ready...\n");

    ////////////////////////////////////////////////
    // 2. Set PIN code
    ////////////////////////////////////////////////  
    error = _4G.enterPIN((char *)"");

    if (error == 1) 
    {
      printf("2. PIN code accepted\n");
    }
    else
    {
      printf("2. PIN code incorrect\n");
    }

    ////////////////////////////////////////////////
    // 2.2. Configure SMS options
    ////////////////////////////////////////////////
    error = _4G.checkConnection(60);

    if (error == 0)
    {
      printf("2. Module connected to network\n");
    }
    else
    {
      printf("2. Module not connected. Connection status: %d\n", error);
    }

    ////////////////////////////////////////////////
    // 2.3. Configure SMS options
    ////////////////////////////////////////////////
    error = _4G.configureSMS();

    if (error == 0)
    {
      printf("4. 4G module configured to use SMS\n");
    }
    else
    {
      printf("4. Error calling 'configureSMS' function. Code: %d\n", error);
    } 
  }
  else
  {
    // Problem with the communication with the 4G module
    printf("4G module not started\n");
    printf("Error code: %d\n", error);
    printf("The code stops here.\n");
    while(1);
  }
}


void loop()
{

  ////////////////////////////////////////////////
  // Check connection to network
  ////////////////////////////////////////////////
  error = _4G.checkConnection(60);

  if (error == 0)
  {
    printf("Wait for incoming SMS...\n");

    //////////////////////////////////////////////
    // Manage incoming data
    //////////////////////////////////////////////
    error = _4G.manageIncomingData(60000);

    if ((error == 0) && (_4G._incomingType == LE910_INCOMING_SMS))
    {     
      printf("SMS index: %d\n", error);

      // clear buffer
      memset(sms_received, '\0', sizeof(sms_received));
      
      // Read incoming SMS
      error = _4G.readSMS(_4G._smsIndex, sms_received);

      if (error == 0)
      {
        printf("SMS body: \n%s\n",sms_received);
      }
      else
      {
        printf("Error reading the sms. Code: %d\n", error);
      }
    }
  }
  else
  {
    printf("Waiting connection. Connection status: %d\n", error);
  }
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
