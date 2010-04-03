/* 
 * File:   main.cpp
 * Author: trung
 *
 * Created on March 31, 2010, 2:12 PM
 */

#include "common.h"
#include "FTPServer.h"

/*
 * 
 */
int main(int argc, char** argv) {
    FTPServer server;

    server.setPort(2122);
    server.setHome("/home/trung/ftp/");
    server.start();

    return (EXIT_SUCCESS);
}

