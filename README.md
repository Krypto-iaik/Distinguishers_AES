# Distinguishers_AES

Secret-Key Distinguishers for AES-128 up to 4 rounds. 
The distinguishers are described using the Subspace Trail notation.

For a complete discussion about the Subspace Trail notation, we refer to the following paper:

Title: "Subspace Trail Cryptanalysis and its Applications to AES"
Authors: Lorenzo Grassi, Christian Rechberger and Sondre Ronjom
Available at: https://eprint.iacr.org/2016/592

Programs:

0) AES.c - AES_common.c

Distinguishers:

1) AES_2RoundDistinguisher.c

2) AES_3RoundDistinguisher.c

3) AES_4RoundDistinguisher.c

The programs should run with almost C-compilers (we have used gcc version 4.8.1).
None of these programs is speed-optimized, they are for verification purposes only.

In all the programs, the main function is always the last one.

The first program contains our implementation of AES (encryption and decryption).
It has been verified against test-vectors.

The other four programs are the DISTINGUISHERs for the secret key case up to five rounds.

1) "AES_2RoundDistinguisher" verifies the 2-rounds secret key distinguisher describes in Sect. 5.1.
In particular, for each test, it generates NUMBER_CP texts (line 6) both in the random and in the subspace case (in this second case, the texts are in the same coset of D) - NUMBER_CP is equal to 2. Then, it encrypts them and counts the number of collision. Remember that in this case, the number of collision is higher in the subspace case (1 collision for each pair of plaintext) than in the random case (approximately 0).
The number of experiment can be chosen by the parameter NUMBER_TEST (line 7), while the secret key can be chosen/changed in the main function.

2) "AES_3RoundDistinguisher" verifies the 3-rounds secret key distinguisher describes in Sect. 5.2.
In particular, for each test, it generates NUMBER_CP texts (line 6) both in the random and in the subspace case (in this second case, the texts are in the same coset of D) - NUMBER_CP is equal to 20. Then, it encrypts them and counts the number of collision. Remember that in this case, the number of collision is higher in the subspace case than in the random case.
The number of experiments can be chosen using the parameter NUMBER_TEST (line 7), while the secret key can be chosen/changed in the main function.

3) "AES_4RoundDistinguisher" verifies the 4-rounds secret key distinguisher describes in Sect. 5.3.
It generates the number NUMBER_CP texts (line 6) of texts necessary for the distinguisher both for the random and for the subspace case - NUMBER_CP is equal to 80000, that is approximately 2^16.25. Then it checks that the number of collisions in the subspace case is 0, while it is higher in the random case. 
The secret key can be chosen/changed in the main function.
The time of execution is approximately of 120 seconds.
