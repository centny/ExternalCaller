/*
 * extcaller.c
 *
 *  Created on: Nov 7, 2012
 *      Author: Scorpion
 */
//#define MAIN_M extcaller
#ifndef WIN32
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#define B_SIZE 1024
FILE *ostdout = 0;
FILE *ostderr = 0;
FILE *nstdout = 0;
int p_pipe = 0;
long ptid;
char buf[B_SIZE];
size_t dlen = 0;
void clean() {
	//clear old
	if (ostdout) {
		stdout = ostdout;
		ostdout = 0;
	}
	if (ostderr) {
		stderr = ostderr;
		ostderr = 0;
	}
	if (nstdout) {
		fclose(nstdout);
		nstdout = 0;
	}
	if (p_pipe) {
		int eof_i = -1;
		write(p_pipe, &eof_i, 1);
		close(p_pipe);
		p_pipe = 0;
	}
	dlen = 0;
	ptid = 0;
}
void killsig(int s) {
	clean();
	printf("receive kill signal:%d\n", s);
	exit(0);
}
int extc(int argc, char** argv) {
	if (argc < 4) {
		printf("invalid args\n");
		return 0;
	}
	pid_t _pid = 0;
	if ((_pid = fork()) > 0) {
		printf("child pid:%d\n", _pid);
		return 0;
	}
	//the parent process thread id.
	ptid = atol(argv[1]);
	if (ptid < 1) {
		return 0;
	}
	p_pipe = open(argv[2], O_WRONLY, O_NONBLOCK);
	if (p_pipe < 0) {
		mkfifo(argv[2], 0777);
		p_pipe = open(argv[2], O_WRONLY, O_NONBLOCK);
		if (p_pipe < 0) {
			return 0;
		}
	}
	dlen = sprintf(buf,"%d\n",getpid());
	write(p_pipe, buf, dlen);
	dlen = 0;
	int i = 3;
	while (i < argc) {
		dlen += sprintf(buf+dlen," %s",argv[i]);
		i++;
	}
	buf[dlen] = 0;
	signal(SIGKILL, killsig);
	signal(SIGTERM, killsig);
	signal(SIGSTOP, killsig);
	signal(SIGHUP, killsig);
	signal(SIGINT, killsig);
	signal(SIGQUIT, killsig);
	printf("begin execute command:%s.\n", buf);
	char *args[4];
	args[0] = "sh";
	args[1] = "-c";
	args[2] = buf;
	args[3] = 0;
	execv("/bin/sh", args);
	clean();
	return 1;
}
#ifdef MAIN_M
int main(int argc,char** argv) {
	return extc(argc,argv);
}
#endif
#endif
