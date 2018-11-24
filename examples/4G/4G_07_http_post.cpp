/*  
 *  --------------- 4G_07 - HTTP POST  --------------- 
 *  
 *  Explanation: This example shows how to send HTTP GET requests
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

// SERVER settings
///////////////////////////////////////
char host[] = "";
unsigned int port = 80;
char resource[] = "/test-get-post.php"; // as example
char data[] = "/test-get-post.php?varA=1&varB=2&varC=3&varD=4&varE=5&varF=6"\
     "&varG=7&varH=8&varI=9&varJ=10&varK=11&varL=12&varM=13&varN=14&varO=15"; // as example
///////////////////////////////////////

// variables
int error;
uint8_t counter;


void setup()
{
  printf("Start program\n");

  printf("********************************************************************\n");
  printf("POST method to the Libelium's test url\n");
  printf("You can use this php to test the HTTP connection of the module.\n");
  printf("The php returns the parameters that the user sends with the URL.\n");
  printf("********************************************************************\n");

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
    // 2. HTTP POST
    ////////////////////////////////////////////////

    printf("2. HTTP POST request...");

    // send the request
    counter = 3;
    error = _4G.http(arduPi4G::HTTP_POST, host, port, resource, data);
    while ((counter > 0) && (error != 0))
    {
        delay(5000);
        error = _4G.http(arduPi4G::HTTP_POST, host, port, resource, data);
        counter--;      
    }

    // check the answer
    if (error == 0)
    {
      printf("Done.\nHTTP code: %i\n", _4G._httpCode);
      printf("Server response: %s\n", _4G._buffer);
    }
    else
    {
      printf("Failed. Error code: %d\n", error);
    }
  }
  else
  {
    // Problem with the communication with the 4G module
    printf("4G module not started\n");
    printf("Error code: %d\n", error);
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
