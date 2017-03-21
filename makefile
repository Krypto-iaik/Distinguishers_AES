CFLAGS=-O3 -Wall

all: AES AES_smallScale AES_2RoundDistinguisher AES_3RoundDistinguisher AES_4RoundDistinguisher AES_5RoundDistinguisher

aes_common: AES_common.h AES_common.c
	$(CC) $(CFLAGS) -c -o AES_common.o AES_common.c

AES: aes_common AES.c
	$(CC) $(CFLAGS) -o AES AES.c AES_common.o

AES_smallScale: aes_common AES_smallScaleVersion.c
	$(CC) $(CFLAGS) -o AES_smallScale AES_smallScaleVersion.c AES_common.o

AES_2RoundDistinguisher: AES_2RoundDistinguisher.c
	$(CC) $(CFLAGS) -o AES_2RoundDistinguisher AES_2RoundDistinguisher.c

AES_3RoundDistinguisher: AES_3RoundDistinguisher.c
	$(CC) $(CFLAGS) -o AES_3RoundDistinguisher AES_3RoundDistinguisher.c

AES_4RoundDistinguisher: AES_4RoundDistinguisher.c
	$(CC) $(CFLAGS) -o AES_4RoundDistinguisher AES_4RoundDistinguisher.c

AES_5RoundDistinguisher: AES_5RoundDistinguisher.c
	$(CC) $(CFLAGS) -o AES_5RoundDistinguisher AES_5RoundDistinguisher.c

clean:
	$(RM) -f AES_common.o
	$(RM) -f AES
	$(RM) -f AES_smallScale
	$(RM) -f AES_2RoundDistinguisher
	$(RM) -f AES_3RoundDistinguisher
	$(RM) -f AES_4RoundDistinguisher
	$(RM) -f AES_5RoundDistinguisher

.PHONY: clean
