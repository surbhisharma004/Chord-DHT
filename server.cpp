#include "server.h"

void server() {
    // Define server and client address structures
    struct sockaddr_in srv, client_address;
    NodeInformation nodeInfo;

    // Set mySelf
    nodeInfo.setMySelf(&nodeInfo);

    int nSocket, clientSocket, n;
    socklen_t client_len;
    char buffer[256]; // Buffer to store data received from clients

    // Initialize Winsock
    WSADATA wsData;
    int wsOk = WSAStartup(MAKEWORD(2, 2), &wsData);
    if (wsOk != 0) {
        cout << "Can't start Winsock! " << wsOk << endl;
        return;
    }

    // Create a socket
    nSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (nSocket == INVALID_SOCKET) {
        cout << "Socket could not be opened. An error occurred." << endl;
        WSACleanup();
        return;
    }
    cout << "Socket created successfully." << endl;

    // Configure the server address structure
    memset(&srv, 0, sizeof(srv));
    srv.sin_addr.s_addr = INADDR_ANY;
    srv.sin_family = AF_INET;
    srv.sin_port = htons(nodeInfo.getPort());

    // Bind the socket to the server address and port
    if (bind(nSocket, (struct sockaddr*)&srv, sizeof(srv)) == SOCKET_ERROR) {
        cout << "Server: Socket already taken. Force taking...." << endl;
        int yes = 1;
        if (setsockopt(nSocket, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char*>(&yes), sizeof(yes)) == -1) {
            cout << "setsockopt failed: " << strerror(errno) << endl;
            closesocket(nSocket);
            WSACleanup();
            return;
        }
    }
    cout << "Socket bound successfully." << endl;

    while (true) {
        // Listen for incoming connections
        if (listen(nSocket, 1000) == SOCKET_ERROR) {
            cout << "Error in listen: " << strerror(errno) << endl;
            closesocket(nSocket);
            WSACleanup();
            return;
        }
        cout << "Listening for connections..." << endl;

        // Accept an incoming connection
        client_len = sizeof(client_address);
        clientSocket = accept(nSocket, (struct sockaddr*)&client_address, &client_len);
        if (clientSocket == INVALID_SOCKET) {
            cout << "Server: Not accepting new socket " << strerror(errno) << endl;
            closesocket(nSocket);
            WSACleanup();
            return;
        }
        cout << "Accepted connection from client." << endl;

        // Clear the buffer and receive data from the client
        memset(buffer, 0, sizeof(buffer));
        n = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (n == SOCKET_ERROR) {
            cout << "Server: Error reading from socket " << strerror(errno) << endl;
            closesocket(clientSocket);
            closesocket(nSocket);
            WSACleanup();
            return;
        }
        cout << "Received data: " << buffer << endl;

        // Process "notify" command from the client
        if (strncmp(buffer, "notify", 6) == 0) 
        {
            int i = 7;
            char arr[20] = {0};
            while (buffer[i] != ' ' && buffer[i] != '\0') {
                arr[i - 7] = buffer[i];
                i++;
            }
            arr[i - 7] = '\0';

            long long int id = atoll(arr);
            string ip;
            int port;
            sscanf(buffer + i, "%s %d", &ip[0], &port);

            NodeInformation node;
            node.setId(id);
            node.setSuccessor(ip, port, id);

            node.notify(make_pair(make_pair(ip, port), id));

            // Send response to the client
            string response = "Message Received.";
            send(clientSocket, response.c_str(), response.length(), 0);
            cout << "Response sent to client." << endl;
        } 

        // Process "pred" command from the client
        else if (strncmp(buffer, "pred", 4) == 0) 
        {
            int i = 5;
            char arr[20] = {0};
            while (buffer[i] != ' ' && buffer[i] != '\0') {
                arr[i - 5] = buffer[i];
                i++;
            }
            arr[i - 5] = '\0';

            long long int id = atoll(arr);
            string ip;
            int port;
            sscanf(buffer + i, "%s %d", &ip[0], &port);

            NodeInformation node;
            node.setId(id);
            node.setPredecessor(ip, port, id);

            nodeInfo.notifyPredecessor(node);

            // Send response to the client
            string response = "ok pred";
            send(clientSocket, response.c_str(), response.length(), 0);
            cout << "Predecessor updated and response sent to client." << endl;
        }

        //process succ command
        else if (strncmp(buffer, "succ", 4) == 0) 
        {
            int i = 5;
            char command[256];
            NodeInformation x;
            char ypred[256] = {0};  // Initialize to zero
            char arr[20] = {0};

            // Extract the ID from the buffer
            while (buffer[i] != ' ' && buffer[i] != '\0') {
                arr[i - 5] = buffer[i];
                i++;
            }
            arr[i - 5] = '\0';
            x.setId(atoll(arr));

            // Extract the IP and port
            int j = 0;
            i++;  // Move past the space
            while (buffer[i] != ' ' && buffer[i] != '\0') {
                command[j++] = buffer[i++];
            }
            command[j] = '\0';

            string ip;
            int port;
            sscanf(command, "%s %d", &ip[0], &port);
            x.setSuccessor(ip, port, x.getId());

            // If the new successor is different from the current one
            if (x.getId() != nodeInfo.getSuccessor().second) {
                if (nodeInfo.getSuccessor().second != nodeInfo.getId()) {
                    // Notify the current successor to update its predecessor
                    string successorIp = nodeInfo.getSuccessor().first.first;
                    int successorPort = nodeInfo.getSuccessor().first.second;
                    snprintf(ypred, sizeof(ypred), "pred %lld %s %d", x.getId(), ip.c_str(), port);

                    //`client` is a function that sends messages to other nodes
                    // The function should handle the communication and error checking
                    client(successorIp, successorPort, ypred);
                }

                // Update the successor in the current node and finger table
                nodeInfo.setSuccessor(ip, port, x.getId());
                vector<pair<pair<string,int>, long long int>> fingerTable = nodeInfo.getFingerTable();
                fingerTable[1] = {{ip, port}, x.getId()};

            }

            // Send acknowledgment to the client
            string response = "ok succ";
            n = send(clientSocket, response.c_str(), response.length(), 0);
            if (n == SOCKET_ERROR) {
                cerr << "SERVER: ERROR, writing to socket" << endl;
                closesocket(clientSocket);
                WSACleanup();
                exit(1);
            }
        }

        else if (strncmp(buffer, "findPredecessor", 15) == 0) 
        {
            // Construct the response with predecessor information
            auto predecessor = nodeInfo.getPredecessor();
            string response = to_string(predecessor.first.second) + " " + predecessor.first.first + " " + to_string(predecessor.second);

            // Write the response to the socket
            n = send(clientSocket, response.c_str(), response.length(), 0);
            if (n < 0) {
                cerr << "SERVER: ERROR, writing to socket" << endl;
                exit(1);
            }
        }
 
        else if (strncmp(buffer, "findSuccessor", 13) == 0) 
        {
            int id;
            char ip[256];
            int port;

            // Parse the ID from the buffer
            sscanf(buffer + 14, "%d %s %d", &id, ip, &port);

            // Call findSuccessor function to get the successor node
            pair<pair<string, int>, long long int> successorInfo = nodeInfo.findSuccessor(id);

            // Extract IP, port, and ID from the successorInfo pair
            string successorIP = successorInfo.first.first;
            int successorPort = successorInfo.first.second;
            long long int successorId = successorInfo.second;

            // Create a NodeInformation object representing the successor node
            NodeInformation y;
            y.setIp(successorIP);
            y.setPort(successorPort);
            y.setId(successorId);

            // Construct the response with the successor information
            string response = to_string(y.getId()) + " " + y.getSuccessor().first.first + " " + to_string(y.getSuccessor().first.second);

            // Write the response to the socket
            n = send(clientSocket, response.c_str(), response.length(), 0);
            if (n < 0) {
                cerr << "ERROR writing to socket" << endl;
                exit(1);
            }
        }
        else if (strncmp(buffer, "getdata", 7) == 0)
        {
            int i;
            char key[20], d[20];
            for (i = 0; i < 21; ++i)
                key[i] = '\0';
            sscanf(buffer, "%s %s", d, key);
            string value = getData(key); // Assuming getData returns a string
            n = send(clientSocket, value.c_str(), value.length(), 0);
            if (n < 0)
            {
                cerr << "SERVER: ERROR writing to socket" << endl;
                closesocket(clientSocket);
                WSACleanup();
                exit(1);
            }
        }

        else if (strncmp(buffer, "putdata", 7) == 0)
        {
            int i;
            char key[20], value[20], d[20];
            for (i = 0; i < 20; ++i)
            {
                key[i] = '\0';
                value[i] = '\0';
            }
            sscanf(buffer, "%s %s %s", d, key, value);
            putData(key, value);
        }
        else
        {
            cout << "SERVER: I am not supposed to match this case" << endl;
            cout << buffer << endl;
            fflush(stdout);
        }
        
        // Close the client socket
        closesocket(clientSocket);
    }

    // Close the server socket and clean up Winsock
    closesocket(nSocket);
    WSACleanup();
}
