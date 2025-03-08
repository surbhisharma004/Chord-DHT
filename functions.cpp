#include "functions.h"

using namespace std;
// Global data map
DataMap dataMap;

// Mutex for thread safety
mutex dataMapMutex;

// Function to put data into the map
void putData(const string& key, const string& value) {
    // Lock the mutex to ensure thread safety
    lock_guard<mutex> lock(dataMapMutex);
    // Insert or update the value in the map
    dataMap[key] = value;
}

// Function to get data from the map
string getData(const string& key) {
    // Lock the mutex to ensure thread safety
    lock_guard<mutex> lock(dataMapMutex);
    // Search for the key in the map
    DataMap::iterator it = dataMap.find(key);
    if (it != dataMap.end()) {
        // Return the value if the key is found
        return it->second;
    } else {
        // Return an empty string if the key is not found
        return "";
    }
}

