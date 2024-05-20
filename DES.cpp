// C++ program to implement the DES algorithm
// Where user inputs a message and a 64-bit hexadecimal key to encrypt
// The program will output the 16 keys generated and the encrypted message
// The program will also output the decrypted message
// author: Bryan Mendoza
// date: 04/03/2024
#include <iostream>
#include <string>
#include <bitset>
#include <cmath>
using namespace std;

// Array to hold the 16 keys
string round_keys[16];

// String to hold the plain text
string pt;

// Function to convert a number in decimal to binary
string convertDecimalToBinary(int decimal)
{
    string binary;
    while (decimal != 0)
    {
        binary = (decimal % 2 == 0 ? "0" : "1") + binary;
        decimal = decimal / 2;
    }
    while (binary.length() < 4)
    {
        binary = "0" + binary;
    }
    return binary;
} // end of convertDecimalToBinary

// Function to convert a number in binary to decimal
int convertBinaryToDecimal(string binary)
{
    int decimal = 0;
    int counter = 0;
    int size = binary.length();
    for (int i = size - 1; i >= 0; i--)
    {
        if (binary[i] == '1')
        {
            decimal += pow(2, counter);
        }
        counter++;
    } // Return the decimal number
    return decimal;
} // end of convertBinaryToDecimal

// Function to do a circular left shift by 1
string shift_left_once(string key_chunk){
    string shifted = "";
    for (int i = 1; i < 28; i++)
    {
        shifted += key_chunk[i];
    } // The first bit is shifted to the last
    shifted += key_chunk[0];
    return shifted;
    // Return the shifted key chunk
} // end of shift_left_once

// Function to do a circular left shift by 2
string shift_left_twice(string key_chunk){ 
    string shifted=""; 
    for(int i = 0; i < 2; i++){ 
        for(int j = 1; j < 28; j++){ 
            shifted += key_chunk[j]; 
        } // The first bit is shifted to the last
        shifted += key_chunk[0]; 
        key_chunk= shifted; 
        shifted =""; 
    } // Return the shifted key chunk
    return key_chunk; 
} // end of shift_left_twice

// Function to compute xor between two strings
string Xor(string a, string b)
{
    string result = "";
    int size = b.size();
    for (int i = 0; i < size; i++)
    {
        if (a[i] != b[i])
        {
            result += "1";
        } // If the bits are the same, append 0
        else
        {
            result += "0";
        } // Return the result
    } // end of for loop
    return result; 
} // end of Xor
// Function to generate the 16 keys
void generate_keys(string key){
	// The PC1 table 
	int pc1[56] = {
	57,49,41,33,25,17,9, 
	1,58,50,42,34,26,18, 
	10,2,59,51,43,35,27, 
	19,11,3,60,52,44,36,		 
	63,55,47,39,31,23,15, 
	7,62,54,46,38,30,22, 
	14,6,61,53,45,37,29, 
	21,13,5,28,20,12,4 
	};
	// The PC2 table
	int pc2[48] = { 
	14,17,11,24,1,5, 
	3,28,15,6,21,10, 
	23,19,12,4,26,8, 
	16,7,27,20,13,2, 
	41,52,31,37,47,55, 
	30,40,51,45,33,48, 
	44,49,39,56,34,53, 
	46,42,50,36,29,32 
	}; 
	// Compressing the key using the PC1 table
	string perm_key =""; 
	for(int i = 0; i < 56; i++){ 
		perm_key+= key[pc1[i]-1]; 
	} 
	// Dividing the result into two equal halves
	string left= perm_key.substr(0, 28); 
	string right= perm_key.substr(28, 28); 
    cout<<"\nGenerating the 16 keys of the DES encryption Algorthim."<<endl;
    cout<<"--------------------------------------------------------------------------------"<<endl;
    cout<<"16 rounds of DES encryption:"<<endl;
    cout <<"Initial key: " << key << endl;
    cout<<"--------------------------------------------------------------------------------"<<endl;
	// Generating the 16 keys
	for(int i=0; i<16; i++){ 
		// For rounds 1, 2, 9, 16 the key_chunks are shifted by one.
		if(i == 0 || i == 1 || i == 8 || i == 15 ){
			left= shift_left_once(left); 
			right= shift_left_once(right);
		}  
        // For other rounds, the key_chunks are shifted by two
		else{
			left= shift_left_twice(left);  
			right= shift_left_twice(right); 
		} // end of if/else
	// combining the two chunks
	string combined_key = left + right;
	string round_key = ""; 
	// PC2 table is used to transpose the key bits
	for(int i = 0; i < 48; i++){ 
		round_key += combined_key[pc2[i]-1]; 
	}   // Saving the key generated
	round_keys[i] = round_key;
		cout<<"Key "<<i+1<<": "<<round_keys[i]<<endl; 
	} // end of for loop
} // end of generate_keys

// Implementing the algorithm
string DES()
{
    // The initial permutation table
    int initial_permutation[64] = {
        58, 50, 42, 34, 26, 18, 10, 2,
        60, 52, 44, 36, 28, 20, 12, 4,
        62, 54, 46, 38, 30, 22, 14, 6,
        64, 56, 48, 40, 32, 24, 16, 8,
        57, 49, 41, 33, 25, 17, 9, 1,
        59, 51, 43, 35, 27, 19, 11, 3,
        61, 53, 45, 37, 29, 21, 13, 5,
        63, 55, 47, 39, 31, 23, 15, 7};
    // The expansion table
    int expansion_table[48] = {
        32, 1, 2, 3, 4, 5, 4, 5,
        6, 7, 8, 9, 8, 9, 10, 11,
        12, 13, 12, 13, 14, 15, 16, 17,
        16, 17, 18, 19, 20, 21, 20, 21,
        22, 23, 24, 25, 24, 25, 26, 27,
        28, 29, 28, 29, 30, 31, 32, 1};
    // The substitution boxes. The should contain values from 0 to 15 in any order.
    int substition_boxes[8][4][16] =
        {{ // S1
             14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7,
             0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8,
             4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0,
             15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13},
         { //  S2
             15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10,
             3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5,
             0, 14, 7, 11, 10, 4, 13, 1, 5, 8, 12, 6, 9, 3, 2, 15,
             13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12, 0, 5, 14, 9},
         { // S3
             10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8,
             13, 7, 0, 9, 3, 4, 6, 10, 2, 8, 5, 14, 12, 11, 15, 1,
             13, 6, 4, 9, 8, 15, 3, 0, 11, 1, 2, 12, 5, 10, 14, 7,
             1, 10, 13, 0, 6, 9, 8, 7, 4, 15, 14, 3, 11, 5, 2, 12},
         { // S4
             7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15,
             13, 8, 11, 5, 6, 15, 0, 3, 4, 7, 2, 12, 1, 10, 14, 9,
             10, 6, 9, 0, 12, 11, 7, 13, 15, 1, 3, 14, 5, 2, 8, 4,
             3, 15, 0, 6, 10, 1, 13, 8, 9, 4, 5, 11, 12, 7, 2, 14},
         { // S5
             2, 12, 4, 1, 7, 10, 11, 6, 8, 5, 3, 15, 13, 0, 14, 9,
             14, 11, 2, 12, 4, 7, 13, 1, 5, 0, 15, 10, 3, 9, 8, 6,
             4, 2, 1, 11, 10, 13, 7, 8, 15, 9, 12, 5, 6, 3, 0, 14,
             11, 8, 12, 7, 1, 14, 2, 13, 6, 15, 0, 9, 10, 4, 5, 3},
         { // S6
             12, 1, 10, 15, 9, 2, 6, 8, 0, 13, 3, 4, 14, 7, 5, 11,
             10, 15, 4, 2, 7, 12, 9, 5, 6, 1, 13, 14, 0, 11, 3, 8,
             9, 14, 15, 5, 2, 8, 12, 3, 7, 0, 4, 10, 1, 13, 11, 6,
             4, 3, 2, 12, 9, 5, 15, 10, 11, 14, 1, 7, 6, 0, 8, 13},
         { // S7
             4, 11, 2, 14, 15, 0, 8, 13, 3, 12, 9, 7, 5, 10, 6, 1,
             13, 0, 11, 7, 4, 9, 1, 10, 14, 3, 5, 12, 2, 15, 8, 6,
             1, 4, 11, 13, 12, 3, 7, 14, 10, 15, 6, 8, 0, 5, 9, 2,
             6, 11, 13, 8, 1, 4, 10, 7, 9, 5, 0, 15, 14, 2, 3, 12},
         { // S8
             13, 2, 8, 4, 6, 15, 11, 1, 10, 9, 3, 14, 5, 0, 12, 7,
             1, 15, 13, 8, 10, 3, 7, 4, 12, 5, 6, 11, 0, 14, 9, 2,
             7, 11, 4, 1, 9, 12, 14, 2, 0, 6, 10, 13, 15, 3, 5, 8,
             2, 1, 14, 7, 4, 10, 8, 13, 15, 12, 9, 0, 3, 5, 6, 11}};

    // The permutation table
    int permutation_tab[32] = {
        16, 7, 20, 21,
        29, 12, 28, 17,
        1, 15, 23, 26,
        5, 18, 31, 10,
        2, 8, 24, 14,
        32, 27, 3, 9,
        19, 13, 30, 6,
        22, 11, 4, 25};
    // The inverse permutation table
    int inverse_permutation[64] = {
        40, 8, 48, 16, 56, 24, 64, 32,
        39, 7, 47, 15, 55, 23, 63, 31,
        38, 6, 46, 14, 54, 22, 62, 30,
        37, 5, 45, 13, 53, 21, 61, 29,
        36, 4, 44, 12, 52, 20, 60, 28,
        35, 3, 43, 11, 51, 19, 59, 27,
        34, 2, 42, 10, 50, 18, 58, 26,
        33, 1, 41, 9, 49, 17, 57, 25};
    // Applying the initial permutation
    string perm = "";
    for (int i = 0; i < 64; i++)
    {
        perm += pt[initial_permutation[i] - 1]; // Return the permuted string
    } // end of for loop
    // Dividing the result into two equal halves
    string left = perm.substr(0, 32); // The left half
    string right = perm.substr(32, 32); // The right half
    // The plain text is encrypted 16 times
    for (int i = 0; i < 16; i++)
    {
        string right_expanded = "";
        // The right half of the plain text is expanded
        for (int i = 0; i < 48; i++)
        {
            right_expanded += right[expansion_table[i] - 1]; // Return the expanded string
        } // end of for loop
        // The result is xored with a key
        string xored = Xor(round_keys[i], right_expanded); // The result is passed through 8 substitution boxes
        string res = ""; // 
        // The result is divided into 8 equal parts and passed
        // through 8 substitution boxes. After passing through a
        // substituion box, each box is reduced from 6 to 4 bits.
        for (int i = 0; i < 8; i++)
        {
            // Finding row and column indices to lookup the substituition box
            string row1 = xored.substr(i * 6, 1) + xored.substr(i * 6 + 5, 1); // Finding row
            int row = convertBinaryToDecimal(row1); /// we convert the binary to decimal
            string col1 = xored.substr(i * 6 + 1, 4); // we find the column
            int col = convertBinaryToDecimal(col1); // we convert the binary to decimal
            int val = substition_boxes[i][row][col]; // we get the value from the substitution box
            res += convertDecimalToBinary(val); // we convert the decimal to binary
        }
        // Another permutation is applied
        string perm2 = "";
        for (int i = 0; i < 32; i++) // we loop through the permutation table 32 times
        {
            perm2 += res[permutation_tab[i] - 1]; // return the permuted string
        } // end of for loop
        // The result is xored with the left half
        xored = Xor(perm2, left);
        // The left and the right parts of the plain text are swapped
        left = xored;
        if (i < 15) // we check if i is less than 15 if true we swap the left and right
        {
            swap(left, right); // swap the left and right
        } // end of if
    } // end of for loop
    // The halves of the plain text are reversed
    string combined_text = left + right;
    // The final permutation is applied
    string ciphertext = "";
    for (int i = 0; i < 64; i++) // we loop through the inverse permutation table 64 times
    {
        ciphertext += combined_text[inverse_permutation[i] - 1]; // return the permuted string
    } // end of for loop
    // Return the ciphertext
    return ciphertext;
} // end of DES
// Function to perform expansion as per DES
string expansion(string input)
{
    int expansion_table[48] = {
        32, 1, 2, 3, 4, 5,
        4, 5, 6, 7, 8, 9,
        8, 9, 10, 11, 12, 13,
        12, 13, 14, 15, 16, 17,
        16, 17, 18, 19, 20, 21,
        20, 21, 22, 23, 24, 25,
        24, 25, 26, 27, 28, 29,
        28, 29, 30, 31, 32, 1};

    string expanded;
    // Expand the input
    for (int i = 0; i < 48; i++) // we loop through the expansion table 48 times
    {
        expanded += input[expansion_table[i] - 1]; // return the expanded string
    }
    return expanded; 
} // end of expansion

// Function to perform S-box substitution as per DES
string sBox(string input)
{
    int sBox[8][4][16] = {
{
             14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7,
             0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8,
             4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0,
             15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13},
         {
             15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10,
             3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5,
             0, 14, 7, 11, 10, 4, 13, 1, 5, 8, 12, 6, 9, 3, 2, 15,
             13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12, 0, 5, 14, 9},
         {
             10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8,
             13, 7, 0, 9, 3, 4, 6, 10, 2, 8, 5, 14, 12, 11, 15, 1,
             13, 6, 4, 9, 8, 15, 3, 0, 11, 1, 2, 12, 5, 10, 14, 7,
             1, 10, 13, 0, 6, 9, 8, 7, 4, 15, 14, 3, 11, 5, 2, 12},
         {
             7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15,
             13, 8, 11, 5, 6, 15, 0, 3, 4, 7, 2, 12, 1, 10, 14, 9,
             10, 6, 9, 0, 12, 11, 7, 13, 15, 1, 3, 14, 5, 2, 8, 4,
             3, 15, 0, 6, 10, 1, 13, 8, 9, 4, 5, 11, 12, 7, 2, 14},
         {
             2, 12, 4, 1, 7, 10, 11, 6, 8, 5, 3, 15, 13, 0, 14, 9,
             14, 11, 2, 12, 4, 7, 13, 1, 5, 0, 15, 10, 3, 9, 8, 6,
             4, 2, 1, 11, 10, 13, 7, 8, 15, 9, 12, 5, 6, 3, 0, 14,
             11, 8, 12, 7, 1, 14, 2, 13, 6, 15, 0, 9, 10, 4, 5, 3},
         {
             12, 1, 10, 15, 9, 2, 6, 8, 0, 13, 3, 4, 14, 7, 5, 11,
             10, 15, 4, 2, 7, 12, 9, 5, 6, 1, 13, 14, 0, 11, 3, 8,
             9, 14, 15, 5, 2, 8, 12, 3, 7, 0, 4, 10, 1, 13, 11, 6,
             4, 3, 2, 12, 9, 5, 15, 10, 11, 14, 1, 7, 6, 0, 8, 13},
         {
             4, 11, 2, 14, 15, 0, 8, 13, 3, 12, 9, 7, 5, 10, 6, 1,
             13, 0, 11, 7, 4, 9, 1, 10, 14, 3, 5, 12, 2, 15, 8, 6,
             1, 4, 11, 13, 12, 3, 7, 14, 10, 15, 6, 8, 0, 5, 9, 2,
             6, 11, 13, 8, 1, 4, 10, 7, 9, 5, 0, 15, 14, 2, 3, 12},
         {
             13, 2, 8, 4, 6, 15, 11, 1, 10, 9, 3, 14, 5, 0, 12, 7,
             1, 15, 13, 8, 10, 3, 7, 4, 12, 5, 6, 11, 0, 14, 9, 2,
             7, 11, 4, 1, 9, 12, 14, 2, 0, 6, 10, 13, 15, 3, 5, 8,
             2, 1, 14, 7, 4, 10, 8, 13, 15, 12, 9, 0, 3, 5, 6, 11,}
       
    }; // The s-boxes are used to substitute the bits
    // The input is divided into 8 equal parts
    string substituted;
    for (int i = 0; i < 8; i++) // we loop through the s-boxes 8 times
    {
        string row_str = input.substr(i * 6, 1) + input.substr(i * 6 + 5, 1); 
        int row = stoi(row_str, nullptr, 2); 
        string col_str = input.substr(i * 6 + 1, 4);
        int col = stoi(col_str, nullptr, 2);
        substituted += bitset<4>(sBox[i][row][col]).to_string(); 
    }
    return substituted;
}  // end of sBox
// Function to perform P-box permutation as per DES
string pBox(string input)
{
    int pBox_permutation[32] = {
        16, 7, 20, 21,
        29, 12, 28, 17,
        1, 15, 23, 26,
        5, 18, 31, 10,
        2, 8, 24, 14,
        32, 27, 3, 9,
        19, 13, 30, 6,
        22, 11, 4, 25};

    string pboxed;
    for (int i = 0; i < 32; i++)
    {
        pboxed += input[pBox_permutation[i] - 1];
    } // Return the p-boxed string
    return pboxed;
} // end of pBox

// Function to perform the Initial Permutation (IP)
string initialPermutation(string input)
{
    int initial_permutation[64] = {
        58, 50, 42, 34, 26, 18, 10, 2,
        60, 52, 44, 36, 28, 20, 12, 4,
        62, 54, 46, 38, 30, 22, 14, 6,
        64, 56, 48, 40, 32, 24, 16, 8,
        57, 49, 41, 33, 25, 17, 9, 1,
        59, 51, 43, 35, 27, 19, 11, 3,
        61, 53, 45, 37, 29, 21, 13, 5,
        63, 55, 47, 39, 31, 23, 15, 7};

    string permuted;
    for (int i = 0; i < 64; i++)
    {
        permuted += input[initial_permutation[i] - 1];
    }
    return permuted;
} // end of initialPermutation

// Function to perform the Final Permutation (IP^−1)
string finalPermutation(string input)
{
    int final_permutation[64] = {
        40, 8, 48, 16, 56, 24, 64, 32,
        39, 7, 47, 15, 55, 23, 63, 31,
        38, 6, 46, 14, 54, 22, 62, 30,
        37, 5, 45, 13, 53, 21, 61, 29,
        36, 4, 44, 12, 52, 20, 60, 28,
        35, 3, 43, 11, 51, 19, 59, 27,
        34, 2, 42, 10, 50, 18, 58, 26,
        33, 1, 41, 9, 49, 17, 57, 25};

    string permuted;
    for (int i = 0; i < 64; i++)
    {
        permuted += input[final_permutation[i] - 1];
    }
    return permuted;
}// end of finalPermutation

int main(){  
    // User input for key
    cout << "Enter the key (56-Bits BINARY): ";
    string key;
    cin >> key;

    // User input for plaintext
    cout << "Enter the plaintext(64-Bits BINARY): ";
    cin >> pt;

    // Calling the function to generate 16 keys
    generate_keys(key); 

    // Applying the algo
    string ct = DES(); 

    // Reversing the round_keys array for decryption
    string apt = pt;

    cout<<"--------------------------------------------------------------------------------"<<endl;
    // print the plain text
    cout<<"Plain text: "<<pt<<endl; 
    // Output the ciphertext
    cout << "Cipher Text: " << ct << endl;
    cout<<"--------------------------------------------------------------------------------"<<endl;
    
    // Expansion
    string expanded = expansion(pt);
    cout << "Expanded input: " << expanded << endl;
    cout<<"--------------------------------------------------------------------------------"<<endl;

    // P-box permutation
    string pboxed = pBox(expanded);
    cout << "P-boxed input: " << pboxed << endl;
    cout<<"--------------------------------------------------------------------------------"<<endl;

    // sbox
    string substituted = sBox(pt);
    cout << "Substituted input: " << substituted << endl;
    cout<<"--------------------------------------------------------------------------------"<<endl;

    // Initial permutation
    string ip = initialPermutation(pt);
    cout << "Initial Permutation: " << ip << endl;
    cout<<"--------------------------------------------------------------------------------"<<endl;

    // Final permutation
    string fp = finalPermutation(pt);
    cout << "Final Permutation: " << fp << endl;
    cout<<"--------------------------------------------------------------------------------"<<endl;

    // Reversing the round_keys array for decryption
	int i = 15;
	int j = 0;
	while(i > j)
	{
		string temp = round_keys[i];
		round_keys[i] = round_keys[j];
		round_keys[j] = temp;
		i--;
		j++;
	} // end of while loop
	pt = ct;
	string decrypted = DES();

	cout<<"Decrypted text: "<<decrypted<<endl;
    cout<<"--------------------------------------------------------------------------------"<<endl;
	// Comapring the initial plain text with the decrypted text
	if (decrypted == apt){ // we check if the decrypted text is equal to the initial plain text
		cout<<"Plain text encrypted and decrypted successfully."<<endl;
        cout<<"The decrypted text matches as the initial plain text."<<endl;
	}
    cout<<"--------------------------------------------------------------------------------"<<endl;
    return 0;
}  // end of main
// end of DES.cpp