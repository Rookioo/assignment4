// Programming Project 4 (System Programming and Linux Shell)
// Used WSL and programmed in VSCode with Github connected
// Programmers: Joe / Dustin
#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>


// main
using namespace std;

int main(){
    
    //vector<string>hisVec = gethistory("hit.txt");
	string line;

	while(true){
	    cout<<printWrkDir()<<"S";
	    getline(cin,line);
	// 	hisvec.push_back(line); //updatehisFile(hisvec);

		if(line == "exit") exit(0);
	// 	elseif(line == "pwd") cout<<pwd();
	// 	elseif(line == "history")printhis(hisVec);
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
	// 	else {
	// 		// ls, ps, ./a.out, java j
	// 		execvp(cmd[0], cmd)
	// 	}
	}
    return 0;
}

// functions
vector<string>getHistory(string) {
	string pwd; // Use shared C function from assignment
}

// prints current working directory
string printWrkDir() {
    char cwd[PATH_MAX]

    string pwd = getcwd(cwd, sizeof(cwd));
    return pwd;
}


// hisvec() //Opens file  

// void printHis(vectorstr>u){=}