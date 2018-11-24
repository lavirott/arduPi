/*  
 *  ------ 4G 02 - Getting module info -------- 
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

int temperature;
uint8_t error;


void setup()
{
}


void loop()
{
  printf("Start program\n");

  //////////////////////////////////////////////////
  // 1. Switch on the 4G module
  //////////////////////////////////////////////////
  error = _4G.ON();

  // check answer
  if (error == 0)
  {    
    printf("4G module ready\n");
    
    ////////////////////////////////////////////////
    // 1.1. Hardware revision 
    ////////////////////////////////////////////////   
    error = _4G.getInfo(arduPi4G::INFO_HW);
    if (error == 0)
    {
      printf("1.1. Hardware revision: %s\n", _4G._buffer);
    }

    ////////////////////////////////////////////////
    // 1.2. Manufacturer identification
    ////////////////////////////////////////////////
    error = _4G.getInfo(arduPi4G::INFO_MANUFACTURER_ID);
    if (error == 0)
    {
      printf("1.2. Manufacturer identification: %s\n", _4G._buffer);
    }
 
    ////////////////////////////////////////////////
    // 1.3. Model identification
    ////////////////////////////////////////////////
    error = _4G.getInfo(arduPi4G::INFO_MODEL_ID);
    if (error == 0)
    {
      printf("1.3. Model identification: %s\n", _4G._buffer);
    }

    ////////////////////////////////////////////////
    // 1.4. Revision identification
    ////////////////////////////////////////////////
    error = _4G.getInfo(arduPi4G::INFO_REV_ID);
    if (error == 0)
    {
      printf("1.4. Revision identification: %s\n", _4G._buffer);
    }

    ////////////////////////////////////////////////
    // 1.5. IMEI identification
    ////////////////////////////////////////////////
    error = _4G.getInfo(arduPi4G::INFO_IMEI);
    if (error == 0)
    {
      printf("1.5. IMEI: %s\n", _4G._buffer);
    }

    ////////////////////////////////////////////////
    // 1.6. IMSI
    ////////////////////////////////////////////////
    error = _4G.getInfo(arduPi4G::INFO_IMSI);
    if (error == 0)
    {
      printf("1.6. IMSI: %s\n", _4G._buffer);
    }

    ////////////////////////////////////////////////
    // 1.7. ICCID
    ////////////////////////////////////////////////
    error = _4G.getInfo(arduPi4G::INFO_ICCID);
    if (error == 0)
    {
      printf("1.7. ICCID: %s\n", _4G._buffer);
    }
    
    ////////////////////////////////////////////////
    // 1.8. Show APN settings
    ////////////////////////////////////////////////
    printf("1.8. Show APN:\n");
    _4G.show_APN();
    
    ////////////////////////////////////////////////
    // 1.9. Get temperature
    ////////////////////////////////////////////////
    error = _4G.getTemp();
    if (error == 0)
    {
      printf("1.9a. Temperature interval: %d\n", _4G._tempInterval);
      printf("1.9b. Temperature: %d Celsius degrees\n", _4G._temp);
    }
    else
    {
      
      printf("Error calling 'getTemp' function");
    }
  }
  else
  {
    // Problem with the communication with the 4G module
    printf("4G module not started");
  }

  _4G.OFF();

  delay(5000);
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

