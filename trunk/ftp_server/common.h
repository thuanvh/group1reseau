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
#include <pthread.h>
#include <vector>

using namespace std;

#define BUFFER 4096
#define MAX_CLIENT 20
#define YES 1
#define NO 1

/*  All the FTP response codes that this program supports.  */
char
  READY_FOR_NLST[]     = "150 Binary data connection for directory list",
  READY_FOR_TRANSFER[] = "150 Binary data connection for transfer",
  TYPE_RESPONSE[]      = "200 Only TYPE I is implemented.\r\n",
  MODE_RESPONSE[]      = "200 Only MODE S is implemented.\r\n",
  NOOP_RESPONSE[]      = "200 NOOP command successful.\r\n",
  PORT_RESPONSE[]      = "200 PORT command successful.\r\n",
  WELCOME_MESSAGE[]    = "220 DePaula FTP server (MyOS 5.7) ready.\r\n",
  GOODBYE[]            = "221 Goodbye.\r\n",
  TRANSFER_COMPLETE[]  = "226 Binary Transfer complete\r\n",
  USER_LOGGED_IN[]     = "230 User logged in.\r\n",
  CWD_SUCCESSFUL[]     = "250 CWD command successful.\r\n",
  PWD_PREFIX[]         = "257 Current directory is ",   /* Must have more */
  USERNAME_MESSAGE[]   = "331 Password required for login.\r\n",
  DATA_CONNECT_FAIL[]  = "425 Data port connection failure.\r\n",
  ERROR_MESSAGE[]      = "500 Command not Understood.\r\n",
  PASSWORD_FAILURE[]   = "530 Login Incorrect.\r\n",
  USE_USER_FIRST[]     = "503 Login with USER First.\r\n",
  NO_SUCH_FILE[]       = "550 No such file or location.\r\n",
  CANNOT_CREATE_FILE[] = "550 Cannot create file.\r\n";

/*  Des commandes FTP supportees par ce serveur.     */
enum _CommandType { ERROR, TYPE, MODE, NLST, RETR, NOOP, STOR,
		    USER, PASS, QUIT, PWD, CWD, PORT, PASV, SYST };
typedef enum _CommandType CommandType;

typedef struct _thread_data {
    int cid;
} THREAD_DATA, *PTHREAD_DATA;

#endif	/* _COMMON_H */

