#pragma once
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

class NetworkClient {
private:
    SOCKET clientSocket;
    bool isConnected;

public:
    NetworkClient();
    ~NetworkClient();
    
    bool connect(const char* serverIP, int port);
    void disconnect();
    bool sendCommand(const std::string& command);
    std::string receiveResponse();
    bool isClientConnected() const { return isConnected; }
};