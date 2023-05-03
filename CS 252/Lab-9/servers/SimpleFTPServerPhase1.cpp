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
    if (argc != 3) {
        fprintf(stderr, "Error: expected two arguments 'portNum' and 'fileToTransfer'\n");
        exit(1);
    }
    int server,client;
    server = socket(AF_INET, SOCK_STREAM, 0);
    int server_port = stoi(argv[1]);
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
    string filename = argv[2];
    ifstream infile(filename);
    if (!infile.is_open()) {
        fprintf(stderr, "Error: Given file not present or not readable \n");
        exit(3);
    }
    int lis_err = listen(server,1);
    cout<<"BindDone: "<< server_port<<endl;
    if(lis_err !=-1){
    cout<<"ListenDone: "<< server_port<<endl;  
    int sin_size = sizeof(struct sockaddr_in);
    socklen_t len = static_cast<socklen_t>(sin_size);
    client = accept(server, (struct sockaddr *)&their_addr,&len);
    cout<<"Client: "<< (their_addr.sin_addr.s_addr)<<":"<<their_addr.sin_port<<endl;
    char buffer[1024];
    int total_sent=0;
    while (infile) {
        infile.read(buffer, 1024);
        int bytes_read = infile.gcount();
        if (bytes_read > 0) {
            int bytes_sent = send(client, buffer, bytes_read, 0);
            total_sent += bytes_sent;
        }
    }
    cout<<"TransferDone: "<<total_sent<<" bytes"<<endl;

    close(client);
    close(server);
    }

}