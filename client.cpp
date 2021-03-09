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
#include <time.h>
#include <signal.h>

#define DEBUG
#define PROTOCOL "RPS UDP 1.0\n"


void INThandler(int sig)
{
  exit(0);
}

int main(int argc, char *argv[])
{
  if (argc != 2)
  {
    printf("Wrong format IP:PORT\n");
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
  sa.ai_family = AF_INET;
  sa.ai_socktype = SOCK_DGRAM;
  sa.ai_protocol = 17;

  if (int rv = getaddrinfo(Desthost, Destport, &sa, &si) != 0)
  {
    fprintf(stderr, "%s\n", gai_strerror(rv));
    exit(0);
  }

  struct sockaddr_in servaddr;

  int sockfd;

  for (p = si; p != NULL; p = p->ai_next)
  {
    if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
    {
      printf("Error: Failed to create socket.\n");
      continue;
    }
    break;
  }

  if (p == NULL)
  {
    printf("NULL\n");
    freeaddrinfo(si);
    close(sockfd);
    exit(0);
  }

  struct timeval tv;
  tv.tv_sec = 2;
  tv.tv_usec = 0;
  ssize_t sentbytes;
  int tries = 0;
  int bytes = 0;
  socklen_t addr_len = sizeof(servaddr);
  char recvBuffer[256];
  char sendBuffer[256];
  bool isRunning = true;


  printf("Sent message to server.\n");

  setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv));
  signal(SIGINT, INThandler);

 while(bytes <= 0 && tries < 3)
 {
   memset(recvBuffer,0, sizeof(recvBuffer));
   if ((sentbytes = sendto(sockfd, PROTOCOL, sizeof(PROTOCOL), 0, p->ai_addr, p->ai_addrlen)) == -1)
    {
      printf("Error: Couldnt send to the server.");
      exit(0);
    }

    bytes = recvfrom(sockfd, recvBuffer, sizeof(recvBuffer), 0, (struct sockaddr *)&servaddr, &addr_len);

    if (bytes == -1 && tries < 2)
    {
      printf("Server did not respond, trying again.\n");
    }
    tries++;
  }
  if (bytes == -1)
  {
    printf("Error:Couldnt recieve from server.\n");
    exit(0);
  }
  else
  {
    if(strcmp(recvBuffer, "NOT OK\n") >= 0)
    {
      printf("PROTOCOL IS NOT SUPPORTED... Leaving\n");
      exit(0);
    }
    while(isRunning)
    {

    }
  }
 }

  close(sockfd);
  return 0;
}