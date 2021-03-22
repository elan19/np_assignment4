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

#define VERSION "RPS TCP 1.0\n"
#define MENU "Please select:\n1. Play\n2. Watch\n3.Leaderboard\n\n0.Exit\n"

struct cli
{
  struct timeval tid;
  bool isInGame;
  bool spectating;
  bool isInQueue;
  bool isReady;
  int sockID;
};
std::vector<cli *> clients;
std::vector<cli *> queue;

void checkJobbList(int signum)
{
  struct timeval t;
  gettimeofday(&t, NULL);
  for (size_t i = 0; i < clients.size(); i++)
  {
    if (t.tv_sec - clients.at(i)->tid.tv_sec > 10)
    {
      //clients.erase(clients.begin() + i);
      printf("lämnnnna dååå\n");
    }
  }

  return;
}

int main(int argc, char *argv[])
{

  /* Do more magic */

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

  int sockfd, connfd, len;
  struct sockaddr_in cli;

  struct addrinfo sa, *si, *p;
  memset(&sa, 0, sizeof(sa));
  sa.ai_family = AF_UNSPEC;
  sa.ai_socktype = SOCK_STREAM;
  sa.ai_flags = AI_PASSIVE;

  struct timeval tv;
  tv.tv_sec = 5;
  tv.tv_usec = 0;

  if (int rv = getaddrinfo(Desthost, Destport, &sa, &si) != 0)
  {
    fprintf(stderr, "%s\n", gai_strerror(rv));
    exit(0);
  }

  for (p = si; p != NULL; p = p->ai_next)
  {
    if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
    {
      perror("Error: Couldnt connect.\n");
      continue;
    }

    if ((bind(sockfd, p->ai_addr, p->ai_addrlen)) != 0)
    {
      printf("Error: Couldnt bind!\n");
      close(sockfd);
      continue;
    }
    break;
  }

  //setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char *)&tv, sizeof(tv));
  if (p == NULL)
  {
    printf("NULL\n");
    exit(0);
  }

  freeaddrinfo(si);
  //signal(SIGINT, INThandler);

  if (listen(sockfd, 1) == -1)
  {
    printf("Error: Listen failed!\n");
    exit(0);
  }

  len = sizeof(cli);
  char buffer[256];
  char recvBuffer[256];
  fd_set currentSockets;
  fd_set readySockets;
  FD_ZERO(&currentSockets);
  FD_ZERO(&readySockets);
  FD_SET(sockfd, &currentSockets);
  int fdMax = sockfd;
  int nfds = 0;
  int recieve = 0;

  while (true)
  {
    readySockets = currentSockets;

    nfds = select(fdMax + 1, &readySockets, NULL, NULL, NULL);
    if (nfds == -1)
    {
      printf("Something went wrong with the select\n");
      break;
    }

    for (int i = sockfd; i < fdMax + 1; i++)
    {

      if (FD_ISSET(i, &readySockets))
      {
        if (i == sockfd)
        {
          if ((connfd = accept(sockfd, (struct sockaddr *)&cli, (socklen_t *)&len)) == -1)
          {
            continue;
          }
          else
          {
            struct cli newClient;
            newClient.isInGame = false;
            newClient.isInQueue = false;
            newClient.isReady = false;
            newClient.spectating = false;
            gettimeofday(&newClient.tid, NULL);
            newClient.sockID = connfd;
            FD_SET(newClient.sockID, &currentSockets);
            clients.push_back(&newClient);
            char buf[sizeof(VERSION)] = VERSION;
            send(connfd, buf, strlen(buf), 0);
            printf("Server protocol: %s\n", buf);
            if (newClient.sockID > fdMax)
            {
              fdMax = newClient.sockID;
            }
          }
        }
        else
        {
          memset(recvBuffer, 0, sizeof(recvBuffer));
          recieve = recv(i, recvBuffer, sizeof(recvBuffer), 0);
          if (recieve <= 0)
          {
            close(i);
            for (size_t j = 0; j < clients.size(); j++)
            {
              if (i == clients[j]->sockID)
              {
                clients.erase(clients.begin() + j);
                FD_CLR(i, &currentSockets);
                break;
              }
            }
            continue;
          }
          else if(strstr(recvBuffer, "OK") != nullptr)
          {
            send(i, MENU, strlen(MENU), 0);
          }
          else
          {
            send(i, "ERROR Wrong format on the message\n", strlen("ERORR Wrong format on the message\n"), 0);
          }
        }
        FD_CLR(i, &readySockets);
      }
    }
  }

#ifdef DEBUG
  printf("Host %s, and port %d.\n", Desthost, port);
#endif

  close(sockfd);
  return 0;
}
