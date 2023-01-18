#include "demo.hpp"

using namespace std;

void Server::start() {
    // Initialize server socket and bind to a port
    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(6667);
    listen_socket = socket(AF_INET, SOCK_STREAM, 0);
    bind(listen_socket, (struct sockaddr*)&server_address, sizeof(server_address));

    // Start listening for incoming connections
    listen(listen_socket, 5);

    // Run the main server loop
    run();
}

void Server::stop() {
    // Close all connected clients and the server socket
    for (map<int, User>::iterator it = clients.begin(); it != clients.end(); ++it) {
        close(it->first);
    }
    close(listen_socket);
}

void Server::run() {
    fd_set read_fds;
    int max_fd;
    while (true) {
        FD_ZERO(&read_fds);
        FD_SET(listen_socket, &read_fds);
        max_fd = listen_socket;

//        std::cout << "Server listened something" << std::endl;
        // Add all connected clients to the read set
        for (map<int, User>::iterator it = clients.begin(); it != clients.end(); ++it) {
            FD_SET(it->first, &read_fds);
            max_fd = max(max_fd, it->first);
        }

        // Wait for incoming data
        select(max_fd + 1, &read_fds, NULL, NULL, NULL);

        // Handle new connections
        if (FD_ISSET(listen_socket, &read_fds)) {
            int new_socket = accept(listen_socket, NULL, NULL);
            handle_connection(new_socket);
        }

        // Handle incoming data from connected clients
        for (map<int, User>::iterator it = clients.begin(); it != clients.end(); ++it) {
            std::cout << "FLARE" << std::endl;
            if (FD_ISSET(it->first, &read_fds)) {
                char buffer[4096];
                int bytes_received = recv(it->first, buffer, 4096, 0);
                if (bytes_received <= 0) {
                    handle_disconnection(it->first);
                } else {
                    string message(buffer, bytes_received);
                    handle_message(it->first, message);
                }
            }
        }
    }
}

void Server::handle_connection(int socket) {
//    std::cout << "Server is connected to client" << std::endl;

    // Send a welcome message to the new client
    string welcome_message = ":localhost 001 Welcome to the IRC server!\r\n";
    send(socket, welcome_message.c_str(), welcome_message.length(), 0);
}

void Server::handle_disconnection(int socket) {
//    std::cout << "Server is disconnected whatever the reason" << std::endl;

    // Remove the disconnected client from the client list
    clients.erase(socket);
    close(socket);
}

void Server::handle_message(int socket, string message) {
 //   std::cout << "Server got message and try to understand it" << std::endl;
    string command = message.substr(0, message.find(" "));
    if (command == "NICK") {
        // Handle the NICK command
        int startIndex = message.find(" ") + 1;
        string new_nickname = message.substr(startIndex);
        clients[socket].nickname = new_nickname;
        cout << "Client " << clients[socket].nickname << " changed their nickname to " << new_nickname << endl;
    } else if (command == "USER") {
        // Handle the USER command
        int startIndex = message.find(" ") + 1;
        string userInfo = message.substr(startIndex);
        int secondSpace = userInfo.find(" ");
        clients[socket].username = userInfo.substr(0, secondSpace);
        int thirdSpace = userInfo.find(" ", secondSpace + 1);
        clients[socket].hostname = userInfo.substr(secondSpace + 1, thirdSpace - secondSpace - 1);
        clients[socket].realname = userInfo.substr(thirdSpace + 1);
        cout << "Client " << clients[socket].nickname << " set their username to " << clients[socket].username << ", hostname to " << clients[socket].hostname << " and realname to " << clients[socket].realname << endl;
    } else if (command == "JOIN") {

        // Handle the JOIN command
        int startIndex = message.find(" ") + 1;
        string channel_name = message.substr(startIndex);
        if (channel_name[0] == '#') {
            if (channels.find(channel_name) == channels.end()) {
                // Create a new channel
                channels[channel_name] = Channel();
                channels[channel_name].name = channel_name;
            }
            // Add the user to the channel
            channels[channel_name].users.push_back(clients[socket]);
            clients[socket].channels.push_back(channel_name);
            cout << "Client " << clients[socket].nickname << " joined the channel " << channel_name << endl;
        } else {
            // Send an error message if the channel name is invalid
            string error_message = ":localhost 461 JOIN :Not enough parameters\r\n";
            send(socket, error_message.c_str(), error_message.length(), 0);
        }
    } else if (command == "PRIVMSG") {
        // Handle the PRIVMSG command
        int startIndex = message.find(" ") + 1;
        string privmsg = message.substr(startIndex);
        int secondSpace = privmsg.find(" ");
        string channel_name = privmsg.substr(0, secondSpace);
        if (find(clients[socket].channels.begin(), clients[socket].channels.end(), channel_name) != clients[socket].channels.end()) {
            string message_content = privmsg.substr(secondSpace + 1);
            cout << "Client " << clients[socket].nickname << " sent message to channel " << channel_name << ": " << message_content << endl;
        } else {
            // Send an error message if the user is not in the channel
            string error_message = ":localhost 403 " + clients[socket].nickname + " " + channel_name + " :No such channel\r\n";
            send(socket, error_message.c_str(), error_message.length(), 0);
        }
    } else {
        // Print the message to the console
        cout << "Received message from client " << clients[socket].nickname << ": " << message << endl;
    }
}

// void Server::handle_message(int socket, string message) {
//     // Parse the incoming message and handle the command
//     vector<string> message_parts = split(message, ' ');
//     string command = message_parts[0];
//     if (command == "NICK") {
//         // Handle the NICK command
//     } else if (command == "USER") {
//         // Handle the USER command
//     } else if (command == "JOIN") {
//         // Handle the JOIN command
//     } else if (command == "PART") {
//         // Handle the PART command
//     } else if (command == "PRIVMSG") {
//         // Handle the PRIVMSG command
//     } else if (command == "QUIT") {
//         // Handle the QUIT command
//     } else {
//         // Handle unknown commands
//     }
// }

int main() {
    Server server;
    server.start();
    server.stop();
    return 0;
}