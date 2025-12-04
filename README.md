Encrypted Console Chat Application (C++ / Winsock)
Overview
This project is a console-based client–server chat application built in C++ for Windows using the Winsock API.
The server listens on port 4500 and supports multiple concurrent clients, allowing them to exchange messages in real time.
Messages are protected with a simple symmetric XOR cipher and every decrypted message is logged to a file for auditing and debugging.

Features
TCP client–server architecture (IPv4, stream sockets)

Multi‑client support using C++ threads on the server

Simple symmetric XOR “encryption” layer

Thread‑safe logging of all messages to chat.log

Console‑based UI, easy to run and test during interviews or demos

Architecture
Server

Creates a TCP socket, binds to port 4500, and starts listening.

Accepts incoming client connections and spawns a new thread per client.

Receives encrypted messages, decrypts them using a shared key, logs the plaintext message, re‑encrypts with a prefix (Client <id>:), and broadcasts to all other connected clients.

Manages connected clients in a shared std::vector<SOCKET> protected by a std::mutex.

Client

Connects to the server using IP address (e.g., 127.0.0.1) and port 4500.

Reads user input from the console, encrypts it with the same XOR key, and sends it to the server.

Listens for broadcast messages from the server, decrypts them, and prints them to the console.

Encryption

Uses a simple XOR cipher with a shared key string.

The same function is used for both encryption and decryption, since applying XOR twice with the same key recovers the original text.

Intended for educational purposes only, not real‑world security.

Logging

The server opens chat.log in append mode at startup.

Each decrypted message is written as:

Client <socket_id>: <message>

Logging is wrapped in a logMessage() function that uses a std::mutex to ensure thread‑safe access from multiple client threads.

Technologies Used
C++ (modern C++ features, STL)

Winsock2 for TCP networking on Windows

std::thread and std::mutex for concurrency

std::ofstream for file logging

Getting Started
Prerequisites
Windows OS

C++ compiler with Winsock support (e.g., MinGW‑w64 g++, MSVC)

Git (optional, for cloning the repository)

Clone the Repository
bash
git clone <your-repo-url>
cd chat-app   # or your folder name
Build (MinGW g++ example)
From a Developer Command Prompt / PowerShell in the project folder:

powershell
g++ server.cpp -o server.exe -lws2_32
g++ client.cpp -o client.exe -lws2_32
If you use Visual Studio, create a Console project, add server.cpp / client.cpp, and link against ws2_32.lib.

Running the Application
1. Start logging viewer (optional, for demo)
In Terminal 1:

powershell
cd "C:\Users\Acer\OneDrive\Desktop\c++\chat-app"   # adjust path
Get-Content .\chat.log -Wait
This shows new log lines as they are written by the server.

2. Start the server
In Terminal 2:

powershell
cd "C:\Users\Acer\OneDrive\Desktop\c++\chat-app"
.\server.exe
You should see a message like:

text
Server started on port 4500
3. Start one or more clients
In Terminal 3:

powershell
cd "C:\Users\Acer\OneDrive\Desktop\c++\chat-app"
.\client.exe 127.0.0.1 4500
Open additional terminals and run the same client command to simulate multiple users.

4. Chat
Type messages in any client window and press Enter.

All connected clients should see the broadcast message (after decryption on their side).

Terminal 1 (log viewer) should show each message in plaintext as it is received and logged by the server.

Project Structure
server.cpp – Winsock TCP chat server, multi‑client handling, encryption, logging.

client.cpp – Console chat client, encryption, send/receive logic.

chat.log – Log file created at runtime; contains all server‑side message history.

Possible Improvements
Replace XOR cipher with a real encryption library (e.g., TLS or a standard crypto library).

Add user names / login instead of using socket IDs.

Implement a /quit command for clean disconnection from the client.

Add message timestamps to the log.

Create a GUI front‑end (Qt, WinForms, or web) on top of the same protocol.

