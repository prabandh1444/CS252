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
    if (argc != 2) {
        fprintf(stderr, "Error: expected one argument 'portNum' \n");
        exit(1);
    }
    int MaxnoOfconnections = 2;
    int server;
    int clients[MaxnoOfconnections]={-1};
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
    int lis_err = listen(server,10);
    cout<<"BindDone: "<< server_port<<endl;
    if(lis_err !=-1){
    cout<<"ListenDone: "<< server_port<<endl;
    }  
    int sin_size = sizeof(struct sockaddr_in);
    socklen_t len = static_cast<socklen_t>(sin_size);
    fd_set set;
    while(true){
    FD_ZERO(&set);
    FD_SET(server, &set);
    for(int i=0;i<MaxnoOfconnections;i++){
        if(clients[i] >= 0){
            FD_SET(clients[i], &set);
        }
    }
    select(FD_SETSIZE, &set, NULL, NULL, NULL);
        if (FD_ISSET(server, &set)) {
            int new_des = accept(server, (struct sockaddr *)&their_addr, &len);
            if(new_des > 0){
            for(int i=0;i<MaxnoOfconnections;i++){
            if(clients[i] < 0){
             clients[i] = new_des;
             break;
            }}
            }
            cout<<"Client: "<< (their_addr.sin_addr.s_addr)<<":"<<their_addr.sin_port<<endl;
     }else{
        for(int i=0;i<MaxnoOfconnections;i++){
            if (clients[i] > 0 && (FD_ISSET(clients[i], &set))){
                char buffer[1024];
                memset(buffer, 0, 1024);
                int activity = recv(clients[i], buffer, 1024, 0);
                if(activity>0){
                if (strncmp(buffer, "get ", 4) != 0) {
                cout<<"UnknownCmd"<<endl;
                fprintf(stderr, "Error: Invalid request format from client\n");
                exit(1);
                }
                string filename(buffer+4, activity);
                cout<<"FileRequested: "<<filename<<endl;
                int total_sent=0;
                ifstream infile(filename);
                while (infile) {
                infile.read(buffer, 1024);
                int bytes_read = infile.gcount();
                if (bytes_read > 0) {
                int bytes_sent = send(clients[i], buffer, bytes_read, 0);
                total_sent += bytes_sent;
                }}
                cout<<"TransferDone: "<<total_sent<<" bytes"<<endl;
                memset(buffer, 0, 1024);
                close(clients[i]);
                clients[i] = -1;
                FD_CLR(clients[i], &set);
                }
                }
            }
        }
        
    }

    close(server);

}