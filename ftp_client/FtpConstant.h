/* 
 * File:   FtpConstant.h
 * Author: langthang
 *
 * Created on April 3, 2010, 1:19 AM
 */

#ifndef _FTPCONSTANT_H
#define	_FTPCONSTANT_H

#include <string>
using namespace std;

// class contient des constantes des annonces et des commandlines
class FtpConstant{
    
public:

    // des code de l'annonce
    const static int CODE_LOGIN_REFUSED = 530;
    const static int CODE_TIME_OUT = 421;
    const static int CODE_UNKNOWN = 230;
    const static int CODE_ACCEPT_USER_CONNECT = 230;
    const static int CODE_NEED_PASSWORD = 331;
    const static int CODE_PASSIVE_MODE = 227;
    const static int CODE_ACCEPT_DATA_CONNECT = 150;
    const static int CODE_QUIT = 221;
    const static int CODE_FILE_FAILED = 550;
    const static int CODE_PORT_ACCEPT = 200;
    const static int CODE_FILE_SEND_OK = 226;

   

    // connection status
    const static int FTP_NOT_CONNECT = 0;
    const static int FTP_CONNECTED = 1;
    
    const static int FTP_LOGIN = 0;
    const static int FTP_NOT_LOGIN = 1;

    
    // command code
    const static int FTP_CD = 2;
    const static int FTP_LS = 3;
    const static int FTP_GET = 4;
    const static int FTP_PUT = 5;
    const static int FTP_QUIT = 6;
    const static int FTP_USER = 7;

    const static int CLIENT_FTP_OPEN = 0;
    const static int CLIENT_FTP_CLOSE = 1;

   
};


#endif	/* _FTPCONSTANT_H */

