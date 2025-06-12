#include <iostream>
#include <thread>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define SERVER_IP "127.0.0.1"
#define PORT 8080
#define BUFFER_SIZE 1024

void receive_messages(int server_socket) {
    char buffer[BUFFER_SIZE];
    int bytes_received;

    while (true) {
        bytes_received = recv(server_socket, buffer, BUFFER_SIZE - 1, 0);
        if (bytes_received <= 0) {
            std::cout << "\nDisconnected from server.\n";
            break;
        }

        buffer[bytes_received] = '\0';
        std::cout << buffer << std::endl;
    }
}

int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        std::cerr << "Socket creation failed.\n";
        return 1;
    }

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr);

    if (connect(sock, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Connection to server failed.\n";
        return 1;
    }

    std::string name;
    std::cout << "Enter your name: ";
    std::getline(std::cin, name);
    send(sock, name.c_str(), name.length(), 0);

    std::thread receiver(receive_messages, sock);
    receiver.detach();

    std::string message;
    while (true) {
        std::getline(std::cin, message);
        if (message == "/exit") {
            break;
        }
        send(sock, message.c_str(), message.length(), 0);
    }

    close(sock);
    return 0;
}
