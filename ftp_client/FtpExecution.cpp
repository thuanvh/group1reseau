#include "FtpExecution.h"
#include "FtpConstant.h"
#include "iostream"

FtpExecution::FtpExecution(){
    this->ascii = false;
}

FtpExecution::~FtpExecution(){
    //
}

int FtpExecution::openConnection(const string& host, const string& port) {

    struct sockaddr_in _address;
    struct hostent * _hostent;

    int _socket = -1;
    int _code;

    string _buffer;

    if (!inet_aton(host.c_str(), &_address.sin_addr))
    {
        _hostent = gethostbyname(host.c_str());
        if (!_hostent)  {
            herror("gethostbyname");
            return -1;
        }
        else
            _address.sin_addr.s_addr =
                    inet_addr(inet_ntoa(*((struct in_addr *)_hostent->h_addr)));
    }
    
    if((_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)   {
        herror("socket");
        return -1;
    }

    _address.sin_family=AF_INET;
    _address.sin_port=htons(atoi(port.c_str()));

    if(connect(_socket, (struct sockaddr *)&_address,
                                sizeof(struct sockaddr)) == -1) {
        herror("connect");
        return -1;
    }

    _code = FtpConnection::receiveResponse(_socket, _buffer);

    if (_code == -1)    {
        herror("receive");
        return -1;
    }
    
    cout<<_buffer;
    return _socket;
}

int FtpExecution::closeConnection(int socket) {
    string _buffer;
    int _result = FtpConnection::sendCommand(socket, FTP_COMMAND_QUIT+"\r\n");
    FtpConnection::receiveResponse(socket, _buffer);
    cout<<endl<<_buffer;
    return _result;
}

int FtpExecution::makeDirectory(int socket, string& pathName)   {
    string buffer;
    int result = FtpConnection::sendCommand(socket, FTP_COMMAND_MKDIR + " " + pathName + "\r\n");
    FtpConnection::receiveResponse(socket, buffer);
    cout<<buffer;
    return result;
}

int FtpExecution::removeDirectory(int socket, string& pathName)
{
    string buffer;
    int result = FtpConnection::sendCommand(socket, FTP_COMMAND_RMDIR + " " + pathName + "\r\n");
    FtpConnection::receiveResponse(socket, buffer);
    cout<<buffer;
    return result;
}

int FtpExecution::deleteFile(int socket, string& pathName)
{   
    int result = FtpConnection::sendCommand(socket, FTP_COMMAND_DEL + " " + pathName + "\r\n");
    string buffer;
    if(FtpConnection::receiveResponse(socket, buffer) != -1)    {
        cout<<buffer;
    }
    return result;
}

int FtpExecution::user(int socket, const string& username) {
    
    string _buffer;
    string str_user_command = FTP_COMMAND_USER + " " + username + "\r\n";

    int _result = FtpConnection::sendCommand(socket, str_user_command);
    int   _code =  FtpConnection::receiveResponse(socket, _buffer);
    
    cout<<_buffer;

    _code = atoi(_buffer.c_str());

    if (_code == FtpConstant::CODE_NEED_PASSWORD) {
        // username ok, need password
        string password;
        password = getpass("Password: ");
        _result = login( socket, password);
    }

    return _result;
}

int FtpExecution::login(int socket, const string& password) {
    string _buffer;
    string str_pass_command = FTP_COMMAND_PASS + " " + password+"\r\n";
    
    int _result = FtpConnection::sendCommand(socket, str_pass_command);
    int _code   = FtpConnection::receiveResponse(socket, _buffer);
    cout<< _buffer;
    _code = atoi(_buffer.c_str());
    if (_code == FtpConstant::CODE_ACCEPT_USER_CONNECT) {
        // login successful
        _result = FtpConnection::sendCommand(socket, FTP_COMMAND_SYST+"\r\n");
        _code = FtpConnection::receiveResponse(socket, _buffer);
        cout<< _buffer;
    }
    
    return _result;
}

int FtpExecution::connectPassive(int sock)
{
	struct sockaddr_in addr;
	string buffer;
        int port[2];
        string ip;
	int sockData;
        
	FtpConnection::sendCommand(sock, FTP_COMMAND_PASV+"\r\n");

	int code = FtpConnection::receiveResponse(sock, buffer);
	cout<<buffer;
        
	if(code == -1)  {
            cout<< STRING_TIME_OUT<<endl;
            return -1;
	}

        FtpExecution::getAddressPort(buffer, port, ip);

        /*get the socket descriptor*/
	if((sockData = socket(AF_INET, SOCK_STREAM, 0))==-1)
	{
            herror("socket");
            return -1;
	}

	addr.sin_family=AF_INET;
	addr.sin_addr.s_addr = inet_addr (ip.c_str());
	addr.sin_port= htons (256 * port[0] + port[1]);
	bzero(&(addr.sin_zero),8);

	//Connecter
	if(connect(sockData, (struct sockaddr *)&addr,
                            sizeof(struct sockaddr)) == -1)
	{
            herror("connect");
            return -1;
	}

	return sockData;
}

int FtpExecution::changeDirectory(int socket, string& pathname)
{
    string _buffer;
    FtpConnection::sendCommand(socket, FTP_COMMAND_CD + " " + pathname + "\r\n");
    if(FtpConnection::receiveResponse(socket, _buffer) != -1)   {
        cout<<_buffer;
        if(_buffer.substr(0,3) == "550")
            return 0;
        else
            return 1;
    }
    return 0;
}

int FtpExecution::listFile(int socket, string& folderPath) {
    
    int sockData = connectPassive(socket);
    string _cmd;
    if (folderPath == "")
        _cmd = FTP_COMMAND_LS + "\r\n";
    else
        _cmd = FTP_COMMAND_LS + " " + folderPath + "\r\n";

    FtpConnection::sendCommand(socket, _cmd);

    char *list = new char[1000];
    int i;
    if ((i = FtpConnection::receiveTimeout(socket, list, 999, 0)) != 1) {
        list[i] = 0;
        cout<<list;
    }

    while((i = FtpConnection::receiveTimeout(sockData, list, 1000, 0)) > 0) {
        list[i] = 0;
        cout<<list;
    }

    close(sockData);

    if ((i = FtpConnection::receiveTimeout(socket, list, 999, 0)) != 1) {
        list[i] = 0;
        cout<<list;
    }
    return 0;
}

int FtpExecution::getFile(int socket, string& fileName)
{
    FILE *file;

    int sockData = connectPassive(socket);
    char *list = new char[1000];
    int i;

    memset(list, 0, 1000);
    string _cmd = FTP_COMMAND_GET+ " " + fileName + "\r\n";
    FtpConnection::sendCommand(socket, _cmd);

    if ((i = FtpConnection::receiveTimeout(socket, list, 999, 0)) != 1) {
        list[i] = 0;
        cout<<list;
        string str = list;
        if(str.substr(0,3) == "550")
            return -1;
    }
    else return -1;

    time_t begin = time(NULL);
    
    if (ascii)
        file = fopen(fileName.c_str(), "w");
    else
        file = fopen(fileName.c_str(), "wb");
    
    if (file == NULL)   {
        cout<<STRING_FILE_FAILED<<endl;
        return -1;
    }
    else    {
        int k = 0;
        while((i = FtpConnection::receiveTimeout(sockData, list, 999, 0)) > 0)  {
            if (ascii)  {
                for (int j = 0 ; j < i; j++)    {
                    if (list[j] != '\r')
                        putc (list[j], file);
                    if (list[j] == '\n')
                        k = k + 1;
                }
            }
            else
                fwrite(list, sizeof(char), i, file);
        }

        if(ascii && k > 0) {
            cout<<"WARNING! "<< k <<" bare linefeeds received in ASCII mode"<<endl;
            cout<<"File may not have transferred correctly."<<endl;
        }

        close(sockData);
        double t = difftime ( time(NULL), begin);
        cout<<"Time to receive: "<<t<<endl;
    }
    
    fclose(file);

    memset(list, 0, 999);
    if ((i = FtpConnection::receiveTimeout(socket, list, 256, 0)) != 1){
        list[i] = 0;
        cout<<list;
    }
    return 0;
}

int FtpExecution::putFile(int socket, string& fileName)
{
     FILE *file;
    if (ascii)
        file = fopen(fileName.c_str(), "r");
    else
        file = fopen(fileName.c_str(), "rb");

    if (file == NULL)   {
        cout<<STRING_FILE_FAILED<<endl;
        return -1;
    }
     
    int sockData = connectPassive(socket);

    char *list = new char[1001];
    int i;

    string cmd = FTP_COMMAND_PUT + " " + fileName + "\r\n";
    FtpConnection::sendCommand(socket, cmd);

    time_t begin = time(NULL);
    memset(list, 0, 1000);
    if ((i = FtpConnection::receiveTimeout(socket, list, 256, 0)) != 1) {
        list[i] = 0;
        cout<<list;
        string str = list;
        if(str.substr(0,3) == "550")
            return -1;
    }
    else
        return -1;

    while(!feof(file))  {
        i=sizeof(char) * fread(list, sizeof(char), 1000, file);
        FtpConnection::sendTimeout(sockData, list, i, 0);
    }

    close(sockData);
    double t = difftime ( time(NULL), begin);
    cout<<"Time to send: "<<t<<endl;
    
    fclose(file);
    memset(list, 0, 1000);
    if ((i = FtpConnection::receiveTimeout(socket, list, 256, 0)) != 1){
        list[i] = 0;
        cout<<list;
    }
    
    return 0;
}

int FtpExecution::toBinary(int socket, bool isBinary)
{
    ascii = !isBinary;
    string _buffer;
    string cmd;
    
    if (isBinary)
        cmd = FTP_COMMAND_TYPE_BINARY + "\r\n";
    else
        cmd = FTP_COMMAND_TYPE_ASCII + "\r\n";

    FtpConnection::sendCommand(socket, cmd);
    
    if(FtpConnection::receiveResponse(socket, _buffer) != -1)
        cout<< _buffer;
    
    return 0;
}
