#include <iostream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

using namespace std;

class Client {
public:
    int sockfd;
    struct sockaddr_in server_addr;

    Client() {
        // Create socket
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0) {
            cerr << "Error creating socket." << endl;
            exit(1);
        }

        // Initialize server address
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(6667);
        server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    }

    void connectToServer() {
        if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
            cerr << "Error connecting to server." << endl;
            exit(1);
        }
    }

    void sendMessage(string message) {
        write(sockfd, message.c_str(), message.length());
    }

    void disconnectFromServer() {
        close(sockfd);
    }
};

int main() {
    Client client;
    client.connectToServer();

    string message;
    while (true) {
        cout << "Enter message to send to server: ";
        getline(cin, message);
        if (message == "/QUIT") {
            break;
        }
        client.sendMessage(message);

        //read the server message
        string server_message = "";
        char buffer[1024];
        int n = read(client.sockfd, buffer, 1024);
        if (n < 0) {
            cerr << "Error reading data from server." << endl;
        } else {
            server_message.append(buffer, n);
            size_t pos = server_message.find("\r\n");
            while (pos == string::npos) {
                n = read(client.sockfd, buffer, 1024);
                if (n < 0) {
                    cerr << "Error reading data from server." << endl;
                    break;
                }
                server_message.append(buffer, n);
                pos = server_message.find("\r\n");
            }
            cout << "Server message: " << server_message.substr(0, pos) << endl;
        }
    }
    
    client.disconnectFromServer();
    return 0;
}