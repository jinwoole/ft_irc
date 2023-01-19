#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

using namespace std;

const int PORT = 6667;

struct User {
    string nickname;
    string username;
    string hostname;
    string realname;
};

User parseUserCommand(string command) {
    User user;
    vector<string> parts;
    stringstream ss(command);
    string token;
    int j = 5;

    while (getline(ss, token, ' ')) {
        parts.push_back(token);
    }
    user.nickname = parts[1];
    user.username = parts[2];
    user.hostname = parts[3];
    string realname = parts[4];
    if (realname[0] == ':') {
        realname.erase(0, 1);
    }
    for (int i = 0; i < parts.size() - 4; i++)
    {
        realname += (" " + parts[j]);
        j++;
    }

    user.realname = realname;
    return user;
}

int main() {
    // Create a socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        cerr << "Error creating socket" << endl;
        return 1;
    }

    // Bind the socket to a port
    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(sock, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        cerr << "Error binding socket" << endl;
        return 1;
    }

    // Listen for incoming connections
    listen(sock, 5);

    // Accept a connection
    int client_sock = accept(sock, NULL, NULL);
    if (client_sock < 0) {
        cerr << "Error accepting connection" << endl;
        return 1;
    }

    while (true) {
        // Receive data from the client
        char buffer[1024];
        int bytes_received = recv(client_sock, buffer, sizeof(buffer), 0);
        if (bytes_received < 0) {
            cerr << "Error receiving data" << endl;
            return 1;
        } else if (bytes_received == 0) {
            // Connection closed by the client
            cout << "Connection closed by the client" << endl;
            break;
        }

        // Process the received data
        string command(buffer, bytes_received);
        stringstream ss(command);
        string line;

        while(getline(ss, line, '\n')){
            stringstream lineStream(line);

            std::cout << line << endl;
            string firstWord;
            lineStream >> firstWord;            
            if (firstWord == "CAP") {

            }
            else if (firstWord == "NICK") {
                string nickname;
                lineStream >> nickname;
                cout << "Nickname: " << nickname << endl << endl;
            } else if (firstWord == "USER") {
                User user = parseUserCommand(line);
                cout << "Nickname: " << user.nickname << endl;
                cout << "Username: " << user.username << endl;
                cout << "Hostname: " << user.hostname << endl;
                cout << "Realname: " << user.realname << endl;
            } else {
                cout << "Unknown command: " << line << endl;
            }
        }
    }

    close(client_sock);
    return 0;
}
