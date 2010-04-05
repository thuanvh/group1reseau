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
    /*
    static int sendTimeout(int sock, void* buf,int len, int flags);
    static int sendCommand(int sock, string cmd);
    static int receiveTimeout(int sock, char* buf, int len, int flags);
    static int receiveResponse(int sock, string& cmd);
     * */
    static int sendTimeout(int sock, void *buf,int len, int flags)
{
	//Pour contrôler le temps (timeout)
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

//Demmander
//cmd : commandement de ftp
/** Envoyer une commande de ftp au serveur distant, les commandes sont décris
 * @param sock l'identificateur du socket qu'on va utiliser
 * @param cmd la commande
 */
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

	//Envoyer
	return send(sock, cmd.c_str(), cmd.size(), 0);
}

/** Recevoir des signaux ou données
 * @param sock l'identificateur du socket qu'on va utiliser
 * @param buf le buffer pour archiver les informations qu'on reçoit
 * @param len le longeur du buffer
 * @param flags (0: default, MSG OOB: out-of-bound message, MSG PEEK: look at message without removing)
 */
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

	//Recevoir
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
	//Recevoir
	int _recv = recv(sock, _buf, _len, _flags);
        contenu = _buf;
        return _recv;
}
};



#endif	/* _FTPCONNECTION_H */

