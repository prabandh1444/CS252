#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <fstream>
#include <string.h>
#include <cstring>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <algorithm>
#include <vector>
using namespace std;

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Error: expected one argument 'portNum' \n");
        exit(1);
    }
    int server;
    int server_port = stoi(argv[1]);
    server = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr;
    struct sockaddr_in their_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET; 
    server_addr.sin_port = htons(server_port); 
    server_addr.sin_addr.s_addr = INADDR_ANY; 
    memset(&(server_addr.sin_zero), '\0', 8); 
    int bind_err = bind(server, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)); 
    if(bind_err == -1){
        fprintf(stderr, "Error: Bind on the given port Failed \n");
        exit(2);
    }
    int lis_err = listen(server,10);
    cout<<"BindDone: "<< server_port<<endl;
    if(lis_err !=-1){
        cout<<"ListenDone: "<< server_port<<endl;
    }

    fd_set master;
    FD_ZERO(&master);
    FD_SET(server, &master);
    int fdmax = server;

    while(true){
        fd_set read_fds = master;
        int activity = select(fdmax+1, &read_fds, NULL, NULL, NULL);
        for (int i = 0; i <= fdmax; i++) {
            if (FD_ISSET(i, &read_fds)) {
                if (i == server) {
                    int sin_size = sizeof(struct sockaddr_in);
                    socklen_t len = static_cast<socklen_t>(sin_size);
                    int new_client = accept(server, (struct sockaddr *)&their_addr, &len);
                    if (new_client < 0) {
                        fprintf(stderr, "Error: Failed to accept new connection\n");
                        continue;
                    }
                    else {
                        FD_SET(new_client, &master);
                        if (new_client > fdmax) {
                            fdmax = new_client; 
                        }
                        cout<<"Client: "<< (their_addr.sin_addr.s_addr)<<" : "<<their_addr.sin_port<<endl;
                    }
                }
                else {
                    char buffer[1024];
                    memset(buffer, 0, 1024);
                    int size_of_filename = recv(i, buffer, 1024, 0);
                    if (size_of_filename == 0) {
                        close(i); 
                        FD_CLR(i, &master); 
                    } else {
                    if (strncmp(buffer, "get ", 4) != 0) {
                    cout << "UnknownCmd" << endl;
                    fprintf(stderr, "Error: Invalid request format from client\n");
                    exit(1);
                    }
                    string filename(buffer + 4, size_of_filename);
                    cout << "FileRequested: " << filename << endl;
                    ifstream infile(filename);
                    int total_sent = 0;
                    while (infile) {
                    infile.read(buffer, 1024);
                    int bytes_read = infile.gcount();
                    if (bytes_read > 0) {
                    int bytes_sent = send(i, buffer, bytes_read, 0);
                    total_sent += bytes_sent;
                    }
                    }
                    cout << "TransferDone: " << total_sent << " bytes" << endl;
                    close(i); 
                    FD_CLR(i, &master); 
                    }
                }
            }
        }
     }
        return 0;
    }
