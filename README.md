# Chord DHT Implementation in C++

## What is Chord DHT?

Chord Distributed Hash Table (DHT) is a scalable, distributed system that provides efficient lookup of keys in a peer-to-peer (P2P) network. It enables nodes to find the location of keys/values in a decentralized manner using a consistent hashing technique.

## Project Overview

This project implements a Chord DHT in C++ with the following components:

### Files Included

- **client.cpp, client.h**: Implements the client-side operations for interacting with the Chord DHT network.
  
- **constants.h**: Defines constants used throughout the project, such as chord ring size and hashing algorithms.
  
- **functions.cpp, functions.h**: Contains utility functions used across different modules of the project.
  
- **headers.h**: Includes all necessary header files across the project to simplify inclusion in other files.
  
- **main.cpp**: Entry point of the application, initializes nodes and demonstrates functionalities.
  
- **nodeInformation.cpp, nodeInformation.h**: Handles node-specific information and operations within the Chord DHT network.
  
- **server.cpp, server.h**: Implements the server-side operations for handling incoming requests and maintaining chord ring consistency.
  
- **threads.cpp, threads.h**: Manages multithreading aspects of the application, supporting concurrent operations.

### Functionality Implemented

1. **Node Joining and Stabilization**:
   - **joinNode**: Method to allow a new node to join an existing Chord DHT network.
   - **stabilize**: Periodic stabilization process to ensure the consistency of the chord ring.

2. **Key Lookup and Storage**:
   - **findSuccessor**: Locates the successor node responsible for a given key.
   - **store**: Stores key-value pairs in the appropriate node according to the Chord DHT algorithm.

3. **Node Failure Handling**:
   - **fixFingers**: Corrects finger tables in case of node failures or changes.
   - **handleFailure**: Implements mechanisms to handle failures gracefully within the network.

4. **Routing and Messaging**:
   - **sendMessage**: Sends messages between nodes using a reliable communication protocol.
   - **routeMessage**: Routes messages through the Chord DHT network to their intended recipients.

5. **Multithreading and Concurrency**:
   - **Thread management**: Utilizes threads for concurrent operations, enhancing performance and responsiveness.

## Getting Started

To compile and run this Chord DHT implementation:

1. Clone the repository:
2. Compile the project using a C++ compiler (e.g., g++): g++ -o myprogram main.cpp server.cpp nodeInformation.cpp client.cpp functions.cpp threads.cpp -lssl -lcrypto -lws2_32
3. Run the executable: myprogram.exe

## Usage

- Modify `main.cpp` to customize network initialization and demonstrate specific functionalities of the Chord DHT.
- Explore other files (`client.cpp`, `server.cpp`, etc.) to understand the implementation details and extend functionality as needed.

## Contributing

Contributions are welcome! Feel free to fork the repository, make improvements, and submit pull requests.




