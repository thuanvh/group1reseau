/*
 * File:   FtpCommand.cpp
 * Author: langthang
 *
 * Created on April 3, 2010, 1:13 AM
 */


#include <iostream>
#include "FtpCommand.h"
#include "FtpConstant.h"


FtpCommand::FtpCommand() {
	_status = true;
        _login = false;
        _socket = -1;
        _connection_status = FtpConstant::FTP_NOT_CONNECT;
        //this->_ftpExecution = new FtpExecution();
        //this->_ftpConnection = new FtpConnection();
}

FtpCommand::~FtpCommand() {
}

string& FtpCommand::getHost() {
	return _host;
}

string& FtpCommand::getPort() {
	return _port;
}

bool FtpCommand::isRunning() {
	return _status;
}

int FtpCommand::getArguments(int argc, char* argv[]) {

    if(strcmp(argv[0],MY_CLIENT_FTP_COMMAND_OPEN.c_str()) == 0)
    {
        _code = MY_CLIENT_FTP_COMMAND_OPEN;
     /*   if(argc == 1)
        {
            _code = "OPEN";
            string str_host_port;
            cout<<"to: ";
            cin >> str_host_port;
            splitString(str_host_port, " ", _host, _port);
            if(_port.compare("") == 0)
                _port = "21";
            _connection_status = FtpConstant::FTP_OPEN_CONNECTION;
        }*/
            
        if (argc == 2) {
            _host = argv[1];
            _port = "21";
        }
        if(argc == 3){
            _host = argv[1];
            _port = argv[2];
        }
    }
    return 1;
}

void FtpCommand::handleCommand()
{
    //int socket = -1;
    int code = FtpCommand::getIntCommandCode(_code);
    //readCommandLine();

    /*
    bool cont = true;
    char buf[256];
    char *command = new char[256];
    char *param = new char[256];
    char *param1 = (char *)"";
    while (cont)
    {*/
    //socket = this->_ftpExecution.openConnection(_host,_port);

    if(code == FtpConstant::FTP_QUIT) {
       if (_socket != -1) {
            this->_ftpExecution.closeConnection(_socket);
            _socket = -1;
            _status = false;
            return;
        }
        else
        {
           _status = false;
           return;
        }
    }
    
    if (code == FtpConstant::CLIENT_FTP_OPEN)
    {
        if (_socket == -1)
        {
            int sock = this->_ftpExecution.openConnection(_host,_port);
            if(sock != -1)
            {
                string username;
                cout<<"Name: ";
                getline(cin,username);
                code = _ftpExecution.user(sock, username);
                if(code == -1)
                    _login = false;
                else
                {
                    _socket = sock;
                    _login = true;
                }
            }
            return;
        }
        else
        {
            cout<<STRING_ALREADY_CONNECTED<<endl;
            return;
        }
    }

    if (_socket != -1)
    {
        if (code == FtpConstant::FTP_USER)
        {
            code = _ftpExecution.user(_socket, _info);
            if(code == -1)
                _login = false;
             else
                _login = true;
            return;
        }

        if (_login == true)
        {
            if (code == FtpConstant::FTP_LS)
            {
                _ftpExecution.listFile(_socket, _info);
                return;
            }
            if (code == FtpConstant::FTP_CD)
            {
                _ftpExecution.cd(_socket, _info);
                return;
            }
            if (code == FtpConstant::FTP_GET)
            {
                _ftpExecution.getFile(_socket, _info);
                return;
            }
            if (code == FtpConstant::FTP_PUT)
            {
                _ftpExecution.putFile(_socket, _info);
                return;
            }
            /*
            if (strcmp(command, "binary") == 0)
            {
                binary(sock, true);
                continue;
            }

            if (strcmp(command, "ascii") == 0)
            {
                binary(sock, false);
                continue;
            }
            */
            if (code == FtpConstant::CLIENT_FTP_CLOSE)
            {
                _ftpExecution.closeConnection(_socket);
                _socket = -1;
                return;
            }
        }
        else
        {
            cout<<STRING_NOT_CONNECTED<<endl;
            return;
        }
    }
    else
    {
        cout<<STRING_NOT_CONNECTED<<endl;
        return;
    }
    cout<<STRING_COMMAND_INVALID<<endl;
    
}

void FtpCommand::readCommandLine()
{
    string command, command1, command2, command3, command4;
    cout<<"ftp>";
    getline (cin, command);
    cin.clear();
    FtpCommand::splitString(command," ",command1, command2);
    if(strcmp(command2.c_str(),"") == 0)
    {
        _code = command1;
        _info = command2;
        if(_code == MY_CLIENT_FTP_COMMAND_OPEN)
        {
            cout<<"(to) ";
            getline (cin, command2);
            splitString(command2," ",command3,command4);
            if(strcmp(command4.c_str(),"") == 0)
            {
                _host = command3;
                _port = "21";
            }
            else
            {
                _host = command3;
                _port = command4;
            }
        }
    }
    else
    {
        _code = command1;
        _info = command2;
        if(_code == MY_CLIENT_FTP_COMMAND_OPEN)
        {
            splitString(command2," ",command3,command4);
            if(strcmp(command4.c_str(),"") == 0)
            {
                _host = command3;
                _port = "21";
            }
            else
            {
                _host = command3;
                _port = command4;
            }
        }
    }
}
