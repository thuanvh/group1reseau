/* 
 * File:   FTPProcess.h
 * Author: trung
 *
 * Created on April 2, 2010, 4:37 PM
 */

#ifndef _FTPPROCESS_H
#define	_FTPPROCESS_H

#include "common.h"

class FTPProcess {
public:
    FTPProcess();
    virtual ~FTPProcess();
    int createConnection();
    int getHost() const;
    int getPort() const;
    int setInfo(string info);
    void closeConnection();
    int cmdNLST(string path);
    int cmdLIST(string path);
    int cmdRETR(string path);
    int cmdSTOR(string path);
    int cmdPWD();
    int cmdCWD(string path);
    int cmdType(string type);
    int cmdSYST();
    void setClientID(int clientID);
    void setDir(string dir);
    string getDir() const;
    int cmdPASV(in_addr_t host);
    bool isAscii() const;
    bool isPasv() const;

private:
    int clientID, port, sid;
    bool pasv, ascii;
    in_addr_t client;
    string dir;

    char* sperm(char *buff, __mode_t mode);
};

#endif	/* _FTPPROCESS_H */

