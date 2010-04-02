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
    int start();
    void setHome(string home);
    string getHome() const;
    void setPort(int port);
    int getPort() const;
    
private:
    int port;
    static string home;

    int listenning(struct sockaddr_in *name);
    static void* handle(void* parametres);
};

#endif	/* _FTPSERVER_H */

