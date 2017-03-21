#ifndef __AES_H__
#define __AES_H__

#define N_Round 10

typedef unsigned char word8;

void addRoundKey(word8 *p, word8 key[][4]);
void invAddRoundKey(word8 *p, word8 key[][4][N_Round+1], int costante);

void shiftRows(word8 *p);
void inverseShiftRows(word8 *p);

void mixColumn(word8 *p);
void inverseMixColumn(word8 *p);

#endif // __AES_H__
