#include "ExtCaller.h"
#ifdef WIN32
#include <Windows.h>
#else
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <sstream>

using namespace std;
//extern "C" {
//FILE *pidopen(const char *command, const char *mode, pid_t *pid);
//void pidclose(FILE* pipe, pid_t pid);
//}
void ExtCaller::addArgs(string arg) {
	if (arg.size() < 1) {
		cout << "the arg can't be empty" << endl;
		return;
	}
	this->args.push_back(arg);
}
void ExtCaller::addEnv(string key, string value) {
	if (key.size() < 1) {
		cout << "the key name can't be empty" << endl;
		return;
	}
	this->env[key] = value;
}

bool ExtCaller::isRunning(){
	return running;
}
int ExtCaller::execute() {
	running = true;
	map<string, string>::iterator it;
	for (it = this->env.begin(); it != this->env.end(); it++) {
#ifdef WIN32
		SetEnvironmentVariable(it->first.c_str(),it->second.c_str());
#else
		setenv(it->first.c_str(), it->second.c_str(), 1);
#endif
	}
#ifdef WIN32
	basic_ostream<char> *rout;
	if (out) {
		rout = out;
	} else {
		rout = &cout;
	}
	string cmdline = cmd;
	for (size_t i = 0; i < this->args.size(); i++) {
		cmdline.append(" " + this->args[i]);
	}
	//创建匿名管道
	SECURITY_ATTRIBUTES sa = {sizeof(SECURITY_ATTRIBUTES), NULL, TRUE};
	HANDLE hRead;
	HANDLE hWrite;
	if (!CreatePipe(&hRead, &hWrite, &sa, 0))
	{
		printf("CreatePipe error:%d\n",GetLastError());
		running=false;
		return -1;
	}
	//设置命令行进程启动信息(以隐藏方式启动命令并定位其输出到hWrite)
	STARTUPINFO si = {sizeof(STARTUPINFO)};
	GetStartupInfo(&si);
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	si.wShowWindow = SW_HIDE;
	si.hStdError = hWrite;
	si.hStdOutput = hWrite;
	//启动命令行
	if (!CreateProcess(NULL, (char *)cmdline.c_str(), NULL, NULL, TRUE, NULL, NULL, NULL, &si, &pi))
	{
		CloseHandle(hWrite);
		CloseHandle(hRead);
		printf("CreateProcess error:%d\n",GetLastError());
		running=false;
		return -1;
	}
	//立即关闭hWrite
	/*
	 这儿需要说明的是：为什么需要在启动命令行之后，会需要立即关闭hWrite？
	 原因是这样的：
	 首先，hWrite是Windows内核对象，内核对象有个特点，只有所有使用者都关闭该内核对象后，
	 该内核对象才会真正关闭。这儿hWrite已经被上面的进程所使用了，所以此处关闭hWrite并不会导致该句柄失效。
	 其次，这样做有个好处在于，但命令行程序结束后，hWrite就会随之真正关闭。这样的话，才会让后续的ReadFile
	 函数能够结束——否则它会一直等待直到hWrite被关闭（由于ReadFile是个同步函数，如果上面不关闭，以后永远
	 没有关闭机会了）
	 */
	CloseHandle(hWrite);

	//读取命令行返回值
	char buff[10240] = {0};
	DWORD dwRead = 0;
	while (ReadFile(hRead, buff, 10240, &dwRead, NULL))
	{
		rout->write(buff,dwRead);
	}
	CloseHandle(hRead);
	this->pi.hProcess=NULL;
#else
	stringstream ss;
	ss << this->wdir << "/extc" << getpid();
	string spipe = ss.str();
	stringstream cmdline;
	cmdline << this->extc;
	cmdline << " " << getpid();
	cmdline << " " << spipe;
	cmdline << " " << this->cmd;
	for (size_t i = 0; i < this->args.size(); i++) {
		cmdline << " " << this->args[i];
	}
	unlink(spipe.c_str());
	if (mkfifo(spipe.c_str(), 0777) < 0) {
		printf("create named pipe error\n");
		running = false;
		return -1;
	}
	cout << "execute:" << cmdline.str() << endl;
	system(cmdline.str().c_str());
	int ipipe = open(spipe.c_str(), O_RDONLY);
	if (ipipe < 1) {
		printf("open named pipe error\n");
		running = false;
		return -1;
	}
	char buf[1024];
	while (1) {
		int len = read(ipipe, buf, 1024);
		printf("read pipe\n");
		if (len < 1) {
			break;
		}
		if (len == 1 && buf[0] == -1) {
			printf("extc process exit\n");
			break;
		}
		buf[len] = 0;
		if (this->extc_pid == 0) {
			this->extc_pid = atol(buf);
			printf("extc pid:%d\n", this->extc_pid);
		}
	}
	unlink(spipe.c_str());
	close(ipipe);
#endif
	running = false;
	return 1;
}

void ExtCaller::stop() {
#ifdef WIN32
	if(this->pi.hProcess) {
		TerminateProcess(this->pi.hProcess,0);
	}
#else
	if (this->extc_pid) {
		printf("kill %d\n", this->extc_pid);
		kill(this->extc_pid, SIGKILL);
		sleep(1);
		if (running) {
			stringstream ss;
			ss << this->wdir << "/extc" << getpid();
			int ppipe = open(ss.str().c_str(), O_RDONLY);
			if (!ppipe) {
				return;
			}
			int eop = -1;
			write(ppipe, &eop, 1);
			close(ppipe);
		}
	}
#endif
}
ExtCaller::ExtCaller() {
#ifdef WIN32
	this->out = 0;
	this->pi.hProcess=NULL;
#else
	this->wdir = "/tmp";
	this->extc = "/usr/local/bin/extc";
	this->extc_pid = 0;
	this->running = false;
#endif
}

ExtCaller::~ExtCaller(void) {
#ifdef WIN32
	this->stop();
#endif
}
