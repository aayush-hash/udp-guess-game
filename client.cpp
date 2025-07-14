#include <iostream>
#include <winsock2.h>
#include <string>

#pragma comment(lib, "ws2_32.lib")
#define PORT 8080

int main() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    SOCKET clientSocket = socket(AF_INET, SOCK_DGRAM, 0);

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    char buffer[1024];

    std::cout << "Welcome to the UDP Guessing Game!\n";
    std::cout << "Type your guess (1–100) or 'exit' to quit.\n";

    while (true) {
        std::string input;
        std::cout << "\nEnter your guess: ";
        std::getline(std::cin, input);

        if (input.empty()) continue;

        sendto(clientSocket, input.c_str(), input.size(), 0,
               (sockaddr*)&serverAddr, sizeof(serverAddr));

        if (input == "exit") {
            std::cout << "Exiting game...\n";
            break;
        }

        int serverLen = sizeof(serverAddr);
        memset(buffer, 0, sizeof(buffer));
        int bytesReceived = recvfrom(clientSocket, buffer, sizeof(buffer), 0,
                                     (sockaddr*)&serverAddr, &serverLen);
        if (bytesReceived == SOCKET_ERROR) {
            std::cerr << "Error receiving from server.\n";
            break;
        }

        buffer[bytesReceived] = '\0'; // Null-terminate message
        std::string response = buffer;

        std::cout << "Server: " << response << std::endl;

        // Optional: detect win and show celebration
        if (response.find("CORRECT") != std::string::npos) {
            std::cout << response << " Game restarting...\n";
        }
    }

    closesocket(clientSocket);
    WSACleanup();
    return 0;
}
