/* 
 * File:   FTPCommand.cpp
 * Author: trung
 * 
 * Created on March 31, 2010, 4:50 PM
 */

#include "FTPCommand.h"

FTPCommand::FTPCommand() {
}

FTPCommand::~FTPCommand() {
}

CommandType FTPCommand::getCommandType(string name) {
    CommandType result;

    if (strcasecmp(name.c_str(), "TYPE") == 0) {
        return TYPE;
    } else if (strcasecmp(name.c_str(), "MODE") == 0) {
        return MODE;
    } else if (strcasecmp(name.c_str(), "NLST") == 0) {
        return NLST;
    } else if (strcasecmp(name.c_str(), "RETR") == 0) {
        return RETR;
    } else if (strcasecmp(name.c_str(), "NOOP") == 0) {
        return NOOP;
    } else if (strcasecmp(name.c_str(), "STOR") == 0) {
        return STOR;
    } else if (strcasecmp(name.c_str(), "USER") == 0) {
        return USER;
    } else if (strcasecmp(name.c_str(), "PASS") == 0) {
        return PASS;
    } else if (strcasecmp(name.c_str(), "QUIT") == 0) {
        return QUIT;
    } else if (strcasecmp(name.c_str(), "PWD") == 0) {
        return PWD;
    } else if (strcasecmp(name.c_str(), "CWD") == 0) {
        return CWD;
    } else if (strcasecmp(name.c_str(), "PORT") == 0) {
        return PORT;
    } else if (strcasecmp(name.c_str(), "PASV") == 0) {
        return PASV;
    } else if (strcasecmp(name.c_str(), "SYST") == 0) {
        return SYST;
    } else {
        return ERROR;
    }

    return result;
}
