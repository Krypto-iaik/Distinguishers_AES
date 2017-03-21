typedef unsigned char word8;

/*Suppose that p = p1 \xor p2, that is the sum of two plaintexts.
I ask myself if it belong to a subspace D:
0 - not belong;
1 - belong to D (dim 12)
*/

int belongToU(const word8 p[4][4])
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

int belongToV(const word8 p[4][4])
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

int belongToW(const word8 p[4][4])
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
