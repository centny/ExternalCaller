#include "ExtCaller.h"
#include <signal.h>
#include <boost/thread.hpp>
using namespace boost;
ExtCaller *ec = 0;
void eextCaller() {
	ec->cmd = "ping";
//	ec->addArgs("TEST");
	ec->addArgs("www.baidu.com");
	ec->addArgs(">/tmp/www");
	ec->addEnv("TEST", "testvalue");
	ec->execute();
	cout << "ext end--------" << endl;
}
void killsig(int s) {
	sleep(2);
	printf("receive kill signal:%d\n", s);
	ec->stop();
}
int main(int argc, char** argv) {
	signal(SIGTERM, killsig);
	signal(SIGINT, killsig);
#ifdef WIN32
	signal(SIGBREAK,killsig);
#else
	signal(SIGKILL, killsig);
	signal(SIGSTOP, killsig);
	signal(SIGHUP, killsig);
	signal(SIGQUIT, killsig);
#endif
	ec = new ExtCaller();
//	thread thr(boost::bind(killsig, 1));
	eextCaller();
	delete ec;
//	thr.join();
	printf("main end\n");
	return 0;
}
