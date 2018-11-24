/*
 *  ------------------------ 4G_11 - TCP client  --------------------------
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

char frame[] = "Node01-2016/10/6-Value_1";

// SERVER settings
///////////////////////////////////////
char host[] = "";
uint16_t remote_port = 1010;
uint16_t local_port  = 4000; 
///////////////////////////////////////

// define Socket ID (from 'SOCKET_1' to 'SOCKET_6')
///////////////////////////////////////
uint8_t socketId = arduPi4G::SOCKET_1;
///////////////////////////////////////

// define variables
uint8_t  error;
uint32_t previous;
uint8_t  socketIndex;


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
  // 1. Switch ON
  //////////////////////////////////////////////////
  error = _4G.ON();

  if (error == 0)
  {
    printf("1. 4G module ready...\n");

    ////////////////////////////////////////////////
    // Enter PIN code
    ////////////////////////////////////////////////

    /*
      printf("Enter PIN code...");
      if (_4G.enterPIN("****") == 1)
      {
      printf("PIN code accepted\n");
      }
      else
      {
      printf("PIN code incorrect\n");
      }
    */

    ////////////////////////////////////////////////
    // 2. TCP socket
    ////////////////////////////////////////////////

    error = _4G.openSocketClient(socketId, arduPi4G::TCP, host, remote_port);

    if (error == 0)
    {
      printf("2.1. Opening a socket... done!\n");

      printf("IP address:%s\n", _4G._ip);

      //////////////////////////////////////////////
      // 2.2. Send data
      //////////////////////////////////////////////

      error = _4G.send(socketId, (char*)"This is a test message from Cooking Hacks!\n\n");

      if (error == 0)
      {
        printf("2.2. Sending a string... done!\n");
      }
      else
      {
        printf("2.2. Error sending data. Code: %d\n", error);
      }

      //////////////////////////////////////////////
      // 2.3. Create a frame and send it through the connection
      //////////////////////////////////////////////

      error = _4G.send(socketId, (uint8_t*)frame, sizeof(frame));
      if (error == 0)
      {
        printf("2.3. Sending a frame... done!\n");
      }
      else
      {
        printf("2.3. Error sending a frame. Code: %d\n", error);
      }

      //////////////////////////////////////////////
      // 2.4. Receive data
      //////////////////////////////////////////////

      // Wait for incoming data from the socket (if the other side responds)
      printf("2.4. Waiting to receive data...\n");

      error = _4G.receive(socketId, 60000);

      if (error == 0)
      {
        if (_4G.socketInfo[socketId].size > 0)
        {
          printf("\n-----------------------------------\n");
          printf("Data received:\n %s\n", _4G._buffer);
          printf("-----------------------------------\n");
        }
        else
        {
          printf("NO data received\n");
        }
      }
      else
      {
        printf("No data received. Error code: %d\n", error);
      }
    }
    else
    {
      printf("2.1. Error opening socket. Error code: %d\n", error);
    }

    //////////////////////////////////////////////
    // 2.5. Close socket
    //////////////////////////////////////////////
    error = _4G.closeSocketClient(socketId);

    if (error == 0)
    {
      printf("2.5. Socket closed OK\n");
    }
    else
    {
      printf("2.5. Error closing socket. Error code: %d\n", error);
    }
  }
  else
  {
    // Problem with the communication with the 4G module
    printf("1. 4G module not started\n");
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
