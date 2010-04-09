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

    int changeDirectory(int socket, string& folderPath);

    int listFile(int socket, string& pathName);

    int getFile(int socket, string& fileName);

    int putFile(int socket, string& fileName);

    int makeDirectory(int socket, string& pathName);

    int removeDirectory(int socket, string& pathName);

    int deleteFile(int socket, string& pathName);

    int toBinary(int socket, bool isBinary);

    static string getCurrentDir()
    {
       char cwd[1024];
       if (getcwd(cwd, sizeof(cwd)) != NULL)
           return cwd;
       else
           perror("getcwd() error");
       return "";
    }

    static string splitFileName(const string& path)
    {
        string current = getCurrentDir();
        size_t found;
        found=path.find_last_of("/\\");
        return current + "/" + path.substr(found+1);
    }
    
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

