/* 
 * File:   FTPProcess.cpp
 * Author: trung
 * 
 * Created on April 2, 2010, 4:37 PM
 */

#include "FTPProcess.h"

FTPProcess::FTPProcess() {
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
    return host;
}
int FTPProcess::getPort() const {
    return port;
}

FTPProcess::~FTPProcess() {
}

void FTPProcess::closeConnection() {
    shutdown(sid, 2);
    close(sid);
}

int FTPProcess::cmdCWD(string path) {
    string tmp;
    struct stat buffer;
    int         status;

    if (path.find('/') == 0) {
        tmp = path;
    } else {
        tmp = dir + path;
    }

    if (stat(tmp.c_str(), &buffer) == 0) {
        dir = tmp;
        if (dir.find_last_of('/') != dir.length()-1) {
            dir.append("/");
        }
        send(clientID, CWD_SUCCESSFUL, strlen(CWD_SUCCESSFUL), 0);
    } else {
        send(clientID, NO_SUCH_FILE, strlen(NO_SUCH_FILE), 0);
    }

    return 1;
}

int FTPProcess::cmdNLST(string path) {
    int            slash_flag = NO;         /*  If the -F flag is set       */
    char data[BUFFER]; /*  Buffer for data blocks      */
    DIR *name_list; /*  File handle for temp file   */
    struct dirent *file_info; /*  Local files in the directory*/
    struct stat stat_buff; /*  File info (is it a dir?)    */
    string tmp;

    if (strcmp(path.c_str(), "-F") == 0) {
        slash_flag = YES;
    }

    /*
     *  In order to get the local file list, a new function will need to be
     *  called, that is, opendir() which makes use of a DIR structure, and
     *  another function called readdir().  This list will be gone through
     *  and all directories will have a '/' put on it if the options flag
     *  is set.
     */

    if ((name_list = opendir(dir.c_str())) == NULL) {
        printf("Error:  Unable to open local directory.\n");
        send(sid, NO_SUCH_FILE, strlen(NO_SUCH_FILE), 0);
    } else {

        /*
         *  Steps:  Get the next file in the directory, then get the file info
         *  to check to see if it is a directory.  If it is, then check to see
         *  if a slash needs to be placed on it.
         */

        while ((file_info = readdir(name_list)) != NULL) {
            tmp.assign(dir);
            tmp.append(file_info->d_name);

            if (stat(tmp.c_str(), &stat_buff) != 0) {
                printf("Error unable to fetch stat information.\n");
            } else {

                /*
                 *  If a simple file, do no processing, and print it out.
                 */

                memset(data, 0x00, sizeof(data));
                if (!S_ISDIR(stat_buff.st_mode)) {
                    sprintf(data, "%s\r\n", file_info->d_name);
                    send(sid, data, strlen(data), 0);

                    /*
                     *  It is a directory, so print it out with the optional '/' on it
                     *  while making sure not to send the '.' and '..' directories.
                     */

                } else if ((strcmp(file_info->d_name, ".") != 0) &&
                        (strcmp(file_info->d_name, "..") != 0)) {
                    if (slash_flag = YES) {
                        sprintf(data, "%s/\r\n", file_info->d_name);
                    } else {
                        sprintf(data, "%s\r\n", file_info->d_name);
                    }
                    send(sid, data, strlen(data), 0);
                }

            }
        }
    }
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

    if ((local_file = fopen(tmp.c_str(), "rb")) == NULL) {
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

    string tmp;

    if (path.find('/') == 0) {
        tmp = path;
    } else {
        tmp = dir + path;
    }

    /*
    *  Open the local file for writing.
    */

    if ((local_file = fopen(tmp.c_str(), "wb")) == NULL) {
        printf("Error:  Unable to create local file \"%s\" for writing.\n");
        return -1;
    } else {

        /*
        *  Keep on reading from the data port, until recv returns with a 0,
        *  and write out the result.
        */

        errorlevel = recv(sid, listing, sizeof(char) * BUFFER, NULL);
        while (errorlevel != 0) {
          fwrite(listing, errorlevel, 1, local_file);
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

    host = i4;
    host = (host << 8) | i3;
    host = (host << 8) | i2;
    host = (host << 8) | i1;

    port = (i5 << 8) | i6;

    send(clientID, PORT_RESPONSE, strlen(PORT_RESPONSE), 0);
    addr.s_addr = host;
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
    host_data = gethostbyaddr(&host, sizeof(host), AF_INET);
    if (host_data == NULL) {
        printf("Error: Unable to resolve to an address.\n");
        return -1;
    } else {
        errorlevel = sid = socket(AF_INET, SOCK_STREAM, 0);
        if (errorlevel == -1) {
          return -1;
        } else {
            /*  Comment out the next line to make the call blocking
            fcntl(socket_desc, F_SETFL, O_NONBLOCK); */

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