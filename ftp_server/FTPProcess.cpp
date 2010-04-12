/* 
 * File:   FTPProcess.cpp
 * Author: trung
 * 
 * Created on April 2, 2010, 4:37 PM
 */

#include "FTPProcess.h"

FTPProcess::FTPProcess() {
    pasv = false;
    ascii = false;
}
void FTPProcess::setClientID(int clientID) {
    this->clientID = clientID;
}
void FTPProcess::setDir(string dir) {
    this->dir = dir;
}
string FTPProcess::getDir() const {
    return dir;
}
int FTPProcess::getHost() const {
    return client;
}
int FTPProcess::getPort() const {
    return port;
}

FTPProcess::~FTPProcess() {
}

int FTPProcess::cmdType(string type) {
    if (strcmp(type.c_str(), "I") == 0) {
        ascii = false;
        send(clientID, TYPE_I_RESPONSE, strlen(TYPE_I_RESPONSE), 0);
    } else if (strcmp(type.c_str(), "A") == 0) {
        ascii = true;
        send(clientID, TYPE_A_RESPONSE, strlen(TYPE_A_RESPONSE), 0);
    } else {
        send(clientID, ERROR_MESSAGE, strlen(ERROR_MESSAGE), 0);
    }

    return 1;
}

int FTPProcess::cmdSYST() {
    send(clientID, SERVER_TYPE, strlen(SERVER_TYPE), 0);
    return 1;
}

void FTPProcess::closeConnection() {
    printf("Closing data conneciton.\n");
    pasv = false;
    shutdown(sid, 2);
    close(sid);
}

int FTPProcess::cmdCWD(string path) {
    string tmp;
    struct stat buffer;
    char real[BUFFER];

    if (path.find('/') == 0) {
        tmp = path;
    } else {
        tmp = dir + path;
    }

    if (stat(tmp.c_str(), &buffer) == 0) {
        memset(real, 0x00, sizeof(real));
        realpath(tmp.c_str(), real);
        dir.assign(real);
        if (dir.find_last_of('/') != dir.length()-1) {
            dir.append("/");
        }
        send(clientID, CWD_SUCCESSFUL, strlen(CWD_SUCCESSFUL), 0);
    } else {
        send(clientID, NO_SUCH_FILE, strlen(NO_SUCH_FILE), 0);
    }

    return 1;
}

int FTPProcess::cmdLIST(string path) {
    string cmd;
    FILE *pipe;
    char tmp[BUFFER];
    int i;
    string str;
    size_t b,e,l;
    bool con = true, addhome = true;

    if ((l = path.length()) == 0) {
        cmd = "ls -l " + dir;
    } else {
        cmd = "ls -l ";
        b = 0;
        while(con) {
            e = path.find(" ");
            if (e == string::npos) {
                con = false;
                e = b-1;
            }
            str = path.substr(b, l - e - 1);
            b = e + 1;
            if (str.find('-') == 0) {
                cmd.append(str + " ");
            } else if (str.find('/') == 0) {
                addhome = false;
                cmd.append(str + " ");
            } else {
                addhome = false;
                cmd.append(dir + str + " ");
            }
        }
        if (addhome) cmd.append(dir);
    }

    pipe = popen(cmd.c_str(), "r" );
    if (pipe == NULL ) {
        send(sid, NO_SUCH_FILE, strlen(NO_SUCH_FILE), 0);
        return -1;
    }

    while(!feof(pipe) ) {
        if( fgets( tmp, 256, pipe ) != NULL ) {
            i = strlen(tmp);
            tmp[i-1] = '\r';
            tmp[i] = '\n';
            tmp[i+1] = 0x00;
            send(sid, tmp, strlen(tmp), 0);
        }
    }
    pclose(pipe);

    return 1;
}

int FTPProcess::cmdPWD() {
    char   response[BUFFER];

    sprintf(response, "%s%s\r\n", PWD_PREFIX, dir.c_str());
    send(clientID, response, strlen(response), 0);
    return 1;
}

int FTPProcess::cmdRETR(string path) {
    int            errorlevel;             /*  General return codes         */
    char           listing[BUFFER];        /*  Buffer for the file names    */
    FILE           *local_file;            /*  File handle for local file   */
    string tmp;

    if (path.find('/') == 0) {
        tmp = path;
    } else {
        tmp = dir + path;
    }

    /*
    *  Open the local file for reading.
    */
    if (ascii) {
        local_file = fopen(tmp.c_str(), "r");
    } else {
        local_file = fopen(tmp.c_str(), "rb");
    }

    if (local_file == NULL) {
        printf("Error:  Unable to open local file \"%s\" for writing.\n",
               tmp.c_str());
        return -1;
    } else {

        /*
         *  Keep writing to the file until the connection is closed by the
         *  remote client.
         */

        errorlevel = fread(listing, sizeof(char), BUFFER, local_file);
        while (errorlevel != 0) {
            send(sid, listing, errorlevel, NULL);
            errorlevel = fread(listing, sizeof(char), BUFFER, local_file);
        }
        fclose(local_file);
        return 1;
    }
}

int FTPProcess::cmdSTOR(string path) {
    int            errorlevel;             /*  General return codes         */
    char           listing[BUFFER];        /*  Buffer for the file names    */
    FILE           *local_file;            /*  File handle for local file   */
    bool flag = false;

    string tmp;

    if (path.find('/') == 0) {
        tmp = path;
    } else {
        tmp = dir + path;
    }

    /*
    *  Open the local file for writing.
    */

    if (ascii) {
        local_file = fopen(tmp.c_str(), "w");
    } else {
        local_file = fopen(tmp.c_str(), "wb");
    }

    if (local_file == NULL) {
        printf("Error:  Unable to create local file \"%s\" for writing.\n", tmp.c_str());
        return -1;
    } else {

        /*
        *  Keep on reading from the data port, until recv returns with a 0,
        *  and write out the result.
        */

        errorlevel = recv(sid, listing, sizeof(char) * BUFFER, NULL);
        while (errorlevel != 0) {
            if (ascii) {
                for (int j = 0 ; j < errorlevel; j++) {
                    if (listing[j] == 0x0D) {
                        if (j+1 < errorlevel && listing[j+1] == 0x0A) {
                            j++;
                        }
                    }
                    putc((char)listing[j], local_file);
                }
            } else {
                fwrite(listing, errorlevel, 1, local_file);
            }
            errorlevel = recv(sid, listing, sizeof(char) * BUFFER, NULL);
        }
        fclose(local_file);
        return 1;
    }
}

int FTPProcess::setInfo(string info) {
    int i1, i2, i3, i4, i5, i6;
    struct in_addr addr;

    sscanf(info.c_str(), "%d,%d,%d,%d,%d,%d", &i1, &i2, &i3, &i4, &i5, &i6);

    client = i4;
    client = (client << 8) | i3;
    client = (client << 8) | i2;
    client = (client << 8) | i1;

    port = (i5 << 8) | i6;

    send(clientID, PORT_RESPONSE, strlen(PORT_RESPONSE), 0);
    addr.s_addr = client;
    printf("Data port set to %s, socket %d\n", inet_ntoa(addr), port);
    
    return 1;
}

int FTPProcess::createConnection() {
    int errorlevel;
    struct hostent*    host_data;
    struct sockaddr_in name;

    /*
    *  Get the host information from the system, and check to make sure
    *  that the result is valid.
    */
    host_data = gethostbyaddr(&client, sizeof(client), AF_INET);
    if (host_data == NULL) {
        printf("Error: Unable to resolve to an address.\n");
        return -1;
    } else {
        errorlevel = sid = socket(AF_INET, SOCK_STREAM, 0);
        if (errorlevel == -1) {
          return -1;
        } else {
            name.sin_family = host_data->h_addrtype;
            name.sin_port = htons(port);
            name.sin_addr.s_addr = * ((int *) (host_data->h_addr_list[0]));

            errorlevel = connect(sid, (struct sockaddr *) &name,
                               sizeof(name));

            if (errorlevel == -1) {
                close(sid);
                return -1;
            } else {
                printf("Data connection to %s opened.\n", host_data->h_name);
            }
        }
    }
    return 1;
}

int FTPProcess::cmdPASV(in_addr_t host) {
    int new_port;
    char tmp[BUFFER];
    char send_data[BUFFER];
    int server_sock, client_sock;
    sockaddr_in server_echo, client_echo;

    srand (time(NULL));
    // get a random port from 30000 -> 60000;
    new_port = rand() % 30000 + 30000;
    pasv = true;
    memset(tmp, 0x00, sizeof(tmp));
    memset(send_data, 0x00, sizeof(send_data));
    sprintf(tmp, "(%d,%d,%d,%d,%d,%d)"
            ,(host & 0x000000FF)
            ,(host >> 8) & 0x000000FF
            ,(host >> 16) & 0x000000FF
            ,(host >> 24) & 0x000000FF
            ,(new_port >> 8) & 0x000000FF
            ,(new_port & 0x000000FF));
    sprintf(send_data, PASSIVE_MODE, tmp);
    printf("port: %d", new_port);
    
    
    // create server socket
    if ((server_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        printf("Unable to create a socket");
        return -1;
    }

    // initialize socket address structure
    memset(&server_echo, 0x00, sizeof(server_echo));
    server_echo.sin_family = AF_INET;
    server_echo.sin_addr.s_addr = htonl(INADDR_ANY);
    server_echo.sin_port = htons(new_port);

    // bind socket to the port
    if (bind(server_sock, (sockaddr *)&server_echo, sizeof(server_echo)) < 0) {
        printf("error when binding to this socket");
        return -1;
    }

    // listen on this socket
    if (listen(server_sock, 1) < 0) {
        printf("error when listenning on this socket");
        return -1;
    }

    unsigned int client_len = sizeof(client_echo);
    // waiting for client connection
    send(clientID, send_data, strlen(send_data), 0);
    if ((client_sock = accept(server_sock, (sockaddr*)&client_echo, &client_len)) < 0) {
        printf("error when accepting on this socket");
        return -1;
    }
    printf("Accepted client: %s\n", inet_ntoa(client_echo.sin_addr));
    
    sid = client_sock;

    return 1;
}
bool FTPProcess::isAscii() const {
    return ascii;
}
bool FTPProcess::isPasv() const {
    return pasv;
}
