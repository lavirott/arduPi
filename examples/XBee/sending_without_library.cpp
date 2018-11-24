#include "arduPi.h"
#include <stdio.h>

#define SOCKET_PW 3
#define HIGH 0x1
#define LOW  0x0

void setup() {
  // Activate socket
  pinMode(SOCKET_PW, OUTPUT);
  digitalWrite(SOCKET_PW, HIGH);
  
  // Initialize serial communication
  Serial.begin(9600);
}

void loop(){
  Serial.print('H');
  delay(1000);
  Serial.print('L');
  delay(1000);
}

int main (){
  setup();
  while(1){
    loop();
  }
  return (0);
}
