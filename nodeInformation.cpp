#include "nodeInformation.h"


char targetIP[16];
int targetPort;

char predBuffer[256];	// find predecessor buffer
char succBuffer[256];	// find successor buffer
char notBuffer[256]; // notify buffer
char getBuffer[256]; // getData buffer
char putBuffer[256]; // putData buffer

NodeInformation::NodeInformation(){
	fingerTable = vector< pair< pair<string,int> , long long int > >(M+1);
}

// Compute the SHA1 hash of the input and convert it to a long long
string NodeInformation::compute_sha1(const string& input) {
    unsigned char hash[SHA_DIGEST_LENGTH];
    SHA1(reinterpret_cast<const unsigned char*>(input.c_str()), input.length(), hash);

    // Convert the first 8 bytes of the hash to a hexadecimal string
    stringstream ss;
    ss << hex << setfill('0');
    for (int i = 0; i < 8; ++i) {
        ss << setw(2) << static_cast<int>(hash[i]);
    }
    return ss.str();
}

// initialise the finger table
void NodeInformation::setFingerTable(string ip, int port, long long int hash){
    for(int i=0;i<=M;i++){
        fingerTable[i].first.first = ip;
        fingerTable[i].first.second = port;
        fingerTable[i].second = hash;
    }
}

// to access the finger table
vector< pair< pair<string,int> , long long int > > NodeInformation::getFingerTable(){
	return fingerTable;
}

// Notify function implementation
void NodeInformation::notify(pair<pair<string, int>, long long int> node) {
    predecessor = node;

    if (successor.second == id) {
        successor = node;
    }
}

// Get the port of the node
int NodeInformation::getPort() {
    return port;
}

// Set the port of the node
void NodeInformation::setPort(int newPort) {
    port = newPort;
}

// Get the ID of the node
long long int NodeInformation::getId() {
    return id;
}

//Set the ID of the node
void NodeInformation::setIp(string nodeIp){
    ip = nodeIp;
}

// Set the ID of the node
void NodeInformation::setId(long long int nodeId) {
    id = nodeId;
}

// Set the successor node
void NodeInformation::setSuccessor(string ip, int port, long long int hash) {
    successor.first.first = ip;
    successor.first.second = port;
    successor.second = hash;
}

// Set the predecessor node
void NodeInformation::setPredecessor(string ip, int port, long long int hash) {
    predecessor.first.first = ip;
    predecessor.first.second = port;
    predecessor.second = hash;
}

// Get the predecessor node
pair<pair<string, int>, long long int> NodeInformation::getPredecessor() {
    return predecessor;
}

// Get the successor node
pair<pair<string, int>, long long int> NodeInformation::getSuccessor() {
    return successor;
}

string NodeInformation::getIp(){
    return ip;
}


// Print the node information
void NodeInformation::print() {
    cout << "Node Id: " << id << endl;
    cout << "Node IP: " << ip << endl;
    cout << "Port: " << port << endl;
}

// Set mySelf node
void NodeInformation::setMySelf(NodeInformation* selfNode) {
    mySelf = selfNode;
}

// Helper function to check if a node's ID falls between two other IDs
bool inBetween(long long id, long long start, long long end, bool inclusive) {
    if (start < end) {
        return inclusive ? (id >= start && id <= end) : (id > start && id < end);
    } else {
        return inclusive ? (id >= start || id <= end) : (id > start || id < end);
    }
}

// Notify predecessor function implementation
void NodeInformation::notifyPredecessor(NodeInformation& x) {
    if (mySelf->getId() == x.getId()) return;

    if ((predecessor.first.second == 0) || inBetween(x.getId(), predecessor.second, mySelf->getId(), false)) {
        predecessor = make_pair(make_pair(x.getPredecessor().first.first, x.getPredecessor().first.second), x.getPredecessor().second);
        cout << "NOTIFY: Setting Predecessor to " << predecessor.first.first << ":" << predecessor.first.second << endl;
    }
}


// Find the successor node for a given key and return it as a NodeInformation object
pair< pair<string,int> , long long int > NodeInformation::findSuccessor(long long int key) {
    // If the key is between current node and its successor, return the successor
    if (inBetween(key, id, successor.second, false)) {
        return successor;
    } 
    // Otherwise, find the closest preceding node in the finger table
    else {
        for (int i = M; i >= 0; i--) {
            if (fingerTable[i].second != id && inBetween(fingerTable[i].second, id, key, false)) {
                successor.first.first = fingerTable[i].first.first;
                successor.first.second = fingerTable[i].first.second;
                successor.second = fingerTable[i].second;
                return successor;
            }
        }
    }
    // If no closer node found in the finger table, return the current node's successor
    return successor;
}

void NodeInformation::stabilize() {
    char command[256];
    NodeInformation x;
    while (1) {
        //sleep(100);
        if (this->id != this->successor.second || this->id != this->fingerTable[1].second) {
            strcpy(targetIP, this->successor.first.first.c_str());
            targetPort = this->successor.first.second;
            //printf("STABILIZE: Target IP %s and port %d\nsuccessor ip %s and successor port %d \n", targetIP, targetPort, this->successor.first.first.c_str(), this->successor.first.second);fflush(stdout);
            for (int i = 0; i < 255; ++i) predBuffer[i] = '\0';
            strcpy(predBuffer, "findPredecessor");
            //printf("STABILIZE: predBuffer value : ");puts(predBuffer);printf("\n");fflush(stdout);
            client(successor.first.first, successor.first.second, predBuffer);

            char arr[20];
            int i = 0;
            while (predBuffer[i] != ' ') {
                arr[i] = predBuffer[i];
                i++;
            }
            arr[i] = '\0';
            x.id = atoi(arr);

            //for(int i = 0; i < 17; i++) x.id[i] = predBuffer[i];
            int j = 0;
            for (; i < 256; i++) command[j++] = predBuffer[i];
            sscanf(command, "%s %d", x.ip, &(x.port));
            for (int i = 0; i < 255; ++i) predBuffer[i] = '\0';

            if (x.port != 0 && inBetween(x.id, this->id, this->successor.second, 0)) {
                this->successor.second = x.id;
                this->fingerTable[1].second = x.id;
                this->successor.first.first = x.ip;
                this->fingerTable[1].first.first = x.ip;
                this->successor.first.second = x.port;
                this->fingerTable[1].first.second = x.port;
                //printf("STABILIZE: successor updated to %s:%d\n", this->successor.first.first.c_str(), this->successor.first.second);
            }

            //call for notify
            strcpy(targetIP, this->successor.first.first.c_str());
            targetPort = this->successor.first.second;
            // printf(" Contact for notify target IP %s and target Port %d\nsuccessor ip %s and successor port %d \n",targetIP, targetPort, this->successor.first.first.c_str(), this->successor.first.second);
            for (int i = 0; i < 255; ++i) notBuffer[i] = '\0';
            sprintf(notBuffer, "notify %d %s %d", this->id, this->ip.c_str(), this->port);
            //Tell successor to check if I am your predecessor.
            client(targetIP, targetPort, notBuffer);

            for (int i = 0; i < 256; i++) notBuffer[i] = '\0';
        }
        Sleep(10);
    }
}


void NodeInformation::join(const NodeInformation& x) {
    char command[256];
    char id[100];
    for (int i = 0; i < 100; ++i) {
        id[i] = '\0';
    }

    // Generate the ID for this node based on its IP and port
    sprintf(id, "%s:%d", ip.c_str(), port);
    string hash = compute_sha1(string(id));
    long long int hashed_id = std::stoll(hash, nullptr, 16);
    setId(hashed_id);

    // Initialize successor, predecessor, and finger table with this node's information
    setSuccessor(ip, port, hashed_id);
    setPredecessor(ip, port, hashed_id);
    setFingerTable(ip, port, hashed_id);

    // Send a message to the provided node to find its successor
    sprintf(succBuffer, "findSuccessor %lld %s %d", id, ip.c_str(), port);
    client(x.ip, x.port, succBuffer);

    // Parse the response to get the successor information
    char arr[20];
    int i = 0;
    while (succBuffer[i] != ' ') {
        arr[i] = succBuffer[i];
        i++;
    }
    arr[i] = '\0';
    long long int successorId = atoll(arr);

    int j = 0;
    for (; i < 255; i++) {
        command[j++] = succBuffer[i];
    }
    sscanf(command, "%s %d", successor.first.first, &(successor.first.second));

    // Update successor ID and finger table entry
    successor.second = successorId;
    fingerTable[1].second = successorId;

    fingerTable[1].first.first, successor.first.first;
    fingerTable[1].first.second = successor.first.second;

    // Notify the successor to update its predecessor
    sprintf(notBuffer, "notify %lld %s %d", id, ip.c_str(), port);
    client(successor.first.first, successor.first.second, notBuffer);
}

void NodeInformation::fixFinger() {
    int next = 0;
    while (true) {
        next = next + 1;
        if (next > M) {
            next = 1;
            Sleep(10 * 1000); // Sleep for 10 seconds
        }
        long long int fingerId = (mySelf->getId() + static_cast<long long>(pow(2, next - 1))) % static_cast<long long>(pow(2, M));
        fingerTable[next] = findSuccessor(fingerId);
    }
}


long long int NodeInformation::hexStringToLongLong(const string& hexStr) {
    stringstream ss;
    ss << hex << hexStr.substr(0, 16); // Convert the first 16 characters to a long long
    long long int result;
    ss >> result;
    return result;
}


// Define a standalone function to call stabilize
void stabilizeWrapper() {
    NodeInformation nodeInfo;
    nodeInfo.stabilize();
}

// Define a standalone function to call fixFinger
void fixFingerWrapper() {
    NodeInformation nodeInfo;
    nodeInfo.fixFinger();
}

// Set up the ring and start the necessary threads
void NodeInformation::createRing() {
    // Generate the ID for this node based on its IP and port
    string id = ip + ":" + to_string(port);
    string hash = compute_sha1(id);
    long long int hashed_id = hexStringToLongLong(hash);
    setId(hashed_id);

    cout << "My ID => [" << getId() << "], IP => [" << ip << "], Port => [" << port << "]" << endl;

    // Initialize successor, predecessor, and finger table with this node's information
    setSuccessor(ip, port, hashed_id);
    cout << "Successor initialized: " << successor.first.first << ":" << successor.first.second << ", ID: " << successor.second << endl;
    setPredecessor(ip, port, hashed_id);
    cout << "Predecessor initialized: " << predecessor.first.first << ":" << predecessor.first.second << ", ID: " << predecessor.second << endl;
    setFingerTable(ip, port, hashed_id);
    cout << "Finger Table Initialised" << endl;

    // Create and run the necessary threads
    cout << "Creating server thread..." << endl;
    int stid = create(server);
    cout << "Server thread created with ID: " << stid << endl;
    run(stid);

    cout << "Creating stabilization thread..." << endl;
    int staid = create(stabilizeWrapper);
    cout << "Stabilization thread created with ID: " << staid << endl;
    run(staid);

    cout << "Creating fixFinger thread..." << endl;
    int ffid = create(fixFingerWrapper);
    cout << "FixFinger thread created with ID: " << ffid << endl;
    run(ffid);

    cout << "CREATE: A new ring is created with IP:Port = " << id << endl;
}


void NodeInformation::put(const string& key, const string& value) {
    NodeInformation x, y;
    x.setIp("");
    x.setPort(0);
    cout << "PUT: Putting key " << key << " and value " << value << endl;
    string hash = compute_sha1(key);
    long long int hashed_id = hexStringToLongLong(hash);
    x.setId(hashed_id);

    // Find the node on which we have to put the value
    pair<pair<string, int>, long long int> successor_y = findSuccessor(x.getId());
    cout << "PUT: FIND successor returned id => [" << successor_y.second << "] ip => [" << successor_y.first.first << "] port=>[" << successor_y.first.second << "]" << endl;

    strcpy(targetIP, y.getIp().c_str());
    targetPort = y.getPort();
    memset(putBuffer, 0, 255);
    sprintf(putBuffer, "putdata %d %s", x.getId(), value.c_str());
    cout << "PUT: sending data to ip =>[" << targetIP << "] and port =>[" << targetPort << "]" << endl;

    client(y.getIp(), y.getPort(), putBuffer);
}

void NodeInformation::get(const string& key, string& value) {
    NodeInformation x, y;
    x.setIp("");
    x.setPort(0);
    value.clear();
    
    string hash = compute_sha1(key);
    long long int hashed_id = hexStringToLongLong(hash);
    x.setId(hashed_id);
    pair<pair<string, int>, long long int> successor_y = findSuccessor(x.getId());
    
    cout << "GET: key => [" << key << "] hash of key => [" << x.getId() << "] successor ip => [" << y.getIp() << "] and port => [" << y.getPort() << "]" << endl;

    strcpy(targetIP, y.getIp().c_str());
    targetPort = y.getPort();

    memset(getBuffer, 0, 255);
    sprintf(getBuffer, "getdata %d", x.getId());
    client(targetIP, targetPort, getBuffer); // Retrieve value corresponding to the key

    sscanf(getBuffer, "%s", &value);
    if (value.empty()) {
        cerr << "Value NOT found" << endl;
    } else {
        cout << "Value for the key [" << key << "] is [" << value << "]" << endl;
    }
}
