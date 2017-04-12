#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "AES_common.h"
#include "AES_sbox.h"
#include "multiplication.h"
#include "subspace_checks.h"

#define NUMBER_CP 20
#define NUMBER_TEST 10

word8 play[NUMBER_CP][16], cipher[NUMBER_CP][16];

word8 randomByte(){
  return (word8) randomInRange(0, 255);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**The function generates the number of texts (IN THE SAME COSET OF D) chosen by the parameter NUMBER_CP.
Then, for each possible pairs it counts the number of pairs of texts that belong to the same coset of M.*/

int AEStest(word8 key[4][4])
{
    int i, j, l, k, flag, flag2;
    int numero = 0;

    word8 p1[4][4], c1[4][4], temp[4][4];

    //Generate the texts in the same coset of U
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

/**The function generates the random texts (the number is chosen by the parameter NUMBER_CP), which correspond to the ciphertext.
Then, for each possible pairs it counts the number of pairs of texts that belong to the same coset of M.*/

int randomTest(word8 key[][4])
{
    int i, j, l, k, flag, t1, t2, flag2;
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

/**The program checks that in the subspace case and considering 3 Reduced Round AES, the probability that two texts collide
in the same coset of M is higher in the subspace case than in the random case */

int main()
{
    //Secret key
    word8 key[4][4] = {
        0x00, 0x44, 0x88, 0xcc,
        0x11, 0x55, 0x99, 0xdd,
        0x22, 0x66, 0xaa, 0xee,
        0x33, 0x77, 0xbb, 0xff
    };

    unsigned long int i;

    int numero = 0, nn, nn2, numero2 = 0, flag = 0, flag2 = 0;

    float n1, n2;

    printf("CHECK DISTINGUISHER for 3-Round Reduced AES.\n\n");
    printf("Number of texts: %d - Number of tests: %d\n\n", NUMBER_CP, NUMBER_TEST);

    //initialize seed
    srand (time(NULL));

    for(i=0; i < NUMBER_TEST; i++)
    {
        nn = randomTest(key);
        nn2 = AEStest(key);

        numero += nn;
        numero2 += nn2;

        printf("Number of collisions - AES case: %d\n", nn2);
        printf("Number of collisions - random case: %d\n", nn);

        if(nn2<nn)
        {
            flag = 1;
            printf("Distinguisher FAILS!\n");
        }
        if(nn2==nn)
        {
            flag2 = 1;
        }
        printf("\n");
    }

    printf("\n");

    n1 = ((float) numero)/NUMBER_TEST;
    n2 = ((float) numero2)/NUMBER_TEST;

    printf("Conclusion:\n");
    printf("Average number of collisions - AES Case: %f\n", n2);
    printf("Average number of collisions - Random Case: %f\n\n", n1);

    if(flag==1)
    {
        printf("Distinguisher FAILS at least one time!\n");
        return 0;
    }
    if(flag2==1)
    {
        printf("At least one time, the distinguisher is not able to distinguish among the two cases!\n");
    }
    else
        printf("Everything works!\n");

    return (0);
}

