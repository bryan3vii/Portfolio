/*
 * Bryan Mendoza
 * CS478 - Computer Security
 * Programming Assignment 3 - Part 2
 * Program to decrypt a file using a symmetric key
*/
#include <iostream>
#include <fstream>

using namespace std;

// Function to get user input for file names
void getUserParameters(string& msgSigned, string& publicKey, string& symKey) {
    cout << "Enter the name of the signed encrypted file(plaintext.signed): ";
    cin >> msgSigned;

    cout << "Enter the name of the associated public signature key file(MYpubkey.pem): "; 
    cin >> publicKey;

    cout << "Enter the name of the symmetric key file used for decryption(symmetric.txt): ";
    cin >> symKey;
}

int main(int argc, char *argv[]) {
    // Declare variables to store file names
    string msgSigned;
    string publicKey;
    string symKey;

    // Check if command line arguments are provided
    if (argc == 1) {
        // If no arguments, prompt user to input file names
        cout << "No parameters given (3 needed), using default file parameters\n\n";
        getUserParameters(msgSigned, publicKey, symKey);
    }
    // If incorrect number of arguments provided
    else if (argc != 4) {
        // Inform user and exit
        cout << "Incorrect number of parameters " << (argc-1) << "!\n";
        cout << "Need 3 parameters:\n a signed encrypted file, the associated public signature key, and the symmetric key used for decryption, in that order.";
        exit(1);
    }
    else {
        // If 3 arguments provided, use them as file names
        msgSigned = argv[1];
        publicKey = argv[2];
        symKey = argv[3];
        cout << "\n";
    }

    // Verify the signature of the encrypted file
    int out = system(("openssl dgst -verify " + publicKey + " -keyform PEM -sha256 -signature " + msgSigned + " encrypted.txt").c_str());

    if (out != 0) {
        // If verification fails, inform user and exit
        cout << "Unable to verify, Exiting...\n";
        exit(0);
    }

    // Decrypt the encrypted file using the symmetric key
    out = system("openssl enc -d -des-ecb -out decrypted.txt -in encrypted.txt -kfile symmetric.txt -iter 10000 -pbkdf2");

    if (out != 0) {
        // If decryption fails, inform user and exit
        cout << "Error During Decryption... Exiting...\n";
        exit(0);
    }

    // Inform user that decryption was successful
    cout << "After verification, the decryption of file \'encrypted.txt\' using key \'symmetric.txt\' is stored in file \'decryption.txt\'.\n\n";

    return 0;
}
