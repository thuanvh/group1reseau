/* 
 * File:   FTPProcess.cpp
 * Author: trung
 * 
 * Created on April 2, 2010, 4:37 PM
 */

#include "FTPProcess.h"

FTPProcess::FTPProcess() {
}
void FTPProcess::setClientID(int clientID) {
    this->clientID = clientID;
}
void FTPProcess::setDir(string dir) {
    this->dir = dir;
}
string FTPProcess::getDir() const {
    return dir;
}
int FTPProcess::getHost() const {
    return host;
}
int FTPProcess::getPort() const {
    return port;
}

FTPProcess::~FTPProcess() {
}

