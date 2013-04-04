#pragma once
#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <string>
#ifdef WIN32
#include <Windows.h>
#endif
using namespace std;
class ExtCaller {
public:
#ifdef WIN32
	basic_ostream<char>* out;
	PROCESS_INFORMATION pi;
#else
	string extc; //the extc path.
	string wdir; //default is /tmp
	pid_t extc_pid;
#endif
	string cmd;
	map<string, string> env;
	vector<string> args;
private:
	bool running;
public:
	void addArgs(string arg);
	void addEnv(string key, string value);
	int execute();
	void stop();
	bool isRunning();
	ExtCaller();
	~ExtCaller(void);
};

