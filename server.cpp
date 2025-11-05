#include <iostream>      // For input/output
#include <fstream>       // For file handling
#include <winsock2.h>    // For Winsock networking
#include <ws2tcpip.h>    // For IPv6 support
#include <cstdint>   // For uint32_t type
 
#pragma comment(lib, "ws2_32.lib") // Link Winsock library

using namespace std;

#define PORT "5000"      // Port number server will listen on
#define FILENAME "test.txt" // Fixed file to send

// Simple CRC32 calculation function
unsigned int calculateCRC(const string &data) {
    unsigned int crc = 0xFFFFFFFF;  //start with all 1's binary
    for (char c : data) {  //takes the every character from input
        crc ^= (unsigned char)c;    //xor the current character with CRC register
        for (int i = 0; i < 8; i++) {    //each character is processed by bit 8 times
            if (crc & 1)
                crc = (crc >> 1) ^ 0xEDB88320;
            else
                crc >>= 1;
        }
    }
    return crc ^ 0xFFFFFFFF;  //inverts all bits to make it compatible with standard crc32 algorithm
}

int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) { // Initialize Winsock or network programming. Checks if initialization failed
        cout << "WSAStartup failed!" << endl;
        return 1;
    }

    // Setup server address info (IPv6)
    addrinfo hints = {}, *res;
    hints.ai_family = AF_INET6;       // IPv6
    hints.ai_socktype = SOCK_STREAM;  // TCP
    hints.ai_flags = AI_PASSIVE;      // For binding

    if (getaddrinfo(NULL, PORT, &hints, &res) != 0) { // Get address info
        cout << "getaddrinfo failed!" << endl;
        WSACleanup();
        return 1;
    }

    SOCKET serverSocket = socket(res->ai_family, res->ai_socktype, res->ai_protocol); // Create socket
    if (serverSocket == INVALID_SOCKET) {
        cout << "Socket creation failed!" << endl;
        freeaddrinfo(res);
        WSACleanup();
        return 1;  //Creates a TCP/IPv6 socket for the server. Checks if creation failed
    }

    if (bind(serverSocket, res->ai_addr, (int)res->ai_addrlen) == SOCKET_ERROR) { // Bind socket
        cout << "Bind failed!" << endl;
        closesocket(serverSocket);
        freeaddrinfo(res);
        WSACleanup();
        return 1;  //Binds the server socket to the specified port
    }

    freeaddrinfo(res); // No longer needed

    if (listen(serverSocket, 1) == SOCKET_ERROR) { // Start listening
        cout << "Listen failed!" << endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;//Starts listening for client connections with backlog of 1
    }

    cout << "Server listening on port " << PORT << "..." << endl;  //Shows that the server is ready and listening

    SOCKET clientSocket = accept(serverSocket, NULL, NULL); // Accept client connection
    if (clientSocket == INVALID_SOCKET) {
        cout << "Accept failed!" << endl;
        closesocket(serverSocket);//Accepts a connection from a client; checks for errors
        WSACleanup();
        return 1;
    }

    cout << "Client connected!" << endl;

    // Open the file to send
    ifstream file(FILENAME, ios::binary);
    if (!file) {
        cout << "File not found: " << FILENAME << endl;
        closesocket(clientSocket);
        closesocket(serverSocket);
        WSACleanup();//Opens the file to send in binary mode. Checks if file exist
        return 1;
    }

    // Read file content into string
    string fileData((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());

    // Send file size first
    uint32_t fileSize = fileData.size();
    send(clientSocket, (char*)&fileSize, sizeof(fileSize), 0);

    // Send file data
    send(clientSocket, fileData.c_str(), fileData.size(), 0);

    // Calculate CRC and send to client
    unsigned int crc = calculateCRC(fileData);
    send(clientSocket, (char*)&crc, sizeof(crc), 0);

    cout << "File sent successfully with CRC: " << crc << endl;

    // Close sockets and cleanup
    closesocket(clientSocket);
    closesocket(serverSocket);
    WSACleanup();

    return 0;
}
