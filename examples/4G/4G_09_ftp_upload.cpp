/*
 *  --- 4G_09 - Uploading files to a FTP server  ---
 *
 *  Explanation: This example shows how to upload a file to a FTP server
 *  from Waspmote
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
char SD_FILE[]     = "./FTP_FILES/SENT.TXT";
char SERVER_FILE[] = "COOKING.TXT";
///////////////////////////////////////////////////////////////////////

// define variables
int error;
uint32_t previous;
uint8_t sd_answer;


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
    // 2.1. FTP open session
    ////////////////////////////////////////////////

    error = _4G.ftpOpenSession(ftp_server, ftp_port, ftp_user, ftp_pass);

    // check answer
    if (error == 0)
    {
      printf("2.1. FTP open session OK\n");

      previous = millis();

      //////////////////////////////////////////////
      // 2.2. FTP upload
      //////////////////////////////////////////////
      
      error = _4G.ftpUpload(SERVER_FILE, SD_FILE);

      if (error == 0)
      {

        printf("2.2. Uploading SD file to FTP server done! \n");
        printf("Upload time: %d s\n",(millis() - previous) / 1000);
      }
      else
      {
        printf("2.2. Error calling 'ftpUpload' function. Error: %d\n", error);
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
        printf("CMEE error: %d\n", _4G._errorCode);
      }
    }
    else
    {
      printf( "2.1. FTP connection error: %d\n", error);
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
