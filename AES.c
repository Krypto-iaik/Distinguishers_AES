#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "AES_common.h"
#include "AES_sbox.h"
#include "multiplication.h"

#define NUMBER_TEST 100

word8 randomByte(){
  return (word8) randomInRange(0, 255);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
//EXAMPLE DECRYPTION

int main(){

  //Ciphertext
  const unsigned char initialMessage[4][4] = {
    0x69, 0x6a, 0xd8, 0x70,
    0xc4, 0x7b, 0xcd, 0xb4,
    0xe0, 0x04, 0xb7, 0xc5,
    0xd8, 0x30, 0x80, 0x5a
  };

  //Key
  const unsigned char initialKey[4][4] = {
    0x00, 0x04, 0x08, 0x0c,
    0x01, 0x05, 0x09, 0x0d,
    0x02, 0x06, 0x0a, 0x0e,
    0x03, 0x07, 0x0b, 0x0f
  };

  word8 plaintext[4][4];

  deencryption(initialMessage, initialKey, &(plaintext[0][0]));

  printf("Ciphertext\n");
  printtt(initialMessage);
  printf("\n");

  printf("key\n");
  printtt(initialKey);
  printf("\n");

  printf("plaintext\n");
  printtt(plaintext);
  printf("\n");

  return 0;

}*/

/*
//EXAMPLE ENCRYPTION

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

}*/

/*TEST*/

int main(){

  word8 initialMessage[4][4], initialKey[4][4], finalMessage[4][4], prova[4][4];

  int i, j, k, flag;

  srand(1337);/*/time(NULL));*/

  for(i=0;i<NUMBER_TEST; i++){

    flag=0;

    /*generation of (random) key and plaintext*/

    for(j=0;j<4;j++){
      for(k=0;k<4;k++){
        initialMessage[j][k]=randomByte();
        initialKey[j][k]=randomByte();
      }
    }

    printf("Plaintext\n");
    printtt(initialMessage);
    printf("\n");

    printf("Master Key\n");
    printtt(initialKey);
    printf("\n");

    encryption(initialMessage, initialKey, finalMessage);

    printf("Ciphertext\n");
    printtt(finalMessage);
    printf("\n");

    decryption(finalMessage, initialKey, prova);

    for(j=0;j<4;j++){
      for(k=0;k<4;k++){
          if(prova[j][k]!=initialMessage[j][k])
            flag=1;
      }
    }

    if(flag==1)
      printf("Error %d\n", i);
    else
      printf("OK %d\n", i);

  }

  return 0;

}

