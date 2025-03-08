//g++ -o myprogram main.cpp server.cpp nodeInformation.cpp client.cpp functions.cpp threads.cpp -lssl -lcrypto -lws2_32

#include <iostream>
#include <string>
#include <map>
#include <thread>
#include "nodeInformation.h"
#include "client.h"
#include "server.h"
#include "constants.h"
#include "functions.h"
#include "threads.h"

using namespace std;

bool serverStarted = false;

NodeInformation mySelf;
vector<pair<pair<string, int>, long long int>> fingerTable = mySelf.getFingerTable();
pair<pair<string, int>, long long int> successor = mySelf.getSuccessor();
pair<pair<string, int>, long long int> predecessor = mySelf.getPredecessor();

void getInput() {
    string command, temp;
    cout << "Type 'help' for commands" << endl << endl;

    while (true) {
        cout << "Command > ";
        getline(cin, command);

        if (command.substr(0, 4) == "help") {
            cout << "Commands:\n1)create(to create a ring)\n2)get(to get data corresponding)\n3)join(To join into ring)\n4)port(to set port)\n5)put(to put data into ring)\n6)finger\n7)successor\n8)predecessor\n9)dump\n10)quit(to quit the ring)\n";
        } else if (command.substr(0, 4) == "quit") {
            cout << "Quitting the ring." << endl;
            exit(0);
        } else if (command.substr(0, 4) == "port") {
            int port;
            istringstream iss(command);
            iss >> temp >> port;
            mySelf.setPort(port);

            if (mySelf.getPort() < 1024 || mySelf.getPort() > 65535) {
                cout << "Invalid port number. Setting default port 4444" << endl;
                mySelf.setPort(4444);
            } else {
                cout << "Port value is set to " << mySelf.getPort() << endl;
            }
        } else if (command.substr(0, 6) == "create") {
            mySelf.createRing();
        } else if (command.substr(0, 4) == "join") {
            NodeInformation x;
            string ip;
            int port;
            istringstream iss(command);
            iss >> temp >> ip >> port;
            x.setIp(ip);
            x.setPort(port);
            mySelf.join(x);
        } else if (command.substr(0, 3) == "put") {
            string key, value;
            istringstream iss(command);
            iss >> temp >> key >> value;
            mySelf.put(key, value);
        } else if (command.substr(0, 3) == "get") {
            string key, value;
            istringstream iss(command);
            iss >> temp >> key;
            mySelf.get(key, value); 
            cout << "Value: " << value << endl;
        } else if (command.substr(0, 6) == "finger") {
            for (int i = 1; i < 33; ++i) {
                cout << "fingerTable[" << i << "] => " << fingerTable[i].first.first << endl;
            }
        } else if (command.substr(0, 4) == "dump") {
            cout << "-------NODE INFO---------" << endl;
            cout << "Successor at - " << successor.first.first << " : " << successor.first.second << endl;
            cout << "Predecessor at - " << predecessor.first.first << " : " << predecessor.first.second << endl;
            cout << "---------Finger table entries------" << endl;
            for (int i = 1; i < 33; ++i) {
                cout << "---------fingerTable[" << i << "]  -------" << endl;
                cout << "IP = " << fingerTable[i].first.first << " . PORT = " << fingerTable[i].first.second << " . ID = " << fingerTable[i].second << endl;                
            }
        } else if (command.substr(0, 11) == "predecessor") {
            cout << "Predecessor is ip => [" << predecessor.first.first << "] and port => [" << predecessor.first.second << "]" << endl;
        } else if (command.substr(0, 9) == "successor") {
            cout << "Successor is ip => [" << successor.first.first << "] and port => [" << successor.first.second << "]" << endl;
        } else {
            cout << "Command not valid try again" << endl;
        }
    }
}

int main() {
    create(getInput);
    start();

    return 0;
}
