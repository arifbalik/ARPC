#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include "arpc_client/arpc_client.h"


#define PORT_NAME "/dev/ttyS4"
static int fd = 0;

void sendByte(uint8_t byte) {
  while (write(fd, &byte, 1) != 1)
    ;
}
void receiveByte(uint8_t *byte) {
  while (read(fd, byte, 1) != 1)
    ;
}
void shutdown(uint8_t errorCode) {
 exit(errorCode);
}


void init(){
  fd = open(PORT_NAME, O_RDWR | O_NOCTTY);

  if (fd < 0) {
    perror("serialport");
    exit(-1);
  }

 struct termios tty; 
  if (tcgetattr(fd, &tty) != 0) 
  { 
    perror("tcgetattr");
    exit(-1);
  } 
  // set baud rate to 9600 
  cfsetospeed(&tty, B2500000); 
  cfsetispeed(&tty, B2500000); 
  // set data bits to 8, parity to none, stop bits to 1 
  tty.c_cflag &= ~PARENB; 
  tty.c_cflag &= ~CSTOPB; 
  tty.c_cflag &= ~CSIZE; 
  tty.c_cflag |= CS8; 
  // apply settings to serial port 
  if (tcsetattr(fd, TCSANOW, &tty) != 0) 
  { 
    perror("tcsetattr");
    exit(-1);
  } 

}

int main() {
  init();

  pinMode(A0, OUTPUT);

  while(1){
    digitalWrite(A0, HIGH);
    delay(1000);
    digitalWrite(A0, LOW);
    delay(1000);
  }

  close(fd);
  return 0;
}