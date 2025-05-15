#include "../../include/network/client.h"
#include <iostream>

// Di bagian atas file, tambahkan:
NetworkClient networkClient;

NetworkClient::NetworkClient() : clientSocket(INVALID_SOCKET), isConnected(false) {
    // Inisialisasi Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup gagal" << std::endl;
    }
}

NetworkClient::~NetworkClient() {
    disconnect();
    WSACleanup();
}

bool NetworkClient::connect(const char* serverIP, int port) {
    // Buat socket
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Pembuatan socket gagal" << std::endl;
        return false;
    }

    // Siapkan alamat server
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    inet_pton(AF_INET, serverIP, &serverAddr.sin_addr);

    // Hubungkan ke server
    if (::connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Koneksi ke server gagal" << std::endl;
        closesocket(clientSocket);
        clientSocket = INVALID_SOCKET;
        return false;
    }

    isConnected = true;
    return true;
}

void NetworkClient::disconnect() {
    if (clientSocket != INVALID_SOCKET) {
        closesocket(clientSocket);
        clientSocket = INVALID_SOCKET;
    }
    isConnected = false;
}

bool NetworkClient::sendCommand(const std::string& command) {
    if (!isConnected) {
        std::cerr << "Tidak terhubung ke server" << std::endl;
        return false;
    }

    int result = send(clientSocket, command.c_str(), command.length(), 0);
    if (result == SOCKET_ERROR) {
        std::cerr << "Gagal mengirim data" << std::endl;
        disconnect();
        return false;
    }
    return true;
}

std::string NetworkClient::receiveResponse() {
    if (!isConnected) {
        return "Tidak terhubung ke server";
    }

    char buffer[1024] = {0};
    int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    
    if (bytesRead <= 0) {
        std::cerr << "Koneksi terputus atau error" << std::endl;
        disconnect();
        return "Koneksi terputus";
    }

    buffer[bytesRead] = '\0';
    return std::string(buffer);
}