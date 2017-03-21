#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "AES_common.h"

const unsigned char sBox[16] = {
  0x6, 0xB, 0x5, 0x4, 0x2, 0xE, 0x7, 0xA, 0x9, 0xD, 0xF, 0xC, 0x3, 0x1, 0x0, 0x8
};

//Inverse S-box
const unsigned char inv_s[16] = {
  0xE, 0xD, 0x4, 0xC, 0x3, 0x2, 0x0, 0x6, 0xF, 0x8, 0x7, 0x1, 0xB, 0x9, 0x5, 0xA
};

int randomInRange(int min, int max){

  int range = max - min + 1;
  int a, b, c, d;

  a = rand() % range;
  b = rand() % range;
  c = rand() % range;
  d = (a*b) % range;
  d = (d+c) % range;

  return (min + d);

}

word8 randomByte(){
  return (word8) randomInRange(0, 15);
}

/*Multiplication*/

word8 multiplicationX(word8 byte){

  word8 bitTemp;

  bitTemp = (byte>>3) & 0x1;
  byte = (byte<<1) & 0xf;

  if(bitTemp==0)
    return byte;
  else
    return (byte^0x03);

}

/*Multiplication byte times x^n*/

word8 multiplicationXN(word8 byte, int n){

  int i;

  for(i=0;i<n;i++)
    byte=multiplicationX(byte);

  return byte;

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
