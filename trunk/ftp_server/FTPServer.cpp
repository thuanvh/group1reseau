/* 
 * File:   FTPServer.cpp
 * Author: trung
 * 
 * Created on March 31, 2010, 2:58 PM
 */

#include "FTPServer.h"

FTPServer::FTPServer() {
    port = 0;
    home = "";
}

void FTPServer::setHome(string home) {
    this->home = home;
}

string FTPServer::getHome() const {
    return home;
}

void FTPServer::setPort(int port) {
    this->port = port;
}

int FTPServer::getPort() const {
    return port;
}

FTPServer::~FTPServer() {
}

void FTPServer::start() {

}

int FTPServer::listenning(sockaddr_in* name) {
    int data_id, /*  Socket Number of Data Socket */
        host_id, /*  Information about local sys. */
        errorlevel; /*  General purpose return codes */
    char port_name[BUFFER]; /*  Name of the local machine    */
    hostent *host_data; /*  Container struct host info   */

    /*  Get the name of the local host to get the network information  */
    if (gethostname(port_name, BUFFER) == -1) {
        cout << "Error gethostname" << endl;
        return -1;
    } else {

        /*
         *  Set up the socket, it is assumed that this is going to be an
         *  AF_INET connection, with a connection based socket.
         */
        errorlevel = data_id = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (errorlevel == -1) {
            cout << "Unable to create a socket" << endl;
            return -1;
        }

        /*
         *  Set up the actual connection.  Needs a special struct filled
         *  to contain the host address and the port to connect to.
         *  If this works, then the connection is completed, and passed
         *  back to the calling function.
         */

        if (port == 0) port = 21;

        host_data = gethostbyname(port_name);
        host_id = *((int *) (host_data->h_addr_list[0]));
        name->sin_family = host_data->h_addrtype;
        name->sin_port = htons(port);
        //name->sin_addr.s_addr = * ((int *) (host_data->h_addr_list[0]));
        name->sin_addr.s_addr = htonl(INADDR_ANY);

        errorlevel = bind(data_id, (struct sockaddr *) name, sizeof (*name));
        if (errorlevel == -1) {
            cout << "Error: bind" << endl;
            close(data_id);
            return -1;
        }

        /*
         *  Before the connection is completed, the listening buffer size
         *  is set to the default BUFFER size.
         */

        errorlevel = listen(data_id, BUFFER * BUFFER);
        if (errorlevel == -1) {
            cout << "Error: listen" << endl;
            close(data_id);
            return -1;
        }

        return data_id;
    }
}