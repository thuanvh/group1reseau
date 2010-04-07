/* 
 * File:   main.cpp
 * Author: trung
 *
 * Created on March 31, 2010, 2:12 PM
 */

#include "common.h"
#include "FTPServer.h"

/*
 * check if a string contains digit characters
 */
bool isNumeric(char *tmp) {
    bool result = true;
    int n = strlen(tmp);

    for(int i=0; i<n; i++) {
        if (!isdigit(tmp[i])) {
            result = false;
            break;
        }
    }

    return result;
}

int main(int argc, char** argv) {
    FTPServer server;
    int port;
    string tmp;
    struct stat buffer;

    if (argc != 3) {
        cout << "Usage: ftp_server port home_directory" << endl;
        return (EXIT_FAILURE);
    }
    if (isNumeric(argv[1])) {
        port = atoi(argv[1]);
    } else {
        cout << "port must be numeric type!" << endl;
        return (EXIT_FAILURE);
    }
    tmp = argv[2];
    if (stat(tmp.c_str(), &buffer) != 0) {
        cout << tmp << " does not exist!" << endl;
        return (EXIT_FAILURE);
    }

    server.setPort(port);
    server.setHome(tmp);
    server.start();

    return (EXIT_SUCCESS);
}

