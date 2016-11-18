#include "shell.h"

using namespace std;

process::process(vector<string> argstr)
{
	for (unsigned int  i = 0; i < argstr.size(); ++i)
	{
    		argv.push_back(argstr[i]);
	}
	numArg = argstr.size();
}

process::~process(){

}

int process::getNumArg()
{
	return numArg;
}

vector<string> process::getArg()
{
	return argv;
}



