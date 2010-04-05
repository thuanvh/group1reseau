#include "FtpExecution.h"
#include "FtpConstant.h"
#include "iostream"

FtpExecution::FtpExecution(){
    this->ascii = false;
    
}

FtpExecution::~FtpExecution(){
    
}

int FtpExecution::openConnection(const string& host, const string& port) {
	struct sockaddr_in _address;
	struct hostent * _hostent;
	int _socket = -1;
	int _code;
        
	//cout<<"Server name: "<<host<<" and port = "<<port;

	//Get l'adresse du serveur
	if (!inet_aton(host.c_str(), &_address.sin_addr))
	{
            _hostent = gethostbyname(host.c_str());
            if (!_hostent)
            {
                herror("gethostbyname");
                return -1;
            }
            else
                _address.sin_addr.s_addr =
                        inet_addr(inet_ntoa(*((struct in_addr *)_hostent->h_addr)));
	}


	//Créer le socket
	if((_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
            herror("socket");
            return -1;
	}

	_address.sin_family=AF_INET;
	_address.sin_port=htons(atoi(port.c_str()));

	//Connecter
	if(connect(_socket, (struct sockaddr *)&_address,
                                    sizeof(struct sockaddr)) == -1)
	{
            herror("connect");
            return -1;
	}

	/*
        char buf[256];
	memset(buf, 0, 256);
	_code = FtpConnection::receiveTimeout(_socket, buf, 256, 0);
	if (_code == -1)
	{
            herror("receive");
            return -1;
	}
         * */
        string _buffer;
        _code = FtpConnection::receiveResponse(_socket, _buffer);
        
	if (_code == -1)
	{
            herror("receive");
            return -1;
	}

	cout<<_buffer;
	return _socket;
}

int FtpExecution::closeConnection(int socket) {

    int _result = FtpConnection::sendCommand(socket, FTP_COMMAND_QUIT+"\r\n");

    /*char _buffer[256];
    memset(_buffer, 0, 256);
    */
    string _buffer;
    FtpConnection::receiveResponse(socket, _buffer);

    cout<<endl<<_buffer;
    return _result;
}

int FtpExecution::user(int socket, const string& username) {
    //char _cmd[256];
    //char _buf[256];
    //memset(_buf, 0, 256);
    
    string _buffer;
    string str_user_command = FTP_COMMAND_USER + " " + username + "\r\n";

    int _result = FtpConnection::sendCommand(socket, str_user_command);
    int   _code =  FtpConnection::receiveResponse(socket, _buffer);
    //int   _code =  FtpConnection::receiveTimeout(socket, _buf, 256, 0);
    
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
    }
    
    return _result;
}

/** Etablir la mode passive pour envoyer des données
 * @param sock l'identificateur du socket qu'on va utiliser
 */
int FtpExecution::connectPassive(int sock)
{
	struct sockaddr_in addr;
	char buf[256];
	char ip[15];
	char strPort[4];
	int port[2];
	char *str;
	int sockData;
        //string _buffer;

	//Demander
	FtpConnection::sendCommand(sock, FTP_COMMAND_PASV+"\r\n");

	memset(buf, 0, 256);
	int code = FtpConnection::receiveTimeout(sock, buf, 256, 0);
	cout<<buf;
        
	//printf("\nCode = %d   ", code);
	if(code == -1)
	{
            cout<< STRING_TIME_OUT<<endl;
            return -1;
	}

	//Prendre l'adresse du serveur
	str = strchr(buf, '(') + 1;
	int nComma = 0;
	int i = 0;
	memset(ip, 0, 15);
	while (nComma < 4)
	{
            if (str[i] == ',')
            {
                    nComma ++;
                    if (nComma < 4) ip[i] = '.';
            }
            else ip[i] = str[i];
            i ++;
	}

	//Prendre les portes
	str += i * sizeof(char);
	char *str1 = strchr(str, ',');
	str1[0] = 0;
	port[0] = atoi(str);
	str = str1 + sizeof(char);
	str1 = strchr(str, ')');
	str1[0] = 0;

	port[1] = atoi(str);

	//Créer le socket pour prendre des données
        /*get the socket descriptor*/
	if((sockData=socket(AF_INET, SOCK_STREAM, 0))==-1)
	{
            herror("socket");
            return -1;
	}

	addr.sin_family=AF_INET;
	addr.sin_addr.s_addr = inet_addr (ip);
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

int FtpExecution::cd(int socket, const string pathname)
{
    string _buffer;
    string _cmd = FTP_COMMAND_CD + " " + pathname + "\r\n";

    FtpConnection::sendCommand(socket, _cmd);

    if(FtpConnection::receiveResponse(socket, _buffer) != -1)
    {
        cout<<_buffer;
    }

    return 0;
}

int FtpExecution::listFile(int socket, string folderPath) {
    
    int sockData = connectPassive(socket);
   // cout<<"\nsocket = "<<sockData<<endl;

    //Demander
    string _cmd;
    if (folderPath == "")
        folderPath = "/home";
    _cmd = FTP_COMMAND_LS + " " + folderPath+"\r\n";

    FtpConnection::sendCommand(socket, _cmd);

    char *list = new char[1000];
    int i;
    if ((i = FtpConnection::receiveTimeout(socket, list, 999, 0)) != 1)
    {
        list[i] = 0;
        cout<<list;
    }

    //Recevoir et afficher la liste
    while((i = FtpConnection::receiveTimeout(sockData, list, 1000, 0)) > 0)
    {
        list[i] = 0;
        cout<<list;
    }

    close(sockData);
    if ((i = FtpConnection::receiveTimeout(socket, list, 999, 0)) != 1)
    {
        list[i] = 0;
        cout<<list;
    }
    return 0;
}

int FtpExecution::getFile(int socket, string fileName)
{
    if(fileName != "")
    {
        if(fileName[0] != '/')
        {
            fileName = "/home/" + fileName;
        }
    }
    int sockData = connectPassive(socket);
    //printf("\nsock = %d\n", sockData);

    char *list = new char[1000];
    int i;
    memset(list, 0, 1000);

    string _cmd = FTP_COMMAND_GET+ fileName + "\r\n";
    FtpConnection::sendCommand(socket, _cmd);

    if ((i = FtpConnection::receiveTimeout(socket, list, 999, 0)) != 1)
    {
        list[i] = 0;
        cout<<list;
    }
    time_t begin = time(NULL);


    FILE *file;
    if (ascii)
    {
        file = fopen(fileName.c_str(), "a");
    }
    else
    {
        file = fopen(fileName.c_str(), "ab");
    }
    if (file == NULL)
    {
        cout<<STRING_FILE_FAILED<<endl;
        return -1;
    }

    cout<<"\nDownloading ...\n";
    while((i = FtpConnection::receiveTimeout(sockData, list, 999, 0)) > 0)
    {
        if (ascii)
        {
            for (int j = 0 ; j < i; j++) if (list[j] != '\r')
                putc (list[j], file);
        }
        else
        {
            fwrite(list, sizeof(char), i, file);
        }
    }

    close(sockData);
    fclose(file);

    double t = difftime ( time(NULL), begin);
    printf("\nTime execute: %f\n", t);

    memset(list, 0, 999);
    if ((i = FtpConnection::receiveTimeout(socket, list, 256, 0)) != 1)
    {
        list[i] = 0;
        cout<<list;
    }
    return 0;
}

int FtpExecution::putFile(int socket, string fileName)
{
    if(fileName != "")
    {
        if(fileName[0] != '/')
        {
            fileName = "/home/" + fileName;
        }
    }

    int sockData = connectPassive(socket);
    cout<<"\nsock = "<<sockData<<endl;

    char *list = new char[1001];
    int i;

    string cmd = FTP_COMMAND_PUT + fileName + "\r\n";
    //char *cmd = new char[256];
    //sprintf(cmd, "STOR %s\r\n", fileName.c_str());
    FtpConnection::sendCommand(socket, cmd);

    time_t begin = time(NULL);
    memset(list, 0, 1000);
    if ((i = FtpConnection::receiveTimeout(socket, list, 256, 0)) != 1)
    {
        list[i] = 0;
        cout<<list;
    }

    FILE *file;
    if (ascii)
    {
        file = fopen(fileName.c_str(), "r");
    }
    else
    {
        file = fopen(fileName.c_str(), "rb");
    }

    //Envoyer le fichier
    printf("\nUploading ...\n");
    while(!feof(file))
    {
        i=sizeof(char) * fread(list, sizeof(char), 1000, file);
        FtpConnection::sendTimeout(sockData, list, i, 0);
    }

    close(sockData);
    fclose(file);
    double t = difftime ( time(NULL), begin);
    printf("\nTime execute: %f\n", t);

    memset(list, 0, 1000);
    if ((i = FtpConnection::receiveTimeout(socket, list, 256, 0)) != 1)
    {
        list[i] = 0;
        cout<<list;
    }
    
    return 0;
}
