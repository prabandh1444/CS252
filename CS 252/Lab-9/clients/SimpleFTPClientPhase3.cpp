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
using namespace std;

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Error: expected three arguments 'portNum','fileToTransfer' and 'timeInterval'\n");
        exit(1);
    }
    int client;
    int timeInterval = stoi(argv[3]);
    client = socket(AF_INET, SOCK_STREAM, 0); 
    char* server_addr_str = strtok(argv[1], ":");
    int server_port = stoi(strtok(NULL, ":"));
    struct sockaddr_in client_addr;
    memset(&client_addr, 0, sizeof(client_addr));
    client_addr.sin_family = AF_INET; 
    client_addr.sin_port = htons(server_port); 
    inet_pton(AF_INET, server_addr_str, &client_addr.sin_addr);
    memset(&(client_addr.sin_zero), '\0', 8);
    int conn_err = connect(client, (struct sockaddr *)&client_addr, sizeof(struct sockaddr));
    if(conn_err == -1){
        fprintf(stderr, "Error: Connection to server failed'\n");
        exit(2);
    }
    string req_filename = argv[2];
    string request = "get " + req_filename;
    cout<<"ConnectDone: "<<argv[1]<<":"<<server_port<<endl;
    char buffer[1000];
    memset(buffer, 0, 1000);
    send(client, request.c_str(), request.size(), 0);
    int total_received = 0;
    int bytes_received;
    ofstream outfile(req_filename);
    while ((bytes_received = recv(client, buffer, 1000, 0)) > 0) {
        outfile.write(buffer, bytes_received);
        total_received += bytes_received;
        memset(buffer, 0, 1000);
        usleep(1000*timeInterval);
        
    }
    close(client);
    cout<<"FileWritten: "<<total_received<<" bytes"<<endl;
    outfile.close();
    }