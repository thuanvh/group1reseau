/* 
 * File:   FTPServer.cpp
 * Author: trung
 * 
 * Created on March 31, 2010, 2:58 PM
 */

#include <ifaddrs.h>

#include "FTPServer.h"
#include "FTPCommand.h"
#include "FTPProcess.h"

string FTPServer::home = "";

FTPServer::FTPServer() {
    port = 0;
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

void* FTPServer::handle(void* parametres) {
    THREAD_DATA input;

    memcpy(&input, parametres, sizeof (input));

    int socket_id,
        port_number = 0, /*  The port number for the data conn  */
        host_id = 0, /*  The host address for the connect.  */
        login = NO, /*  If the user is loged in yet or not */
        user_entered = NO; /*  If the USER command has been used  */
    char data[BUFFER]; /*  Buffer for the command from client */
    CommandType command; /*  Form of command based on word_1    */
    FTPCommand cmd;
    vector<string> msgs;
    string username;
    string password;
    FTPProcess pro;

    socket_id = input.cid;
    cmd.setSocketID(socket_id);
    pro.setClientID(socket_id);
    pro.setDir(FTPServer::home);
    /*
     *  Welcome Message
     */
    send(socket_id, WELCOME_MESSAGE, strlen(WELCOME_MESSAGE), 0);

    /*
     *  There is only one account possible for this program, and that is
     *  the 'ftp' user name.  Keep looping until it is entered properly,
     *  so that the client is logged in.  Any password that is greater than
     *  one character will be accepted.
     */

    do {
        cmd.getCommandName(msgs);
        command = cmd.getCommandType(msgs[0]);

        /*
         *  This switch statement cycles until the user either logs in
         *  properly via a USER, and a PASS statement, or they opt to
         *  QUIT.  Once a correct password has been entered, or they
         *  choose to quit, continue into the next, and primary loop.
         */

        switch (command) {
            case USER:
                username.assign(msgs[1]);
                send(socket_id, USERNAME_MESSAGE, strlen(USERNAME_MESSAGE), 0);
                user_entered = YES;
                break;
            case PASS:
                password.assign(msgs[1]);
                if (user_entered == NO) {
                    send(socket_id, USE_USER_FIRST, strlen(USE_USER_FIRST), 0);
                } else {
                    if (strcmp(username.c_str(), "ftp") == 0) {
                        send(socket_id, USER_LOGGED_IN, strlen(USER_LOGGED_IN), 0);
                        login = YES;
                    } else {
                        send(socket_id, PASSWORD_FAILURE, strlen(PASSWORD_FAILURE), 0);
                        user_entered = NO;
                    }
                }
                break;
            case QUIT:
                send(socket_id, GOODBYE, strlen(GOODBYE), 0);
                break;
            default:
                send(socket_id, ERROR_MESSAGE, strlen(ERROR_MESSAGE), 0);
        }

    } while (login == NO && command != QUIT);

    /*
     *  Get a message from the client, and process it depending on the
     *  command desired.
     */

    while (command != QUIT) {

        cmd.getCommandName(msgs);
        command = cmd.getCommandType(msgs[0]);

        switch (command) {
            case TYPE:
                pro.cmdType(msgs[1]);
                break;
            case MODE:
                send(socket_id, MODE_RESPONSE, strlen(MODE_RESPONSE), 0);
                break;
            case RETR:
                if (!pro.isPasv()) {
                    if (pro.createConnection() == -1) {
                        send(socket_id, DATA_CONNECT_FAIL, strlen(DATA_CONNECT_FAIL), 0);
                        break;
                    }
                }
                memset(data, 0x00, sizeof(data));
                if (pro.isAscii()) {
                    sprintf(data, "%s (%u,%u).\r\n",
                        READY_A_FOR_TRANSFER, host_id, port_number);
                } else {
                    sprintf(data, "%s (%u,%u).\r\n",
                        READY_I_FOR_TRANSFER, host_id, port_number);
                }
                send(socket_id, data, strlen(data), 0);
                if (pro.cmdRETR(msgs[1]) == 1) {
                    send(socket_id, TRANSFER_COMPLETE, strlen(TRANSFER_COMPLETE), 0);
                } else {
                    send(socket_id, NO_SUCH_FILE, strlen(NO_SUCH_FILE), 0);
                }
                pro.closeConnection();
                break;
            case STOR:
                if (!pro.isPasv()) {
                    if (pro.createConnection() == -1) {
                        send(socket_id, DATA_CONNECT_FAIL, strlen(DATA_CONNECT_FAIL), 0);
                        break;
                    }
                }
                memset(data, 0x00, sizeof(data));
                if (pro.isAscii()) {
                    sprintf(data, "%s (%u,%u).\r\n",
                        READY_A_FOR_TRANSFER, host_id, port_number);
                } else {
                    sprintf(data, "%s (%u,%u).\r\n",
                        READY_I_FOR_TRANSFER, host_id, port_number);
                }
                send(socket_id, data, strlen(data), 0);
                if (pro.cmdSTOR(msgs[1]) == 1) {
                    send(socket_id, TRANSFER_COMPLETE, strlen(TRANSFER_COMPLETE), 0);
                } else {
                    send(socket_id, CANNOT_CREATE_FILE, strlen(CANNOT_CREATE_FILE), 0);
                }
                pro.closeConnection();
                break;
            case QUIT:
                send(socket_id, GOODBYE, strlen(GOODBYE), 0);
                close(socket_id);
                break;
            case PWD:
                pro.cmdPWD();
                break;
            case CWD:
                pro.cmdCWD(msgs[1]);
                break;
            case PORT:
                pro.setInfo(msgs[1]);
                break;
            case PASV:
                pro.cmdPASV(input.host);
                break;
            case SYST:
                pro.cmdSYST();
                break;
            case LIST:
                if (!pro.isPasv()) {
                    if (pro.createConnection() == -1) {
                        send(socket_id, DATA_CONNECT_FAIL, strlen(DATA_CONNECT_FAIL), 0);
                        break;
                    }
                }
                memset(data, 0x00, sizeof(data));
                sprintf(data, "%s.\r\n",
                        READY_FOR_NLST);
                send(socket_id, data, strlen(data), 0);
                pro.cmdLIST((msgs.size()==2)?msgs[1]:"");
                send(socket_id, TRANSFER_COMPLETE, strlen(TRANSFER_COMPLETE), 0);
                pro.closeConnection();
                break;
            default:
                send(socket_id, ERROR_MESSAGE, strlen(ERROR_MESSAGE), 0);
        }
    }
}

int FTPServer::start() {
    int sid, cid;
    sockaddr_in ss, sc;
    unsigned int length;
    pthread_t tid;
    THREAD_DATA data;
    sockaddr_in addr;
    socklen_t len = sizeof(addr);

    memset(&ss, 0x00, sizeof (ss));
    sid = listenning(&ss);
    if (sid == -1) {
        cout << "Error on listenning";
        return -1;
    }
    
    cout << "Server started" << " on " << port << endl;
    cout << "Press Ctrl+C to stop server" << endl;
    // main loop
    while (1) {
        length = sizeof (sc);
        cid = accept(sid, (sockaddr*) & sc, &length);
        if (cid > 0) {
            // traiter la requete de client ici
            // get IP of current socket id
            getsockname(cid, (sockaddr*)&addr, &len);
            //cout << "my ip: " << inet_ntoa(addr.sin_addr) << endl;
            data.cid = cid;
            data.host = addr.sin_addr.s_addr;
            pthread_create(&tid, NULL, &handle, &data);
        }
    }

    return 1;
}

int FTPServer::listenning(sockaddr_in* name) {
    int data_id, /*  Socket Number of Data Socket */
            host_id, /*  Information about local sys. */
            errorlevel; /*  General purpose return codes */
    char port_name[BUFFER]; /*  Name of the local machine    */
    hostent *host_data; /*  Container struct host info   */

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

    name->sin_family = AF_INET;
    name->sin_port = htons(port);
    name->sin_addr.s_addr = htonl(INADDR_ANY);

    errorlevel = bind(data_id, (struct sockaddr *) name, sizeof (*name));

    if (errorlevel == -1) {
        cout << "Error on binding" << endl;
        close(data_id);
        return -1;
    }

    /*
     *  Before the connection is completed, the listening buffer size
     *  is set to the default BUFFER size.
     */

    errorlevel = listen(data_id, MAX_CLIENT);
    if (errorlevel == -1) {
        cout << "Error on listenning" << endl;
        close(data_id);
        return -1;
    }

    return data_id;
}
