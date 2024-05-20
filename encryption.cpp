/*
 * Bryan Mendoza
 * CS478 - Computer Security
 * Programming Assignment 3 - Part 2
 * Program to encrypt a file using a symmetric key
*/
#include <iostream>
#include <fstream>

using namespace std;

// Function to get user input for file names
void getUserParameters(string& encryptedKey, string& thirdPublicKey, string& privateKey) {
    cout << "Enter the name of the encrypted symmetric key file(symm_key.bin): ";
    cin >> encryptedKey;

    cout << "Enter the name of the associated public signature key file(pubkey.pem): ";
    cin >> thirdPublicKey;

    cout << "Enter the name of your personal private key file(privKey.pem): ";
    cin >> privateKey;
}

// Function to perform RSA decryption
bool decryptSymmetricKey(const string& encryptedKey, const string& thirdPublicKey) {
    int out = system(("openssl rsautl -verify -inkey " + thirdPublicKey + " -pubin -in " + encryptedKey + " -raw > symmetric.txt").c_str());
    return out == 0;
}

// Function to perform file encryption using symmetric key
bool encryptFile(const string& inputFile, const string& outputFile, const string& symmetricKey) {
    int out = system(("openssl enc -des-ecb -in " + inputFile + " -out " + outputFile + " -kfile " + symmetricKey + " -iter 10000 -pbkdf2").c_str());
    return out == 0;
}

// Function to sign a file using private key
bool signFile(const string& inputFile, const string& signatureFile, const string& privateKey) {
    int out = system(("openssl dgst -sign " + privateKey + " -keyform PEM -sha256 -out " + signatureFile + " " + inputFile).c_str());
    return out == 0;
}

int main(int argc, char *argv[]) {
    string privateKey;
    string encryptedKey;
    string thirdPublicKey;

    // Check if command line arguments are provided
    if (argc == 1) {
        // If no arguments, prompt user to input file names
        cout << "No parameters given (3 needed), using default file parameters\n\n";
        getUserParameters(encryptedKey, thirdPublicKey, privateKey);
    }
    else if (argc != 4) {
        // Inform user and exit if incorrect number of arguments provided
        cout << "Incorrect number of parameters " << (argc-1) << "!\n";
        cout << "Need 3 parameters:\n an encrypted symmetric key file, the associated public signature key, and your personal private key, in that order.";
        exit(1); 
    }
    else {    
        // If 3 arguments provided, use them as file names
        encryptedKey = argv[1];
        thirdPublicKey = argv[2];
        privateKey = argv[3];
        cout << "\n";
    }

    // Decrypt the encrypted symmetric key using the third party public key
    if (!decryptSymmetricKey(encryptedKey, thirdPublicKey)) {
        cout << "Error Occurred during decryption. Please Try Again!\n";
        exit(1);
    }

    // Encrypt plaintext.txt using the decrypted symmetric key
    if (!encryptFile("plaintext.txt", "encrypted.txt", "symmetric.txt")) {
        cout << "Error Occurred during encryption. Please Try Again!\n";
        exit(1);
    }

    // Sign the encrypted file using the private key
    cout << "Default password for signature is 'password'.\n";
    if (!signFile("encrypted.txt", "plaintext.signed", privateKey)) {
        cout << "Error Occurred during signature. Please Try Again!\n";
        exit(1);
    }

    // Inform user that the signature file is created successfully
    cout << "\nSignature file for encrypted message 'encrypted.txt', 'plaintext.signed', created using data file 'plaintext.txt', encryption key file 'symmetric.txt', and private key file '"<< privateKey <<"' for signature.\n\n";  

    return 0;
}
