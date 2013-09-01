#ifndef _HELPER_H
#define _HELPER_H 1

extern ssize_t readn(int fd, void * buf, size_t count);
extern ssize_t writen(int fd, const void * buf, size_t count);

extern ssize_t recv_peek(int sockfd, void *buf, size_t len);
extern ssize_t readline(int sockfd, void *buf, size_t maxline);

#endif // _HELPER_H 
