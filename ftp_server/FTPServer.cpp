/* 
 * File:   FTPServer.cpp
 * Author: trung
 * 
 * Created on March 31, 2010, 2:58 PM
 */

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
            data_id, /*  Socket ID for the data conneciton  */
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

    /*
     *  Welcome Message
     */
    socket_id = input.cid;
    cmd.setSocketID(socket_id);
    pro.setClientID(socket_id);
    pro.setDir(FTPServer::home);

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

        /*
         *  Get a command from the client to process.  At this point all of
         *  the implemented commands are now available for the user to make
         *  full use of.  Each of the commands calls a different function,
         *  or replies with some information message.  For those commands
         *  that require a data conneciton, the conneciton is made before
         *  calling other routines, and then the socket number is passed
         *  instead (NLST, RECV, STOR, are the only three that need to do
         *  this.)
         */

        cmd.getCommandName(msgs);
        command = cmd.getCommandType(msgs[0]);

        switch (command) {
            case TYPE:
                send(socket_id, TYPE_RESPONSE, strlen(TYPE_RESPONSE), 0);
                break;
            case MODE:
                send(socket_id, MODE_RESPONSE, strlen(MODE_RESPONSE), 0);
                break;
            case NLST:
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
                pro.cmdNLST(msgs[1]);
                send(socket_id, TRANSFER_COMPLETE, strlen(TRANSFER_COMPLETE), 0);
                printf("Closing data conneciton.\n");
                pro.closeConnection();
                break;
            case RETR:
                if (!pro.isPasv()) {
                    if (pro.createConnection() == -1) {
                        send(socket_id, DATA_CONNECT_FAIL, strlen(DATA_CONNECT_FAIL), 0);
                        break;
                    }
                }
                memset(data, 0x00, sizeof(data));
                sprintf(data, "%s (%u,%u).\r\n",
                        READY_FOR_TRANSFER, host_id, port_number);
                send(socket_id, data, strlen(data), 0);
                if (pro.cmdRETR(msgs[1]) == 1) {
                    send(socket_id, TRANSFER_COMPLETE, strlen(TRANSFER_COMPLETE), 0);
                } else {
                    send(socket_id, NO_SUCH_FILE, strlen(NO_SUCH_FILE), 0);
                }
                printf("Closing data conneciton.\n");
                pro.closeConnection();
                break;
            case NOOP:
                send(socket_id, NOOP_RESPONSE, strlen(NOOP_RESPONSE), 0);
                return NULL;
            case STOR:
                if (!pro.isPasv()) {
                    if (pro.createConnection() == -1) {
                        send(socket_id, DATA_CONNECT_FAIL, strlen(DATA_CONNECT_FAIL), 0);
                        break;
                    }
                }
                memset(data, 0x00, sizeof(data));
                sprintf(data, "%s (%u,%u).\r\n",
                        READY_FOR_TRANSFER, host_id, port_number);
                send(socket_id, data, strlen(data), 0);
                if (pro.cmdSTOR(msgs[1]) == 1) {
                    send(socket_id, TRANSFER_COMPLETE, strlen(TRANSFER_COMPLETE), 0);
                } else {
                    send(socket_id, CANNOT_CREATE_FILE, strlen(CANNOT_CREATE_FILE), 0);
                }
                printf("Closing data conneciton.\n");
                pro.closeConnection();
                break;
            case QUIT:
                send(socket_id, GOODBYE, strlen(GOODBYE), 0);
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

    memset(&ss, 0x00, sizeof (ss));
    sid = listenning(&ss);
    if (sid == -1) {
        cout << "Error on listenning";
        return -1;
    }

    cout << "server started" << endl;
    // main loop
    while (1) {
        length = sizeof (sc);
        cid = accept(sid, (sockaddr*) & sc, &length);
        if (cid > 0) {
            // traiter la requete de client ici
            data.cid = cid;
            data.host = sc.sin_addr.s_addr;
            pthread_create(&tid, NULL, &handle, &data);
            cout << "server ip: " << inet_ntoa(sc.sin_addr) << endl;
        }
    }
}

int FTPServer::listenning(sockaddr_in* name) {
    int data_id, /*  Socket Number of Data Socket */
            host_id, /*  Information about local sys. */
            errorlevel; /*  General purpose return codes */
    char port_name[BUFFER]; /*  Name of the local machine    */
    hostent *host_data; /*  Container struct host info   */

    /*  Get the name of the local host to get the network information  */
    memset(port_name, 0x00, sizeof(port_name));
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

        errorlevel = listen(data_id, MAX_CLIENT);
        if (errorlevel == -1) {
            cout << "Error: listen" << endl;
            close(data_id);
            return -1;
        }

        return data_id;
    }
}
