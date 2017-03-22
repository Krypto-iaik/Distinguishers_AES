CFLAGS=-O3 -Wall

all: AES AES_smallScale AES_2RoundDistinguisher AES_3RoundDistinguisher AES_4RoundDistinguisher

multiplication: multiplication.h multiplication.c
	$(CC) $(CFLAGS) -c -o multiplication.o multiplication.c
	
subspace_checks: subspace_checks.h subspace_checks.c
	$(CC) $(CFLAGS) -c -o subspace_checks.o subspace_checks.c

aes_common: AES_common.h AES_common.c
	$(CC) $(CFLAGS) -c -o AES_common10.o AES_common.c -DN_Round=10
	$(CC) $(CFLAGS) -c -o AES_common2.o AES_common.c -DN_Round=2
	$(CC) $(CFLAGS) -c -o AES_common3.o AES_common.c -DN_Round=3
	$(CC) $(CFLAGS) -c -o AES_common4.o AES_common.c -DN_Round=4


AES: aes_common multiplication AES.c
	$(CC) $(CFLAGS) -o AES AES.c AES_common10.o multiplication.o

AES_2RoundDistinguisher: aes_common multiplication subspace_checks AES_2RoundDistinguisher.c
	$(CC) $(CFLAGS) -o AES_2RoundDistinguisher AES_2RoundDistinguisher.c AES_common2.o subspace_checks.o multiplication.o

AES_3RoundDistinguisher: aes_common multiplication subspace_checks AES_3RoundDistinguisher.c
	$(CC) $(CFLAGS) -o AES_3RoundDistinguisher AES_3RoundDistinguisher.c AES_common3.o subspace_checks.o multiplication.o

AES_4RoundDistinguisher: aes_common multiplication subspace_checks AES_4RoundDistinguisher.c
	$(CC) $(CFLAGS) -o AES_4RoundDistinguisher AES_4RoundDistinguisher.c AES_common4.o subspace_checks.o multiplication.o


clean:
	$(RM) -f AES_common10.o AES_common2.o AES_common3.o AES_common4.o 
	$(RM) -f multiplication.o 
	$(RM) -f subspace_checks.o
	$(RM) -f AES
	$(RM) -f AES_2RoundDistinguisher
	$(RM) -f AES_3RoundDistinguisher
	$(RM) -f AES_4RoundDistinguisher
	
.PHONY: clean
