#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "AES_common.h"
#include "AES_sbox.h"
#include "multiplication.h"
#include "subspace_checks.h"

#define NUMBER_CP 80000
//where 2^16.25 = 77936

word8 play[NUMBER_CP][16], cipher[NUMBER_CP][16];

word8 randomByte(){
  return (word8) randomInRange(0, 255);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**The function generates the number of texts (IN THE SAME COSET OF D) chosen by the parameter NUMBER_CP.
Then, for each possible pairs it counts the number of pairs of texts that belong to the same coset of M.*/

int AEStest(word8 key[4][4])
{
    int i, j, l, k, flag = 0, flag2 = 0;
    int numero = 0;

    word8 p1[4][4], c1[4][4], temp[4][4];

    //Generate (different) texts in the same coset of U
    for(k=0;k<4;k++)
    {
        for(l=0;l<4;l++)
            temp[l][k]=randomByte();
    }

    for(i=0; i<NUMBER_CP; i++)
    {
        do{
            flag2 = 0;

            temp[0][0] = randomByte();
            temp[1][1] = randomByte();
            temp[2][2] = randomByte();
            temp[3][3] = randomByte();

            for(k=0; ((k<i)&&(i>0)); k++)
            {
                flag = 0;

                for(l=0;l<4;l++)
                {
                    if(play[k][l] == temp[l][l])
                        flag++;
                }

                if(flag == 4)
                    flag2 = 1;

            }

            if(flag2 == 0)
            {
                for(l=0;l<4;l++)
                    play[i][l] = temp[l][l];
            }

        }while(flag2 == 1);

        encryption(temp, key, c1);

        for(k = 0; k<4; k++)
        {
            for(l=0;l<4;l++)
                cipher[i][k + l*4] = c1[k][l];
        }
    }

    //Count the number of pairs that belong to the same cosets!
    for(i = 0; i < NUMBER_CP; i++)
    {
        for(j=i+1; j < NUMBER_CP; j++)
        {

            for(k = 0; k<4; k++)
            {
                for(l = 0; l<4; l++)
                {
                    p1[k][l] = cipher[i][k + l*4] ^ cipher[j][k + l*4];
                }
            }

            numero = numero + belongToW(p1);
        }
    }

    return numero;

}

/**The function generates the random texts (the number is chosen by the parameter NUMBER_CP), which correspond to the ciphertexts.
Then, for each possible pairs it counts the number of pairs of texts that belong to the same coset of M.*/

int randomTest(word8 key[][4])
{
    int i, j, l, k, flag = 0, t1, t2, flag2;
    int numero = 0;

    word8 p1[4][4], c1[4][4], temp[4][4];

    for(i=0; i<NUMBER_CP; i++)
    {

        do
        {
            flag2 = 0;

            for(k=0;k<4;k++)
            {
                for(l=0;l<4;l++)
                    temp[l][k]=randomByte();
            }

            for(k=0; ((k<i)&&(i>0)); k++)
            {
                flag = 0;

                for(l=0;l<16;l++)
                {
                    t1 = l%4;
                    t2 = (int)((l-t1)/4);

                    if(play[k][l] == temp[t1][t2])
                        flag++;
                }

                if(flag == 16)
                    flag2 = 1;
            }

            if(flag2 == 0)
            {
                for(l=0;l<16;l++)
                {
                    t1 = l%4;
                    t2 = (int)((l-t1)/4);

                    play[i][l] = temp[t1][t2];
                }
            }

        }while(flag2 == 1);

        encryption(temp, key, c1);

        for(k = 0; k<4; k++)
        {
            for(l=0;l<4;l++)
                cipher[i][k + l*4] = c1[k][l];
        }
    }

    //Count the number of pairs that belong to the same cosets!
    for(i = 0; i < NUMBER_CP; i++)
    {
        for(j=i+1; j < NUMBER_CP; j++)
        {

            for(k = 0; k<4; k++)
            {
                for(l = 0; l<4; l++)
                {
                    p1[k][l] = cipher[i][k + l*4] ^ cipher[j][k + l*4];
                }
            }

            numero = numero + belongToW(p1);
        }
    }

    return numero;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**The program verifies the impossible differential distinguisher on 4 Reduced Round AES.
That is, it checks that the probability that two texts collide in the same coset of M is 0 subspace case,
while at least one collision occur in the random case */

//FOR 80000 texts, TIME OF EXECUTIONs: 120 sec.

int main()
{
    //Secret key
    word8 key[4][4] = {
        0x00, 0x44, 0x88, 0xcc,
        0x11, 0x55, 0x99, 0xdd,
        0x22, 0x66, 0xaa, 0xee,
        0x33, 0x77, 0xbb, 0xff
    };

    int numero = 0, numero2 = 0;

    printf("IMPOSSIBLE DIFFERENTIAL DISTINGUISHER on 4-Round Reduced AES.\n\n");
    printf("The program checks that the probability that two texts collide in the same coset of M is 0 subspace case, ");
    printf("while at least one collision occur in the random case.\n\n");
    printf("Number of texts: %d\n\n", NUMBER_CP);
    //initialize seed
    srand (time(NULL));

    numero = randomTest(key);
    numero2 = AEStest(key);

    printf("Number of collisions - AES case: %d\n", numero2);
    printf("Number of collisions - random case: %d\n", numero);

    if(numero2 != 0)
        printf("something FAILS...\n");

    if(numero == 0)
        printf("Distinguisher can not take a decision...\n");
    else
        printf("Distinguisher WORKS!!!\n");

    return 0;
}

