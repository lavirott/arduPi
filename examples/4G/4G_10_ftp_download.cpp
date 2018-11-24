/*
 *  --- 4G_10 - Downloading files to a FTP server  ---
 *
 *  Explanation: This example shows how to download a file from a FTP
 *  server to Waspmote SD card
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
 *  along with this program.  If not, see http://www.gnu.org/licenses/.
 *
 *  Version:           1.2
 *  Design:            David Gascón
 *  Implementation:    Alejandro Gállego, Yuri Carmona, Luis Miguel Marti
 *  Port to Raspberry: Ruben Martin
 */

#include <arduPi4G.h>

// APN settings
///////////////////////////////////////
char apn[] = "";
char login[] = "";
char password[] = "";
///////////////////////////////////////

// SERVER settings
///////////////////////////////////////
char ftp_server[] = "";
uint16_t ftp_port = 21;
char ftp_user[] = "";
char ftp_pass[] = "";
///////////////////////////////////////

///////////////////////////////////////////////////////////////////////
// Define filenames for SD card and FTP server:
//  - If the file is in the root directory: "FILE.TXT" or "/FILE.TXT"
//  - If the file is inside a folder: "/FOLDER/FILE.TXT"
///////////////////////////////////////////////////////////////////////
char SD_FILE[] = "./FTP_FILES/RECEIVED.TXT";
char SERVER_FILE[] = "COOKING.TXT";
///////////////////////////////////////////////////////////////////////

int error;
uint32_t previous;


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
    printf("Enter PIN code...\n");
   
    if (_4G.enterPIN((char *)"") == 1)
    {
      printf("PIN code accepted\n");
    }
    else
    {
      printf("PIN code incorrect\n");
    }
    
    ////////////////////////////////////////////////
    // 2.1. FTP open session
    ////////////////////////////////////////////////

    error = _4G.ftpOpenSession(ftp_server, ftp_port, ftp_user, ftp_pass);

    if (error == 0)
    {
      printf("2.1. FTP open session OK\n");

      previous = millis();

      //////////////////////////////////////////////
      // 2.2. FTP download
      //////////////////////////////////////////////
      error = _4G.ftpDownload(SD_FILE, SERVER_FILE);

      if (error == 0)
      {

        printf("2.2. Download SD file from FTP server done! ");
        printf("Download time: %d s\n", (millis() - previous) / 1000);
      }
      else
      {
        printf("2.2. Error calling 'ftpDownload' function. Error: %d\n", error);
      }

      //////////////////////////////////////////////
      // 2.3. FTP close session
      //////////////////////////////////////////////

      error = _4G.ftpCloseSession();

      if (error == 0)
      {
        printf("2.3. FTP close session OK\n");
      }
      else
      {
        printf("2.3. Error calling 'ftpCloseSession' function. error: %d\n", error);
      }
    }
    else
    {
      printf( "FTP connection error: %d\n", error);
    }
  }
  else
  {
    // Problem with the communication with the 4G module
    printf("4G module not started\n");
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
