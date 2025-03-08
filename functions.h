#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "headers.h"
#include "nodeInformation.h"
#include "threads.h"
#include "server.h"
#include "constants.h"
#include "client.h"

typedef std::map<std::string, std::string> DataMap;

// Function to put data into the map
void putData(const std::string& key, const std::string& value);

// Function to get data from the map
std::string getData(const std::string& key);

#endif // FUNCTIONS_H
