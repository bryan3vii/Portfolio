/* 
 * Name: Bryan Mendoza
 * Date: 03/06/2024
 * Course: CS 478 - Computer Security
 * Description: This program decrypts a message using a frequency analysis and a dictionary check.
 * The program reads the ciphertext from a file called "ciphertext.txt" and writes the plaintext to a file called "Plaintext.txt".
 * The program also checks the plaintext against a dictionary file called "dictionary.txt" and displays the number of words matched.
*/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define NUM_CHARACTERS 27 // Number of characters in the English alphabet + 1 for null terminator

char CipherText[1000]; // Global variable to store the cipher text
char PlainText[1000]; // Global variable to store the plain text
int FrequencyTable[26][3]; // Global variable to store the frequency table

// Function to convert a string to uppercase
void UpperCaseString(char *s) {
    for (int i = 0; s[i] != '\0'; ++i) {
        if (islower(s[i])) {
            s[i] = toupper(s[i]);
        }
    }
} // End of UpperCaseString

int dictionaryCheck(); // Function prototype for dictionaryCheck

int decryptWithKey(char *cipherKey) {
    char tempChar;
    int i, j, count = 0, index, temp;

    // Copy the cipher text to plain text
    strcpy(PlainText, CipherText);

    // Set the frequency table with the provided cipher key
    for (i = 0; i < 26; i++) {
        FrequencyTable[i][2] = cipherKey[i];
    }

    // Sort the frequency table based on character frequency
    for (i = 0; i < 26; i++) {
        for (j = 0; j < 26 - i - 1; j++) {
            if (FrequencyTable[j][0] > FrequencyTable[j + 1][0]) {
                temp = FrequencyTable[j][0];
                FrequencyTable[j][0] = FrequencyTable[j + 1][0];
                FrequencyTable[j + 1][0] = temp;

                tempChar = FrequencyTable[j][1];
                FrequencyTable[j][1] = FrequencyTable[j + 1][1];
                FrequencyTable[j + 1][1] = tempChar;

                tempChar = FrequencyTable[j][2];
                FrequencyTable[j][2] = FrequencyTable[j + 1][2];
                FrequencyTable[j + 1][2] = tempChar;
            }
        }
    } // End of sorting frequency table

    // Decrypt the plain text using the frequency table
    while (count < strlen(PlainText)) {
        index = PlainText[count] - 'A';
        if (index >= 0 && index < 27) {
            PlainText[count] = FrequencyTable[index][2];
        }
        count++;
    }

    return 0;
} // End of decryptWithKey

int decrypt() {
    char cipherKey[27];
    printf("\n\t\tHow to run below:\t\t\n");
    printf("Make sure to match key based steps below.\n");
    printf("1. Key should be in order: 'A B C D E F G H I J K L M N O P Q R S T U V W X Y Z'\n");
    printf("Example of the order:      'E T O N I S A H R C L M U G F D W V Y K P B X J Q Z'\n");
    printf("Enter your cipher key:\n");
    fflush(stdout);
    scanf("%s", cipherKey);

    decryptWithKey(cipherKey);

    return 0;
} // End of decrypt

int main() {
    char choice;

    do {
        // Resetting variables
        memset(CipherText, 0, sizeof(CipherText));
        memset(PlainText, 0, sizeof(PlainText));
        memset(FrequencyTable, 0, sizeof(FrequencyTable));

        FILE *ptr;
        ptr = fopen("ciphertext.txt", "r");
        if (!ptr) {
            printf("ERROR: Unable to open ciphertext.txt\n");
            return 1;
        }
        int count = 0;
        char currentChar;
        while ((currentChar = fgetc(ptr)) != EOF) {
            if (isalpha(currentChar)) {
                FrequencyTable[toupper(currentChar) - 'A'][1]++;
            }
            CipherText[count++] = currentChar;
        }
        fclose(ptr);

        // Displaying frequency analysis before decryption
        printf("Frequency Analysis:\n");
        printf("Character   Frequency\n");
        for (int i = 0; i < 26; i++) {
            printf("%c           %d\n", 'A' + i, FrequencyTable[i][1]);
        }

        // Provide decryption options
        printf("\nChoose a decryption option:\n");
        printf("1. Decrypt with default key: ETAOINSHRDLCUMWFGYPBVKJXQZ (Standard English letter Frequency)\n");
        printf("2. Decrypt with custom key: DALJGXBQNCEHYUMOWTPIVSRZFK (My custom key used to decrypt message (First Iteration part of README))\n");
        printf("2. Decrypt with custom key: MOLXVJKQACEHWUGNFTYIBSRZDP (My custom key used to decrypt message (FINAL KEY USED TO DECRYPT))\n");
        printf("4. Enter a custom key for decryption (User-Input for custom key)\n");
        printf("User-Option: ");
        scanf(" %c", &choice);

        while (1) {
            switch (choice) {
                case '1':
                    decryptWithKey("ETAOINSHRDLCUMWFGYPBVKJXQZ"); // standard English letter frequency
                    break;
                case '2':
                    decryptWithKey("DALJGXBQNCEHYUMOWTPIVSRZFK"); // first iteration of manual cryptanalysis
                    break;
                case '3':
                    decryptWithKey("MOLXVJKQACEHWUGNFTYIBSRZDP"); // key that will decrypt the message
                    break;
                case '4':
                    decrypt(); // user input for custom key
                    break;
                default:
                    printf("Invalid option. Please choose again.\n");
                    printf("User-Option: ");
                    scanf(" %c", &choice);
                    continue;;
            }
            break;
        }

        dictionaryCheck();

        printf("\nCipherText: \n%s\n", CipherText);
        printf("\nPlainText: \n%s\n", PlainText);

        // Writing the plaintext to file
        FILE *plaintextFile = fopen("Plaintext.txt", "w");
        if (!plaintextFile) {
            printf("Error: Unable to open Plaintext.txt for writing\n");
            return 1;
        }
        fprintf(plaintextFile, "%s", PlainText);
        fclose(plaintextFile);

        printf("Do you want to decrypt another message? (Y/N): ");
        scanf(" %c", &choice);
    } while (toupper(choice) == 'Y');

    return 0;
}

int dictionaryCheck() {
    char word[10000];
    int count = 0, score = 0;
    FILE *dictFile = fopen("dictionary.txt", "r");
    if (!dictFile) {
        printf("Error: dictionary.txt file not found.\n");
        return -1;
    }
    while (fgets(word, 100, dictFile)) {
        word[strcspn(word, "\r\n")] = '\0'; // remove trailing newline characters
        UpperCaseString(word);
        if (strstr(PlainText, word)) {
            score += strlen(word); // Increase score by length of the matched word
        }
        count++;
    }
    fclose(dictFile);
    printf("Total number of words matched with dictionary.txt: %d out of %d.\n", score, count);
    return score;
}


