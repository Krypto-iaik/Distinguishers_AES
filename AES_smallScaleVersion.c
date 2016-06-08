#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N_Round 10

typedef unsigned char word8;

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

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*initialization state*/

void initialization(word8 *p, unsigned char initialMessage[][4]){

  int i, j;

  for(i=0; i<4; i++){
    for(j=0; j<4; j++){
      *(p+j+4*i) = initialMessage[i][j];
    }
  }

}

void initialization2(word8 *p, unsigned char initialMessage[][4]){

  int i, j;

  for(i=0; i<4; i++){
    for(j=0; j<4; j++){
      *(p+(N_Round+1)*j+(N_Round+1)*4*i) = initialMessage[i][j];
    }
  }

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*Add Round Key*/

void addRoundKey(word8 *p, word8 key[][4]){

  int i, j;

  for(i=0; i<4; i++){
    for(j=0; j<4; j++){
      *(p+j+4*i) ^= key[i][j];
    }
  }

}

void addRoundKey2(word8 *p, word8 key[][4][N_Round+1], int costante){

  int i, j;

  for(i=0; i<4; i++){
    for(j=0; j<4; j++){
      *(p+j+4*i) ^= key[i][j][costante];
    }
  }

}

/*Inverse Add round key*/

void invAddRoundKey(word8 *p, word8 key[][4][N_Round+1], int costante){

  word8 keytemp[4][4];

  int i, j;

  for(i=0;i<4;i++){
    for(j=0;j<4;j++)
      keytemp[i][j]=key[i][j][costante];
  }

  inverseMixColumn(&(keytemp[0][0]));

   for(i=0; i<4; i++){
    for(j=0; j<4; j++){
      *(p+j+4*i) ^= keytemp[i][j];
    }
  }

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*print the state*/

void printtt(word8 file[][4]){

  int i, j;

  for(i=0; i<4; i++){
    for(j=0; j<4; j++){
      printf("0x%x, ", file[i][j]);
    }
    printf("\n");
  }

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*Byte sub transformation with S-box*/

word8 byteTransformation(word8 byte){

  return sBox[byte];

}

/*Inverse byte sub transformation with Inverse S-box*/

word8 inverseByteTransformation(word8 byte){

  return inv_s[byte];

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*ByteTransformation*/

void byteSubTransformation(word8 *puntatore){

  int i, j;

  for(i=0; i<4; i++){
    for(j=0; j<4; j++)
      *(puntatore+j+4*i)=byteTransformation(*(puntatore+j+4*i));
  }
}

/*Inverse Byte Transformation*/

void inverseByteSubTransformation(word8 *puntatore){

  int i, j;

  for(i=0; i<4; i++){
    for(j=0; j<4; j++)
      *(puntatore+j+4*i)=inverseByteTransformation(*(puntatore+j+4*i));
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*Generation Round key*/

/*Calcolo trasformata terza colonna*/
void nuovaColonna(word8 *pColonna, int numeroRound){

  word8 temp, rCostante, colonnaTemp[4];
  int i;

  //rotazione degli elementi
  temp=*pColonna;
  for(i=0;i<3;i++){
    colonnaTemp[i]=*(pColonna+i+1);
  }
  colonnaTemp[3]=temp;

  //S-box
  for(i=0;i<4;i++)
    colonnaTemp[i]=byteTransformation(colonnaTemp[i]);

  //ultimoStep
  if(numeroRound==0)
    rCostante=0x01;
  else{
    rCostante=0x02;
    for(i=1;i<numeroRound;i++)
       rCostante=multiplicationX(rCostante);
  }

  colonnaTemp[0]^=rCostante;

  //return colonna
  for(i=0;i<4;i++){
    *(pColonna+i)=colonnaTemp[i];
  }

}

void generationRoundKey(word8 *pKey, int numeroRound){

  int i, j;

  word8 colonnaTemp[4];

  //calcolo la trasformata della terza colonna
  for(i=0;i<4;i++)
    colonnaTemp[i]=*(pKey + 3 + 4*i);

  nuovaColonna(&(colonnaTemp[0]), numeroRound);

  //nuova chiave//

  //prima colonna
  for(i=0;i<4;i++)
    *(pKey+4*i)=*(pKey+4*i)^colonnaTemp[i];

  //altre colonne
  for(i=1;i<4;i++){

    for(j=0;j<4;j++){
      *(pKey+i+4*j)=*(pKey+i+4*j)^*(pKey+i+4*j-1);
    }

  }

}

void generationRoundKey2(word8 *pKey, int numeroRound, word8 *pKeyPrecedente){

  int i, j;

  word8 colonnaTemp[4];

  numeroRound--;

  //calcolo la trasformata della terza colonna
  for(i=0;i<4;i++)
    colonnaTemp[i]=*(pKeyPrecedente + 3*(N_Round+1) + 4*i*(N_Round+1));

  nuovaColonna(&(colonnaTemp[0]), numeroRound);

  //nuova chiave//

  //prima colonna
  for(i=0;i<4;i++)
    *(pKey+4*(N_Round+1)*i)=*(pKeyPrecedente+4*(N_Round+1)*i)^colonnaTemp[i];

  //altre colonne
  for(i=1;i<4;i++){

    for(j=0;j<4;j++){
      *(pKey+i*(N_Round+1)+4*(1+N_Round)*j)=*(pKeyPrecedente+i*(N_Round+1)+4*(1+N_Round)*j)^*(pKey+(i-1)*(1+N_Round)+4*(1+N_Round)*j);
    }

  }

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*shift rows*/

void shiftRows(word8 *p){

  word8 temp[3];
  int i, j;

  for(i=1;i<4;i++){
    for(j=0;j<i;j++)
      temp[j]=*(p+4*i+j);

    for(j=0;j<(4-i);j++)
      *(p+4*i+j)=*(p+4*i+j+i);

    for(j=(4-i);j<4;j++)
      *(p+4*i+j)=temp[j-4+i];
  }

}

/*inverse shift rows*/

void inverseShiftRows(word8 *p){

  word8 temp[3];
  int i, j;

  for(i=1;i<4;i++){
    for(j=3;j>(3-i);j--)
      temp[j-1]=*(p+4*i+j);

    for(j=3;j>i-1;j--)
      *(p+4*i+j)=*(p+4*i+j-i);

    for(j=0;j<i;j++)
      *(p+4*i+j)=temp[3-i+j];
  }

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*MixColumn*/

void mixColumn(word8 *p){

  int i, j;
  word8 colonna[4], nuovaColonna[4];

  //separo le colonne e calcolo le nuove
  for(i=0;i<4;i++){

    //prendo la colonna i-sima
    for(j=0;j<4;j++){
      colonna[j]=*(p + i + 4*j);
    }

    //calcolo nuova colonna
    nuovaColonna[0]= multiplicationX(colonna[0]) ^ multiplicationX(colonna[1]) ^ colonna[1] ^ colonna[2] ^ colonna[3];
    nuovaColonna[1]= colonna[0] ^ multiplicationX(colonna[1]) ^ multiplicationX(colonna[2]) ^ colonna[2] ^ colonna[3];
    nuovaColonna[2]= colonna[0] ^ colonna[1] ^ multiplicationX(colonna[2]) ^ multiplicationX(colonna[3]) ^ colonna[3];
    nuovaColonna[3]= multiplicationX(colonna[0]) ^ colonna[0] ^ colonna[1] ^ colonna[2] ^ multiplicationX(colonna[3]);

    //reinserisco colonna
    for(j=0;j<4;j++){
      *(p + i + 4*j)=nuovaColonna[j];
    }

  }

}

/*inverse MixColumn*/

void inverseMixColumn(word8 *p){

  int i, j;
  word8 colonna[4], nuovaColonna[4];

  //separo le colonne e calcolo le nuove
  for(i=0;i<4;i++){

    //prendo la colonna i-sima
    for(j=0;j<4;j++){
      colonna[j]=*(p + i + 4*j);
    }

    //calcolo nuova colonna
    nuovaColonna[0]= multiplicationXN(colonna[0], 3) ^ multiplicationXN(colonna[0], 2) ^ multiplicationX(colonna[0]) ^
                     multiplicationXN(colonna[1], 3) ^ multiplicationX(colonna[1]) ^ colonna[1] ^ multiplicationXN(colonna[2], 3) ^
                     multiplicationXN(colonna[2], 2) ^ colonna[2] ^ multiplicationXN(colonna[3], 3) ^ colonna[3];

    nuovaColonna[1]= multiplicationXN(colonna[0], 3) ^ colonna[0] ^ multiplicationXN(colonna[1], 3) ^ multiplicationXN(colonna[1], 2) ^
                     multiplicationX(colonna[1]) ^ multiplicationXN(colonna[2], 3) ^ multiplicationX(colonna[2]) ^ colonna[2] ^
                     multiplicationXN(colonna[3], 3) ^ multiplicationXN(colonna[3], 2) ^ colonna[3];

    nuovaColonna[2]= multiplicationXN(colonna[0], 3) ^ multiplicationXN(colonna[0], 2) ^ colonna[0] ^ multiplicationXN(colonna[1], 3) ^
                    colonna[1] ^ multiplicationXN(colonna[2], 3) ^ multiplicationXN(colonna[2], 2) ^ multiplicationX(colonna[2]) ^
                    multiplicationXN(colonna[3], 3)^multiplicationX(colonna[3]) ^ colonna[3];

    nuovaColonna[3]= multiplicationXN(colonna[0], 3)^ multiplicationX(colonna[0]) ^ colonna[0] ^ multiplicationXN(colonna[1], 3) ^
                    multiplicationXN(colonna[1], 2)^colonna[1] ^ multiplicationXN(colonna[2], 3)^colonna[2] ^ multiplicationXN(colonna[3], 3)^
                    multiplicationXN(colonna[3], 2)^multiplicationX(colonna[3]);

    //reinserisco colonna
    for(j=0;j<4;j++){
      *(p + i + 4*j)=nuovaColonna[j];
    }

  }

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*Encryption of small scale AES*/

void encryption(word8 initialMessage[][4], word8 initialKey[][4], word8 *ciphertext){

  int i, j;

  //initialization state
  unsigned char state[4][4];
  initialization(&(state[0][0]), initialMessage);

  //initialization key
  unsigned char key[4][4];
  initialization(&(key[0][0]), initialKey);

  //Initial Round
  addRoundKey(&(state[0][0]), key);

  //Round
  for(i=0; i<N_Round-1; i++){
    generationRoundKey(&(key[0][0]), i);
    byteSubTransformation(&(state[0][0]));
    shiftRows(&(state[0][0]));
    mixColumn(&(state[0][0]));
    addRoundKey(&(state[0][0]), key);

  }

  //Final Round
  generationRoundKey(&(key[0][0]), N_Round-1);
  byteSubTransformation(&(state[0][0]));
  shiftRows(&(state[0][0]));
  addRoundKey(&(state[0][0]), key);

  //store the ciphertext
  for(i=0; i<4; i++){
    for(j=0; j<4; j++)
      *(ciphertext+j+4*i)=state[i][j];
  }

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*Decryption of small scale AES*/

void decryption(word8 initialMessage[][4], word8 initialKey[][4], word8 *deciphertext){

  int i, j;

  //initialization state
  unsigned char state[4][4];
  initialization(&(state[0][0]), initialMessage);

  //compute keys//

  unsigned char key[4][4][N_Round+1];
  //initialization key
  initialization2(&(key[0][0][0]), initialKey);

  //next round key
  for(i=1; i<N_Round+1; i++){
    generationRoundKey2(&(key[0][0][i]), i, &(key[0][0][i-1]));
  }


  //Initial Round
  addRoundKey2(&(state[0][0]), key, N_Round);


  //Round
  for(i=N_Round-1; i>0; i--){
    inverseByteSubTransformation(&(state[0][0]));

    inverseShiftRows(&(state[0][0]));

    inverseMixColumn(&(state[0][0]));

    invAddRoundKey(&(state[0][0]), key, i);

  }

  //Final Round
  inverseByteSubTransformation(&(state[0][0]));
  inverseShiftRows(&(state[0][0]));
  addRoundKey2(&(state[0][0]), key, 0);

  for(i=0;i<4;i++){
    for(j=0;j<4;j++)
      *(deciphertext+j+4*i)=state[i][j];
  }

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//EXAMPLE encryption

int main(){

  //plaintext
  const unsigned char initialMessage[4][4] = {
    0x00, 0x44, 0x88, 0xcc,
    0x11, 0x55, 0x99, 0xdd,
    0x22, 0x66, 0xaa, 0xee,
    0x33, 0x77, 0xbb, 0xff
  };

  //key
  const unsigned char initialKey[4][4] = {
    0x00, 0x04, 0x08, 0x0c,
    0x01, 0x05, 0x09, 0x0d,
    0x02, 0x06, 0x0a, 0x0e,
    0x03, 0x07, 0x0b, 0x0f
  };

  unsigned char finalMessage[4][4];

  encryption(initialMessage, initialKey, &(finalMessage[0][0]));

  printf("plaintext\n");
  printtt(initialMessage);
  printf("\n");

  printf("key\n");
  printtt(initialKey);
  printf("\n");

  printf("ciphertext\n");
  printtt(finalMessage);
  printf("\n");

  return 0;

}
