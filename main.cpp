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

int main(){
    
    vector<string>hisVec = getHistory("hit.txt");
	string line;

	while(true){
	    cout<< "\n" << printWrkDir()<<"$ ";
	    getline(cin,line);
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
	// 	else {
	// 		// ls, ps, ./a.out, java j
	// 		execvp(cmd[0], cmd)
	// 	}
	}
    return 0;
}


