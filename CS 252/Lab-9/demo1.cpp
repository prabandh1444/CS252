if (FD_ISSET(server, &set)) {
    if(client1 == -1){
    client1 = accept(server, (struct sockaddr *)&their_addr_1, &len);
    cout<<"Client: "<< (their_addr_1.sin_addr.s_addr)<<" : "<<their_addr_1.sin_port<<endl;
    FD_SET(client1, &set);
    if (client1 > max_fd) {
         max_fd = client1;}
    }
    if(client2 == -1){
        client2 = accept(server, (struct sockaddr *)&their_addr_2, &len);
        cout<<"Client: "<< (their_addr_2.sin_addr.s_addr)<<" : "<<their_addr_2.sin_port<<endl;
        FD_SET(client2, &set);
        if (client2 > max_fd) {
            max_fd = client2;}
        }
    }
    if (FD_ISSET(client1, &set)) {
        cout<<"Hi"<<endl;
        char buffer[1024];
        memset(buffer, 0, 1024);
        if (read(client1, buffer, sizeof(buffer)) == 0) {
            cout<<"Client1 disconnected"<<endl;
            close(client1);
            FD_CLR(client1, &set);
        }
            else {
                string cmd(buffer);
                if (cmd.substr(0, 4) == "get ") {
                    cout<<"UnknownCmd"<<endl;
                    fprintf(stderr, "Error: Invalid request format from client\n");
                        exit(1);
                    }
                string filename = cmd.substr(4, cmd.size()-5);
                cout<<"Client1 requested file: "<<filename<<endl;
                ifstream infile(filename);
                int total_sent=0;
                while (infile) {
                    infile.read(buffer, 1024);
                    int bytes_read = infile.gcount();
                    if (bytes_read > 0) {
                        int bytes_sent = send(client1, buffer, bytes_read, 0);
                        total_sent += bytes_sent;
                        }
                        }
                cout<<"TransferDone: "<<total_sent<<" bytes"<<endl;
}
            }
        if (FD_ISSET(client2, &set)) {
                char buffer[1024];
                memset(buffer, 0, 1024);
                if (read(client2, buffer, sizeof(buffer)) == 0) {
                    cout<<"Client2 disconnected"<<endl;
                    close(client2);
                    FD_CLR(client2, &set);
                }
                else {
                    string cmd(buffer);
                    if (cmd.substr(0, 4) == "get ") {
                         cout<<"UnknownCmd"<<endl;
                         fprintf(stderr, "Error: Invalid request format from client\n");
                         exit(1);
                    }
                    string filename = cmd.substr(4, cmd.size()-5);
                    cout<<"Client1 requested file: "<<filename<<endl;
                    ifstream infile(filename);
                    int total_sent=0;
                    while (infile) {
                        infile.read(buffer, 1024);
                        int bytes_read = infile.gcount();
                        if (bytes_read > 0) {
                            int bytes_sent = send(client2, buffer, bytes_read, 0);
                            total_sent += bytes_sent;
                            }
                        }
                    cout<<"TransferDone: "<<total_sent<<" bytes"<<endl;
                    close(client2); 
                    }
                }