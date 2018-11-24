/*  
 *  ------ P2P Code Example -------- 
 *  
 *  Explanation: This example shows how to configure the module
 *  for P2P mode and the corresponding parameters. The user must 
 *  keep in mind that every time the module is switched on, 
 *  the radio settings MUST be set again
 *  
 *  Copyright (C) 2015 Libelium Comunicaciones Distribuidas S.L. 
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
 *  Version:           0.4
 *  Design:            David Gascon
 *  Implementation:    Luismi Marti, Ruben Martin
 */

#include "arduPiLoRaWAN.h"

// socket to use
//////////////////////////////////////////////
uint8_t sock = SOCKET0;
//////////////////////////////////////////////

// define radio settings
//////////////////////////////////////////////
uint8_t power = 15;
uint32_t frequency = 868100000;
char spreading_factor[] = "sf12";
char coding_rate[] = "4/5";
uint16_t bandwidth = 125;
char crc_mode[] = "on";
//////////////////////////////////////////////

// variable
uint8_t error;

void setup() 
{
  printf("Radio P2P example - Module configuration\n\n");


  //////////////////////////////////////////////
  // 1. switch on
  //////////////////////////////////////////////

  error = LoRaWAN.ON(sock);

  // Check status
  if (error == 0)
  {
    printf("1. Switch ON OK\n");     
  }
  else 
  {
    printf("1. Switch ON error = %d\n", error); 
  }
  printf("-------------------------------------------------------\n");


  //////////////////////////////////////////////
  // 2. Enable P2P mode
  //////////////////////////////////////////////

  error = LoRaWAN.macPause();

  // Check status
  if (error == 0)
  {
    printf("2. P2P mode enabled OK\n");
  }
  else 
  {
    printf("2. Enable P2P mode error = %d\n", error);
  }
  printf("-------------------------------------------------------\n");



  //////////////////////////////////////////////
  // 3. Set/Get Radio Power
  //////////////////////////////////////////////

  // Set power
  error = LoRaWAN.setRadioPower(power);

  // Check status
  if (error == 0)
  {
    printf("3.1. Set Radio Power OK\n");
  }
  else 
  {
    printf("3.1. Set Radio Power error = %d\n", error);
  }

  // Get power
  error = LoRaWAN.getRadioPower();

  // Check status
  if (error == 0) 
  {
    printf("3.2. Get Radio Power OK. "); 
    printf("Power: %d\n",LoRaWAN._radioPower);
  }
  else 
  {
    printf("3.2. Get Radio Power error = %d\n", error);
  }
  printf("-------------------------------------------------------\n");



  //////////////////////////////////////////////
  // 4. Set/Get Radio Frequency
  //////////////////////////////////////////////

  // Set frequency
  error = LoRaWAN.setRadioFreq(frequency);

  // Check status
  if (error == 0)
  {
    printf("4.1. Set Radio Frequency OK\n");
  }
  else 
  {
    printf("4.1. Set Radio Frequency error = %d\n", error);
  }

  // Get frequency
  error = LoRaWAN.getRadioFreq();

  // Check status
  if (error == 0) 
  {
    printf("4.2. Get Radio Frequency OK. "); 
    printf("Frequency: %d\n",LoRaWAN._radioFreq);
  }
  else 
  {
    printf("4.2. Get Radio Frequency error = %d\n", error); 
  }
  printf("-------------------------------------------------------\n");



  //////////////////////////////////////////////
  // 5. Set/Get Radio Spreading Factor (SF)
  //////////////////////////////////////////////

  // Set SF
  error = LoRaWAN.setRadioSF(spreading_factor);

  // Check status
  if (error == 0)
  {
    printf("5.1. Set Radio SF OK\n");
  }
  else 
  {
    printf("5.1. Set Radio SF error = %d\n", error);
  }

  // Get SF
  error = LoRaWAN.getRadioSF();

  // Check status
  if (error == 0) 
  {
    printf("5.2. Get Radio SF OK. "); 
    printf("Spreading Factor: %s\n",LoRaWAN._radioSF);
  }
  else 
  {
    printf("5.2. Get Radio SF error = %d\n", error); 
  }
  printf("-------------------------------------------------------\n");



  //////////////////////////////////////////////
  // 6. Set/Get Radio Coding Rate (CR)
  //////////////////////////////////////////////

  // Set CR
  error = LoRaWAN.setRadioCR(coding_rate);

  // Check status
  if (error == 0)
  {
    printf("6.1. Set Radio CR OK\n");
  }
  else 
  {
    printf("6.1. Set Radio CR error = %d\n", error);
  }

  // Get CR
  error = LoRaWAN.getRadioCR();

  // Check status
  if (error == 0) 
  {
    printf("6.2. Get Radio CR OK. "); 
    printf("Coding Rate: %s\n", LoRaWAN._radioCR);
  }
  else 
  {
    printf("6.2. Get Radio CR error = %d\n", error); 
  }
  printf("-------------------------------------------------------\n");



  //////////////////////////////////////////////
  // 7. Set/Get Radio Bandwidth (BW)
  //////////////////////////////////////////////

  // Set BW
  error = LoRaWAN.setRadioBW(bandwidth);

  // Check status
  if (error == 0)
  {
    printf("7.1. Set Radio BW OK\n");
  }
  else 
  {
    printf("7.1. Set Radio BW error = %d\n", error);
  }

  // Get BW
  error = LoRaWAN.getRadioBW();

  // Check status
  if (error == 0) 
  {
    printf("7.2. Get Radio BW OK. "); 
    printf("Bandwidth: %d\n",LoRaWAN._radioBW);
  }
  else 
  {
    printf("7.2. Get Radio BW error = %d\n", error); 
  }
  printf("-------------------------------------------------------\n");



  //////////////////////////////////////////////
  // 8. Set/Get Radio CRC mode
  //////////////////////////////////////////////

  // Set CRC
  error = LoRaWAN.setRadioCRC(crc_mode);

  // Check status
  if (error == 0)
  {
    printf("8.1. Set Radio CRC mode OK\n");
  }
  else 
  {
    printf("8.1. Set Radio CRC mode error = %d\n", error);
  }

  // Get CRC
  error = LoRaWAN.getRadioCRC();

  // Check status
  if (error == 0) 
  {
    printf("8.2. Get Radio CRC mode OK. "); 
    printf("CRC status: %u\n",LoRaWAN._crcStatus);
  }
  else 
  {
    printf("8.2. Get Radio CRC mode error = %d\n", error); 
  }
  printf("-------------------------------------------------------\n");


  printf("-------------------------------------------------------\n");
  printf("Now the LoRaWAN module is ready for P2P communications.\n");
  printf("The user must keep in mind that every time the module is\n");  
  printf("switched on, the radio settings MUST be set again.\n");
  printf("Please check the next examples...\n");
  printf("-------------------------------------------------------\n\n");

}


void loop() 
{

}


//////////////////////////////////////////////
// Main loop setup() and loop() declarations
//////////////////////////////////////////////

int main (){
	setup();
	while(1){
		loop();
	}
	return (0);
}




