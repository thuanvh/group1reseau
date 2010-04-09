/*
 * File:   FtpCommand.h
 * Author: langthang
 *
 * Created on April 3, 2010, 1:13 AM
 */

#ifndef _FtpCommand_H
#define	_FtpCommand_H
#include <string>
#include "FtpExecution.h"
using namespace std;

    // information string to inform the error or the warning ...
    const static string STRING_LOGIN_REFUSED = "Login incorrect. Please login with USER and PASS.";
    const static string STRING_UNKNOWN = "Unknown problem.";
    const static string STRING_NEED_PASSWORD = "Please specify the password.";
    const static string STRING_NOT_CONNECTED = "Not connected.";
    const static string STRING_DATA_CONNECTED = "Connected.";
    const static string STRING_USER_CONNECTED = "Welcome to my FTP service.";
    const static string STRING_ALREADY_CONNECTED = "Already connected.";
    const static string STRING_QUIT = "Goodbye.";
    const static string STRING_PORT_ACCEPT = "PORT command successful.";
    const static string STRING_FILE_SEND_OK = "File send OK";
    const static string STRING_COMMAND_INVALID = "Invalid command";

     // FTP command string
    const static string MY_FTP_COMMAND_CD = "cd";
    const static string MY_FTP_COMMAND_LS = "ls";
    const static string MY_FTP_COMMAND_GET = "get";
    const static string MY_FTP_COMMAND_PUT = "put";
    const static string MY_FTP_COMMAND_USER = "user";
    const static string MY_FTP_COMMAND_QUIT = "quit";
    const static string MY_FTP_COMMAND_BINARY = "binary";
    const static string MY_FTP_COMMAND_ASCII = "ascii";
    const static string MY_FTP_COMMAND_MKDIR = "mkdir";
    const static string MY_FTP_COMMAND_RMDIR = "rmdir";
    const static string MY_FTP_COMMAND_DEL = "del";

    // client defined command
    const static string MY_CLIENT_FTP_COMMAND_OPEN = "open";
    const static string MY_CLIENT_FTP_COMMAND_CLOSE = "close";

class FtpCommand {
private:
    
    FtpConnection _ftpConnection;
    FtpExecution _ftpExecution;

    int _connection_status;
    string _host;
    string _port;
    
    string _code;
    string _info;

    bool _status;
    bool _login;

    int _socket;

public:
    FtpCommand();
    virtual ~FtpCommand();

    int getArguments(int argc, char* argv[]);
    void readCommandLine();
    void handleCommand();

    string& getCommandCode();
    string& getCommandInfo();

    string& getHost();
    string& getPort();

    bool isRunning();
    bool isLogin();

    static int getIntCommandCode(const string& command_code){
        int code = -1;
        
        if (command_code == MY_FTP_COMMAND_CD)
                code = FtpConstant::FTP_CD;

        if (command_code == MY_FTP_COMMAND_GET)
                code = FtpConstant::FTP_GET;

        if (command_code == MY_FTP_COMMAND_LS)
                code = FtpConstant::FTP_LS;

        if (command_code == MY_FTP_COMMAND_PUT)
                code = FtpConstant::FTP_PUT;

        if (command_code == MY_FTP_COMMAND_QUIT)
                code = FtpConstant::FTP_QUIT;

        if (command_code == MY_FTP_COMMAND_USER)
                code = FtpConstant::FTP_USER;

        if (command_code == MY_FTP_COMMAND_MKDIR)
                code = FtpConstant::FTP_MKDIR;

        if (command_code == MY_FTP_COMMAND_RMDIR)
                code = FtpConstant::FTP_RMDIR;

        if (command_code == MY_FTP_COMMAND_DEL)
                code = FtpConstant::FTP_DEL;

        if (command_code == MY_FTP_COMMAND_BINARY)
                code = FtpConstant::FTP_BINARY;

        if (command_code == MY_FTP_COMMAND_ASCII)
                code = FtpConstant::FTP_ASCII;

        if (command_code == MY_CLIENT_FTP_COMMAND_CLOSE)
                code = FtpConstant::CLIENT_FTP_CLOSE;
        
        if (command_code == MY_CLIENT_FTP_COMMAND_OPEN)
                code = FtpConstant::CLIENT_FTP_OPEN;

        return code;
    }

     //will split a string into 2
    static void splitString(string str, string separator, 
    string &first, string &second) {
        size_t i = str.find(separator); //find seperator

        if(i != string::npos)  {
            size_t y = 0;

            if(!str.empty())  {
                first=""; second="";

                while(y != i)    {
                    first += str[y++]; //creating first string
                }

                y += separator.length(); //jumping forward separator length

                while(y != str.length()) {
                    second += str[y++]; //creating second string
                }
            }
        }
        else   {
          first = str;
          second = ""; //if seperator is not there then second string == empty string
        }
    }
};

#endif	/* _FtpCommand_H */