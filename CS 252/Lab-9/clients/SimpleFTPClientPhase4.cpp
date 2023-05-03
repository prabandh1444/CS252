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
    if (argc != 5) {
        fprintf(stderr, "Error: expected two arguments 'portNum','op','fileToTransfer' and 'timeInterval'\n");
        exit(1);
    }
    int client;
    int timeInterval = stoi(argv[4]);
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
    string put_get = argv[2];
    string req_filename = argv[3];
    cout<<"ConnectDone: "<<argv[1]<<":"<<server_port<<endl;
    if(strncmp(put_get.c_str(),"get",3)==0){
    string request = "get " + req_filename;
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
    cout<<"FileWritten: "<<total_received<<" bytes"<<endl;
    outfile.close();}
    if(strncmp(put_get.c_str(),"put",3)==0){
    string request = "put " + req_filename;
    char buffer[1000];
    memset(buffer, 0, 1000);
    send(client, request.c_str(), request.size(), 0);
    int total_sent = 0;
    ifstream infile(req_filename);
     while (infile) {
        infile.read(buffer, 1000);
        int bytes_read = infile.gcount();
        if (bytes_read > 0) {
        int bytes_sent = send(client, buffer, bytes_read, 0);
        memset(buffer, 0, 1000);
        total_sent += bytes_sent;
            }
        }
    cout<<"FileWritten: "<<total_sent<<" bytes"<<endl;
    infile.close();
    }
    close(client);

    }