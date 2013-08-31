#ifndef _HELPER_H
#define _HELPER_H 1

struct packet
{
	int len;
	char buf[1024];
};

ssize_t readn(int fd, void * buf, size_t count);
ssize_t writen(int fd, const void * buf, size_t count);

#endif // _HELPER_H 
