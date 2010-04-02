/* 
 * File:   FTPCommand.h
 * Author: trung
 *
 * Created on March 31, 2010, 4:50 PM
 */

#ifndef _FTPCOMMAND_H
#define	_FTPCOMMAND_H

#include "common.h"

class FTPCommand {
public:
    FTPCommand();
    virtual ~FTPCommand();
    CommandType getCommandType(string name);
    int getCommandName(vector<string> &msgs);
    void setSocketID(int socketID);
    int getSocketID() const;
    
private:
    int socketID;

};

#endif	/* _FTPCOMMAND_H */

