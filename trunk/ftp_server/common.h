/* 
 * File:   common.h
 * Author: trung
 *
 * Created on March 31, 2010, 3:18 PM
 */

#ifndef _COMMON_H
#define	_COMMON_H

#include <stdlib.h>
#include <stdio.h>
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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>

using namespace std;

#define BUFFER 4096
#define MAX_CLIENT 20
#define YES 1
#define NO 0

/*  All the FTP response codes that this program supports.  */

#define READY_FOR_NLST          "150 ASCII mode data connection for directory list"
#define READY_I_FOR_TRANSFER    "150 Binary data connection for transfer"
#define READY_A_FOR_TRANSFER    "150 ASCII mode data connection for transfer"
#define TYPE_A_RESPONSE         "200 Type set to A.\r\n"
#define TYPE_I_RESPONSE         "200 Type set to I.\r\n"
#define MODE_RESPONSE           "200 Only MODE S is implemented.\r\n"
#define NOOP_RESPONSE           "200 NOOP command successful.\r\n"
#define PORT_RESPONSE           "200 PORT command successful.\r\n"
#define WELCOME_MESSAGE         "220 FTP server (Group 1) ready.\r\n"
#define GOODBYE                 "221 Goodbye.\r\n"
#define TRANSFER_COMPLETE       "226 Transfer complete\r\n"
#define USER_LOGGED_IN          "230 User logged in.\r\n"
#define CWD_SUCCESSFUL          "250 CWD command successful.\r\n"
#define PWD_PREFIX              "257 Current directory is "
#define USERNAME_MESSAGE        "331 Password required for login.\r\n"
#define DATA_CONNECT_FAIL       "425 Data port connection failure.\r\n"
#define ERROR_MESSAGE           "500 Command not Understood.\r\n"
#define PASSWORD_FAILURE        "530 Login Incorrect.\r\n"
#define USE_USER_FIRST          "503 Login with USER First.\r\n"
#define NO_SUCH_FILE            "550 No such file or location.\r\n"
#define CANNOT_CREATE_FILE      "550 Cannot create file.\r\n"
#define PASSIVE_MODE            "227 Entering Passive Mode %s.\r\n"
#define SERVER_TYPE             "215 UNIX Type: L8\r\n"

/*  Des commandes FTP supportees par ce serveur.     */
enum _CommandType { ERROR, TYPE, MODE, NLST, RETR, NOOP, STOR,
		    USER, PASS, QUIT, PWD, CWD, PORT, PASV, SYST, LIST };
typedef enum _CommandType CommandType;

typedef struct _thread_data {
    int cid;
    in_addr_t host;
} THREAD_DATA, *PTHREAD_DATA;

#endif	/* _COMMON_H */

