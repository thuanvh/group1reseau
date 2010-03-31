/* 
 * File:   FTPServer.h
 * Author: trung
 *
 * Created on March 31, 2010, 2:58 PM
 */

#ifndef _FTPSERVER_H
#define	_FTPSERVER_H

#include "common.h"

class FTPServer {
public:
    FTPServer();
    virtual ~FTPServer();
    void start();
    void setHome(string home);
    string getHome() const;
    void setPort(int port);
    int getPort() const;

private:
    int port;
    string home;

    int listenning(struct sockaddr_in *name);
};

#endif	/* _FTPSERVER_H */

