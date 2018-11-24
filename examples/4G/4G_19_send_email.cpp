/*
    --------------- 4G_19 - Sending e-mail ---------------

    Explanation: This example shows how to set up the module to send
    e-mails

    Copyright (C) 2016 Libelium Comunicaciones Distribuidas S.L.
    http://www.libelium.com

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    Version:           1.2
    Design:            David Gascón
    Implementation:    Luis Miguel Martí
*/

#include "arduPi4G.h"

// APN settings
///////////////////////////////////////
char apn[] = "m2m.tele2.com";
char login[] = "";
char password[] = "";
///////////////////////////////////////


// E-mail sender parameters
//////////////////////////////////////////////////
char sender_address[] = "test1@libelium.com";
char sender_user[] = "test1@libelium.com";
char sender_password[] = "eHr73j";
//////////////////////////////////////////////////

// E-mail SMTP parameters
//////////////////////////////////////////////////
char smtp_server[] = "smtp.libelium.com";
uint8_t smtp_security = arduPi4G::EMAIL_NONSSL;
uint16_t smtp_port = 25;
//////////////////////////////////////////////////

// E-mail to send
//////////////////////////////////////////////////
char receiver_address[] = "r.martin@libelium.com";
char subject[] = "Subject of email"; // maximum: 100 bytes
char message[] = "This is an e-mail message from Cooking";
//////////////////////////////////////////////////

// variable
uint8_t error;


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

  if (error == 0)
  {
    printf("1. 4G module ready...\n");

    ////////////////////////////////////////////////
    // 1.1. Reset e-mail parameters
    ////////////////////////////////////////////////
        
    error = _4G.emailReset();

    if (error == 0)
    {
      printf("1.1. Reset e-mail parameters OK\n");
    }
    else
    {
      printf("1.1. Error reset configuration. Code: %d\n", error);
    }

    ////////////////////////////////////////////////
    // 1.2. Set SMTP server
    ////////////////////////////////////////////////
    error = _4G.emailSetServerSMTP(smtp_server);

    if (error == 0)
    {
      printf("1.2. SMTP server set OK\n");
    }
    else
    {
      printf("1.2. Error set server. Code: %d\n", error);
    }

    ////////////////////////////////////////////////
    // 1.3. Configure SMTP server security and port
    ////////////////////////////////////////////////
    error = _4G.emailConfigureSMTP(smtp_security, smtp_port);

    if (error == 0)
    {
      printf("1.3. Configure SMTP server OK\n");
    }
    else
    {
      printf("1.3. Error configuring SMTP server. Code: %d\n", error);
    }
    
    ////////////////////////////////////////////////
    // 1.4. Set sender address
    ////////////////////////////////////////////////
    error = _4G.emailSetSender(sender_address, sender_user, sender_password);

    if (error == 0)
    {
      printf("1.4. Sender addres set OK\n");
    }
    else
    {
      printf("1.4. Error set address. Code: %d\n", error);
    }

    ////////////////////////////////////////////////
    // 1.5. Save e-mail configuration settings
    ////////////////////////////////////////////////
    error = _4G.emailSave();

    if (error == 0)
    {
      printf("1.5. Save configuration OK\n");
    }
    else
    {
      printf("1.5. Error saving configuration. Code: %d\n", error);
    }
  }
  else
  {
    printf("1. Error starting module. Code: %d\n", error);
  }

  printf("*** Setup done ***\n\n");
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
    // 2. Send e-mail
    ////////////////////////////////////////////////
    error = _4G.emailSend(receiver_address, subject, message);

    if (error == 0)
    {
      printf("2. Sending e-mail OK\n");
    }
    else
    {
      printf("2. Error sending e-mail. Code: %d\n", error);
    }
  }
  else
  {
    // Problem with the communication with the 4G module
    printf("1. 4G module not started. Error code: %d\n", error);
  }

  ////////////////////////////////////////////////
  // 3. Powers off the 4G module
  ////////////////////////////////////////////////
  printf("3. Switch OFF 4G module");
  _4G.OFF();

  ////////////////////////////////////////////////
  // 4. Sleep
  ////////////////////////////////////////////////
  delay (10000);
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
