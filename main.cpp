// Programming Project 4 (System Programming and Linux Shell)
// Used WSL and programmed in VSCode with Github connected
// Programmers: Joe / Dustin
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <linux/limits.h> // For PATH_MAX
#include <sys/wait.h>
#include <fcntl.h>


// main
using namespace std;


// functions
 vector<string>getHistory(string fileName) {
 	// Use shared C function from assignment
	vector<string> hisVec;

   ifstream inFile;
   inFile.open( fileName);
   if (!inFile.is_open()) {
	// file not found because this is the first command
      return hisVec;
   }
   string line;
   while (getline(inFile, line)) {
      if (!line.empty()) {
         hisVec.push_back(line);
      }
   }
	return hisVec;
 }


// prints current working directory
string printWrkDir() {
    char cwd[PATH_MAX];

    string pwd = getcwd(cwd, sizeof(cwd));
    return pwd;
}

// prints history from vector
void printHis(vector<string>hisVec) {
	for (size_t i = 0; i < hisVec.size(); i++) {
		cout << i+1 << "  " << hisVec[i] << "\n";
	}
}


// Updates history file with history vector
void updatehisFile(const string& fileName, string line) {
   ofstream outFile;
   outFile.open(fileName, ios::app); // ios::app for append mode
   if (!outFile.is_open()) {
      cerr << "Error: could not write to file " << fileName << "\n";
      return;
   }
   outFile << line << "\n";
}

// tokenizing functions
vector<string> splitTokens(char *input) {
   vector<string> tokens;
   const char *delims = " \t\n"; // split on spaces, tab, newlines

   char *saveptr;
   char *token = strtok_r(input, delims, &saveptr);
   while (token != nullptr) {
      tokens.push_back(string(token)); // store in C++ string
      token = strtok_r(nullptr, delims, &saveptr);
   }
   return tokens;
}

char** copyToCArray(const vector<string>& tokens) {
   char** c_array = new char*[tokens.size() + 1];

   for (size_t i = 0; i < tokens.size(); i++) {
   size_t len = tokens[i].size() + 1; // include null terminator
   c_array[i] = new char[len];

   strncpy(c_array[i], tokens[i].c_str(), len);
   c_array[i][len - 1] = '\0'; // guarantee null terminator
   }

   c_array[tokens.size()] = NULL;
   return c_array;
}

int main(){
    
    vector<string>hisVec = getHistory("hit.txt");
	string line;

	while(true){
	    cout<< "\n" << printWrkDir()<<"$ ";
	    getline(cin,line);
		if (line.empty()) continue;
		
	 	hisVec.push_back(line); //
		updatehisFile("hit.txt", line);

		if(line == "exit") exit(0);
	 	else if (line == "pwd") cout<<printWrkDir();
	 	else if(line == "history") printHis(hisVec);
	// 	elseif(stdOutRed(line)>-1) {
	// 		// part2 Proj3
	// 	}
	// 	elseif(stdInRed(line)) {
	// 		cmd<f;
	// 		execvp
	// 	}
	// 	elseif(hasPiping(line) > -1) {
	// 		// full twochildren_pipe_dup2-1.cpp
	// 		// copy paste code except use only one input
	// 	}
		else { // normal command
			char buffer[256];
			strncpy(buffer, line.c_str(), sizeof(buffer));
			buffer[sizeof(buffer)-1] = '\0';

			vector<string> tokens = splitTokens(buffer);

			char** cmd = copyToCArray(tokens);

			pid_t pid = fork();
			if (pid < 0) {
				perror("Fork failed");
			}
			else if (pid == 0) {
				execvp(cmd[0], cmd);
				perror("execvp failed");
				exit(1);
			}
			else {
				wait(NULL);
			}
			
			for (size_t i = 0; i < tokens.size(); i++) {
				delete[] cmd[i];
			}
			delete[] cmd;
		}
	}
    return 0;
}


