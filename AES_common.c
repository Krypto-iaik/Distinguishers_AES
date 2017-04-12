#include <stdio.h>
#include <stdlib.h>

#include "AES_common.h"

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

/*initialization state*/

void initialization(word8 *p, const word8 initialMessage[4][4]){

  int i, j;

  for(i=0; i<4; i++){
    for(j=0; j<4; j++){
      *(p+j+4*i) = initialMessage[i][j];
    }
  }

}

void initialization2(word8 *p, const word8 initialMessage[4][4]){

  int i, j;

  for(i=0; i<4; i++){
    for(j=0; j<4; j++){
      *(p+(N_Round+1)*j+(N_Round+1)*4*i) = initialMessage[i][j];
    }
  }

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*print the state*/

void printtt(const word8 file[4][4]){

  int i, j;

  for(i=0; i<4; i++){
    for(j=0; j<4; j++){
      printf("0x%02x, ", file[i][j]);
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

void byteSubTransformation(word8 p[4][4]){

  int i, j;

  for(i=0; i<4; i++){
    for(j=0; j<4; j++)
      p[i][j]=byteTransformation(p[i][j]);
  }
}

/*Inverse Byte Transformation*/

void inverseByteSubTransformation(word8 p[4][4]){

  int i, j;

  for(i=0; i<4; i++){
    for(j=0; j<4; j++)
      p[i][j]=inverseByteTransformation(p[i][j]);
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

void shiftRows(word8 p[4][4]){

  word8 temp[3];
  int i, j;

  for(i=1;i<4;i++){
    for(j=0;j<i;j++)
      temp[j]=p[i][j];

    for(j=0;j<(4-i);j++)
      p[i][j]=p[i][j+i];

    for(j=(4-i);j<4;j++)
      p[i][j]=temp[j-4+i];
  }

}

/*inverse shift rows*/

void inverseShiftRows(word8 p[4][4]){

  word8 temp[3];
  int i, j;

  for(i=1;i<4;i++){
    for(j=3;j>(3-i);j--)
      temp[j-1]=p[i][j];

    for(j=3;j>i-1;j--)
      p[i][j] = p[i][j-i];

    for(j=0;j<i;j++)
      p[i][j] = temp[3-i+j];
  }

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*MixColumn*/

void mixColumn(word8 p[4][4]){

  int i, j;
  word8 colonna[4], nuovaColonna[4];

  //separo le colonne e calcolo le nuove
  for(i=0;i<4;i++){

    //prendo la colonna i-sima
    for(j=0;j<4;j++){
      colonna[j]=p[j][i];
    }

    //calcolo nuova colonna
    nuovaColonna[0]= multiplicationX(colonna[0]) ^ multiplicationX(colonna[1]) ^ colonna[1] ^ colonna[2] ^ colonna[3];
    nuovaColonna[1]= colonna[0] ^ multiplicationX(colonna[1]) ^ multiplicationX(colonna[2]) ^ colonna[2] ^ colonna[3];
    nuovaColonna[2]= colonna[0] ^ colonna[1] ^ multiplicationX(colonna[2]) ^ multiplicationX(colonna[3]) ^ colonna[3];
    nuovaColonna[3]= multiplicationX(colonna[0]) ^ colonna[0] ^ colonna[1] ^ colonna[2] ^ multiplicationX(colonna[3]);

    //reinserisco colonna
    for(j=0;j<4;j++){
      p[j][i]=nuovaColonna[j];
    }

  }

}

/*inverse MixColumn*/

void inverseMixColumn(word8 p[4][4]){

  int i, j;
  word8 colonna[4], nuovaColonna[4];

  //separo le colonne e calcolo le nuove
  for(i=0;i<4;i++){

    //prendo la colonna i-sima
    for(j=0;j<4;j++){
      colonna[j]=p[j][i];
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
      p[j][i]=nuovaColonna[j];
    }

  }

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*Add Round Key*/

void addRoundKey(word8 p[4][4], word8 key[][4]){

  int i, j;

  for(i=0; i<4; i++){
    for(j=0; j<4; j++){
      p[i][j] ^= key[i][j];
    }
  }

}

void addRoundKey2(word8 p[4][4], word8 key[][4][N_Round+1], int costante){

  int i, j;

  for(i=0; i<4; i++){
    for(j=0; j<4; j++){
      p[i][j] ^= key[i][j][costante];
    }
  }

}

/*Inverse Add round key*/

void invAddRoundKey(word8 p[4][4], word8 key[][4][N_Round+1], int costante){

  word8 keytemp[4][4];

  int i, j;

  for(i=0;i<4;i++){
    for(j=0;j<4;j++)
      keytemp[i][j]=key[i][j][costante];
  }

  inverseMixColumn(keytemp);

   for(i=0; i<4; i++){
    for(j=0; j<4; j++){
      p[i][j] ^= keytemp[i][j];
    }
  }

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*Encryption*/

void encryption(const word8 initialMessage[][4], const word8 initialKey[][4], word8 ciphertext[4][4]){

  int i, j;

  //initialization state
  word8 state[4][4];
  initialization(&(state[0][0]), initialMessage);

  //initialization key
  word8 key[4][4];
  initialization(&(key[0][0]), initialKey);

  //Initial Round
  addRoundKey(state, key);

  //Round
  for(i=0; i<N_Round-1; i++){
    generationRoundKey(&(key[0][0]), i);
    byteSubTransformation(state);
    shiftRows(state);
    mixColumn(state);
    addRoundKey(state, key);

  }

  //Final Round
  generationRoundKey(&(key[0][0]), N_Round-1);
  byteSubTransformation(state);
  shiftRows(state);
  addRoundKey(state, key);

  //store the ciphertext
  for(i=0; i<4; i++){
    for(j=0; j<4; j++)
      ciphertext[i][j]=state[i][j];
  }

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*Decryption*/

void decryption(const word8 initialMessage[][4], const word8 initialKey[][4], word8 plaintext[4][4]){

  int i, j;

  //initialization state
  word8 state[4][4];
  initialization(&(state[0][0]), initialMessage);

  //compute keys//

  word8 key[4][4][N_Round+1];
  //initialization key
  initialization2(&(key[0][0][0]), initialKey);

  //next round key
  for(i=1; i<N_Round+1; i++){
    generationRoundKey2(&(key[0][0][i]), i, &(key[0][0][i-1]));
  }


  //Initial Round
  addRoundKey2(state, key, N_Round);


  //Round
  for(i=N_Round-1; i>0; i--){
    inverseByteSubTransformation(state);
    inverseShiftRows(state);
    inverseMixColumn(state);
    invAddRoundKey(state, key, i);

  }

  //Final Round
  inverseByteSubTransformation(state);
  inverseShiftRows(state);
  addRoundKey2(state, key, 0);

  for(i=0;i<4;i++){
    for(j=0;j<4;j++)
      plaintext[i][j]=state[i][j];
  }

}
