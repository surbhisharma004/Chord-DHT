#include "client.h"

using namespace std;

void client(const string& ip, int port, const string& message) {
    WSADATA wsaData;
    int sockfd, n;
    struct sockaddr_in serverAddr;
    char buffer[256];

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cerr << "CLIENT: Winsock initialization failed" << endl;
        exit(1);
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        cerr << "Error opening socket" << endl;
        WSACleanup();
        exit(1);
    }

    memset((char*)&serverAddr, 0, sizeof(serverAddr));

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = inet_addr(ip.c_str());
    if (connect(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        cerr << "CLIENT: ERROR, Host with ip => [" << ip << "] and port => [" << port << "] Network Error." << endl;
        closesocket(sockfd);
        WSACleanup();
        exit(1);
    }

    // Ensure buffer is clean and copy the message to buffer
    memset(buffer, 0, sizeof(buffer));
    strncpy(buffer, message.c_str(), sizeof(buffer) - 1);

    n = send(sockfd, buffer, strlen(buffer), 0);
    if (n < 0) {
        cerr << "CLIENT: ERROR writing to socket" << endl;
    }

    memset(buffer, 0, sizeof(buffer));
    n = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
    if (n < 0) {
        cerr << "CLIENT: ERROR reading from socket" << endl;
    } else {
        cout << "CLIENT: Received response: " << buffer << endl;
    }

    closesocket(sockfd);
    WSACleanup();
}
