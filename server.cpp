#include <iostream>
#include <winsock2.h>
#include <ctime>

#pragma comment(lib, "ws2_32.lib")
#define PORT 8080

int main() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    SOCKET serverSocket = socket(AF_INET, SOCK_DGRAM, 0);

    sockaddr_in serverAddr{}, clientAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));
    int clientLen = sizeof(clientAddr);

    srand((unsigned int)time(0));
    int secretNumber = rand() % 100 + 1;
    int guessCount = 0;
    char buffer[1024];

    std::cout << "UDP Server started on port " << PORT << std::endl;
    std::cout << "Secret number is: " << secretNumber << std::endl;

    while (true) {
        memset(buffer, 0, sizeof(buffer));

        int bytesReceived = recvfrom(serverSocket, buffer, sizeof(buffer), 0,
                                     (sockaddr*)&clientAddr, &clientLen);

        if (bytesReceived == SOCKET_ERROR) {
            std::cerr << "recvfrom() failed\n";
            break;
        }

        buffer[bytesReceived] = '\0';
        std::string guessStr = buffer;

        if (guessStr == "exit") {
            std::cout << "Client exited.\n";
            continue;
        }

        int guess = atoi(guessStr.c_str());
        guessCount++;

        std::cout << "Client guessed: " << guess << std::endl;

        std::string response;
        if (guess < secretNumber) {
            response = "HIGHER";
        } else if (guess > secretNumber) {
            response = "LOWER";
        } else {
            response = "CORRECT! You guessed it in " + std::to_string(guessCount) + " tries!";
            std::cout << " Correct! Guessed in " << guessCount << " tries.\n";

            // Restart game
            secretNumber = rand() % 100 + 1;
            guessCount = 0;
            std::cout << "New secret number is: " << secretNumber << std::endl;
        }

        sendto(serverSocket, response.c_str(), response.size(), 0,
               (sockaddr*)&clientAddr, clientLen);
    }

    closesocket(serverSocket);
    WSACleanup();
    return 0;
}
