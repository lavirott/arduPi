/*
 *   ----------------------- 4G_12 - TCP server  -----------------------
 *
 *  Explanation: This example shows how to open a TCP listening socket
 *  and waits for incoming connections. After correctly connecting to a
 *  TCP client, sending/receiving functions are shown
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

// SERVER settings
///////////////////////////////////////
uint16_t local_port = 500;
uint8_t keep_alive  = 240; // From 0 (disabled) to 240 minutes
///////////////////////////////////////

// define Socket ID (from 'SOCKET_1' to 'SOCKET_6')
///////////////////////////////////////
uint8_t socketId = arduPi4G::SOCKET_1;
///////////////////////////////////////

// define variables
uint8_t  error;
uint32_t previous;
uint8_t  socketIndex;
boolean  tcp_server_status = false;
uint8_t  socket_state;


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

  //////////////////////////////////////////////////
  // 3. Switch ON
  //////////////////////////////////////////////////
  error = _4G.ON();

  if (error == 0)
  {
    printf("1. 4G module ready...\n");

    ////////////////////////////////////////////////
    // Enter PIN code
    ////////////////////////////////////////////////

    /*
      printf(F("Setting PIN code..."));
      if (_4G.enterPIN("****") == 1)
      {
      printf(F("PIN code accepted"));
      }
      else
      {
      printf(F("PIN code incorrect"));
      }
    */

  }
  else
  {
    // Problem with the communication with the 4G module
    printf("4G module not started");
  }
}


void loop()
{

  ///////////////////////////////////////////////////////
  // 1. Open TCP Listen on defined port
  ///////////////////////////////////////////////////////

  error = _4G.openSocketServer(socketId, arduPi4G::TCP, local_port, keep_alive);

  if (error == 0)
  {
    printf("1. Socket open\n");
    printf("---------------------------\n");
    printf("IP address: %s\n", _4G._ip);
    printf("Local port: %d\n", local_port);
    printf("----------------------------\n");

    // update flag
    tcp_server_status = true;
  }
  else
  {
    printf("1. Error opening socket. Error code: %d\n", error);

    // update flag
    tcp_server_status = false;
  }

  ///////////////////////////////////////////////////////
  // 2. Wait for incoming connection from TCP client
  ///////////////////////////////////////////////////////

  printf("2. Wait for incoming connection (30 secs): \n");

  error = _4G.manageSockets(30000);

  if (error == 0)
  {
    if (_4G.socketStatus[socketId].state == arduPi4G::STATUS_SUSPENDED)
    {
      printf("Data connection in socket:\n");
      printf("-------------------------------------\n");
      printf("Socket ID: %d\n", _4G.socketStatus[socketId].id);
      printf("Socket State: %d\n", _4G.socketStatus[socketId].state);
      printf("Socket Local IP: %d\n", _4G.socketStatus[socketId].localIp);
      printf("Socket Local Port: %d\n", _4G.socketStatus[socketId].localPort);
      printf("Socket Remote IP: %d\n", _4G.socketStatus[socketId].remoteIp);
      printf("Socket Remote Port: %d\n", _4G.socketStatus[socketId].remotePort);
      printf("-------------------------------------\n");

      // update flag
      tcp_server_status = true;
    }
    else
    {
      printf("3. Incorrect Socket State: %d\n",_4G.socketStatus[socketId].state);

      // update flag
      tcp_server_status = false;
    }
  }
  else
  {
    printf("3. No incoming data. Code: %d\n", error);

    // update flag
    tcp_server_status = false;
  }

  ///////////////////////////////////////////////////////
  // 3. Loop while connected
  ///////////////////////////////////////////////////////

  while (tcp_server_status == true)
  {
    //////////////////////////////////////////////
    // 3.1. Get socket info
    //////////////////////////////////////////////
    error = _4G.getSocketInfo(socketId);

    if (error == 0)
    {
      printf("3.1. Socket Info since it was opened:\n");
      printf("-------------------------------------\n");
      printf("Socket ID: %d\n", _4G.socketInfo[socketId].id);
      printf("Socket Sent bytes: %d\n", _4G.socketInfo[socketId].sent);
      printf("Socket Received bytes: %d\n", _4G.socketInfo[socketId].received);
      printf("Socket pending bytes read: %d\n", _4G.socketInfo[socketId].size);
      printf("Socket bytes sent and not yet acked: %d\n", _4G.socketInfo[socketId].ack);
      printf("-------------------------------------\n");
    }
    else
    {
      printf("3.1. Error getting socket info. Erro code: %d\n", error);
    }

    //////////////////////////////////////////////
    // 3.2. Send data
    //////////////////////////////////////////////
    error = _4G.send(socketId, (char *)"This is a message from Cooking Hacks TCP server\n");

    if (error == 0)
    {
      printf("3.2. Data sent via TCP socket\n");
    }
    else
    {
      printf("3.2. Error sending data via TCP socket\n");
    }

    //////////////////////////////////////////////
    // 3.3. Receive data
    //////////////////////////////////////////////

    // Wait for incoming data from the socket (if the other side responds)
    printf("3.3. Waiting to receive data (30 secs): \n");

    error = _4G.receive(socketId, 30000);

    if (error == 0)
    {
      if (_4G.socketInfo[socketId].size > 0)
      {
        printf("\nData received:\n");
        printf("=====================================\n");
        printf("%s\n", _4G._buffer);
        printf("=====================================\n");
      }
      else
      {
        printf("NO data received\n");
      }
    }
    else
    {
      printf("No data received, Error code %d\n", error);
    }

    //////////////////////////////////////////////
    // 3.4. Check Socket Listen status
    /////////////////////////////////////////////
    error = _4G.getSocketStatus(socketId);

    if (error == 0)
    {
      // get state
      socket_state = _4G.socketStatus[socketId].state;
      
      printf("3.4. Get socket status OK: %d\n", socket_state);

      // check socket status
      if (socket_state == arduPi4G::STATUS_CLOSED)
      {
        printf("SOCKET CLOSED\n");

        // update flag
        tcp_server_status = false;
      }
    }
    else
    {
      printf("3.4. Error getting socket status. Error code: %d\n", error);

      // update flag
      tcp_server_status = false;
    }
  }

  ///////////////////////////////////////////////////////
  // 4. Close socket
  ///////////////////////////////////////////////////////
  error = _4G.closeSocketServer(socketId, arduPi4G::TCP);

  if (error == 0)
  {
    printf("4. Socket closed OK\n");
  }
  else
  {
    printf("4. Error closing socket. Error code: %d\n", error);
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
