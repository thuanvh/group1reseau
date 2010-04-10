/* 
 * File:   FtpConnection.h
 * Author: langthang
 *
 * Created on April 3, 2010, 3:29 AM
 */

#ifndef _FTPCONNECTION_H
#define	_FTPCONNECTION_H
#include "FtpConstant.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <string.h>
#include <stdio.h>
#include <pwd.h>
#include <unistd.h>
#include <time.h>

using namespace std;

class FtpConnection{
    
public:
    
    static int sendTimeout(int sock, void *buf, int len, int flags)
    {
        struct timeval tv;
        fd_set writefds;

        tv.tv_sec = 20;
        tv.tv_usec = 0;

        FD_ZERO(&writefds);
        FD_SET(sock, &writefds);
        select(sock + 1, NULL, &writefds, NULL, &tv);

        if(!FD_ISSET(sock, &writefds))
                return -1;

        //Envoyer
        return send(sock, buf, len, flags);
    }

    static int sendCommand(int sock, string cmd)
    {
        struct timeval tv;
        fd_set writefds;

        tv.tv_sec = 20;
        tv.tv_usec = 0;

        FD_ZERO(&writefds);
        FD_SET(sock, &writefds);
        select(sock + 1, NULL, &writefds, NULL, &tv);

        if(!FD_ISSET(sock, &writefds))
                return -1;

        return send(sock, cmd.c_str(), cmd.size(), 0);
    }

    static int receiveTimeout(int sock, char *buf, int len, int flags)
    {
        struct timeval tv;
        fd_set writefds;

        tv.tv_sec = 20;
        tv.tv_usec = 0;

        FD_ZERO(&writefds);
        FD_SET(sock, &writefds);
        select(sock+1, NULL, &writefds, NULL, &tv);

        if(!FD_ISSET(sock, &writefds))
                return -1;

        return recv(sock, buf, len, flags);
    }

    static int receiveResponse(int sock, string& contenu)
    {
        struct timeval tv;
        fd_set writefds;

        tv.tv_sec = 20;
        tv.tv_usec = 0;

        FD_ZERO(&writefds);
        FD_SET(sock, &writefds);
        select(sock+1, NULL, &writefds, NULL, &tv);

        if(!FD_ISSET(sock, &writefds))
                return -1;

        char _buf[256];
        memset(_buf, 0, 256);
        int _len = 256;
        int _flags = 0;
        
        int _recv = recv(sock, _buf, _len, _flags);
        contenu = _buf;
        return _recv;
    }

    static int receiveLargeResponse(int sock, string& contenu)
    {
        struct timeval tv;
        fd_set writefds;

        tv.tv_sec = 20;
        tv.tv_usec = 0;

        FD_ZERO(&writefds);
        FD_SET(sock, &writefds);
        select(sock+1, NULL, &writefds, NULL, &tv);

        if(!FD_ISSET(sock, &writefds))
                return -1;

        char _buf[1000];
        memset(_buf, 0, 1000);
        int _len = 999;
        int _flags = 0;
        
        int _recv = recv(sock, _buf, _len, _flags);
        contenu = _buf;
        return _recv;
    }
};

#endif	/* _FTPCONNECTION_H */

