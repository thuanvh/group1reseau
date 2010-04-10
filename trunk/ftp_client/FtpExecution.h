/* 
 * File:   FtpExecution.h
 * Author: langthang
 *
 * Created on April 3, 2010, 3:31 AM
 */

#include <string>
#include "FtpConnection.h"
#include <unistd.h>

using namespace std;

#ifndef _FTPEXECUTION_H
#define	_FTPEXECUTION_H

// FTP information: error and warning
const static string STRING_TIME_OUT = "Timeout.";
const static string STRING_FILE_FAILED = "Failed to open file.";

 // FTP command standard string
const static std::string FTP_COMMAND_CD = "CWD";
const static std::string FTP_COMMAND_LS = "LIST";
const static std::string FTP_COMMAND_TYPE = "TYPE";
const static std::string FTP_COMMAND_GET = "RETR";
const static std::string FTP_COMMAND_PUT = "STOR";
const static std::string FTP_COMMAND_QUIT = "QUIT";
const static std::string FTP_COMMAND_USER = "USER";
const static std::string FTP_COMMAND_PASS = "PASS";
const static std::string FTP_COMMAND_PASV = "PASV";
const static std::string FTP_COMMAND_SYST = "SYST";
const static std::string FTP_COMMAND_MKDIR = "MKD";
const static std::string FTP_COMMAND_RMDIR = "RMD";
const static std::string FTP_COMMAND_DEL = "DELE";
const static std::string FTP_COMMAND_TYPE_BINARY = "TYPE I";
const static std::string FTP_COMMAND_TYPE_ASCII = "TYPE A";

// client defined command
const static std::string CLIENT_FTP_COMMAND_OPEN = "OPEN";
const static std::string CLIENT_FTP_COMMAND_CLOSE = "CLOSE";

class FtpExecution{
public:

    // mode ascii or binary to transfer file
    bool ascii;
    
    FtpExecution();
    virtual ~FtpExecution();

    // open a connection
    int openConnection( const string& address, const string& port = "21");
    // close a connection
    int closeConnection(int socket);

    // send command to server
    int sendCommand(const string& code, const string& info);

    void changeTransmissionMethod();

    // username to login to server
    int user(int socket, const string& username);
    // check password for login
    int login(int socket, const string& password);

    // connect to passive mode
    int connectPassive(int sock);

    // method cd change le folder current
    int changeDirectory(int socket, string& folderPath);

    // method ls to get the list file from server
    int listFile(int socket, string& pathName);

    // method get to get content of a file
    int getFile(int socket, string& fileName);

    // method put to put file to server
    int putFile(int socket, string& fileName);

    // create a folder on server
    int makeDirectory(int socket, string& pathName);

    // remove a folder on server
    int removeDirectory(int socket, string& pathName);

    // delete a file on server
    int deleteFile(int socket, string& pathName);

    // change to binary mode ou ascii mode
    int toBinary(int socket, bool isBinary);
    
    // get ip and port of reponse from server
    static void getAddressPort(string buf, int port[], string& address)
    {
        char *ip = new char[15];
	char *str_temps;
        char* buffer;
        strcpy(buffer,buf.c_str());

        // get ip
	str_temps = strchr(buffer, '(') + 1;
	int nSplit = 0;
	int i = 0;
	memset(ip, 0, 15);
        
	while (nSplit < 4)
	{
            if (str_temps[i] == ',')
            {
                    nSplit ++;
                    if (nSplit < 4) ip[i] = '.';
            }
            else ip[i] = str_temps[i];
            i ++;
	}

	// get port
	str_temps += i * sizeof(char);
	char *str1 = strchr(str_temps, ',');
	str1[0] = 0;
	port[0] = atoi(str_temps);

	str_temps = str1 + sizeof(char);
	str1 = strchr(str_temps, ')');
	str1[0] = 0;
	port[1] = atoi(str_temps);
        address = ip;
    }
    
};
    

#endif	/* _FTPEXECUTION_H */

