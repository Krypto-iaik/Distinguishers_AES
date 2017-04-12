typedef unsigned char word8;

/*Multiplication*/

word8 multiplicationX(word8 byte){

  word8 bitTemp;

  bitTemp=byte>>7;
  byte=byte<<1;

  if(bitTemp==0)
    return byte;
  else
    return (byte^0x1B);

}

/*Multiplication byte times x^n*/

word8 multiplicationXN(word8 byte, int n){

  int i;

  for(i=0;i<n;i++)
    byte=multiplicationX(byte);

  return byte;

}
