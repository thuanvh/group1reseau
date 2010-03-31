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
private:

};

#endif	/* _FTPCOMMAND_H */

