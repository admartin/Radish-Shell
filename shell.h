#ifndef SHELL_H
#define SHELL_H

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <cstdio>
#include <sys/types.h>
#include <pwd.h>

using namespace std;

class process {
    private:
	vector<string> argv;
        int numArg;

    public:
	process(vector<string> argstr);
	~process();
	int getNumArg();
	vector<string> getArg();
};

class job {
    private:
      int JID;
      string command;
    public:
	     job(int JID, string command);
       ~job();
       int getJID();
       int getCmd();
};

#endif	//SHELL_H
