#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N_Round 5
#define N_TEST 4100

//random
#define N 624
#define M 397
#define MATRIX_A 0x9908b0dfUL   /* constant vector a */
#define UPPER_MASK 0x80000000UL /* most significant w-r bits */
#define LOWER_MASK 0x7fffffffUL /* least significant r bits */

typedef unsigned char word8;

//S-box
const unsigned char sBox[16] = {
  0x6, 0xB, 0x5, 0x4, 0x2, 0xE, 0x7, 0xA, 0x9, 0xD, 0xF, 0xC, 0x3, 0x1, 0x0, 0x8
};

//Inverse S-box
const unsigned char inv_s[16] = {
  0xE, 0xD, 0x4, 0xC, 0x3, 0x2, 0x0, 0x6, 0xF, 0x8, 0x7, 0x1, 0xB, 0x9, 0x5, 0xA
};

word8 play[16][16], cipher[16][16];

static unsigned long mt[N]; /* the array for the state vector  */
static int mti=N+1; /* mti==N+1 means mt[N] is not initialized */


/**Several ways to generate random number*/

int randomInRange(int min, int max){

  int range = max - min + 1;
  int a, b, c, d;

  a = rand();
  b = rand();
  c = rand();
  d = a*b;
  d = (d+c) % range;

  return (min + d);

}

int randomInRange2(int min, int max){

  int range = max - min + 1;
  int a;

  a = rand() % range;

  return (min + a);

}

/* initializes mt[N] with a seed */
void init_genrand(unsigned long s)
{
    mt[0]= s & 0xffffffffUL;
    for (mti=1; mti<N; mti++)
    {
        mt[mti] =
	    (1812433253UL * (mt[mti-1] ^ (mt[mti-1] >> 30)) + mti);
        mt[mti] &= 0xffffffffUL;
    }
}

/* initialize by an array with array-length */
/* init_key is the array for initializing keys */
/* key_length is its length */
void init_by_array(unsigned long init_key[], int key_length)
{
    int i, j, k;
    init_genrand(19650218UL);
    i=1; j=0;
    k = (N>key_length ? N : key_length);
    for (; k; k--)
    {
        mt[i] = (mt[i] ^ ((mt[i-1] ^ (mt[i-1] >> 30)) * 1664525UL))
          + init_key[j] + j; /* non linear */
        mt[i] &= 0xffffffffUL; /* for WORDSIZE > 32 machines */
        i++; j++;
        if (i>=N) { mt[0] = mt[N-1]; i=1; }
        if (j>=key_length) j=0;
    }
    for (k=N-1; k; k--) {
        mt[i] = (mt[i] ^ ((mt[i-1] ^ (mt[i-1] >> 30)) * 1566083941UL))
          - i; /* non linear */
        mt[i] &= 0xffffffffUL; /* for WORDSIZE > 32 machines */
        i++;
        if (i>=N) { mt[0] = mt[N-1]; i=1; }
    }

    mt[0] = 0x80000000UL; /* MSB is 1; assuring non-zero initial array */
}

/* generates a random number on [0,0xffffffff]-interval */
unsigned long genrand_int32(void)
{
    unsigned long y;
    static unsigned long mag01[2]={0x0UL, MATRIX_A};
    /* mag01[x] = x * MATRIX_A  for x=0,1 */

    if (mti >= N) { /* generate N words at one time */
        int kk;

        if (mti == N+1)   /* if init_genrand() has not been called, */
            init_genrand(5489UL); /* a default initial seed is used */

        for (kk=0;kk<N-M;kk++)
        {
            y = (mt[kk]&UPPER_MASK)|(mt[kk+1]&LOWER_MASK);
            mt[kk] = mt[kk+M] ^ (y >> 1) ^ mag01[y & 0x1UL];
        }
        for (;kk<N-1;kk++)
        {
            y = (mt[kk]&UPPER_MASK)|(mt[kk+1]&LOWER_MASK);
            mt[kk] = mt[kk+(M-N)] ^ (y >> 1) ^ mag01[y & 0x1UL];
        }
        y = (mt[N-1]&UPPER_MASK)|(mt[0]&LOWER_MASK);
        mt[N-1] = mt[M-1] ^ (y >> 1) ^ mag01[y & 0x1UL];

        mti = 0;
    }

    y = mt[mti++];

    /* Tempering */
    y ^= (y >> 11);
    y ^= (y << 7) & 0x9d2c5680UL;
    y ^= (y << 15) & 0xefc60000UL;
    y ^= (y >> 18);

    return y;
}

/* generates a random number on [0,0x7fffffff]-interval */
int genrand_int31()
{
    return (int)(genrand_int32()>>1);
}

/**Generate a random between 0 and 15 (included)*/
word8 randomByte(){

    int a = genrand_int31();

    a = a % 16;

  return (word8) a;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

/*Initialization State*/

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

/*Partial inverse mixcolumn - only on the first column*/

void partialInvMixColumn(word8 *p){

  int j;
  word8 colonna[4], nuovaColonna[4];

  //prendo la colonna i-sima
  for(j=0;j<4;j++){
    colonna[j]=*(p + j);
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
      *(p + j)=nuovaColonna[j];
    }
}


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


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*Print*/

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

/*third column*/
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
    rCostante=0x1;
  else{
    rCostante=0x2;
    for(i=1;i<numeroRound;i++)
       rCostante=multiplicationX(rCostante);
  }

  colonnaTemp[0] ^= rCostante;

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

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**Encryption:
NOTE: we're using a reduced version of AES, with nibble instead of byte (that is, 4 bits instead of 8).
We refer to "Small Scale Variants of the AES" of C. Cid, S. Murphy, and M.J.B. Robshaw for a complete description.
*/

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

  //store key!
  for(i=0; i<4; i++){
    for(j=0; j<4; j++)
      *(ciphertext+j+4*i)=state[i][j];
  }

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*Suppose that p = p1 \xor p2, that is the sum of two plaintexts.
I ask myself if it belong to a subspace D:
0 - not belong;
1 - belong to D (dim 12)
*/

int belongToU(word8 p[][4])
{
    if ((p[0][0] == 0) && (p[1][1] == 0) && (p[2][2] == 0) && (p[3][3] == 0))
            return 1;

    if ((p[1][0] == 0) && (p[2][1] == 0) && (p[3][2] == 0) && (p[0][3] == 0))
            return 1;

    if ((p[2][0] == 0) && (p[3][1] == 0) && (p[0][2] == 0) && (p[1][3] == 0))
            return 1;

    if ((p[3][0] == 0) && (p[0][1] == 0) && (p[1][2] == 0) && (p[2][3] == 0))
            return 1;

    return 0;

}

//Similar to the previous one, but with C instead of D,

int belongToV(word8 p[][4])
{
    if ((p[0][0] == 0) && (p[1][0] == 0) && (p[2][0] == 0) && (p[3][0] == 0))
            return 1;

    if ((p[0][1] == 0) && (p[1][1] == 0) && (p[2][1] == 0) && (p[3][1] == 0))
            return 1;

    if ((p[0][2] == 0) && (p[1][2] == 0) && (p[2][2] == 0) && (p[3][2] == 0))
            return 1;

    if ((p[0][3] == 0) && (p[1][3] == 0) && (p[2][3] == 0) && (p[3][3] == 0))
            return 1;

    return 0;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*Suppose that p = p1 \xor p2, that is the sum of two plaintexts.
I ask myself if it belong to a subspace M:
0 - not belong;
1 - belong to W (dim 12)
*/

int belongToW(word8 p[][4])
{
    /*Consider MC^-1(W) since no mixcolumns at the end!*/

    if ((p[0][0] == 0) && (p[3][1] == 0) && (p[2][2] == 0) && (p[1][3] == 0))
       return 1;

    if ((p[1][0] == 0) && (p[0][1] == 0) && (p[3][2] == 0) && (p[2][3] == 0))
       return 1;

    if ((p[2][0] == 0) && (p[1][1] == 0) && (p[0][2] == 0) && (p[3][3] == 0))
       return 1;

    if ((p[3][0] == 0) && (p[2][1] == 0) && (p[1][2] == 0) && (p[0][3] == 0))
       return 1;

    return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**AES CASE:
for a fixed combination of delta0, delta1, delta2, delta3, it generates the corresponding collection, that is sets of plaintexts
W_\Delta and the corresponding ciphertexts.
Then it counts the number of collision in M.

It returns 1 if there is at least one collision; 0 otherwise.
In this last cast, it prints the possible right key.*/

int contNumberCollisionAES(word8 k1, word8 k2, word8 k3, word8 k4, word8 key[][4])
{
    int i, j, numberCollision, t, s;
    word8 storeMemory[16][4], v[4], temp[16], temp2[16], temp3[4][4];

    long int k;

    //preparation plaintexts
    for(j=0;j<16;j++)
    {
        v[0] = (word8) j;
        v[1] = 0x0;
        v[2] = 0x0;
        v[3] = 0x0;

        partialInvMixColumn(&(v[0]));

        for(i=0;i<4;i++)
        {
            storeMemory[j][i] = inverseByteTransformation(v[i]);
        }

        storeMemory[j][0] ^= k1;
        storeMemory[j][1] ^= k2;
        storeMemory[j][2] ^= k3;
        storeMemory[j][3] ^= k4;

    }

    for(k=0; k<N_TEST; k++)
    {
        //plaintexts
        for(j=0; j<16; j++)
        {
            play[0][j] = randomByte();

            for(i=1; i<16; i++)
            {
                play[i][j] = play[0][j];
            }
        }
        for(j=0; j<16; j++)
        {
            play[j][0] = storeMemory[j][0];
            play[j][5] = storeMemory[j][1];
            play[j][10] = storeMemory[j][2];
            play[j][15] = storeMemory[j][3];
        }
        //ciphertexts
        for(j=0; j<16; j++)
        {
            for(i=0; i<16; i++)
            {
                temp[i] = play[j][i];
            }
            encryption(temp, key, &(temp2[0]));
            for(i=0; i<16; i++)
            {
                cipher[j][i] = temp2[i];
            }
        }

        for(i = 0; i<16; i++)
        {
            for(j=i+1; j<16; j++)
            {
                for(t = 0; t<4; t++)
                {
                    for(s = 0; s<4; s++)
                    {
                        temp3[s][t] = cipher[i][s+4*t] ^ cipher[j][s+4*t];
                    }
                }

                numberCollision = belongToW(temp3);

                if(numberCollision > 0)
                    return 1;

            }
        }
    }

    return 0;

}

/**RANDOM PERMUTATION CASE:
for a fixed combination of delta0, delta1, delta2, delta3, it generates the corresponding collection, that is sets of plaintexts
W_\Delta and the corresponding ciphertexts.
For simplicity, the ciphertexts are generated in a random way.
Then it counts the number of collision in M.

It returns 1 if there is at least one collision; 0 otherwise.*/

int contNumberCollisionRandom()
{
    word8 temp3[4][4], temp[4][4];

    long int i;

    int l, k, j, t, s, numberCollision;
    int flag2, flag;

    for(i=0; i<N_TEST; i++)
    {
        //produce random ciphertexts - it is a random Permutation!
        for(j=0; j<16; j++)
        {
            do
            {
                flag2 = 0;

                for(k=0;k<4;k++)
                {
                    for(l=0;l<4;l++)
                        temp[l][k]=randomByte();
                }

                for(t=0;((t<j)&&(j>0));t++)
                {
                    flag = 0;
                    for(k=0;k<4;k++)
                    {
                        for(l=0;l<4;l++)
                        {
                            if(temp[l][k]==cipher[t][l+4*k])
                                flag++;
                        }
                    }

                    if(flag == 16)
                        flag2 = 1;

                }

                if(flag2 == 0)
                {
                    for(k=0;k<4;k++)
                    {
                        for(l=0;l<4;l++)
                        {
                            cipher[j][l+4*k] = temp[l][k];
                        }
                    }
                }
            }while(flag2 == 1);
        }

        for(i = 0; i<16; i++)
        {
            for(j=i+1; j<16; j++)
            {
                for(t = 0; t<4; t++)
                {
                    for(s = 0; s<4; s++)
                    {
                        temp3[s][t] = cipher[i][s+4*t] ^ cipher[j][s+4*t];
                    }
                }

                numberCollision = belongToW(temp3);

                if(numberCollision > 0)
                    return 1;

            }
        }
    }

    return 0;

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
The following function implements the distinguisher for 5 rounds.
In particular, if var = 0, it generates the set of plaintexts-ciphertexts using the AES mode, and checks that it is AES.
If var = 1, it generates the set of plaintexts-ciphertexts using the random mode, and checks that it is a random permutation.
*/
int distinguisher5Rounds(word8 key[][4], int var)
{
    int k1, k2, k3, k4, number, nnn;
    word8 kk1, kk2, kk3, kk4;

    nnn = 0;

    for(k1 = 0; k1<16; k1++)
    {
        //printf("%d\n", k1);
        kk1 = (word8) k1;
        for(k2 = 0; k2 <16; k2++)
        {
            kk2 = (word8) k2;
            for(k3 = 0; k3<16; k3++)
            {
                kk3 = (word8) k3;
                for(k4 = 0; k4 <16; k4++)
                {
                    kk4 = (word8) k4;

                    if(var == 0)
                        number = contNumberCollisionAES(kk1, kk2, kk3, kk4, key);
                    else
                        number = contNumberCollisionRandom();

                    if(number == 0)
                    {
                        nnn++;
                        printf("0x%x - 0x%x - 0x%x - 0x%x", k1, k2, k3, k4);
                        if((k1 == key[0][0])&&(k2 == key[1][1])&&(k3 == key[2][2])&&(k4 == key[3][3]))
                            printf(" - Right Key!\n");
                        else
                            printf(" - Wrong Key!\n");
                    }
                }
            }
        }
    }

    if(nnn > 0)
        return 0;
    else
        return 1;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**DISTINGUISHER ON 5 ROUNDS - SECRET KEY */

int main()
{
    FILE *fp;

    word8 key[4][4] = {
        0x0, 0x4, 0x8, 0xc,
        0x1, 0x5, 0x9, 0xd,
        0x2, 0x6, 0xa, 0xe,
        0x3, 0x7, 0xb, 0xf
    };

    int j, k, result;

    srand (time(NULL));

    unsigned long init[4], length=4;
    init_by_array(init, length);

    //I want to work with 4 bits, not 8!
    for(k=0;k<4;k++)
    {
        for(j=0;j<4;j++)
            key[j][k] =  key[j][k] & 0x0f;
    }

    printf("Secret Key Distinguisher for 5 Rounds Small Scale AES.\n\n");

    printf("It works as follow: for each one of the 2^32 possible values of Delta (i.e. for each collection), it generates ");
    printf("%d different V_\Delta sets (each one with 2^8 texts). Then it checks if there is at least one collision.\n\n", N_TEST);

    printf("First step: AES\n");
    printf("We check if it recognize an AES permutation and it print the right key.\n");
    printf("Possible keys (row/column): 0/0 - 1/1 - 2/2 - 3/3\n");

    result = distinguisher5Rounds(key, 0);

    printf("Result:\n");
    if(result == 0)
        printf("\t AES\n\n");
    else
        printf("\t Something Fail...\n\n");

    //in this step, the ciphertexts are generated in a random way!
    printf("Second step: Random Permutation\n");
    printf("We check if it recognize a random permutation.\n");
    printf("Possible keys (row/column): 0/0 - 1/1 - 2/2 - 3/3\n");

    result = distinguisher5Rounds(key, 1);

    printf("Result:\n");
    if(result == 1)
        printf("\t No Keys - Random Permutation\n\n");
    else
        printf("\t Something Fail...\n\n");



    return (0);
}

