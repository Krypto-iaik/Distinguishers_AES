#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "AES_common.h"
#include "AES_smallScale_sbox.h"
#include "multiplication.h"

word8 randomByte(){
  return (word8) randomInRange(0, 15);
}

//EXAMPLE encryption

int main(){

  //plaintext
  const word8 initialMessage[4][4] = {
    0x00, 0x44, 0x88, 0xcc,
    0x11, 0x55, 0x99, 0xdd,
    0x22, 0x66, 0xaa, 0xee,
    0x33, 0x77, 0xbb, 0xff
  };

  //key
  const word8 initialKey[4][4] = {
    0x00, 0x04, 0x08, 0x0c,
    0x01, 0x05, 0x09, 0x0d,
    0x02, 0x06, 0x0a, 0x0e,
    0x03, 0x07, 0x0b, 0x0f
  };

  word8 ciphertext[4][4];

  encryption(initialMessage, initialKey, ciphertext);

  printf("plaintext\n");
  printtt(initialMessage);
  printf("\n");

  printf("key\n");
  printtt(initialKey);
  printf("\n");

  printf("ciphertext\n");
  printtt(ciphertext);
  printf("\n");

  return 0;

}
