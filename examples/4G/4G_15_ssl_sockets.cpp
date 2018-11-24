/*
 *  --- 4G_13 - SSL functions for TCP sockets  ---
 *
 *  Explanation: This example shows how to use the SSL commands so as
 *  to open a TCP client socket to the specified server address and port
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
char host[] = "";
uint16_t remote_port = 443;
///////////////////////////////////////

// define data to send through TCP socket
///////////////////////////////////////
char data[] =
  "GET /getpost_frame_parser.php?counter=1&varA=1&varB=2&varC=3&varE=4&varE=5 HTTP/1.1\r\n"\
  "Host: test.yourhost.com\r\n"\
  "Content-Length: 0\r\n\r\n"; // as example
///////////////////////////////////////

// define Socket ID (mandatory SOCKET_1)
///////////////////////////////////////
uint8_t socketId = arduPi4G::SOCKET_1;
///////////////////////////////////////

// define certificate for SSL
////////////////////////////////////////////////////////////////////////
char certificate[] =//
  "-----BEGIN CERTIFICATE-----\r"\
  "MIICGzCCAYQCCQCWP2pnP2wL+TANBgkqhkiG9w0BAQQFADBSMSwwKgYDVQQKDCNM\r"\
  "aWJlbGl1bSBDb211bmlhY2lvbmVzIERpc3RyaWJ1aWRhczELMAkGA1UEBhMCRVMx\r"\
  "FTATBgNVBAMMDGxpYmVsaXVtLmNvbTAeFw0xNTA3MDYxMTAyNDJaFw0yNTA3MDMx\r"\
  "MTAyNDJaMFIxLDAqBgNVBAoMI0xpYmVsaXVtIENvbXVuaWFjaW9uZXMgRGlzdHJp\r"\
  "YnVpZGFzMQswCQYDVQQGEwJFUzEVMBMGA1UEAwwMbGliZWxpdW0uY29tMIGfMA0G\r"\
  "CSqGSIb3DQEBAQUAA4GNADCBiQKBgQCznwc4Rt6HF4CumZqDkMPL9Wn73kyoVDiT\r"\
  "kTST6Gj8IUsqnwftnu959Oqlow5X80foNu/o88zj8bbrSpXsaqaD9Wjt2zDJkdSL\r"\
  "42uElTDY+BuyUEY84L3JJ7InDPSyduayhXlKquNlhjP1SGX/q8WNVPzL05Sw1pPR\r"\
  "QPSt0ow82wIDAQABMA0GCSqGSIb3DQEBBAUAA4GBALEtLedkIjtsCHXxEZeuUA2t\r"\
  "DhIPBt2rIUUKOjcdOtC0AeQzalX1ln279KOoD86NShCRMkKl24SCFgXwQ0e8TcQ3\r"\
  "9Le1A24vGmZtJvc+MFh1bS/b2KmHYOj0ie8QmHBSMVxMIc/opFy3BAmLC9V/90hj\r"\
  "BziFIf5Ff7pZvzBoimQi\r"\
  "-----END CERTIFICATE-----";
////////////////////////////////////////////////////////////////////////

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
      printf("Setting PIN code...\n");
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
    // 2. Set CA certificate
    ////////////////////////////////////////////////

    error = _4G.manageSSL(socketId, arduPi4G::SSL_ACTION_STORE, arduPi4G::SSL_TYPE_CA_CERT, certificate);

    if (error == 0)
    {
      printf("2. Set CA certificate OK\n");
    }
    else
    {
      printf("2. Error setting CA certificate\n");
    }
  }
}

void loop()
{

  //////////////////////////////////////////////////
  // 1. Switch ON
  //////////////////////////////////////////////////
  error = _4G.ON();

  if (error == 0)
  {
    ////////////////////////////////////////////////
    // Enter PIN code
    ////////////////////////////////////////////////

    /*
      printf("Setting PIN code...\n");
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
    // 2. SSL socket
    ////////////////////////////////////////////////

    error = _4G.openSocketSSL(socketId, host, remote_port);

    if (error == 0)
    {
      printf("2.1. Opening a socket... done!\n");

      //////////////////////////////////////////////
      // 2.2. Send data through socket
      //////////////////////////////////////////////
      
      error = _4G.sendSSL(socketId, data);
      if (error == 0)
      {
        printf("2.2. Sending data... done!\n");
      }
      else
      {
        printf("2.2. Error sending data. Code: %d\n", error);
      }

      //////////////////////////////////////////////
      // 2.3. Receive data
      //////////////////////////////////////////////

      // Wait for incoming data from the socket (if the other side responds)
      printf("2.3. Waiting to receive data...\n");

      error = _4G.receiveSSL(socketId, 60000);

      if (error == 0)
      {
        if (_4G._length > 0)
        {
          printf("\n-----------------------------------\n");
          printf("Data received: %s\n", _4G._buffer);
          printf("-----------------------------------\n");
        }
        else
        {
          printf("NO Data received\n");
        }
      }
      else
      {
        printf("No data received. Error: %d\n", error);
      }

      //////////////////////////////////////////////
      // 2.4. Close socket
      //////////////////////////////////////////////
      error = _4G.closeSocketSSL(socketId);

      if (error == 0)
      {
        printf("2.4. Socket closed OK\n");
      }
      else
      {
        printf("2.4. Error closing socket. Error code: %d\n", error);
      }
      
    }
    else
    {
      printf("2.1. Error opening socket. Error code: %d\n", error);
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
