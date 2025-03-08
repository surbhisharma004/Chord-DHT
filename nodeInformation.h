#ifndef NODEINFORMATION_H
#define NODEINFORMATION_H

#include "headers.h"
#include "functions.h"
#include "threads.h"
#include "server.h"
#include "constants.h"
#include "client.h"

using namespace std;

class NodeInformation {
private:
    long long int id;
    int port;
    string ip;
    pair<pair<string, int>, long long int> successor;
    pair<pair<string, int>, long long int> predecessor;
    vector<pair<pair<string, int>, long long int>> fingerTable;
    NodeInformation* mySelf; // Pointer to self node information

public:

    NodeInformation(); 

    int getPort();
    pair<pair<string, int>, long long int> getPredecessor();
    pair<pair<string, int>, long long int> getSuccessor();
    long long int getId();
    string getIp();
    vector<pair<pair<string,int>, long long int>> getFingerTable();


    void setPort(int newPort);
    void setId(long long int nodeId);
    void setIp(string ip);
    void setSuccessor(string ip, int port, long long int hash);
    void setPredecessor(string ip, int port, long long int hash);
    void setMySelf(NodeInformation* selfNode);
    void setFingerTable(string ip, int port, long long int hash);

    void print();
    void stabilize();
    void fixFinger();
    void createRing();
    void put(const string& key, const string& value);
    void get(const string& key, string& value);
    void join(const NodeInformation& x);
    void notifyPredecessor(NodeInformation& x);
    void notify(pair<pair<string, int>, long long int> node);
    long long int hexStringToLongLong(const string& hexStr);
    string compute_sha1(const string& input);
    pair< pair<string,int> , long long int > findSuccessor(long long int nodeId);
    pair< pair<string,int> , long long int > closestPrecedingNode(long long int nodeId);
    

};

bool inBetween(long long id, long long start, long long end, bool inclusive);
void stabilizeWrapper();
void fixFingerWrapper();

#endif 
