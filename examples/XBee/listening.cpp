
#include "arduPiXBee.h"
#include "arduPi.h"
#include <stdio.h>

//////////////////////////////////////////////
uint8_t sock = SOCKET0;

void setup(){
  XBee.ON(sock);
}

void loop(){
  char buffer;

  while (XBee.serialAvailable(sock)) {
    buffer = XBee.receive(sock);
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
