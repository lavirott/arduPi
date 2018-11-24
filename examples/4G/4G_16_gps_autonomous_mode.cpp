/*  
 *  --------------- 4G_14 - GPS Autonomous mode  ---------------
 *
 *  Explanation: This example shows how to use de GPS in autonomous mode
 *
 *  Notes: 
 *   - To work poperly, GPS module needs a 2A(at least) power supply.
 *   - GPS antennae needs direct skyview to fix satellites; work outdoor.
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
uint8_t error;
uint8_t gps_status;
float gps_latitude;
float gps_longitude;


void setup()
{
  printf("Start program\n");

  //////////////////////////////////////////////////
  // 1. Switch on the 4G module
  //////////////////////////////////////////////////
  error = _4G.ON();

  // check answer
  if (error == 0)
  {
    printf("1. 4G module ready...\n");

    ////////////////////////////////////////////////
    // 2. Start GPS feature
    ////////////////////////////////////////////////

    gps_status = _4G.gpsStart();

    // check answer
    if (gps_status == 0)
    {
      printf("2. Starting GPS in autonomous mode... done!\n");
    }
    else
    {
      printf("2. Error calling the 'startGPS' function. Code: %d\n", gps_status);
    }

  }
  else
  {
    // Problem with the communication with the 4G module
    printf("1. 4G module not started\n");
    printf("Error code: %d\n", error);
    printf("The code stops here.\n");
    while (1);
  }
}


void loop()
{
  ////////////////////////////////////////////////
  // 3. Wait for satellite signals and get values
  ////////////////////////////////////////////////
  if (gps_status == 0)
  {
    error = _4G.waitForSignal(20000);
    if (error == 0)
    {
      printf("----------------------------\n");
      printf("Latitude: %s, LatitudeNS: %c, Longitude: %s, LongitudeEW: %c, "\
             "UTC_time:%s, date:%s, Number of satellites: %u, HDOP: %f\n",
              _4G._latitude, 
              _4G._latitudeNS, 
              _4G._longitude, 
              _4G._longitudeEW, 
              _4G._time, 
              _4G._date, 
              _4G._numSatellites, 
              _4G._hdop);
      printf("----------------------------\n");

      // get degrees
      gps_latitude  = _4G.convert2Degrees(_4G._latitude, _4G._latitudeNS);
      gps_longitude = _4G.convert2Degrees(_4G._longitude, _4G._longitudeEW);
      
      printf("Conversion to degrees:\n");
      printf("Latitude: %f\n", gps_latitude);
      printf("Longitude: %f\n\n", gps_longitude);
      delay(10000);
    }
    else
    {
      printf("no satellites fixed\n");
    }
  }
  else
  {
    printf("Restarting the GPS engine\n");
    _4G.gpsStop();

    delay(1000);

    gps_status = _4G.gpsStart();
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
