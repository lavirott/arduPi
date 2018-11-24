/*  
 *  --------------- 4G_01 - Setting PIN codes  --------------- 
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

// define variables
uint8_t PIN_status;
char PIN_code[30];
uint8_t error;
int counter;
int temp;
uint32_t previous;


void setup()
{    
  printf("******************************************************************************\n");
  printf("This example inits the 4G module and request the unlock codes if necessary\n");
  printf("******************************************************************************\n");

  //////////////////////////////////////////////////
  // 1. Switch on the 4G module
  //////////////////////////////////////////////////
  previous = millis();
  error = _4G.ON();
  if (error == 0)  
  {
    printf("1. 4G module ready in ");
    printf("%i", millis()-previous);
    printf("ms\n");
  }
  else
  {
    // Problem with the communication with the 4G module
    printf("1. 4G module not started\n");
    printf("Error code: %d\n", error);
  }
}


void loop()
{

  //////////////////////////////////////////////////
  // 2. Check PIN code
  //////////////////////////////////////////////////
  printf("2. Reading code...");
  PIN_status = _4G.checkPIN();
  
  
  switch (PIN_status)
  {
  case 0:
    printf("SIM and module unlocked. Ready to use\n");
    printf("The sketch will stop here\n");
    while(1);
    break;

  case 1:
    printf("LE910 is awaiting SIM PIN.\n");
    printf("Please, enter the code:\n");
    scanf("%s\n", PIN_code);
    break;

  case 2:
    printf("LE910 is awaiting SIM PUK\n");
    printf("Please, enter the code: \n");
    scanf("%s\n", PIN_code);
    break;

  case 3:
    printf("LE910 is awaiting phone-to-SIM card password.\n");
    printf("Please, enter the code: \n");
    scanf("%s\n", PIN_code);
    break;

  case 4:
    printf("LE910 is awaiting phone-to-very-first-SIM card password.\n");
    printf("Please, enter the code: \n");
    scanf("%s\n", PIN_code);
    break;

  case 5:
    printf("LE910 is awaiting phone-to-very-first-SIM card unblocking password.\n");
    printf("Please, enter the code: \n");
    scanf("%s\n", PIN_code);
    break;

  case 6:
    printf("LE910 is awaiting SIM PIN2\n");
    printf("Please, enter the code: \n");
    scanf("%s\n", PIN_code);
    break;

  case 7:
    printf("LE910 is awaiting SIM PUK2\n");
    printf("Please, enter the code: \n");
    scanf("%s\n", PIN_code);
    break;

  case 8:
    printf("LE910 is awaiting network personalization password\n");
    printf("Please, enter the code: \n");
    scanf("%s\n", PIN_code);
    break;

  case 9:
    printf("LE910 is awaiting network personalization unblocking password\n");
    printf("Please, enter the code: \n");
    scanf("%s\n", PIN_code);
    break;

  case 10:
    printf("LE910 is awaiting network subset personalization password\n");
    printf("Please, enter the code: \n");
    scanf("%s\n", PIN_code);
    break;

  case 11:
    printf("LE910 is awaiting network subset personalization unblocking password\n");
    printf("Please, enter the code: \n");
    scanf("%s\n", PIN_code);
    break;

  case 12:
    printf("LE910 is awaiting service provider personalization password\n");
    printf("Please, enter the code: \n");
    scanf("%s\n", PIN_code);
    break;

  case 13:
    printf("LE910 is awaiting service provider personalization unblocking password\n");
    printf("Please, enter the code: \n");
    scanf("%s\n", PIN_code);
    break;

  case 14:
    printf("LE910 is awaiting corporate personalization password\n");
    printf("Please, enter the code: \n");
    scanf("%s\n", PIN_code);
    break;

  case 15:
    printf("LE910 is awaiting corporate personalization unblocking password\n");
    printf("Please, enter the code: \n");
    scanf("%s\n", PIN_code);
    break;

  default: 
    printf("Error code: %d\n", PIN_status);
    break;
  }

  //////////////////////////////////////////////////
  // 3. Set PIN code
  //////////////////////////////////////////////////
  if ((PIN_status != 0) && (PIN_status < 16))
  {
    printf("3. Entering PIN code...\n");
    if (_4G.enterPIN(PIN_code) == 0) 
    {
      printf("3. done\n");
    }
    else
    {
      printf("3. error\n");
    }
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
