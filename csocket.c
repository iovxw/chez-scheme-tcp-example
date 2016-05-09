#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <netdb.h>

int c_close(int fd) {
  int i = close(fd);
  if (i < 0)
    return (0 - errno);
  return i;
}

/* c_write attempts to write the entire buffer, pushing through
   interrupts, socket delays, and partial-buffer writes */
int c_write(int fd, char *buf, unsigned n) {
  int i = write(fd, buf, n);

  if (i < 0)
    return (0 - errno);
  return i;
}

/* c_read pushes through interrupts and socket delays */
int c_read(int fd, char *buf, unsigned n) {
  int i = read(fd, buf, n);

  if (i < 0)
    return (0 - errno);
  return i;
}

int set_read_timeout(int fd, long sec) {
  struct timeval timeout;
  timeout.tv_sec = sec;
  timeout.tv_usec = 0;

  int i = setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO,
                     (char *)&timeout, sizeof(timeout));
  if (i < 0)
    return (0 - errno);
  return i;
}

int set_write_timeout(int fd, long sec) {
  struct timeval timeout;
  timeout.tv_sec = sec;
  timeout.tv_usec = 0;

  int i = setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO,
                    (char *)&timeout, sizeof(timeout));
  if (i < 0)
    return (0 - errno);
  return i;
}

/* bytes_ready(fd) returns true if there are bytes available
   to be read from the socket identified by fd */
int bytes_ready(int fd) {
  int n;

  (void) ioctl(fd, FIONREAD, &n);
  return n;
}

int dial(char *hostname, int portno) {
  int sockfd, n;
  struct sockaddr_in serveraddr;
  struct hostent *server;

  /* socket: create the socket */
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0)
    return (0 - errno);

  /* gethostbyname: get the server's DNS entry */
  server = gethostbyname(hostname);
  if (server == NULL)
    return (0 - errno);

  /* build the server's Internet address */
  bzero((char *) &serveraddr, sizeof(serveraddr));
  serveraddr.sin_family = AF_INET;
  bcopy((char *)server->h_addr,
        (char *)&serveraddr.sin_addr.s_addr, server->h_length);
  serveraddr.sin_port = htons(portno);

  /* connect: create a connection with the server */
  if (connect(sockfd, &serveraddr, sizeof(serveraddr)) < 0)
    return (0 - errno);

  return sockfd;
}

char* get_error(int code) {
  return strerror(0 - code);
}
