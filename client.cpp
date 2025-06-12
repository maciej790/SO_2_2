#include <iostream>
#include <thread>
#include <string>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

constexpr int BUFFER_LEN = 1024;
constexpr char SERVER_ADDR[] = "127.0.0.1";
constexpr int SERVER_PORT = 8080;

void listenForMessages(int socket_fd) {
    char buffer[BUFFER_LEN];
    ssize_t recv_len;

    while (true) {
        recv_len = recv(socket_fd, buffer, BUFFER_LEN - 1, 0);
        if (recv_len <= 0) {
            std::cout << "\nLost connection to server.\n";
            break;
        }
        buffer[recv_len] = '\0';
        std::cout << buffer << std::endl;
    }
}

int main() {
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        std::cerr << "Could not create socket\n";
        return 1;
    }

    sockaddr_in server{};
    server.sin_family = AF_INET;
    server.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_ADDR, &server.sin_addr);

    if (connect(sock_fd, (sockaddr*)&server, sizeof(server)) < 0) {
        std::cerr << "Unable to connect to server\n";
        return 1;
    }

    std::cout << "Please enter your username: ";
    std::string username;
    std::getline(std::cin, username);
    send(sock_fd, username.c_str(), username.length(), 0);

    std::thread receiverThread(listenForMessages, sock_fd);
    receiverThread.detach();

    std::string inputMsg;
    while (true) {
        std::getline(std::cin, inputMsg);
        if (inputMsg == "/exit") {
            break;
        }
        send(sock_fd, inputMsg.c_str(), inputMsg.length(), 0);
    }

    close(sock_fd);
    return 0;
}
