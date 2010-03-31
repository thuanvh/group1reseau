/* 
 * File:   common.h
 * Author: trung
 *
 * Created on March 31, 2010, 3:18 PM
 */

#ifndef _COMMON_H
#define	_COMMON_H

#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string>
#include <iostream>

using namespace std;

/*  Des commandes FTP supportees par ce serveur.     */
enum _CommandType { ERROR, TYPE, MODE, NLST, RETR, NOOP, STOR,
		    USER, PASS, QUIT, PWD, CWD, PORT, PASV, SYST };
typedef enum _CommandType CommandType;

#define BUFFER 4096

#endif	/* _COMMON_H */

