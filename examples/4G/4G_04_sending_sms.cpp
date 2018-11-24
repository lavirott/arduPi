/*  
 *  --------------- 4G_04 - Sending SMS   --------------- 
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

//////////////////////////////////////////////////
char phone_number[] = ""; //example.- "+34666555444"
char sms_body[] = "Hello from CookingHacks!!!";
//////////////////////////////////////////////////

uint8_t error;
uint8_t counter;


void setup()
{
}


void loop()
{

  //////////////////////////////////////////////////
  // 1. Switch on the 4G module
  //////////////////////////////////////////////////
  error = _4G.ON();

  if (error == 0)
  {
    printf("1. 4G module ready...\n");

    ////////////////////////////////////////////////
    // 2. Waiting for Connection
    ////////////////////////////////////////////////

    // Waiting for network registration
    counter = 3;
    error = _4G.checkConnection(60);
    while ((counter > 0) && (error != 0))
    {
        delay(5000);
        error = _4G.checkConnection(60);
        counter--;      
    }

    if (error == 0)
    {
        printf("2.1. Connected to network\n");
    }
    else
    {
        printf("2.1. Error connecting to network. Code: %d\n",error);
    } 

    ////////////////////////////////////////////////
    // 2. Configure SMS options
    ////////////////////////////////////////////////
    error = _4G.configureSMS();

    if (error == 0)
    {
      printf("2.1. 4G module configured to use SMS\n");		
    }
    else
    {
      printf("2.1. Error calling 'configureSMS' function. Code: %d\n", error);
    } 

    ////////////////////////////////////////////////
    // 3. Send SMS
    ////////////////////////////////////////////////
    printf("2.2. Sending SMS...\n");
    error = _4G.sendSMS( phone_number, sms_body);

    if (error == 0)
    {
      printf("done\n");	
    }
    else
    {
      printf("error. Code: %d\n", error);
    }    
  }
  else
  {
    // Problem with the communication with the 4G module
    printf("1. 4G module not started. Error code: %d\n", error);
  }

  ////////////////////////////////////////////////
  // 3. Powers off the 4G module
  ////////////////////////////////////////////////
  printf("3. Switch OFF 4G module\n");
  _4G.OFF();

  ////////////////////////////////////////////////
  // 4. Sleep
  ////////////////////////////////////////////////
  printf("4. Enter deep sleep...\n");

  delay(10000);
  
  printf("5. Wake up!!\n\n");
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








