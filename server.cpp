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
  int answer;
  size_t gameID;
  int score;
};
struct game
{
  struct cli *player1;
  struct cli *player2;
  int p1score, p2score, winner;
  int p1Answer, p2Answer;
  struct timeval tid;
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
  timer.tv_usec = 0;
  struct timeval gameTimer;
  gameTimer.tv_usec = 0;
  gameTimer.tv_usec = 0;

  while (true)
  {

    readySockets = currentSockets;
    if (games.size() > 0)
    {

      currentClient = -1;
      //printf("games size is over 0\n");
      for (size_t i = 0; i < games.size(); i++)
      {
        size_t number = (gameTimer.tv_sec - games.at(i)->tid.tv_sec);
        //printf("inside games forloop at start\n");
        for (size_t j = 0; j < clients.size(); j++)
        {
          //printf("inside clients first forloop in start\n");
          for (size_t g = 0; g < clients.size(); g++)
          {
            //printf("inside clients second forloop in start\n");
            //printf("Client SOCKID: %d and client1 score: %d and gameID: %d\n", client.at(j).sockID,clients.at(j).score, clients.at(j).gameID);
            //printf("Client SOCKID: %d, client2 score: %d and gameID: %d\n", client.at(g).sockID, clients.at(g).score, clients.at(g).gameID);
            //printf("tjenna\n");
            if (clients.at(j).sockID != clients.at(g).sockID && clients.at(j).isInGame == true && clients.at(g).isInGame == true)
            {
              //printf("jomenvisst\n");
              if (clients.at(j).gameID == i && clients.at(g).gameID == i)
              {
                currentClient = j;
                /*for (size_t k = 0; k < clients.size(); k++)
                {
                  if (clients.at(k).spectating == true && clients.at(k).gameID == i)
                  {
                    currentClient = k;
                  }
                }*/

                //printf("Client SOCKID: %d, client1 score: %d and gameID: %ld\n", clients.at(j).sockID, clients.at(j).answer, clients.at(j).gameID);
                //printf("Client SOCKID: %d, client2 score: %d and gameID: %ld\n", clients.at(g).sockID, clients.at(g).answer, clients.at(g).gameID);
                //printf("GameID is: %ld\n", i);
                //printf("client1 sockID: %d, client 1 score: %d and gameID: %d\n", clients.at(j).sockID, clients.at(j).score, clients.at(j).gameID);
                //printf("client2 sockID: %d, client 2 score: %d and gameID: %d\n", clients.at(g).sockID, clients.at(g).score, clients.at(g).gameID);
                if (clients.at(j).answer != 0 && clients.at(g).answer != 0)
                {
                  for (size_t k = 0; k < clients.size(); k++)
                  {
                    //printf("clients spectating gameID: %ld and real gameID is: %ld\n", clients.at(k).gameID, i);
                    //printf("client spectating: %s\n", clients.at(k).spectating);
                    if (clients.at(k).spectating == true && clients.at(k).gameID == i)
                    {
                      printf("yup\n");
                      if (clients.at(j).answer == 1 && clients.at(g).answer == 2)
                      {
                        send(clients.at(k).sockID, "Player 1 selected: 1. Rock \tPlayer 2 selected: 2. Paper. Player 2 wins\n", strlen("Player 1 selected: 1. Rock \tPlayer 2 selected: 2. Paper. Player 2 wins\n"), 0);
                      }
                      else if (clients.at(j).answer == 1 && clients.at(g).answer == 3)
                      {
                        send(clients.at(k).sockID, "Player 1 selected: 1. Rock \tPlayer 2 selected: 3. Scissor. Player 1 wins\n", strlen("Player 1 selected: 1. Rock \tPlayer 2 selected: 3. Scissor. Player 1 wins\n"), 0);
                      }
                      else if (clients.at(j).answer == 2 && clients.at(g).answer == 1)
                      {
                        send(clients.at(k).sockID, "Player 1 selected: 2. Paper \tPlayer 2 selected: 1. Rock. Player 1 wins\n", strlen("Player 1 selected: 2. Paper \tPlayer 2 selected: 1. Rock. Player 1 wins\n"), 0);
                      }

                      else if (clients.at(j).answer == 2 && clients.at(g).answer == 3)
                      {
                        send(clients.at(k).sockID, "Player 1 selected: 2. Paper \tPlayer 2 selected: 3. Scissor. Player 2 wins\n", strlen("Player 1 selected: 2. Paper \tPlayer 2 selected: 3. Scissor. Player 2 wins\n"), 0);
                      }
                      else if (clients.at(j).answer == 3 && clients.at(g).answer == 1)
                      {
                        send(clients.at(k).sockID, "Player 1 selected: 3. Scissor \tPlayer 2 selected: 1. Rock. Player 2 wins\n", strlen("Player 1 selected: 3. Scissor \tPlayer 2 selected: 1. Rock. Player 2 wins\n"), 0);
                      }
                      else if (clients.at(j).answer == 3 && clients.at(g).answer == 2)
                      {
                        send(clients.at(k).sockID, "Player 1 selected: 3. Scissor \tPlayer 2 selected: 2. Paper. Player 1 wins\n", strlen("Player 1 selected: 3. Scissor \tPlayer 2 selected: 2. Paper. Player 1 wins\n"), 0);
                      }
                    }
                  }
                  //printf("score är inte 0\n");
                  if (clients.at(j).answer == 1 && clients.at(g).answer == 2)
                  {
                    clients.at(g).score++;
                  }
                  else if (clients.at(j).answer == 1 && clients.at(g).answer == 3)
                  {
                    clients.at(j).score++;
                  }
                  else if (clients.at(j).answer == 2 && clients.at(g).answer == 1)
                  {
                    clients.at(j).score++;
                  }
                  else if (clients.at(j).answer == 2 && clients.at(g).answer == 3)
                  {
                    clients.at(g).score++;
                  }
                  else if (clients.at(j).answer == 3 && clients.at(g).answer == 1)
                  {
                    clients.at(g).score++;
                  }
                  else if (clients.at(j).answer == 3 && clients.at(g).answer == 2)
                  {
                    clients.at(j).score++;
                  }
                  clients.at(j).answer = 0;
                  clients.at(g).answer = 0;
                  memset(buffer, 0, sizeof(buffer));
                  sprintf(buffer, "Score %d - %d\n", clients.at(j).score, clients.at(g).score);
                  send(clients.at(j).sockID, buffer, strlen(buffer), 0);
                  memset(buffer, 0, sizeof(buffer));
                  sprintf(buffer, "Score %d - %d\n", clients.at(g).score, clients.at(j).score);
                  send(clients.at(g).sockID, buffer, strlen(buffer), 0);
                  //gameTimer.tv_sec = games.at(i)->tid.tv_sec;

                  for (size_t k = 0; k < clients.size(); k++)
                  {
                    if (clients.at(k).spectating == true && clients.at(k).gameID == i)
                    {
                      memset(buffer, 0, sizeof(buffer));
                      sprintf(buffer, "Score %d - %d\n\n", clients.at(g).score, clients.at(j).score);
                      send(clients.at(k).sockID, buffer, strlen(buffer), 0);
                    }
                  }
                  if (clients.at(j).score < 3 && clients.at(g).score < 3)
                  {
                    printf("Should restart round\n");
                    gettimeofday(&games.at(i)->tid, NULL);
                    printf("%ld\n", gameTimer.tv_sec);
                    //send(clients.at(j).sockID, GAMEOPTIONS, strlen(GAMEOPTIONS), 0);
                    //send(clients.at(g).sockID, GAMEOPTIONS, strlen(GAMEOPTIONS), 0);
                  }
                  else
                  {

                    if (clients.at(j).score == 3)
                    {
                      send(clients.at(j).sockID, "You Won!\n", strlen("You Won!\n"), 0);
                      send(clients.at(g).sockID, "You Lost!\n", strlen("You Lost!\n"), 0);
                    }
                    else if (clients.at(g).score == 3)
                    {
                      send(clients.at(g).sockID, "You Won!\n", strlen("You Won!\n"), 0);
                      send(clients.at(j).sockID, "You Lost!\n", strlen("You Lost!\n"), 0);
                    }
                    else
                    {
                      break;
                    }
                    clients.at(j).gameID = -1;
                    clients.at(g).gameID = -1;
                    clients.at(j).isInGame = false;
                    clients.at(g).isInGame = false;
                    clients.at(j).isInQueue = false;
                    clients.at(j).isReady = false;
                    clients.at(g).isInQueue = false;
                    clients.at(g).isReady = false;
                    clients.at(j).answer = 0;
                    clients.at(g).answer = 0;
                    clients.at(j).score = 0;
                    clients.at(g).score = 0;
                    send(clients.at(j).sockID, MENU, strlen(MENU), 0);
                    send(clients.at(g).sockID, MENU, strlen(MENU), 0);

                    games.erase(games.begin() + i);
                    for (size_t p = 0; p < clients.size(); p++)
                    {
                      if (clients.at(p).gameID > i)
                      {
                        clients.at(p).gameID--;
                      }
                    }
                    for (size_t k = 0; k < clients.size(); k++)
                    {
                      if (clients.at(k).spectating == true)
                      {
                        if (games.size() > 0)
                        {
                          send(clients.at(k).sockID, "Choose a game to watch!\n", strlen("Choose a game to watch!\n"), 0);
                          for (size_t h = 0; h < games.size(); h++)
                          {
                            memset(buffer, 0, sizeof(buffer));
                            sprintf(buffer, "%ld\n", j + 1);
                            send(clients.at(k).sockID, buffer, strlen(buffer), 0);
                          }
                        }
                        else
                        {
                          send(clients.at(k).sockID, "No games active. Try again later!\n", strlen("No games active. Try again later!\n"), 0);
                          send(clients.at(k).sockID, MENU, strlen(MENU), 0);
                          clients.at(k).gameID = -1;
                          clients.at(k).spectating = false;
                        }
                      }
                    }
                  }
                }
                if ((gameTimer.tv_sec - clients.at(j).tid.tv_sec) == 3 && clients.at(j).answer == 0 && (clients.at(g).answer == 1 || clients.at(g).answer == 2 || clients.at(g).answer == 3))
                {
                  clients.at(g).score++;
                  clients.at(j).answer = 0;
                  clients.at(g).answer = 0;
                  memset(buffer, 0, sizeof(buffer));
                  sprintf(buffer, "You forgot to choose!\nScore %d - %d\n", clients.at(j).score, clients.at(g).score);
                  send(clients.at(j).sockID, buffer, strlen(buffer), 0);
                  memset(buffer, 0, sizeof(buffer));
                  sprintf(buffer, "Other player forgot to choose!\nScore %d - %d\n", clients.at(g).score, clients.at(j).score);
                  send(clients.at(g).sockID, buffer, strlen(buffer), 0);
                  gettimeofday(&games.at(i)->tid, NULL);
                  gettimeofday(&clients.at(j).tid, NULL);
                  gettimeofday(&clients.at(g).tid, NULL);
                  if (clients.at(j).score == 3)
                  {
                    send(clients.at(j).sockID, "You Won!\n", strlen("You Won!\n"), 0);
                    send(clients.at(g).sockID, "You Lost!\n", strlen("You Lost!\n"), 0);
                  }
                  else if (clients.at(g).score == 3)
                  {
                    send(clients.at(g).sockID, "You Won!\n", strlen("You Won!\n"), 0);
                    send(clients.at(j).sockID, "You Lost!\n", strlen("You Lost!\n"), 0);
                  }
                  else
                  {
                    break;
                  }
                  clients.at(j).gameID = -1;
                  clients.at(g).gameID = -1;
                  clients.at(j).isInGame = false;
                  clients.at(g).isInGame = false;
                  clients.at(j).isInQueue = false;
                  clients.at(j).isReady = false;
                  clients.at(g).isInQueue = false;
                  clients.at(g).isReady = false;
                  clients.at(j).answer = 0;
                  clients.at(g).answer = 0;
                  clients.at(j).score = 0;
                  clients.at(g).score = 0;
                  send(clients.at(j).sockID, MENU, strlen(MENU), 0);
                  send(clients.at(g).sockID, MENU, strlen(MENU), 0);

                  games.erase(games.begin() + i);
                  for (size_t p = 0; p < clients.size(); p++)
                  {
                    if (clients.at(p).gameID > i)
                    {
                      clients.at(p).gameID--;
                    }
                  }
                  for (size_t k = 0; k < clients.size(); k++)
                  {
                    if (clients.at(k).spectating == true)
                    {
                      if (games.size() > 0)
                      {
                        send(clients.at(k).sockID, "Choose a game to watch!\n", strlen("Choose a game to watch!\n"), 0);
                        for (size_t h = 0; h < games.size(); h++)
                        {
                          memset(buffer, 0, sizeof(buffer));
                          sprintf(buffer, "%ld\n", j + 1);
                          send(clients.at(k).sockID, buffer, strlen(buffer), 0);
                        }
                      }
                      else
                      {
                        send(clients.at(k).sockID, "No games active. Try again later!\n", strlen("No games active. Try again later!\n"), 0);
                        send(clients.at(k).sockID, MENU, strlen(MENU), 0);
                        clients.at(k).gameID = -1;
                        clients.at(k).spectating = false;
                      }
                    }
                  }
                }
                else if ((gameTimer.tv_sec - clients.at(j).tid.tv_sec) == 3 && clients.at(j).answer == 0 && clients.at(g).answer == 0)
                {
                  memset(buffer, 0, sizeof(buffer));
                  sprintf(buffer, "You forgot to choose!\nScore %d - %d\n", clients.at(j).score, clients.at(g).score);
                  send(clients.at(j).sockID, buffer, strlen(buffer), 0);
                  memset(buffer, 0, sizeof(buffer));
                  sprintf(buffer, "You forgot to choose!\nScore %d - %d\n", clients.at(g).score, clients.at(j).score);
                  send(clients.at(g).sockID, buffer, strlen(buffer), 0);
                  gettimeofday(&games.at(i)->tid, NULL);
                  gettimeofday(&clients.at(j).tid, NULL);
                  gettimeofday(&clients.at(g).tid, NULL);
                }
              }
            }
          }
          if (clients.at(j).gameID == i)
          {
            gettimeofday(&gameTimer, NULL);

            if (number >= 0 && number < 5)
            {
              gettimeofday(&clients.at(j).tid, NULL);
              if (number == 0)
              {
                memset(buffer, 0, sizeof(buffer));
                sprintf(buffer, "Both players are now ready\n");
              }
              else if (number == 1)
              {
                memset(buffer, 0, sizeof(buffer));
                sprintf(buffer, "Game is starting in 3 seconds!\n");
              }
              else if (number == 2)
              {
                memset(buffer, 0, sizeof(buffer));
                sprintf(buffer, "Game is starting in 2 seconds!\n");
              }
              else if (number == 3)
              {
                memset(buffer, 0, sizeof(buffer));
                sprintf(buffer, "Game is starting in 1 seconds!\n");
              }
              else if (number == 4)
              {
                memset(buffer, 0, sizeof(buffer));
                sprintf(buffer, GAMEOPTIONS);
              }

              send(clients.at(j).sockID, buffer, strlen(buffer), 0);
              //send(clients.at(g).sockID, "Game is starting in 1 seconds!\n", strlen("Game is starting in 1 seconds!\n"), 0);
              printf("sug\n");

              printf("tid: %ld\n", gameTimer.tv_sec);
              printf("Clients tid: %ld\n", games.at(i)->tid.tv_sec);

              printf("number is: %ld\n", number);
            }
          }
        }

        /*if (games.at(i)->p1Answer != 0 && games.at(i)->p2Answer != 0)
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
        */
      }
    }

    if (games.size() > 0)
    {
    }

    nfds = select(fdMax + 1, &readySockets, NULL, NULL, &timer);

    if (nfds == -1)
    {
      printf("Something went wrong with the select\n");
      break;
    }
    else if (nfds >= 0)
    {
      timer.tv_usec = 1000000;
      //printf("%ld\n", timer.tv_usec);
      /*if (games.size() > 0)
      {
        for (size_t i = 0; i < games.size(); i++)
        {
          
        }
      }*/
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
            newClient.answer = 0;
            newClient.gameID = -1;
            newClient.score = 0;
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
          printf("RECIEVE IS: ");
          recieve = recv(i, recvBuffer, sizeof(recvBuffer), 0);
          printf("%s\n", recvBuffer);

          if (clients.size() > 0)
          {
            printf("client size is: %ld\n", clients.size());
            printf("recieve = %s\n", recvBuffer);
            if (recieve > 0)
            {
              printf("recieve is bigger than 0\n");
              if (strstr(recvBuffer, "OK") != nullptr)
              {
                currentClient = -1;
                for (size_t j = 0; j < clients.size() && currentClient == -1; j++)
                {
                  if (clients.at(j).sockID == i)
                  {
                    currentClient = j;
                  }
                }
                send(clients.at(currentClient).sockID, MENU, strlen(MENU), 0);
                printf("sending MENU\n");
                break;
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
                if (currentClient > -1 && clients.at(currentClient).isInGame == false && clients.at(currentClient).isInQueue == false && clients.at(currentClient).isReady == false && clients.at(currentClient).spectating == false && queue.size() < 2)
                {
                  clients.at(currentClient).isInQueue = true;
                  queue.push_back(&clients.at(currentClient));
                  printf("SOCKID for the client in queue: %d\n", queue.at(queue.size() - 1)->sockID);
                  if (queue.size() < 2)
                  {
                    send(clients.at(currentClient).sockID, "Need another player to start the game\nPress 'Enter' to leave queue\n", strlen("Need another player to start the game\nPress 'Enter' to leave queue\n"), 0);
                  }
                  else if (queue.size() >= 2)
                  {

                    /*struct game newGame;
                    newGame.p1score = 0;
                    newGame.p2score = 0;
                    newGame.p1Answer = 0;
                    newGame.p2Answer = 0;
                    newGame.winner = -1;
                    newGame.player1 = queue.at(0);
                    newGame.player2 = queue.at(1);

                    games.push_back(&newGame);*/
                    if (queue.at(0)->sockID == i || queue.at(1)->sockID == i)
                    {
                      for (size_t j = 0; j < queue.size(); j++)
                      {
                        //queue.at(0)->isInQueue = false;
                        //queue.at(0)->isReady = true;

                        send(queue.at(j)->sockID, "A game is ready, press 'r' to accept!\n", strlen("A game is ready, press 'r' to accept!\n"), 0);
                        //queue.erase(queue.begin());
                      }
                    }
                  }
                }
                else if (currentClient > -1 && clients.at(currentClient).isInGame == false && clients.at(currentClient).isInQueue == false && clients.at(currentClient).isReady == false && clients.at(currentClient).spectating == false)
                {
                  send(clients.at(currentClient).sockID, "Queue is full, try again\n", strlen("Queue is full, try again\n"), 0);
                }
              }
              else if (strcmp(recvBuffer, "2") == 0)
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
                  clients.at(currentClient).spectating = true;
                  //printf("spectating: %s\n", clients.at(currentClient).spectating);

                  if (games.size() > 0)
                  {
                    send(clients.at(currentClient).sockID, "Choose a game to watch!\n", strlen("Choose a game to watch!\n"), 0);
                    for (size_t j = 0; j < games.size(); j++)
                    {
                      memset(buffer, 0, sizeof(buffer));
                      sprintf(buffer, "%ld\n", j + 1);
                      send(clients.at(currentClient).sockID, buffer, strlen(buffer), 0);
                    }
                  }
                  else
                  {
                    send(clients.at(currentClient).sockID, "No games active. Try again later!\n", strlen("No games active. Try again later!\n"), 0);
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
                    send(clients.at(currentClient).sockID, MENU, strlen(MENU), 0);
                  }
                }
              }
              else if (strcmp(recvBuffer, "r") == 0)
              {
                currentClient = -1;
                for (size_t j = 0; j < clients.size() && currentClient == -1; j++)
                {
                  if (clients.at(j).sockID == i)
                  {
                    currentClient = j;
                  }
                }
                printf("game size is: %ld\n", games.size());
                if (queue.size() == 2)
                {
                  for (size_t j = 0; j < queue.size(); j++)
                  {
                    if (queue.at(j)->sockID == i)
                    {
                      queue.at(j)->isInQueue = false;
                      queue.at(j)->isReady = true;
                    }
                  }

                  if (queue.at(0)->isReady == true && queue.at(1)->isReady == false)
                  {
                    send(queue.at(0)->sockID, "Waiting for the other player to be ready\n", strlen("Waiting for the other player to be ready\n"), 0);
                    break;
                  }
                  if (queue.at(0)->isReady == false && queue.at(1)->isReady == true)
                  {
                    send(queue.at(1)->sockID, "Waiting for the other player to be ready\n", strlen("Waiting for the other player to be ready\n"), 0);
                    break;
                  }

                  if (queue.at(0)->isReady == true && queue.at(1)->isReady == true)
                  {
                    printf("Both clients in queue is ready, setting up a game!\n");
                    struct game newGame;
                    newGame.p1score = 0;
                    newGame.p2score = 0;
                    newGame.p1Answer = 0;
                    newGame.p2Answer = 0;
                    newGame.winner = -1;
                    newGame.player1 = queue.at(0);
                    newGame.player2 = queue.at(1);
                    newGame.player1->isReady = true;
                    newGame.player2->isReady = true;
                    gettimeofday(&newGame.tid, NULL);

                    games.push_back(&newGame);

                    for (size_t j = 0; j < clients.size(); j++)
                    {
                      if (clients.at(j).sockID == queue.at(0)->sockID)
                      {
                        clients.at(j).gameID = games.size() - 1;
                        gettimeofday(&clients.at(j).tid, NULL);
                        printf("****clients gameID: %ld\n", clients.at(j).gameID);
                      }
                      else if (clients.at(j).sockID == queue.at(1)->sockID)
                      {
                        clients.at(j).gameID = games.size() - 1;
                        gettimeofday(&clients.at(j).tid, NULL);
                        printf("****clients gameID: %ld\n", clients.at(j).gameID);
                      }
                    }

                    for (int i = 0; i < 2; i++)
                    {
                      queue.erase(queue.begin());
                      printf("Erasing clients from queue, everything worked!\n");
                    }
                    if (games.size() > 0)
                    {
                      for (size_t j = 0; j < games.size(); j++)
                      {
                        /*currentClient = -1;
                    printf("inside the forlopp\n");
                    if (games.at(j)->player1->isReady == true)
                    {
                      //currentClient = games.at(j)->player1->sockID;
                      
                      printf("player1 is now ingame\n");
                    }
                    else if (games.at(j)->player2->isReady == i)
                    {
                      //currentClient = games.at(j)->player2->sockID;
                      games.at(j)->player2->isInGame = true;
                      
                      printf("player2 is now ingame\n");
                    }*/
                        if (games.at(j)->player1->isReady == true && games.at(j)->player2->isReady == true)
                        {
                          //send(games.at(j)->player1->sockID, GAMEOPTIONS, strlen(GAMEOPTIONS), 0);
                          //send(games.at(j)->player2->sockID, GAMEOPTIONS, strlen(GAMEOPTIONS), 0);
                          games.at(j)->player1->isInGame = true;
                          games.at(j)->player2->isInGame = true;
                          for (size_t g = 0; g < clients.size(); g++)
                          {
                            if (clients.at(g).sockID == games.at(j)->player1->sockID)
                            {
                              clients.at(g).isInGame = true;
                            }
                            else if (clients.at(g).sockID == games.at(j)->player2->sockID)
                            {
                              clients.at(g).isInGame = true;
                            }
                          }
                          games.at(j)->player1->isReady = false;
                          games.at(j)->player1->isReady = false;
                          printf("player1 sockid is this: %d\n", games.at(j)->player1->sockID);
                          printf("player2 sockid is this: %d\n", games.at(j)->player2->sockID);
                          printf("Sent out game choice to players\n");
                        }
                      }
                    }
                  }
                }

                printf("games size now: %ld\n", games.size());
              }
              else if (strcmp(recvBuffer, "1") == 0 || strcmp(recvBuffer, "2") == 0 || strcmp(recvBuffer, "3") == 0)
              {
              }
              else
              {
                send(i, "ERROR Wrong format on the message\n", strlen("ERORR Wrong format on the message\n"), 0);
              }
              printf("recieve buffer: %s\n", recvBuffer);
            }
            if (games.size() > 0)
            {
              printf("%ld\n", timer.tv_sec);
              for (size_t j = 0; j < clients.size(); j++)
              {
              }
              currentClient = -1;
              for (size_t j = 0; j < clients.size() && currentClient == -1; j++)
              {
                if (clients.at(j).sockID == i)
                {
                  currentClient = j;
                }
              }
              char checking[256];
              printf("game is bigger than 0\n");
              for (size_t j = 0; j < games.size(); j++)
              {
                for (size_t p = 0; p < clients.size(); p++)
                {
                  if (clients.at(p).spectating == true && clients.at(p).gameID != j)
                  {
                    printf("client is spectating\n");
                    snprintf(checking, sizeof(checking), "%zu", j + 1);
                    printf("%s\n", checking);
                    if (strcmp(recvBuffer, checking) == 0)
                    {
                      clients.at(p).gameID = j;
                      printf("Client specating gameID: %ld\n", clients.at(p).gameID);
                      sprintf(buffer, "Spectating gameID: %ld. Press enter to go back!\n", clients.at(p).gameID);
                      send(clients.at(p).sockID, buffer, strlen(buffer), 0);
                      memset(recvBuffer, 0, sizeof(recvBuffer));
                    }
                  }
                  else if (clients.at(p).spectating == true && clients.at(p).gameID == j)
                  {
                    if (strcmp(recvBuffer, "") == 0)
                    {
                      clients.at(p).gameID = -1;
                      send(clients.at(p).sockID, "Choose a game to watch!\n", strlen("Choose a game to watch!\n"), 0);
                      for (size_t k = 0; k < games.size(); k++)
                      {
                        memset(buffer, 0, sizeof(buffer));
                        sprintf(buffer, "%ld\n", k + 1);
                        printf("sending to spectator\n");
                        send(clients.at(p).sockID, buffer, strlen(buffer), 0);
                      }
                    }
                  }
                  else if (clients.at(p).spectating == true && clients.at(p).gameID == j)
                  {
                  }
                }
                printf("test\n");
                for (size_t g = 0; g < clients.size(); g++)
                {
                  if (clients.at(g).sockID == i && clients.at(g).isInGame == true && clients.at(g).gameID == j)
                  {
                    printf("i is: %d\n", i);
                    if (strcmp(recvBuffer, "1") == 0)
                    {
                      clients.at(g).answer = 1;
                      printf("added 1 to clients score\nClients score is now: %d\n", clients.at(g).answer);
                    }
                    else if (strcmp(recvBuffer, "2") == 0)
                    {
                      clients.at(g).answer = 2;
                      printf("added 2 to clients score\nClients score is now: %d\n", clients.at(g).answer);
                    }
                    else if (strcmp(recvBuffer, "3") == 0)
                    {
                      clients.at(g).answer = 3;
                      printf("added 3 to clients score\nClients score is now: %d\n", clients.at(g).answer);
                    }
                    //memset(recvBuffer, 0, sizeof(recvBuffer));
                    //printf("%d, %d %d\n", i, games.at(j)->player1->sockID, games.at(j)->player2->sockID);
                    printf("player sockid changed is: %d\n", clients.at(g).sockID);
                    /*if (games.at(j)->player1->sockID == clients.at(g).sockID)
                        {
                          printf("player1\n");
                          if ((strcmp(recvBuffer, "1") == 0 || strcmp(recvBuffer, "2") == 0 || strcmp(recvBuffer, "3") == 0))
                          {
                            printf("recvBuffer before check: %s\n", recvBuffer);

                            if (strcmp(recvBuffer, "1") == 0)
                            {
                              games.at(j)->p1Answer = 1;
                              printf("added 1 to p1 answer\n");
                            }
                            else if (strcmp(recvBuffer, "2") == 0)
                            {
                              games.at(j)->p1Answer = 2;
                            }
                            else if (strcmp(recvBuffer, "3") == 0)
                            {
                              games.at(j)->p1Answer = 3;
                            }
                            //gettimeofday(&games.at(j)->player1->tid, NULL);
                          }
                          else
                          {
                            printf("ERROR, Player1 answer is not 0!\n");
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
                            //gettimeofday(&games.at(j)->player2->tid, NULL);
                          }
                        }*/
                  }
                }
              }
            }

            if (recieve <= 0)
            {
              close(i);
              printf("Im now erasing an client\n");
              for (size_t j = 0; j < clients.size(); j++)
              {
                if (i == clients[j].sockID)
                {
                  clients.erase(clients.begin() + j);

                  FD_CLR(i, &currentSockets);
                  break;
                }
              }
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
