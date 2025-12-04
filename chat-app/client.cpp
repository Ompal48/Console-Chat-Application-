// client.cpp (Windows Winsock Version)
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>
#include <thread>

#pragma comment(lib, "ws2_32.lib")

std::string key = "mysecret";

// XOR encryption
void xorCipher(std::string &data) {
    for (size_t i = 0; i < data.size(); i++)
        data[i] ^= key[i % key.size()];
}

void receiveMessages(SOCKET sock) {
    char buffer[4096];
    while (true) {
        int bytes = recv(sock, buffer, sizeof(buffer), 0);
        if (bytes <= 0) break;

        std::string msg(buffer, bytes);
        xorCipher(msg);

        std::cout << msg << std::endl;
    }
}

int main() {
    WSADATA wsa;
    WSAStartup(MAKEWORD(2,2), &wsa);

    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(4500);
    inet_pton(AF_INET, "127.0.0.1", &server.sin_addr);

    connect(sock, (sockaddr*)&server, sizeof(server));

    std::cout << "Connected to chat server.\n";

    std::thread receiver(receiveMessages, sock);
    receiver.detach();

    while (true) {
        std::string msg;
        std::getline(std::cin, msg);
        xorCipher(msg);
        send(sock, msg.c_str(), msg.size(), 0);
    }

    closesocket(sock);
    WSACleanup();
    return 0;
}
