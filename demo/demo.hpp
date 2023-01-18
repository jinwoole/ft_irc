#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <sstream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

using namespace std;

class User {
    public:
        int socket;
        string nickname;
        string username;
        string hostname;
        string realname;
        string quitmsg;
        vector<string> channels;
};

class Channel {
    public:
        string name;
        vector<User> users;
        string topic;
};

class Server {
    public:
        int listen_socket;
        map<int, User> clients;
        map<string, Channel> channels;
        
        void start();
        void stop();
        void run();
        void handle_connection(int socket);
        void handle_disconnection(int socket);
        void handle_message(int socket, string message);
};