// Description: A5/1 stream cipher implementation in C
// author: Bryan Mendoza
// date: 04/03/2024
#include <string.h>
#include <stdio.h>

// A5/1 initialization with given initial values
unsigned char X[19] = {1, 0, 1, 0, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 0};
unsigned char Y[22] = {0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 1, 1};
unsigned char Z[23] = {1, 1, 1, 0, 1, 0, 1, 0, 0, 0, 1, 1, 1, 0, 1, 1, 1, 0, 0, 0, 0, 1, 0};

// A5/1 clocking functions for each register 
// Register X
void clockX() {
    unsigned char majority = (X[7] & X[9]) | (X[7] & X[10]) | (X[9] & X[10]);
    unsigned char x18 = majority ^ X[18]; // Update the MSB, not LSB
    for (int i = 18; i >= 1; i--) {
        X[i] = X[i - 1];
    }
    X[0] = x18;
}
// Register Y
void clockY() {
    unsigned char majority = (Y[9] & Y[11]) | (Y[9] & Y[12]) | (Y[11] & Y[12]);
    unsigned char y21 = majority ^ Y[21]; // Update the MSB, not LSB
    for (int i = 21; i >= 1; i--) {
        Y[i] = Y[i - 1];
    }
    Y[0] = y21;
}
// Register Z
void clockZ() {
    unsigned char majority = (Z[9] & Z[11]) | (Z[9] & Z[16]) | (Z[11] & Z[16]);
    unsigned char z22 = majority ^ Z[22]; // Update the MSB, not LSB
    for (int i = 22; i >= 1; i--) {
        Z[i] = Z[i - 1];
    }
    Z[0] = z22;
}

// Majority function
unsigned char majority(unsigned char a, unsigned char b, unsigned char c) {
    return (a & b) | (a & c) | (b & c);
}

int main() {
    int num_bits = 32;
    unsigned char keystream[32];

    // Initial register values
    int X_val = 0b1010010011000011100; // 19 bits
    int Y_val = 0b0011011100100001111011; // 22 bits
    int Z_val = 0b11101010001110111000010; // 23bits 
    
    for (int i = 0; i < num_bits; i++) {
        unsigned char bit = (X[18] ^ Y[21] ^ Z[22]);
        keystream[i] = bit;

        clockX();
        clockY();
        clockZ();
    }
    printf("Initial Register Values:\n");
    printf("X = 1010010011000011100\nY = 0011011100100001111011\nZ = 11101010001110111000010\n");

    printf("------------------------------------------------\n");

    printf("Keystream (first 32 bits): ");
    for (int i = 0; i < num_bits; i++) {
        printf("%d", keystream[i]);
    }
    printf("\n------------------------------------------------\n");

    // Final Register Values
    printf("Final Register Values:\n");
    printf("\nX Register: ");
    for (int i = 0; i < 19; i++) {
        printf("%d", X[i]);
    }
    printf("\nY Register: ");
    for (int i = 0; i < 22; i++) {
        printf("%d", Y[i]);
    }
    printf("\nZ Register: ");
    for (int i = 0; i < 23; i++) {
        printf("%d", Z[i]);
    }
    printf("\n");
    printf("------------------------------------------------\n");


    // Encrypt and decrypt a message
    unsigned char plaintext[32] = {0,1,1,1,1,1,1,0,0,1,0,1,1,1,0,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
    unsigned char ciphertext[32];
    unsigned char decrypted_text[32];

    for (int i = 0; i < 32; i++) {
        ciphertext[i] = plaintext[i] ^ keystream[i];
    } 

    for (int i = 0; i < 32; i++) {
        decrypted_text[i] = ciphertext[i] ^ keystream[i];
    }
    printf("Plaintext(in Hex): 7e5d7fff \n");
    printf("------------------------------------------------\n");
    printf("Plaintext(in Binary): ");
    for (int i = 0; i < 32; i++) {
        printf("%d", plaintext[i]);
    }
    printf("\nKeystream: ");
    for (int i = 0; i < num_bits; i++) {
        printf("%d", keystream[i]);
    }
    printf("\n------------------------------------------------\n");

    printf("\nCiphertext: ");
    for (int i = 0; i < 32; i++) {
        printf("%d", ciphertext[i]);
    }    
    printf("\nDecrypted Text: ");
    for (int i = 0; i < 32; i++) {
        printf("%d", decrypted_text[i]);
    }

    printf("\n------------------------------------------------\n");
    
    // Check if decrypted text matches plaintext
    if (memcmp(decrypted_text, plaintext, sizeof(decrypted_text)) == 0) {
        printf("Decryption successful!\n");
        printf("Decrytpion Text Matches Plaintext\n");
    } else {
        printf("Decryption failed!\n");
    }
    return 0;
}

