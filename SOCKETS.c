/***********************************************************************************************************************************************************
 * Author: Bryan Mendoza
 * Date October 18, 2023
 * 
 * Description: This program will send an email to a specified email address using the SMTP protocol. 
 *      The program will read the email content from a file and send it to the specified email address.
 *      The IP used is (BLANK) which is the IP Address needed. Port (25) is the protocol used to send
 *      email. Once connected via socket, the program will send the email. My (content.txt) file contains the body of the email. The email
 *      address used is my own email address. The email address is also the sender and recipient. The subject of the email is "CS484 - I 
 *      can program with sockets!".
 *
****************************************************** Start of Project 1  *********************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h> 
#include <string.h> 
#include <netdb.h> //for hostname resolution and network-related functions


// No need to include email addressses (from or to in the .txt file)    
// .txt file will only include body of message and closing             
// Define your sender address, recipient address, and email subject here
#define FROM_ADDRESS "<SOME EMAIL ADDRESS" // Define who you are (own email)
#define TO_ADDRESS "<SOME EMAIL ADDRESS" // Define who you are forwarding to
#define EMAIL_SUBJECT "CS484 - I can program with sockets!" // Define subject here


// Function to read the email content from a file
int readEmailContent(const char* emailFilename, char** emailData) {
    FILE* file = fopen(emailFilename, "r");  // Open the email file for reading
    if (file == NULL) {  // Check if the file opening failed
        perror("Error opening email file");
        return -1;
    } // end if

    fseek(file, 0, SEEK_END);  // Move the file pointer to the end to get the file size
    long fileSize = ftell(file);  // Get the file size
    fseek(file, 0, SEEK_SET);  // Reset the file pointer to the beginning

    *emailData = (char*)malloc(fileSize + 1);  // Allocate memory to store email content
    if (*emailData == NULL) {  // Check if memory allocation failed
        perror("Error allocating memory for email data");
        fclose(file);
        return -1;
    } // end if

    size_t bytesRead = fread(*emailData, 1, fileSize, file);  // Read the email content from the file
    if (bytesRead != fileSize) {  // Check if reading the email data failed
        perror("Error reading email data");
        free(*emailData);
        fclose(file);
        return -1;
    } // end if 

    (*emailData)[bytesRead] = '\0';  // Null-terminate the email data

    fclose(file);  // Close the email file
    return 0;
} // end readEmailContent

// Function to send a command to the SMTP server
int sendCommand(int clientSocket, const char* command) {
    write(clientSocket, command, strlen(command));  // Send the specified command to the server
    return 0;
} // end sendCommand

// Function to read the SMTP server response
int readResponse(int clientSocket, char* response, size_t responseSize) {
    ssize_t bytesRead = read(clientSocket, response, responseSize - 1);  // Read the server response
    if (bytesRead == -1) {  // Check if reading from the server failed
        perror("Error reading from the server");
        return -1;
    }
    response[bytesRead] = '\0';  // Null-terminate the response
    return 0;
} // end readResponse

// Function to perform the SMTP conversation
int performSMTPConversation(int clientSocket, const char *emailData) {
    char serverResponse[1024];  // Buffer for storing server responses

    // Read and display the initial server response
    if (readResponse(clientSocket, serverResponse, sizeof(serverResponse) - 1) == -1) {
        return -1;
    } // end if
    printf("Server Response: %s", serverResponse);

    // Send the EHLO command to identify yourself to the server
    sendCommand(clientSocket, "EHLO example.com\r\n");

    // Add more SMTP commands to set recipients and the email content
    sendCommand(clientSocket, "MAIL FROM: " FROM_ADDRESS "\r\n");
    sendCommand(clientSocket, "RCPT TO: " TO_ADDRESS "\r\n");
    sendCommand(clientSocket, "DATA\r\n");
    sendCommand(clientSocket, "Subject: " EMAIL_SUBJECT "\r\n");
    sendCommand(clientSocket, "From: " FROM_ADDRESS "\r\n");
    sendCommand(clientSocket, "To: " TO_ADDRESS "\r\n");
    sendCommand(clientSocket, "\r\n");  // Empty line before email body
    sendCommand(clientSocket, emailData);  // Email body
    sendCommand(clientSocket, ".\r\n");  // End of email
    sendCommand(clientSocket, "QUIT\r\n");  // Quit the conversation

    // Read and display server response
    if (readResponse(clientSocket, serverResponse, sizeof(serverResponse) - 1) == -1) {
        return -1;
    } // end if 
    printf("Server Response: %s", serverResponse);

    return 0;
} // end perform SMTPConversation

int main(int argc, char* argv[]) {

    if (argc != 4) {  // Check if the correct number of command line arguments is provided
        printf("\nWarning: \n");
        printf("/******* You need SMTP server ip, port and email file name ********/\n");
        fprintf(stderr, "Usage: %s <SMTP Server IP> <SMTP Server Port> <Email Filename>\n", argv[0]);
        exit(1);
    } // end if

    char* smtpServerIP = argv[1];  // Get the SMTP server IP address from command line
    int smtpServerPort = atoi(argv[2]);  // Get the SMTP server port from command line
    char* emailFilename = argv[3];  // Get the email filename from command line

    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);  // Create a socket for communication
    if (clientSocket == -1) {  // Check if socket creation failed
        perror("Socket creation failed");
        exit(1);
    } // end if 

    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(smtpServerPort);

    struct hostent* server = gethostbyname(smtpServerIP);  // Resolve the SMTP server hostname
    if (server == NULL) {  // Check if hostname resolution failed
        perror("Error resolving SMTP server hostname");
        close(clientSocket);
        exit(1);
    } // end if
    bcopy((char*)server->h_addr, (char*)&serverAddr.sin_addr.s_addr, server->h_length);

    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        // Connect to the SMTP server
        perror("Connection to SMTP server failed");
        close(clientSocket);
        exit(1);
    } // end if 

    printf("Connected to the SMTP server\n");

    // Read email content from the file
    char* emailData = NULL;
    if (readEmailContent(emailFilename, &emailData) == -1) {
        close(clientSocket);
        exit(1);
    } // end if 

    // Perform the SMTP conversation and check the return value
    if (performSMTPConversation(clientSocket, emailData) == -1) {
        close(clientSocket);
        fprintf(stderr, "Email sending failed\n");
        exit(1);
    } else {
        printf("Email sent successfully to the recipient\n");
    } // end if/else

    // Close the socket
    close(clientSocket);

    // Free allocated memory
    free(emailData);

    return 0;
} // end main