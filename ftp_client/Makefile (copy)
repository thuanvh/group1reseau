INCLUDE = /usr/local/include/
LIBS_DIR = /usr/local/lib/

SRCS = FtpCommand.cpp FtpExecution.cpp main.cc
OBJS = FtpCommand.o FtpExecution.o main.o
LIBS = boost_system-mt

all : myftp
	
FtpCommand.o: FtpCommand.cpp
	g++ -c FtpCommand.cpp -I$(INCLUDE)

FtpExecution.o: FtpExecution.cpp
	g++ -c FtpExecution.cpp -I$(INCLUDE)

main.o: main.cc
	g++ -c main.cc -I$(INCLUDE)

myftp: $(OBJS)
	g++ -o"myftp" $(OBJS) -L$(LIBS_DIR) -l$(LIBS)

clean:
	rm -f *.o myftp
	