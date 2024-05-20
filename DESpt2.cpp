// C++ program to implement the DES algorithm where user 
// inputs a message and a 64-bit hexadecimal key to encrypt
// the message. The program will then decrypt the ciphertext
// and compare it to the original message.
// author: Bryan Mendoza
// date: 04/03/2024
#include <iostream>
#include <bitset>
#include <vector>
#include <string>
#include <sstream>
#include<cmath>
#include <iomanip>

// Left rotate a bitset of size 28 by n bits
template <size_t N>
std::bitset<N> bitset_left_rotate(const std::bitset<N> &bitset, unsigned int n) {
    std::bitset<N> rotated = (bitset.to_ulong() << n) | (bitset.to_ulong() >> (N - n));
    return rotated;
} // End of bitset_left_rotate

// Expansion box
const int Expansion[] = {32, 1, 2, 3, 4, 5,
                 4, 5, 6, 7, 8, 9,
                 8, 9, 10, 11, 12, 13,
                 12, 13, 14, 15, 16, 17,
                 16, 17, 18, 19, 20, 21,
                 20, 21, 22, 23, 24, 25,
                 24, 25, 26, 27, 28, 29,
                 28, 29, 30, 31, 32, 1};
// S-boxes
const int sBox[8][4][16] = {
    { 
        {14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7},
        {0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8},
        {4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0},
        {15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13},
    },
    {
        {15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10},
        {3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5},
        {0, 14, 7, 11, 10, 4, 13, 1, 5, 8, 12, 6, 9, 3, 2, 15},
        {13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12, 0, 5, 14, 9},
    },
    {
        {10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8},
        {13, 7, 0, 9, 3, 4, 6, 10, 2, 8, 5, 14, 12, 11, 15, 1},
        {13, 6, 4, 9, 8, 15, 3, 0, 11, 1, 2, 12, 5, 10, 14, 7},
        {1, 10, 13, 0, 6, 9, 8, 7, 4, 15, 14, 3, 11, 5, 2, 12},
    },
    {
        {7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15},
        {13, 8, 11, 5, 6, 15, 0, 3, 4, 7, 2, 12, 1, 10, 14, 9},
        {10, 6, 9, 0, 12, 11, 7, 13, 15, 1, 3, 14, 5, 2, 8, 4},
        {3, 15, 0, 6, 10, 1, 13, 8, 9, 4, 5, 11, 12, 7, 2, 14},
    },
    {
        {2, 12, 4, 1, 7, 10, 11, 6, 8, 5, 3, 15, 13, 0, 14, 9},
        {14, 11, 2, 12, 4, 7, 13, 1, 5, 0, 15, 10, 3, 9, 8, 6},
        {4, 2, 1, 11, 10, 13, 7, 8, 15, 9, 12, 5, 6, 3, 0, 14},
        {11, 8, 12, 7, 1, 14, 2, 13, 6, 15, 0, 9, 10, 4, 5, 3},
    },
    {
        {12, 1, 10, 15, 9, 2, 6, 8, 0, 13, 3, 4, 14, 7, 5, 11},
        {10, 15, 4, 2, 7, 12, 9, 5, 6, 1, 13, 14, 0, 11, 3, 8},
        {9, 14, 15, 5, 2, 8, 12, 3, 7, 0, 4, 10, 1, 13, 11, 6},
        {4, 3, 2, 12, 9, 5, 15, 10, 11, 14, 1, 7, 6, 0, 8, 13},
    },
    {
        {4, 11, 2, 14, 15, 0, 8, 13, 3, 12, 9, 7, 5, 10, 6, 1},
        {13, 0, 11, 7, 4, 9, 1, 10, 14, 3, 5, 12, 2, 15, 8, 6},
        {1, 4, 11, 13, 12, 3, 7, 14, 10, 15, 6, 8, 0, 5, 9, 2},
        {6, 11, 13, 8, 1, 4, 10, 7, 9, 5, 0, 15, 14, 2, 3, 12},
    },
    {
        {13, 2, 8, 4, 6, 15, 11, 1, 10, 9, 3, 14, 5, 0, 12, 7},
        {1, 15, 13, 8, 10, 3, 7, 4, 12, 5, 6, 11, 0, 14, 9, 2},
        {7, 11, 4, 1, 9, 12, 14, 2, 0, 6, 10, 13, 15, 3, 5, 8},
        {2, 1, 14, 7, 4, 10, 8, 13, 15, 12, 9, 0, 3, 5, 6, 11},
    },
}; // End of S-boxes

// Permutation box
const int P[] = {16, 7, 20, 21, 29, 12, 28, 17,
                 1, 15, 23, 26, 5, 18, 31, 10,
                 2, 8, 24, 14, 32, 27, 3, 9,
                 19, 13, 30, 6, 22, 11, 4, 25};

// Expansion function
std::bitset<48> Expansion_Function(const std::bitset<32> &input) {
    std::bitset<48> expanded;

    for (size_t i = 0; i < 48; ++i) {
        expanded[i] = input[32 - Expansion[i]];
    } // end of for loop

    return expanded;
} // End of expansion_function

// S-boxes function
std::bitset<32> sboxes(const std::bitset<48> &input) {
    std::bitset<32> output;

    for (size_t i = 0; i < 8; ++i) {
        size_t row = (input[6 * i + 5] << 1) | input[6 * i];
        size_t col = (input[6 * i + 1] << 3) | (input[6 * i + 2] << 2) | (input[6 * i + 3] << 1) | input[6 * i + 4];
        output |= std::bitset<32>(sBox[i][row][col]) << (4 * (7 - i));
    } // end of for loop

    return output;
} // End of sboxes

// P-box function
std::bitset<32> pBox(const std::bitset<32> &input) {
    std::bitset<32> output;

    for (size_t i = 0; i < 32; ++i) {
        output[i] = input[32 - P[i]];
    } // end of for loop

    return output;
} // End of pBox

// Subkey generation function
std::vector<std::bitset<48>> generate_subkeys(const std::bitset<64> &key) {
    // Permutation box for key
    const int PC1[] = {57, 49, 41, 33, 25, 17, 9, 1, 58, 50, 42, 34, 26, 18,
                       10, 2, 59, 51, 43, 35, 27, 19, 11, 3, 60, 52, 44, 36,
                       63, 55, 47, 39, 31, 23, 15, 7, 62, 54, 46, 38, 30, 22,
                       14, 6, 61, 53, 45, 37, 29, 21, 13, 5, 28, 20, 12, 4};
    /// Permutation box for subkey generation
    const int PC2[] = {14, 17, 11, 24, 1, 5, 3, 28, 15, 6, 21, 10, 23, 19, 12, 4,
                       26, 8, 16, 7, 27, 20, 13, 2, 41, 52, 31, 37, 47, 55, 30, 40,
                       51, 45, 33, 48, 44, 49, 39, 56, 34, 53, 46, 42, 50, 36, 29, 32};
    // Number of shifts for each round
    const int shifts[] = {1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1};
    
    // permute the key
    std::bitset<56> permuted_key;
    for (int i = 0; i < 56; ++i) { 
        permuted_key[i] = key[64 - PC1[i]];
    }// end of for loop
    // split the key into two 28-bit keys
    std::bitset<28> left_key = permuted_key.to_ullong() >> 28; // 28 bits
    std::bitset<28> right_key = permuted_key.to_ullong(); // 28 bits

    std::vector<std::bitset<48>> subkeys;
    for (int round = 0; round < 16; ++round) {
        left_key = bitset_left_rotate<28>(left_key, shifts[round]);
        right_key = bitset_left_rotate<28>(right_key, shifts[round]);
        // Combine the two 28-bit keys
        std::bitset<56> combined_key((left_key.to_ulong() << 28) | right_key.to_ulong());
        std::bitset<48> subkey;
        for (int i = 0; i < 48; ++i) {
            subkey[i] = combined_key[56 - PC2[i]];
        } // end of for loop

        subkeys.push_back(subkey); // Add the subkey to the vector
    } // end of for loop
    return subkeys; // Return the vector of subkeys
} // End of generate_subkeys

// DES encryption function
std::bitset<64> des_encrypt(const std::bitset<64> &plaintext, const std::bitset<64> &key) {
    std::cout <<"--------------------------------------------------------------------------------"<<std::endl;
    // Generate subkeys
    std::vector<std::bitset<48>> subkeys = generate_subkeys(key);
    std::cout << "Number of subkeys: " << subkeys.size() << std::endl; // Add this line to display the number of subkeys
    std::cout << "Subkeys:\n" << std::endl;
    for (const auto &subkey : subkeys) {
        std::cout << "  " << subkey << std::endl;
    } // end of for loop

    std::cout <<"--------------------------------------------------------------------------------"<<std::endl;

    // Initial permutation
    std::bitset<32> left = plaintext.to_ulong() >> 32;
    std::bitset<32> right = plaintext.to_ulong();
    // Rounds
    for (int round = 0; round < 16; ++round) {
        std::bitset<32> old_right = right;

        std::cout <<"--------------------------------------------------------------------------------"<<std::endl;

        // Expansion function
        std::bitset<48> expanded_right = Expansion_Function(right);
        std::cout << "Round " << round + 1 << " expansion output: " << expanded_right << std::endl;
        // XOR with key right side
        expanded_right ^= subkeys[round];
        
        // S-boxes substitution
        std::bitset<32> sbox_output = sboxes(expanded_right);
        std::cout << "Round " << round + 1 << " S-box output: " << sbox_output << std::endl;

        // P-box permutation
        std::bitset<32> pBox_output = pBox(sbox_output);
        std::cout << "Round " << round + 1 << " P-box output: " << pBox_output << std::endl;

        // XOR with left side
        right = left ^ pBox_output;

        left = old_right;
    } // end of for loop

    // Final swap
    std::bitset<64> ciphertext((right.to_ulong() << 32) | left.to_ulong()); // Combine the two 32-bit halves
    return ciphertext; // Return the ciphertext
} // End of des_encrypt

int main() {
    // Prompt the user to enter a message
    std::cout << "Enter a message: ";
    std::string message; // Declare a string variable to store the message
    std::getline(std::cin, message); // Read a line of input from the user

    // Display the message you entered
    std::cout << "\nEntered message: " << message << std::endl;

    // Convert message to binary (padding if needed)
    std::stringstream ss;
    for (char c : message) {
        ss << std::bitset<8>(c);
    } // end of for loop
    std::string binary_message = ss.str(); // Convert the stringstream to a string

    // Pad the message
    int padding_size = 64 - (binary_message.size() % 64); // Calculate the padding size
    binary_message.append(padding_size, '0'); // Append the padding to the binary message

    // Create a std::bitset<64> object using the binary message
    std::bitset<64> plaintext(binary_message);

    // Prompt the user to enter a hexadecimal value
    std::cout << "Enter a hexadecimal value (64 bits): ";
    std::string hexInput; // Declare a string variable to store the hexadecimal value
    std::cin >> hexInput; // Read the hexadecimal value from the user

    // Convert the hexadecimal string to an unsigned long long integer
    unsigned long long int keyVal = std::stoull(hexInput, nullptr, 16);

    // Create a std::bitset<64> object using the converted integer
    std::bitset<64> key(keyVal);

    // Display the binary representation of the entered value
    std::cout << "Binary representation: " << key << std::endl;

    // Encrypt the plaintext
    std::bitset<64> ciphertext = des_encrypt(plaintext, key); // Call the des_encrypt function (Causes double print of rounds)
    std::cout <<"--------------------------------------------------------------------------------"<<std::endl;
    // Outputs
    std::cout << "Plaintext (with padding): " << plaintext << std::endl;
    std::cout << "Key: " << key << std::endl;
    std::cout << "Ciphertext: " << ciphertext << std::endl;
    std::cout <<"--------------------------------------------------------------------------------"<<std::endl;
   
    // Decrypt the ciphertext
    // Check if decryption matches the original plaintext
    std::bitset<64> decrypted = des_encrypt(ciphertext, key);
    std::cout << "Decrypted: " << decrypted << std::endl;
    std::cout <<"--------------------------------------------------------------------------------"<<std::endl;
    std::cout << "Original message: " << message << std::endl;
    std::string decrypted_text = decrypted.to_string();
    // Remove padding from decrypted text
    decrypted_text.erase(decrypted_text.find_last_not_of('0') + 1);
    std::cout << "Decrypted Text Matches Original Message: " << (decrypted_text == binary_message ? "No" : "Yes") << std::endl;
    std::cout <<"--------------------------------------------------------------------------------"<<std::endl;
    
    return 0;
}// End of main
