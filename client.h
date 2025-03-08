#pragma once

#include "headers.h"
#include "nodeInformation.h"
#include "functions.h"
#include "threads.h"
#include "server.h"
#include "constants.h"

void client(const std::string& ip, int port, const std::string& message);
