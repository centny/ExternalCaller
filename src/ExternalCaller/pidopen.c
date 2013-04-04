///*
// * pidopen.cpp
// *
// *  Created on: Nov 6, 2012
// *      Author: Scorpion
// */
//
//#include <stdio.h>
//#include <unistd.h>
//#include <signal.h>
//#ifndef WIN32
//static int __child_end=0;
//static int pfp[2]={0};
//void handle_kill(int s) {
////	printf("sssssss\n");
////	close(pfp[__child_end]);
////	exit(0);
//}
//FILE *pidopen(const char *command, const char *mode, pid_t *pid) {
//	//int pfp[2]; /*  the  pipe  and  the  process	*/
//	int parent_end, child_end; /*  of  pipe  	 */
//
//	if (*mode == 'r') { /*  figure  out  direction	 */
//		parent_end = 0;
//		child_end = 1;
//	} else if (*mode == 'w') {
//		parent_end = 0;
//		child_end = 1;
//	} else
//		return NULL ;
//	if (pipe(pfp) == -1) /*  get  a  pipe	 */
//		return NULL ;
//	pid_t _pid;
//	if ((_pid = fork()) == -1) { /*  and  a  process	*/
//		close(pfp[0]); /*  or  dispose  of  pipe	*/
//		close(pfp[1]);
//		return NULL ;
//	}
//
//	/*  ---------------  parent  code  here  -------------------  */
//	/*      need  to  close  one  end  and  fdopen  other  end	 */
//
//	if (_pid > 0) {
//		if (close(pfp[child_end]) == -1)
//			return NULL ;
//		(*pid)=_pid;
//		return fdopen(pfp[parent_end], mode); /*  same  mode  */
//	}
//
//	/*  ---------------  child  code  here  ---------------------  */
//	/*      need  to  redirect  stdin  or  stdout  then  exec  the  cmd	  */
//
//	if (close(pfp[parent_end]) == -1) /*  close  the  other  end	*/
//		exit(1); /*  do  NOT  return	*/
//	if (dup2(pfp[child_end], child_end) == -1)
//		exit(1);
//	if (dup2(pfp[child_end], STDERR_FILENO) == -1)
//		exit(1);
//	if (close(pfp[child_end]) == -1) /*  done  with  this  one	*/
//		exit(1);
//	__child_end=child_end;
//	signal(SIGKILL, handle_kill);
//	/*  all  set  to  run  cmd	*/
////	execl("/bin/sh", "sh", "-c", command, NULL );
//	sleep(1111111);
//	exit(1);
//}
//
//void pidclose(FILE* pipe, pid_t pid) {
//	printf("kill:%d\n",pid);
//	kill(pid, SIGTERM);
//	fclose(pipe);
//}
//
//#endif
