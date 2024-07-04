# Vigenere-Cipher-Decrypter
This is a program which takes an input file(ciphertext) , decrypts it and displays the message. I have coded it from scratch in 'C' language since I found it interesting as a part of the Assignment from the course 'Coding Theory and Cryptography'.
It first finds the shift using the improved attack on shift cipher. It then calculates the index of coincidence for a particular stream.
It finds the probable key lengths based on the index of coincidences which has already found. It does it for every stream and for every stream it finds the key length.
It then decrypts every stream using the improved attack on shift cipher. Then it merges and displays the final message.
I have commented the whole process clearly in the code itself.

