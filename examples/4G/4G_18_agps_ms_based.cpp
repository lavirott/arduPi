/*
 *  --------------- 4G_18 - A-GPS (MS-Based GPS)  ---------------
 *
 *  Explanation: This example shows how to use de A-GPS in MS-Based mode
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

// define variables
uint8_t error;
uint8_t gps_status;
float gps_latitude;
float gps_longitude;
uint32_t previous;
bool gps_autonomous_needed = true;


void setup()
{
  printf("Start program\n");

  //////////////////////////////////////////////////
  // Set operator parameters
  //////////////////////////////////////////////////
  _4G.set_APN(apn, login, password);

  //////////////////////////////////////////////////
  // Show APN settings via USB port
  //////////////////////////////////////////////////
  _4G.show_APN();

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

    // get current time
    previous = millis();

    gps_status = _4G.gpsStart(arduPi4G::GPS_MS_BASED);

    // check answer
    if (gps_status == 0)
    {
      printf("2. GPS started in MS-BASED. Time(secs) = %d\n", (millis()-previous)/1000);
    }
    else
    {
      printf("2. Error calling the 'gpsStart' function. Code: %d\n", gps_status);
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
  // Wait for satellite signals and get values
  ////////////////////////////////////////////////
  if (gps_status == 0)
  {
    error = _4G.waitForSignal(20000);

    if (error == 0)
    {
      printf("3. GPS signal received. Time(secs) = %d\n", (millis()-previous)/1000);

      printf("Acquired position:\n");
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

      ////////////////////////////////////////////////
      // Change to AUTONOMOUS mode if needed
      ////////////////////////////////////////////////
      if (gps_autonomous_needed == true)
      {
        _4G.gpsStop();

        gps_status = _4G.gpsStart(arduPi4G::GPS_AUTONOMOUS);

        // check answer
        if (gps_status == 0)
        {
          printf("GPS started in AUTONOMOUS mode\n");

          // update variable
          gps_autonomous_needed = false;
        }
        else
        {
          printf("Error calling the 'gpsStart' function. Code: %d\n", gps_status);
        }
      }

      delay(10000);
    }
    else
    {
      printf("no satellites fixed. Error: %d\n", error);
    }
  }
  else
  {
    ////////////////////////////////////////////////
    // Restart GPS feature
    ////////////////////////////////////////////////
    printf("Restarting the GPS engine\n");

    // stop GPS
    _4G.gpsStop();
    delay(1000);

    // start GPS
    gps_status = _4G.gpsStart(arduPi4G::GPS_MS_BASED);

    // check answer
    if (gps_status == 0)
    {
      printf("GPS started in MS-BASED. Time(ms) = %d\n", millis() - previous);
    }
    else
    {
      printf("Error calling the 'gpsStart' function. Code: %d\n", gps_status);
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
