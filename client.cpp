#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <signal.h>
#include <curses.h>
#include <sys/select.h>
#include <iostream>
#include <signal.h>

#define DEBUG
#define VERSION "RPS TCP 1.0\n"
#define ERROR "ERROR TO\n"

void INThandler(int sig)
{
  printf("\n");
  exit(0);
}

int main(int argc, char *argv[])
{

  if (argc != 2)
  {
    printf("Wrong format IP:PORT NAME\n");
    exit(0);
  }

  char delim[] = ":";
  char *Desthost = strtok(argv[1], delim);
  char *Destport = strtok(NULL, delim);

  if (Desthost == NULL || Destport == NULL)
  {
    printf("Wrong format.\n");
    exit(0);
  }

  int port = atoi(Destport);

  addrinfo sa, *si, *p;
  memset(&sa, 0, sizeof(sa));
  sa.ai_family = AF_INET;
  sa.ai_socktype = SOCK_STREAM;

  struct timeval tv;
  tv.tv_sec = 5;
  tv.tv_usec = 0;

  if (int rv = getaddrinfo(Desthost, Destport, &sa, &si) != 0)
  {
    fprintf(stderr, "%s\n", gai_strerror(rv));
    exit(0);
  }

  int sockfd;

  for (p = si; p != NULL; p = p->ai_next)
  {
    if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
    {
      perror("Error: Couldnt connect.\n");
      continue;
    }

    if ((connect(sockfd, p->ai_addr, p->ai_addrlen) == -1))
    {
      close(sockfd);
      printf("Error: Couldnt connect.\n");
      continue;
    }
    break;
  }

  if (p == NULL)
  {
    printf("NULL\n");
    exit(0);
  }
  setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char *)&tv, sizeof(tv));
  freeaddrinfo(si);
  signal(SIGINT, INThandler);

  char recvBuf[256];
  char sendBuf[5];
  int bytes;
  fd_set currentSockets;
  fd_set readySockets;
  FD_ZERO(&currentSockets);
  FD_ZERO(&readySockets);
  FD_SET(sockfd, &currentSockets);
  FD_SET(STDIN_FILENO, &currentSockets);
  int fdMax = sockfd;
  int nfds = 0;

  while (1)
  {
    readySockets = currentSockets;
    if (fdMax < sockfd)
    {
      fdMax = sockfd;
    }
    nfds = select(fdMax + 1, &readySockets, NULL, NULL, NULL);
    if (nfds == -1)
    {
      printf("ERROR, something went wrong!\n");
      break;
    }
    if (FD_ISSET(STDIN_FILENO, &readySockets))
    {
      memset(sendBuf, 0, sizeof(sendBuf));
      std::cin.getline(sendBuf, sizeof(sendBuf));
      std::cin.clear();
      if(strstr(sendBuf, "0") != nullptr)
      {
        FD_CLR(STDIN_FILENO, &readySockets);
        exit(0);
      }
      send(sockfd, sendBuf, sizeof(sendBuf), 0);
      FD_CLR(STDIN_FILENO, &readySockets);
    }
    if (FD_ISSET(sockfd, &readySockets))
    {
      memset(recvBuf, 0, sizeof(recvBuf));
      if ((bytes = recv(sockfd, recvBuf, sizeof(recvBuf), 0)) == -1)
      {
        continue;
      }
      else if (strstr(recvBuf, VERSION) != nullptr)
      {
        printf("Server protocol: %s", recvBuf);
        send(sockfd, "OK\n", strlen("OK\n"),0);
      }
      else if (strstr(recvBuf, "ERROR") != nullptr)
      {
        printf("%s", recvBuf);
      }
      else if (strstr(recvBuf, "Server is closing!\n") != nullptr)
      {
        printf("%s", recvBuf);
        exit(0);
      }
      else if(strstr(recvBuf, "A game is ready, press 'Enter' to accept!\n") != nullptr)
      {
        std::system("clear");
        printf("%s", recvBuf);
      }
      else
      {
        printf("%s", recvBuf);
      }
      FD_CLR(sockfd, &readySockets);
    }
  }

#ifdef DEBUG
  printf("Host %s, and port %d.\n", Desthost, port);
#endif

  close(sockfd);
  return 0;
}
