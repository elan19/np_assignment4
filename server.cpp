#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <vector>
#include <signal.h>

#define PROTOCOL "RPS UDP 1.0\n"
#define MENU "Please select:\n1. Play\n2. Watch\n3.Leaderboard\n\n0.Exit\n"

struct cli
{
  struct sockaddr_in addr;
  struct timeval tid;
};
std::vector<cli> clients;

void checkJobbList(int signum)
{
  struct timeval t;
  gettimeofday(&t, NULL);
  for (size_t i = 0; i < clients.size(); i++)
  {
    if (t.tv_sec - clients.at(i).tid.tv_sec > 10)
    {
      clients.erase(clients.begin() + i);
      printf("lämnnnna dååå\n");
    }
  }

  return;
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

  int sockfd;
  addrinfo sa, *si, *p;
  sa.ai_family = AF_UNSPEC;
  sa.ai_socktype = SOCK_DGRAM;
  sa.ai_protocol = 17;

  if (int rv = getaddrinfo(Desthost, Destport, &sa, &si) != 0)
  {
    fprintf(stderr, "%s\n", gai_strerror(rv));
    exit(0);
  }

  struct sockaddr_in clientaddr;

  for (p = si; p != NULL; p = p->ai_next)
  {
    if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
    {
      printf("Error: Failed to create socket.\n");
      continue;
    }
    memset(&clientaddr, 0, sizeof(clientaddr));
    if ((bind(sockfd, p->ai_addr, p->ai_addrlen)) != 0)
    {
      printf("Error: Couldnt bind!\n");
      close(sockfd);
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
  int bytes = 0;
  ssize_t sentbytes;
  socklen_t client_len = sizeof(clientaddr);

  setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char *)&tv, sizeof(tv));

  /* 
     Prepare to setup a reoccurring event every 10s. If it_interval, or it_value is omitted, it will be a single alarm 10s after it has been set. 
  */
  struct itimerval alarmTime;
  alarmTime.it_interval.tv_sec = 2;
  alarmTime.it_interval.tv_usec = 2;
  alarmTime.it_value.tv_sec = 2;
  alarmTime.it_value.tv_usec = 2;

  /* Regiter a callback function, associated with the SIGALRM signal, which will be raised when the alarm goes of */
  signal(SIGALRM, checkJobbList);
  setitimer(ITIMER_REAL, &alarmTime, NULL); // Start/register the alarm.

  bool clientFound = false;
  int clientNr = 0;
  int clientInUse = -1;
  fd_set currentSockets;
  fd_set readySockets;
  FD_ZERO(&currentSockets);
  FD_ZERO(&readySockets);
  FD_SET(sockfd, &currentSockets);
  int fdMax = sockfd;
  int nfds = 0;
  char recvBuffer[256];
  char sendBuffer[256];

  while (true)
  {
    readySockets = currentSockets;

    if (FD_ISSET(sockfd, &readySockets))
    {
      bytes = recvfrom(sockfd, recvBuffer, sizeof(recvBuffer), 0, (struct sockaddr *)&clientaddr, &client_len);
      if (bytes < 0)
      {
        continue;
      }
      else
      {
        clientFound = false;
        for (size_t i = 0; i < clients.size() && clientFound == false; i++)
        {
          if (clients.at(i).addr.sin_addr.s_addr == clientaddr.sin_addr.s_addr && clients.at(i).addr.sin_port == clientaddr.sin_port)
          {
            clientFound = true;
            clientNr = i;
          }
        }
        if (clientFound == false)
        {
          if (strcmp(recvBuffer, PROTOCOL) >= 0)
          {
            struct cli newClient;
            newClient.addr = clientaddr;
            gettimeofday(&newClient.tid, NULL);
            clients.push_back(newClient);
            sendto(sockfd, MENU, sizeof(MENU), 0, (struct sockaddr *)&clientaddr, client_len);
          }
          else
          {
            sendto(sockfd, "ERROR, Wrong protocol!\n", sizeof("Error, Wrong protcol!\n"), 0, (struct sockaddr *)&clientaddr, client_len);
          }
        }
        else
        {
          //check what the client presses
        }
      }
    }
  }
  return (0);
}
