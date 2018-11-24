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

void loop() {
  char buffer;

  while (Serial.available()) {
    buffer = Serial.read();
    printf("%c", buffer);
    fflush(stdout);
  }
}

int main (){
  setup();
  while (1) {
    loop();
  }
  return 0;
}
