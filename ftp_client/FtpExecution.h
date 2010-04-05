/* 
 * File:   FtpExecution.h
 * Author: langthang
 *
 * Created on April 3, 2010, 3:31 AM
 */

#include <string>
#include "FtpConnection.h"
using namespace std;

#ifndef _FTPEXECUTION_H
#define	_FTPEXECUTION_H

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

// client defined command
const static std::string CLIENT_FTP_COMMAND_OPEN = "OPEN";
const static std::string CLIENT_FTP_COMMAND_CLOSE = "CLOSE";

class FtpExecution{
public:

    bool ascii;
    
    FtpExecution();
    virtual ~FtpExecution();

    int openConnection( const string& address, const string& port = "21");
    int closeConnection(int socket);

    int sendCommand(const string& code, const string& info);
    //int receiveResponse(string& code, string& arg);

    void changeTransmissionMethod();

    int user(int socket, const string& username);
    int login(int socket, const string& password);

    int connectPassive(int sock);

    int cd(int socket, const string folderPath);

    int listFile(int socket, string pathName);

    int getFile(int socket, string fileName);
    //int receiveFile(int socket, const string& fileName);

    int putFile(int socket, string fileName);
    //int sendFile(int socket, const string& fileName);

    //int makeclose();
    //int quit();
    
};

 
    

#endif	/* _FTPEXECUTION_H */

