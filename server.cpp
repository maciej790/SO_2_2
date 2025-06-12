#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <cstring>
#include <algorithm>      // konieczne dla std::remove_if
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

constexpr int PORT = 8080;
constexpr int MAX_BUFFER = 1024;

struct ConnectedClient {
    int socket_fd;
    std::string username;
};

std::vector<ConnectedClient> connectedClients;
std::mutex clientsLock;

void sendToAll(const std::string& msg, int exclude_fd) {
    std::lock_guard<std::mutex> guard(clientsLock);
    for (auto& client : connectedClients) {
        if (client.socket_fd != exclude_fd) {
            send(client.socket_fd, msg.c_str(), msg.size(), 0);
        }
    }
}

void clientSession(int client_fd) {
    char buffer[MAX_BUFFER];
    ssize_t recv_len = recv(client_fd, buffer, MAX_BUFFER - 1, 0);
    if (recv_len <= 0) {
        close(client_fd);
        return;
    }
    buffer[recv_len] = '\0';
    std::string userName(buffer);

    {
        std::lock_guard<std::mutex> guard(clientsLock);
        connectedClients.push_back({client_fd, userName});
    }

    std::string joinMsg = ">> " + userName + " has connected.\n";
    std::cout << joinMsg;
    sendToAll(joinMsg, client_fd);

    while (true) {
        recv_len = recv(client_fd, buffer, MAX_BUFFER - 1, 0);
        if (recv_len <= 0) {
            break;
        }
        buffer[recv_len] = '\0';
        std::string chatMsg = userName + ": " + buffer;
        std::cout << chatMsg;
        sendToAll(chatMsg, client_fd);
    }

    {
        std::lock_guard<std::mutex> guard(clientsLock);
        connectedClients.erase(
            std::remove_if(
                connectedClients.begin(),
                connectedClients.end(),
                [client_fd](const ConnectedClient& c) { return c.socket_fd == client_fd; }),
            connectedClients.end()
        );
    }

    std::string leaveMsg = ">> " + userName + " has disconnected.\n";
    std::cout << leaveMsg;
    sendToAll(leaveMsg, client_fd);

    close(client_fd);
}

int main() {
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0) {
        std::cerr << "Failed to create socket\n";
        return 1;
    }

    int enable = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable));

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(listen_fd, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Bind operation failed\n";
        close(listen_fd);
        return 1;
    }

    if (listen(listen_fd, SOMAXCONN) < 0) {
        std::cerr << "Listening failed\n";
        close(listen_fd);
        return 1;
    }

    std::cout << "Server running on port " << PORT << '\n';

    while (true) {
        sockaddr_in client_addr{};
        socklen_t client_len = sizeof(client_addr);
        int client_fd = accept(listen_fd, (sockaddr*)&client_addr, &client_len);

        if (client_fd >= 0) {
            std::thread(clientSession, client_fd).detach();
        }
    }

    close(listen_fd);
    return 0;
}
