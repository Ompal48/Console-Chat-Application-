// server.cpp (Windows Winsock Version)
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <fstream>
#include <algorithm>

#pragma comment(lib, "ws2_32.lib")

std::vector<SOCKET> clients;
std::mutex clients_mutex;

// log file + mutex
std::ofstream logFile("chat.log", std::ios::app);
std::mutex log_mutex;

// Simple XOR encryption
void xorCipher(std::string &data, const std::string &key) {
    if (key.empty()) return;
    for (size_t i = 0; i < data.size(); i++)
        data[i] ^= key[i % key.size()];
}

// thread‑safe log function
void logMessage(const std::string &msg) {
    std::lock_guard<std::mutex> lock(log_mutex);
    if (logFile.is_open()) {
        logFile << msg << std::endl;  // std::endl also flushes
    }
}

void broadcastMessage(const std::string &msg, SOCKET sender) {
    std::lock_guard<std::mutex> lock(clients_mutex);
    for (SOCKET client : clients) {
        if (client == sender) continue;
        send(client, msg.c_str(), static_cast<int>(msg.size()), 0);
    }
}

void handleClient(SOCKET clientSocket, std::string key) {
    char buffer[4096];

    while (true) {
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived <= 0) break;

        // make string from received bytes
        std::string msg(buffer, bytesReceived);

        // decrypt
        xorCipher(msg, key);

        // log decrypted message
        logMessage("Client " + std::to_string(clientSocket) + ": " + msg);

        // re‑encrypt with prefix for broadcast
        std::string encrypted = "Client " + std::to_string(clientSocket) + ": " + msg;
        xorCipher(encrypted, key);

        broadcastMessage(encrypted, clientSocket);
    }

    // Remove client and close socket
    {
        std::lock_guard<std::mutex> lock(clients_mutex);
        closesocket(clientSocket);
        clients.erase(std::remove(clients.begin(), clients.end(), clientSocket), clients.end());
    }
}

int main() {
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) {
        std::cerr << "WSAStartup failed\n";
        return 1;
    }

    if (!logFile.is_open()) {
        std::cerr << "Failed to open chat.log\n";
        WSACleanup();
        return 1;
    }

    SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
    if (listening == INVALID_SOCKET) {
        std::cerr << "Failed to create socket\n";
        WSACleanup();
        return 1;
    }

    sockaddr_in hint{};
    hint.sin_family = AF_INET;
    hint.sin_port = htons(4500);        // fixed port 4500
    hint.sin_addr.s_addr = INADDR_ANY;  // any local address

    if (bind(listening, (sockaddr*)&hint, sizeof(hint)) == SOCKET_ERROR) {
        std::cerr << "Bind failed\n";
        closesocket(listening);
        WSACleanup();
        return 1;
    }

    if (listen(listening, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Listen failed\n";
        closesocket(listening);
        WSACleanup();
        return 1;
    }

    std::string key = "mysecret";

    std::cout << "Server started on port 4500\n";

    while (true) {
        SOCKET client = accept(listening, nullptr, nullptr);
        if (client == INVALID_SOCKET) {
            std::cerr << "Accept failed\n";
            continue;
        }

        {
            std::lock_guard<std::mutex> lock(clients_mutex);
            clients.push_back(client);
        }

        std::thread(handleClient, client, key).detach();
    }

    // not really reached, but for completeness
    closesocket(listening);
    WSACleanup();
    return 0;
}
