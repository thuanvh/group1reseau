/* 
 * File:   main.cpp
 * Author: langthang
 *
 * Created on April 3, 2010, 1:08 AM
 */

#include <stdlib.h>    
#include "FtpCommand.h"

/*
 * 
 */
int main(int argc, char** argv) {
        FtpCommand* cmdline = new FtpCommand();
	cmdline->getArguments(argc, argv);
        do {
            cmdline->readCommandLine();
            cmdline->handleCommand();
	} while (cmdline->isRunning());

        return (EXIT_SUCCESS);
}

