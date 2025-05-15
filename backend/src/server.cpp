#include "../include/server.h"
#include "../include/command_handler.h"
#include "../include/database.h"
#include "../include/user_manager.h"
#include <iostream>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <thread>

#pragma comment(lib, "Ws2_32.lib")

#define PORT 5000

// Fungsi untuk menangani koneksi client
void handleClient(SOCKET clientSocket) {
    std::string username = "";
    bool isLoggedIn = false;
    
    while (true) {
        char buffer[1024] = {0};
        int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
        
        if (bytesRead <= 0) {
            // Koneksi terputus
            if (isLoggedIn) {
                // Hapus dari daftar pengguna aktif
                UserManager::getInstance().removeActiveUser(username);
            }
            break;
        }
        
        std::string data(buffer, bytesRead);
        std::cout << "[Server] Menerima: " << data << std::endl;
        
        std::string response = processCommand(data, username, isLoggedIn);
        
        send(clientSocket, response.c_str(), response.length(), 0);
    }
    
    closesocket(clientSocket);
}

void startServer() {
    // Inisialisasi database
    db::initDatabase();
    
    // Inisialisasi Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) {
        std::cerr << "WSAStartup gagal" << std::endl;
        return;
    }
    
    // Buat socket
    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Pembuatan socket gagal" << std::endl;
        WSACleanup();
        return;
    }
    
    // Siapkan alamat server
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);
    
    // Bind socket
    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Bind gagal" << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return;
    }
    
    // Listen
    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Listen gagal" << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return;
    }
    
    std::cout << "[Server] Mendengarkan pada port " << PORT << "...\n";
    
    // Terima koneksi
    while (true) {
        SOCKET clientSocket;
        sockaddr_in clientAddr;
        int clientAddrSize = sizeof(clientAddr);
        
        clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientAddrSize);
        if (clientSocket == INVALID_SOCKET) {
            std::cerr << "Accept gagal" << std::endl;
            continue;
        }
        
        // Dapatkan informasi client
        char clientIP[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &clientAddr.sin_addr, clientIP, INET_ADDRSTRLEN);
        std::cout << "[Server] Koneksi baru dari " << clientIP << std::endl;
        
        // Buat thread baru untuk menangani client
        std::thread clientThread(handleClient, clientSocket);
        clientThread.detach(); // Lepaskan thread agar berjalan secara independen
    }
    
    closesocket(serverSocket);
    WSACleanup();
}