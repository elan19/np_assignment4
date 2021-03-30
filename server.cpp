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
#include <netdb.h>
#include <vector>
#include <signal.h>
#include <errno.h>

#define VERSION "RPS TCP 1.0\n"
#define MENU "Please select:\n1. Play\n2. Watch\n3.Leaderboard\n\n0.Exit\n"
#define GAMEOPTIONS "Please select an option!\n1. Rock\n2. Paper\n3. Scissor\n"

struct cli
{
  struct timeval tid;
  bool isInGame;
  bool spectating;
  bool isInQueue;
  bool isReady;
  int sockID;
};
struct game
{
  struct cli *player1;
  struct cli *player2;
  int p1score, p2score, winner;
  int p1Answer, p2Answer;
};

std::vector<game *> games;
std::vector<cli> clients;
std::vector<cli *> queue;

void checkJobbList(int signum)
{
  //struct timeval timer;
  /*for (size_t i = 0; i < clients.size(); i++)
  {
    if (t.tv_sec - clients.at(i).tid.tv_sec > 10)
    {
      //clients.erase(clients.begin() + i);
      printf("lämnnnna dååå\n");
    }
  }*/

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
  struct sockaddr_in client;

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

  /* Regiter a callback function, associated with the SIGALRM signal, which will be raised when the alarm goes of */
  //signal(SIGALRM, checkJobbList);
  //setitimer(ITIMER_REAL, &alarmTime, NULL); // Start/register the alarm.

  len = sizeof(client);
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
  int currentClient = 0;
  struct timeval timer;
  timer.tv_sec = 0;
  timer.tv_usec = 1000000;
  struct timeval gameTimer;
  gameTimer.tv_usec = 0;
  gameTimer.tv_usec = 0;

  while (true)
  {
    readySockets = currentSockets;
    if (games.size() > 0)
    {
      for (size_t i = 0; i < games.size(); i++)
      {
        if (games.at(i)->p1Answer != 0 && games.at(i)->p2Answer != 0)
        {
          if (games.at(i)->p1Answer == 1 && games.at(i)->p2Answer == 2)
          {
            games.at(i)->p2score++;
          }
          else if (games.at(i)->p1Answer == 1 && games.at(i)->p2Answer == 3)
          {
            games.at(i)->p1score++;
          }
          else if (games.at(i)->p1Answer == 2 && games.at(i)->p2Answer == 1)
          {
            games.at(i)->p1score++;
          }
          else if (games.at(i)->p1Answer == 2 && games.at(i)->p2Answer == 3)
          {
            games.at(i)->p2score++;
          }
          else if (games.at(i)->p1Answer == 3 && games.at(i)->p2Answer == 1)
          {
            games.at(i)->p2score++;
          }
          else if (games.at(i)->p1Answer == 3 && games.at(i)->p2Answer == 2)
          {
            games.at(i)->p1score++;
          }
          games.at(i)->p1Answer = 0;
          games.at(i)->p2Answer = 0;
          memset(buffer, 0, sizeof(buffer));
          sprintf(buffer, "Score %d - %d\n", games.at(i)->p1score, games.at(i)->p2score);
          send(games.at(i)->player1->sockID, buffer, strlen(buffer), 0);
          memset(buffer, 0, sizeof(buffer));
          sprintf(buffer, "Score %d - %d\n", games.at(i)->p2score, games.at(i)->p1score);
          send(games.at(i)->player2->sockID, buffer, strlen(buffer), 0);
          if (games.at(i)->p1score < 3 && games.at(i)->p2score < 3)
          {
            send(games.at(i)->player1->sockID, GAMEOPTIONS, strlen(GAMEOPTIONS), 0);
            send(games.at(i)->player2->sockID, GAMEOPTIONS, strlen(GAMEOPTIONS), 0);
          }
          else
          {

            if (games.at(i)->p1score == 3)
            {
              send(games.at(i)->player1->sockID, "You Won!\n", strlen("You Won!\n"), 0);
              send(games.at(i)->player2->sockID, "You Lost!\n", strlen("You Lost!\n"), 0);
            }
            else if (games.at(i)->p2score == 3)
            {
              send(games.at(i)->player2->sockID, "You Won!\n", strlen("You Won!\n"), 0);
              send(games.at(i)->player1->sockID, "You Lost!\n", strlen("You Lost!\n"), 0);
            }
            games.at(i)->player1->isInGame = false;
            games.at(i)->player1->isReady = false;
            games.at(i)->player2->isInGame = false;
            games.at(i)->player2->isReady = false;
            send(games.at(i)->player2->sockID, MENU, strlen(MENU), 0);
            send(games.at(i)->player1->sockID, MENU, strlen(MENU), 0);
            games.erase(games.begin() + i);
          }
        }
        /*gettimeofday(&gameTimer, NULL);
        if ((gameTimer.tv_usec - games.at(i)->player1->tid.tv_usec) > 200000 && games.at(i)->player1->isReady == true)
        {
          send(games.at(i)->player1->sockID, "Game is starting!\n", strlen("Game is starting!\n"), 0);
        }
        if ((gameTimer.tv_usec - games.at(i)->player2->tid.tv_usec) > 200000 && games.at(i)->player1->isReady == true)
        {
          send(games.at(i)->player2->sockID, "Game is starting!\n", strlen("Game is starting!\n"), 0);
        }*/
      }
    }

    nfds = select(fdMax + 1, &readySockets, NULL, NULL, &timer);
    if (nfds == -1)
    {
      printf("Something went wrong with the select\n");
      break;
    }
    else if (nfds == 0)
    {
      timer.tv_usec = 1000000;
      //printf("%ld\n", timer.tv_usec);
      for (size_t i = 0; i < games.size(); i++)
      {
        printf("ID1: %d, ID2: %d\n", games.at(i)->player1->sockID, games.at(i)->player2->sockID);
      }
    }

    for (int i = sockfd; i < fdMax + 1; i++)
    {
      if (FD_ISSET(i, &readySockets))
      {
        if (i == sockfd)
        {
          if ((connfd = accept(sockfd, (struct sockaddr *)&client, (socklen_t *)&len)) == -1)
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
            clients.push_back(newClient);
            char buf[sizeof(VERSION)] = VERSION;
            send(connfd, buf, strlen(buf), 0);
            printf("Server protocol: %s\n", buf);
            printf("%d\n", connfd);
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
          if (clients.size() > 0)
          {
            printf("recieve = %s\n", recvBuffer);
            if (recieve > 0)
            {
              for (size_t j = 0; j < games.size(); j++)
              {
                printf("%d, %d %d\n", i, games.at(j)->player1->sockID, games.at(j)->player2->sockID);
                if (games.at(j)->player1->sockID == i)
                {
                  printf("player1\n");
                  if (games.at(j)->p1Answer == 0 && (strcmp(recvBuffer, "1") == 0 || strcmp(recvBuffer, "2") == 0 || strcmp(recvBuffer, "3") == 0))
                  {
                    printf("player1\n");
                    if (strcmp(recvBuffer, "1") == 0)
                    {
                      games.at(j)->p1Answer = 1;
                    }
                    else if (strcmp(recvBuffer, "2") == 0)
                    {
                      games.at(j)->p1Answer = 2;
                    }
                    else if (strcmp(recvBuffer, "3") == 0)
                    {
                      games.at(j)->p1Answer = 3;
                    }
                    gettimeofday(&games.at(j)->player1->tid, NULL);
                  }
                }
                else if (games.at(j)->player2->sockID == i)
                {
                  printf("player2\n");
                  if (games.at(j)->p2Answer == 0 && (strcmp(recvBuffer, "1") == 0 || strcmp(recvBuffer, "2") == 0 || strcmp(recvBuffer, "3") == 0))
                  {
                    printf("player 2\n");
                    if (strcmp(recvBuffer, "1") == 0)
                    {
                      games.at(j)->p2Answer = 1;
                    }
                    else if (strcmp(recvBuffer, "2") == 0)
                    {
                      games.at(j)->p2Answer = 2;
                    }
                    else if (strcmp(recvBuffer, "3") == 0)
                    {
                      games.at(j)->p2Answer = 3;
                    }
                    gettimeofday(&games.at(j)->player2->tid, NULL);
                  }
                }
              }
            }

            if (recieve <= 0)
            {
              close(i);
              for (size_t j = 0; j < clients.size(); j++)
              {
                if (i == clients[j].sockID)
                {
                  clients.erase(clients.begin() + j);
                  FD_CLR(i, &currentSockets);
                  break;
                }
              }
              continue;
            }
            else if (strstr(recvBuffer, "OK") != nullptr)
            {
              send(i, MENU, strlen(MENU), 0);
            }
            else if (strstr(recvBuffer, "1") != nullptr)
            {
              currentClient = -1;
              for (size_t j = 0; j < clients.size() && currentClient == -1; j++)
              {
                if (clients.at(j).sockID == i)
                {
                  currentClient = j;
                }
              }
              if (currentClient > -1 && clients.at(currentClient).isInGame == false && clients.at(currentClient).isInQueue == false && clients.at(currentClient).isReady == false)
              {
                clients.at(currentClient).isInQueue = true;
                queue.push_back(&clients.at(currentClient));
                printf("%d\n", queue.at(queue.size() - 1)->sockID);
                if (queue.size() < 2)
                {
                  send(i, "Need another player to start the game\nPress 'Enter' to leave queue\n", strlen("Need another player to start the game\nPress 'Enter' to leave queue\n"), 0);
                }
                else if (queue.size() >= 2)
                {

                  struct game newGame;
                  newGame.p1score = 0;
                  newGame.p2score = 0;
                  newGame.p1Answer = 0;
                  newGame.p2Answer = 0;
                  newGame.winner = -1;
                  newGame.player1 = queue.at(0);
                  newGame.player2 = queue.at(1);

                  games.push_back(&newGame);

                  for (int j = 0; j < 2; j++)
                  {
                    queue.at(j)->isInQueue = false;
                    queue.at(j)->isInGame = true;
                    send(queue.at(j)->sockID, "A game is ready, press 'Enter' to accept!\n", strlen("A game is ready, press 'Enter' to accept!\n"), 0);
                  }
                  for (int j = 0; j < 2; j++)
                  {
                    queue.erase(queue.begin());
                  }
                }
              }
            }
            else if (strcmp(recvBuffer, "") == 0)
            {
              currentClient = -1;
              if (queue.size() > 0)
              {
                for (size_t j = 0; j < queue.size() && currentClient == -1; j++)
                {
                  if (queue.at(j)->sockID == i)
                  {
                    currentClient = j;
                  }
                }

                if (currentClient > -1 && queue.at(currentClient)->isInGame == false && queue.at(currentClient)->isInQueue == true && queue.at(currentClient)->isReady == false)
                {
                  queue.at(currentClient)->isInQueue = false;
                  queue.erase(queue.begin() + currentClient);
                  send(i, MENU, strlen(MENU), 0);
                }
              }
            }
            else if (strcmp(recvBuffer, "r") == 0)
            {
              currentClient = -1;
              for (size_t j = 0; j < games.size() && currentClient == -1; j++)
              {
                if (games.at(j)->player1->sockID == i)
                {
                  currentClient = games.at(j)->player1->sockID;
                  games.at(j)->player1->isReady = true;
                }
                else if (games.at(j)->player2->sockID == i)
                {
                  currentClient = games.at(j)->player2->sockID;
                  games.at(j)->player2->isReady = true;
                }
                if (games.at(j)->player1->isReady == true && games.at(j)->player2->isReady == true)
                {
                  send(games.at(j)->player1->sockID, GAMEOPTIONS, strlen(GAMEOPTIONS), 0);
                  send(games.at(j)->player2->sockID, GAMEOPTIONS, strlen(GAMEOPTIONS), 0);
                }
                else
                {
                  send(currentClient, "Waiting for the other player to be ready\n", strlen("Waiting for the other player to be ready\n"), 0);
                }
              }
            }
            else if (strcmp(recvBuffer, "1") == 0 || strcmp(recvBuffer, "2") == 0 || strcasecmp(recvBuffer, "3") == 0)
            {
            }
            else
            {
              send(i, "ERROR Wrong format on the message\n", strlen("ERORR Wrong format on the message\n"), 0);
            }
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
