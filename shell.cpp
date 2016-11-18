#include "shell.h"

using namespace std;

void close_pipe(int pipefd [2]) 
{
  if (close(pipefd[0]) == -1) {
    perror("close");
    exit(EXIT_FAILURE);
  } // if
  if (close(pipefd[1]) == -1) {
    perror("close");
    exit(EXIT_FAILURE);
  } // if
} // close_pipe

string getPrompt() {
	string prompt = "Radish:";
	string currentDir = get_current_dir_name();
	struct passwd *pw = getpwuid(getuid());
	string homeDir = pw->pw_dir;

	if (homeDir.compare(currentDir.substr(0,homeDir.length())) == 0) {
		prompt += "~" + currentDir.substr(homeDir.length(), currentDir.length()-homeDir.length());
		prompt += "$ ";
	} else {
		prompt += currentDir;
		prompt += "$ ";
	}

	return prompt;
}

int main()
{
	vector<string> S2D;

	//loop continues until the user sends a signal to the program
	while(true)
	{
		//Declare some necessary variables
		bool builtinCmd = false;
		string userInput;
		string StdIN = "STDIN_FILENO";
  		string StdOUT = "STDOUT_FILENO";
  		string StdERR = "STDERR_FILENO";
		int status = EXIT_SUCCESS;

		int numPipes = 0;
		cout << getPrompt();

		getline(cin,userInput);
		stringstream builtin(userInput);

		string com;
		//The builtins!
		while(builtin >> com)
		{
			if(com.compare("bg") == 0)
			{
				builtinCmd = true;
			}
			else if(com.compare("cd") == 0)
			{
				string args;
				if (builtin >> args)
				{
					const char* path = args.c_str();
					int cd = chdir(path);
					if (cd == -1) {
						perror("cd");
					}
				}
				else
				{
					const char* path = getenv("HOME");
					int cd = chdir(path);
					if (cd == -1) {
						perror("cd");
					}
				}
				builtinCmd = true;
			}
			else if(com.compare("exit") == 0)
			{
				string args;
				if (builtin >> args)
				{
					int exitStatus = stoi(args);
					exit(exitStatus);
				} else {
					exit(status);
				}
			}
			else if(com.compare("export") == 0)
			{
				builtinCmd = true;
			}
			else if (com.compare("fg") == 0)
			{
				builtinCmd = true;
			}
			else if(com.compare("help") == 0)
			{
				cout << endl << "Radish, version 0.6" << endl << endl;
				cout << "Yo, welcome to the raddest shell that there ever was!" << endl;
				cout << "Here are some helpful commands:" << endl << endl;
				cout << "bg JID - Resume the stopped JID in the background" << endl;
				cout << "cd [PATH] - Change the current directory to PATH" << endl;
				cout << "exit [N] - Cause the shell to exit with a status of N" << endl;
				cout << "export NAME[=WORD] - NAME is automatically included in the environment of subsequently executed jobs" << endl;
				cout << "fg JID - Resume job JID in the foreground" << endl;
				cout << "help - Display helpful information" << endl;
				cout << "jobs - List current jobs" << endl << endl;
				builtinCmd = true;
			}
			else if(com.compare("jobs") == 0)
			{
				builtinCmd = true;
			}
		}
		if (builtinCmd == false)
		{
			stringstream stream(userInput);

			//if one of the built in commands do those
			//otherwise call function to parse user input
			string in;
			string st;
			string filename;
			//size_t pos;
			bool flag = false;

			//this loop iterates over the user input
			while(stream >> in)
			{
				string inStr = in;
				//if the input sees a quote, read as one argument until the next quote
				//is seen
				if(inStr.at(0) == '"')
				{
					string arg = in.substr(1, in.size()-1) ;
					arg += " ";
					stream >> in;
					arg += in;
					int k = arg.size()-1;
					while(arg.at(k) != '"' || arg.at(k-1) == 92)
					{
						string temp = in;
						stream >> in;
						if(in.at(0) == 92)
						{
							if(in.at(1) == '"')
							{
								arg += " ";
								arg += temp.substr(1, temp.size()-1);

							}
							else if(in.at(temp.size()-1) == '"')
							{
								arg += " ";
								arg += temp.substr(0, temp.size()-2);
							}
						}
						else
						{
							arg += " ";
							arg += in;
							k = arg.size()-1;
						}
					}
					arg = arg.substr(0, arg.size()-1);
					for(unsigned int i = 0; i < arg.size(); i++)
					{
						if(arg.at(i) == 92)
						{
							string temp = arg.substr(0, i);
							temp += arg.substr(i+1, arg.size()-i);
							arg = temp;
						}
					}
					S2D.push_back(arg);
				}
				//if the input is a pipe char then increase numPipes
				else if(in.compare("|") == 0)
				{
					S2D.push_back(in);
					numPipes++;
				}
				//if input redirection then flag and update the value of stdIN
				//the following word is assumed to be the filename for the redirection
				//and is also updated into the stdIN string
				else if(in.compare("<") == 0)
				{
					flag = true;
					stream >> filename;
					st = filename;
					StdIN = st;
				}
				//if out redirection then flag and update the value of stdOUT
				//the following word is assumed to be the filename for the redirection
				//and is also updated into the stdOUT string along with the append string
				else if(in.compare(">>") == 0)
				{
					flag = true;
					stream >> filename;
					st = filename;
					StdOUT = st + " (append)";
				}
				//if out redirection then flag and update the value of stdOUT
				//the following word is assumed to be the filename for the redirection
				//and is also updated into the stdOUT string along with the truncate string
				else if(in.compare(">") == 0)
				{
					flag = true;
					stream >> filename;
					st = filename;
					StdOUT = st + " (truncate)";
				}
				//if err redirection then flag and update the value of stdERR
				//the following word is assumed to be the filename for the redirection
				//and is also updated into the stdOUT string along with the append string
				else if(in.compare("e>>") == 0)
				{
					flag = true;
					stream >> filename;
					st = filename;
					StdERR = st +" (append)";
				}
				//if err redirection then flag and update the value of stdERR
				//the following word is assumed to be the filename for the redirection
				//and is also updated into the stdOUT string along with the truncate string
				else if(in.compare("e>") == 0)
				{
					flag = true;
					stream >> filename;
					st = filename;
					StdERR = st + " (truncate)";
				}
				//if anything else is seen, if the input is not a pipe
				//and the flag has not been set then add the char to the vector
				//otherwise do nothing and read the next value
				else
				{
					if(flag == false)
					{
						S2D.push_back(in);
					}
					flag = false;
				}
			}

			unsigned int j = 0;

			//iterates for each process, each process was determined
			//by the number of pipes
			//there is always one more process than there are pipes
			string isPipe;

			vector<process> proArray;
			proArray.reserve(500);
			//create new job
			for(int i = 0; i < (numPipes + 1); ++i)
			{
				int argNum = 0;

				vector<string> pro;
				while(j < S2D.size())
				{
					string ispipe = S2D[j];
					
					if(ispipe.compare("|") == 0) 
					{
						j++;
						break;
					}
					else
					{
						pro.push_back(S2D[j]);
						j++;
						argNum++;
					} 
				}
				
			        process p (pro);
				proArray.push_back(p);		
				//add the process to the current job object
				pro.clear();
			}
		
		S2D.clear();
		pid_t id;
		int status;
		if ((id = fork()) < 0) //error
		{
    			perror("FORK ERROR");
			return EXIT_FAILURE;
  		}
		else if (id == 0) //child 
		{
			if(proArray.size() == 1)
			{
				char ** args = new char * [proArray.at(0).getNumArg()+1];
                                        
                                for(int i = 0; i < proArray.at(0).getNumArg(); i++)
                                {
                                        const char * temp = proArray.at(0).getArg().at(i).c_str();
					args[i] = new char [strlen(temp)+1];
                                        strcpy(args[i], temp);
                                }
                                args[proArray.at(0).getNumArg()] = nullptr;

                                execvp(args[0], args);
                                
                                perror("execvp");
                                for (int i = 0; i < proArray.at(0).getNumArg()+1; ++i) delete[] args[i];
                                	delete[] args;
                                exit(EXIT_FAILURE);
			}
			else
			{
                            int ** pipes = new int * [numPipes];
                            for(int i = 0; i < numPipes; i++)
                            {
                                pipes[i] = new int [2];
                            }
                            
                            for(int j = 0; j < numPipes; j++)
                            {
                            
				int pid;
    				// create pipe
    				if (pipe(pipes[j]) == -1) {
       		 			perror("pipe");
        				exit(EXIT_FAILURE);
   				} // if

    				if ((pid = fork()) == -1) {
        				perror("fork");
        				exit(EXIT_FAILURE);
    				} 
				else if (pid == 0) {

        				if (dup2(pipes[j][1], STDOUT_FILENO) == -1) {
            					perror("dup2");
            					exit(EXIT_FAILURE);
        				} // if

        	                        if (close(pipes[j][0]) == -1) {
                                                perror("close");
                                                exit(EXIT_FAILURE);
                                	} // if
                                        if (close(pipes[j][1]) == -1) {
                                                perror("close");
	                                        exit(EXIT_FAILURE);
					}
					char ** args = new char * [proArray.at(j).getNumArg()+1];
				cout << "Process " << j << ": " << proArray.at(j).getArg().at(0).c_str() << endl;
					for(int i = 0; i < proArray.at(j).getNumArg(); i++) 
                                        {
                                         	const char * temp = proArray.at(j).getArg().at(i).c_str();
                             			args[i] = new char [strlen(temp)+1];
                                        	strcpy(args[i], temp); 
			                }
                                        args[proArray.at(j).getNumArg()] = nullptr;
					execvp(args[0], args);
                                
                                        perror("execvp");
                                        for (int i = 0; i < proArray.at(j).getNumArg()+1; ++i) delete[] args[i];
                                        delete[] args;
                                        exit(EXIT_FAILURE);

    				} // if

    				if ((pid = fork()) == -1) {
        				perror("fork");
        				exit(EXIT_FAILURE);
    				} else if (pid == 0) {

        				if (dup2(pipes[j][0], STDIN_FILENO) == -1) {
            					perror("dup2");
            					exit(EXIT_FAILURE);
        				} // if

					if (close(pipes[j][0]) == -1) {
    						perror("close");
    						exit(EXIT_FAILURE);
  					} // if
  					if (close(pipes[j][1]) == -1) {
    						perror("close");
    						exit(EXIT_FAILURE);
					}
        				char ** args = new char * [proArray.at(j+1).getNumArg()+1];
    		cout << "Process " << j+1 << ": " << proArray.at(j+1).getArg().at(0).c_str() << endl;			
					for(int i = 0; i < proArray.at(j+1).getNumArg(); i++)
					{
						const char * temp = proArray.at(j+1).getArg().at(i).c_str();
						args[i] = new char [strlen(temp)+1];
                                        	strcpy(args[i], temp);
					}
    					args[proArray.at(1).getNumArg()] = nullptr;

   					execvp(args[0], args);
                                
    					perror("execvp");
    					for (int i = 0; i < proArray.at(j+1).getNumArg()+1; ++i) delete[] args[i];
    					delete[] args;
    					exit(EXIT_FAILURE);

    				} // if

	                        if (close(pipes[j][0]) == -1) {
                                        perror("close");
                                        exit(EXIT_FAILURE);
                                } // if
                                if (close(pipes[j][1]) == -1) {
                                        perror("close");
                             	        exit(EXIT_FAILURE);
				}
				cout << "Closed pipefds" << endl;
	    			waitpid(pid, nullptr, 0);
				cout << "finished waiting" << endl;
                            }
			    for(int i = 0; i < numPipes; i++)
			    {
				delete[] pipes[i];
			    }
			    delete[] pipes;
			} 
		}
		else //parent 
		{             
      			pid_t endID = waitpid(id, &status, 0);
                        if (endID == -1) 
			{            /* error calling waitpid       */
              			perror("WAITPID ERROR");
                 		return EXIT_FAILURE;
			 }
			 //wait for child to finish if the program is launched in the foreground
                        //otherwise give prompt and repeat
  		}	
	}
	}
}

