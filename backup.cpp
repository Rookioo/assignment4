// Programming Project 4 (System Programming and Linux Shell)
// Framework: WSL VSCode
// Version Management: Github
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

// Handling the < and > operators
void handleRedirection(vector<char*>& args) {
    for (int i = 0; args[i] != NULL; i++) {
        // Output Redirection
        if (strcmp(args[i], ">") == 0) {
            int fd = open(args[i+1], O_WRONLY | O_CREAT | O_TRUNC, 0666);
            if (fd < 0) { perror("open"); exit(1); }
            dup2(fd, STDOUT_FILENO); // Redirect STDOUT to file
            close(fd);
            args[i] = NULL; // Hide '>' and filename from execvp
            break;
        } 
        // Input Redirection
        else if (strcmp(args[i], "<") == 0) {
            int fd = open(args[i+1], O_RDONLY);
            if (fd < 0) { perror("open"); exit(1); }
            dup2(fd, STDIN_FILENO); // Redirect STDIN from file
            close(fd);
            args[i] = NULL;
            break;
        }
    }
}

// handling piping
void handlePiping(string line) {
    size_t pipePos = line.find('|');
    string part1 = line.substr(0, pipePos);
    string part2 = line.substr(pipePos + 1);

    int pipefd[2];
    pipe(pipefd); // fd[0] is read, fd[1] is write

    // Forking first child for the left side of the pipe
    if (fork() == 0) {
        // Child 1: Send output to the pipe
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[0]);
        close(pipefd[1]);

        // tokenize cmd1 here
        vector<char*> args1;
        char* temp = strdup(part1.c_str());
        char* token = strtok(temp, " ");
        while(token) { args1.push_back(token); token = strtok(NULL, " "); }
        args1.push_back(NULL);
        
        execvp(args1[0], args1.data());
        exit(1);
    }

    // Forking second child for the right side
    if (fork() == 0) {
        dup2(pipefd[0], STDIN_FILENO);
        close(pipefd[1]);
        close(pipefd[0]);

        vector<char*> args2;
        char* temp = strdup(part2.c_str());
        char* token = strtok(temp, " ");
        while(token) { args2.push_back(token); token = strtok(NULL, " "); }
        args2.push_back(NULL);

        execvp(args2[0], args2.data());
        exit(1);
    }

    // Parent has to close both and wait so the prompt doesn't get messed up    close(pipefd[0]);
    close(pipefd[1]);
    wait(NULL);
    wait(NULL);
}

int main(){
    vector<string>hisVec = getHistory("hit.txt");
	string line;

	while(true){
	    cout<< "\n" << printWrkDir()<<"$ ";
	    getline(cin,line);
		if (line.empty()) continue;
		
	 	hisVec.push_back(line);
		updatehisFile("hit.txt", line);

		if(line == "exit") exit(0);
	 	else if (line == "pwd") {
            cout<<printWrkDir();
            continue; // Prevent double execution
        }
	 	else if(line == "history") {
            printHis(hisVec);
            continue; // Prevent shell error
        }

        // Tokenizing for external commands
		vector<char*> args;
		char* temp = strdup(line.c_str()); 
		char* token = strtok(temp, " ");
		while (token != NULL) {
			args.push_back(token);
			token = strtok(NULL, " ");
		}
		args.push_back(NULL);

		if (args[0] == NULL) continue;

        bool hasPipe = false;

        for (int i = 0; args[i] != NULL; i++) {
            if (strcmp(args[i], "|") == 0) {
                hasPipe = true;
                break;
            }
        }
        
        if (hasPipe) {
        // Piping
        handlePiping(line); 
        } else {
            // Redirection
            pid_t pid = fork();
            if (pid == 0) {
                // In Child Process
                handleRedirection(args);
                if (execvp(args[0], args.data()) == -1) {
                    perror("Shell Error");
                }
                exit(1); // Exit if child fails
            } else {
                // Parent wait for child to finis
                wait(NULL);
            }
        }

        for (size_t i = 0; i < tokens.size(); i++) {
            delete[] cmd[i];
        }
        delete[] cmd;
	}
    return 0;
}


