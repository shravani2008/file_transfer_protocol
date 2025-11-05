#include <iostream> //Includes the standard library for input/output operations like
#include <fstream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <cstdint>   // For uint32_t type

#pragma comment(lib, "ws2_32.lib")  //Links the Winsock library during compilation.

using namespace std;

#define SERVER "localhost" //  Defines server address, port, and the name of the file that will be saved locally
#define PORT "5000"        // Server port
#define FILENAME "downloaded_test.txt" // Name of downloaded file

// Defines server address, port, and the name of the file that will be saved locall
unsigned int calculateCRC(const string &data) {
    unsigned int crc = 0xFFFFFFFF;
    for (char c : data) {
        crc ^= (unsigned char)c;
        for (int i = 0; i < 8; i++) {
            if (crc & 1)
                crc = (crc >> 1) ^ 0xEDB88320;
            else
                crc >>= 1;
        }
    }
    return crc ^ 0xFFFFFFFF;
}

int main() {
    WSADATA wsaData;//Initializes Winsock for network programming. Checks if initialization failed
    if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) {
        cout << "WSAStartup failed!" << endl;
        return 1;
    }

    addrinfo hints = {}, *res;//Prepares address info structure for connecting to server via IPv6 and TCP.
    hints.ai_family = AF_INET6;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(SERVER, PORT, &hints, &res) != 0) {  //Resolves server address based on hints
        cout << "getaddrinfo failed!" << endl;
        WSACleanup();
        return 1;
    }

    SOCKET sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sock == INVALID_SOCKET) {
        cout << "Socket creation failed!" << endl;
        freeaddrinfo(res);
        WSACleanup();
        return 1;//Creates a socket to connect to server; checks for failure.
    }

    if (connect(sock, res->ai_addr, (int)res->ai_addrlen) == SOCKET_ERROR) {
        cout << "Connect failed!" << endl;
        closesocket(sock);
        freeaddrinfo(res);
        WSACleanup();
        return 1;//Connects the client socket to the server address
    }

    freeaddrinfo(res);
    cout << "Connected to server!" << endl;  //Connects the client socket to the server address

    // Receive file size
   uint32_t fileSize;
    recv(sock, (char*)&fileSize, sizeof(fileSize), 0);

    // Receive file data
    string fileData(fileSize, '\0');
    int received = 0;
    while (received < fileSize) {
        int bytes = recv(sock, &fileData[received], fileSize - received, 0);
        if (bytes <= 0) break;
        received += bytes;
    }//receives the actual file data from server in a loop until all bytes are received

    // Receive CRC from server
    unsigned int serverCRC;
    recv(sock, (char*)&serverCRC, sizeof(serverCRC), 0);

    // Save file locally
    ofstream outFile(FILENAME, ios::binary);
    outFile.write(fileData.c_str(), fileData.size());
    outFile.close();

    // Calculate CRC locally
    unsigned int clientCRC = calculateCRC(fileData);

    cout << "Received file saved as: " << FILENAME << endl;//Checks if CRC matches; prints success or corruption message.
    cout << "Server CRC: " << serverCRC << " | Calculated CRC: " << clientCRC << endl;

    if (serverCRC == clientCRC)
        cout << "File received correctly (No errors)" << endl;
    else
        cout << " CRC mismatch! File corrupted" << endl; //Checks if CRC matches; prints success or corruption message.

    closesocket(sock);//Closes client socket, cleans up Winsock, and ends program.
    WSACleanup();
    return 0;
}
