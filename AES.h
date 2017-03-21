#ifndef __AES_H__
#define __AES_H__

#define N_Round 10

typedef unsigned char word8;

void addRoundKey(word8 p[4][4], word8 key[][4]);
void invAddRoundKey(word8 p[4][4], word8 key[][4][N_Round+1], int costante);

void byteSubTransformation(word8 p[4][4]);
void inverseByteSubTransformation(word8 p[4][4]);

void shiftRows(word8 p[4][4]);
void inverseShiftRows(word8 p[4][4]);

void mixColumn(word8 p[4][4]);
void inverseMixColumn(word8 p[4][4]);

#endif // __AES_H__
