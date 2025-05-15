#pragma once
#include <winsock2.h>

// Fungsi utama server
void startServer();
void handleClient(SOCKET clientSocket);