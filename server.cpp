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
  size_t totalTime;
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
std::vector<size_t> timeScore;

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

  int sockfd, connfd, len;
  struct sockaddr_in client;

  struct addrinfo sa, *si, *p;
  memset(&sa, 0, sizeof(sa));
  sa.ai_family = AF_UNSPEC;
  sa.ai_socktype = SOCK_STREAM;
  sa.ai_flags = AI_PASSIVE;

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

  if (p == NULL)
  {
    printf("NULL\n");
    exit(0);
  }

  freeaddrinfo(si);

  if (listen(sockfd, 1) == -1)
  {
    printf("Error: Listen failed!\n");
    exit(0);
  }

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
  struct timeval timeChecker;
  timeChecker.tv_sec = 0;
  gameTimer.tv_usec = 0;

  while (true)
  {
    gettimeofday(&gameTimer, NULL);
    gettimeofday(&timeChecker, NULL);
    readySockets = currentSockets;
    if (games.size() > 0)
    {

      currentClient = -1;
      for (size_t i = 0; i < games.size(); i++)
      {
        size_t number = (gameTimer.tv_sec - games.at(i)->tid.tv_sec);
        for (size_t j = 0; j < clients.size(); j++)
        {
          for (size_t g = 0; g < clients.size(); g++)
          {
            if (clients.at(j).sockID != clients.at(g).sockID && clients.at(j).isInGame == true && clients.at(g).isInGame == true)
            {
              if (clients.at(j).gameID == i && clients.at(g).gameID == i)
              {
                currentClient = j;
                if (clients.at(j).answer != 0 && clients.at(g).answer != 0)
                {
                  for (size_t k = 0; k < clients.size(); k++)
                  {
                    if (clients.at(k).spectating == true && clients.at(k).gameID == i)
                    {
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
                  clients.at(j).totalTime = clients.at(j).totalTime + (gameTimer.tv_sec - games.at(i)->tid.tv_sec);
                  clients.at(g).totalTime = clients.at(g).totalTime + (gameTimer.tv_sec - games.at(i)->tid.tv_sec);

                  for (size_t k = 0; k < clients.size(); k++)
                  {
                    if (clients.at(k).spectating == true && clients.at(k).gameID == i)
                    {
                      memset(buffer, 0, sizeof(buffer));
                      sprintf(buffer, "Score %d - %d\n\n", clients.at(j).score, clients.at(g).score);
                      send(clients.at(k).sockID, buffer, strlen(buffer), 0);
                    }
                  }
                  if (clients.at(j).score < 3 && clients.at(g).score < 3)
                  {
                    gettimeofday(&games.at(i)->tid, NULL);
                  }
                  else
                  {
                    if (clients.at(j).score == 3)
                    {
                      int totalRounds = 0;
                      size_t averageTime = 0;
                      totalRounds = (clients.at(j).score + clients.at(g).score);
                      averageTime = (clients.at(j).totalTime / totalRounds);
                      send(clients.at(j).sockID, "You Won!\n", strlen("You Won!\n"), 0);
                      send(clients.at(g).sockID, "You Lost!\n", strlen("You Lost!\n"), 0);
                      timeScore.push_back(averageTime);
                    }
                    else if (clients.at(g).score == 3)
                    {
                      int totalRounds = 0;
                      size_t averageTime = 0;
                      totalRounds = (clients.at(j).score + clients.at(g).score);
                      averageTime = (clients.at(g).totalTime / totalRounds);
                      send(clients.at(g).sockID, "You Won!\n", strlen("You Won!\n"), 0);
                      send(clients.at(j).sockID, "You Lost!\n", strlen("You Lost!\n"), 0);
                      timeScore.push_back(averageTime);
                    }
                    else
                    {
                      break;
                    }
                    clients.at(j).totalTime = 0;
                    clients.at(g).totalTime = 0;
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
                          send(clients.at(k).sockID, "Choose a game to watch or press 'Enter' to leave!\n", strlen("Choose a game to watch or press 'Enter' to leave!\n"), 0);
                          for (size_t h = 0; h < games.size(); h++)
                          {
                            memset(buffer, 0, sizeof(buffer));
                            sprintf(buffer, "%ld\n", j + 1);
                            send(clients.at(k).sockID, buffer, strlen(buffer), 0);
                          }
                          if (strcmp(recvBuffer, "") == 0)
                          {
                            clients.at(g).spectating = false;
                            send(clients.at(g).sockID, MENU, strlen(MENU), 0);
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
                else if ((gameTimer.tv_sec - clients.at(j).tid.tv_sec) == 3 && clients.at(j).answer == 0 && (clients.at(g).answer == 1 || clients.at(g).answer == 2 || clients.at(g).answer == 3))
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
                  clients.at(j).totalTime = clients.at(j).totalTime + (gameTimer.tv_sec - games.at(i)->tid.tv_sec);
                  clients.at(g).totalTime = clients.at(g).totalTime + (gameTimer.tv_sec - games.at(i)->tid.tv_sec);
                  for (size_t k = 0; k < clients.size(); k++)
                  {
                    if (clients.at(k).spectating == true && clients.at(k).gameID == i)
                    {
                      send(clients.at(k).sockID, buffer, strlen(buffer), 0);
                    }
                  }
                  gettimeofday(&games.at(i)->tid, NULL);
                  gettimeofday(&clients.at(j).tid, NULL);
                  gettimeofday(&clients.at(g).tid, NULL);
                  if (clients.at(j).score == 3)
                  {
                    int totalRounds = 0;
                    size_t averageTime = 0;
                    totalRounds = (clients.at(j).score + clients.at(g).score);
                    averageTime = (clients.at(j).totalTime / totalRounds);
                    send(clients.at(j).sockID, "You Won!\n", strlen("You Won!\n"), 0);
                    send(clients.at(g).sockID, "You Lost!\n", strlen("You Lost!\n"), 0);
                    timeScore.push_back(averageTime);
                  }
                  else if (clients.at(g).score == 3)
                  {
                    int totalRounds = 0;
                    size_t averageTime = 0;
                    totalRounds = (clients.at(j).score + clients.at(g).score);
                    averageTime = (clients.at(g).totalTime / totalRounds);
                    send(clients.at(g).sockID, "You Won!\n", strlen("You Won!\n"), 0);
                    send(clients.at(j).sockID, "You Lost!\n", strlen("You Lost!\n"), 0);
                    timeScore.push_back(averageTime);
                  }
                  else
                  {
                    break;
                  }
                  clients.at(j).totalTime = 0;
                  clients.at(g).totalTime = 0;
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
                        clients.at(g).gameID = -1;
                        send(clients.at(k).sockID, "Choose a game to watch or press 'Enter' to leave!\n", strlen("Choose a game to watch or press 'Enter' to leave!\n"), 0);
                        for (size_t h = 0; h < games.size(); h++)
                        {
                          memset(buffer, 0, sizeof(buffer));
                          sprintf(buffer, "%ld\n", h + 1);
                          send(clients.at(k).sockID, buffer, strlen(buffer), 0);
                          memset(recvBuffer, 0, sizeof(recvBuffer));
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
                else if (((gameTimer.tv_sec - clients.at(j).tid.tv_sec) == 3) && clients.at(j).answer == 0 && clients.at(g).answer == 0)
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
            if (clients.at(j).gameID == i && clients.at(g).gameID == i && clients.at(j).sockID != clients.at(g).sockID && clients.at(j).isInGame == true && clients.at(g).isInGame == true && ((gameTimer.tv_sec - clients.at(j).tid.tv_sec) == 1))
            {

              if (number >= 0 && number < 5)
              {
                gettimeofday(&clients.at(j).tid, NULL);
                gettimeofday(&clients.at(g).tid, NULL);
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

                if (clients.at(j).isInGame == true && clients.at(j).spectating == false && clients.at(g).spectating == false)
                {
                  send(clients.at(j).sockID, buffer, strlen(buffer), 0);
                  send(clients.at(g).sockID, buffer, strlen(buffer), 0);
                  for (size_t k = 0; k < clients.size(); k++)
                  {
                    if (clients.at(k).spectating == true && clients.at(k).gameID == i)
                    {
                      send(clients.at(k).sockID, buffer, strlen(buffer), 0);
                    }
                  }
                }
              }
            }
          }
        }
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
            newClient.totalTime = 0;
            gettimeofday(&newClient.tid, NULL);
            newClient.sockID = connfd;
            FD_SET(newClient.sockID, &currentSockets);
            clients.push_back(newClient);
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

          if (clients.size() > 0)
          {
            if (recieve > 0)
            {
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
                  if (queue.size() < 2)
                  {
                    send(clients.at(currentClient).sockID, "Need another player to start the game\nPress 'Enter' to leave queue\n", strlen("Need another player to start the game\nPress 'Enter' to leave queue\n"), 0);
                  }
                  else if (queue.size() >= 2)
                  {

                    if (queue.at(0)->sockID == i || queue.at(1)->sockID == i)
                    {
                      for (size_t j = 0; j < queue.size(); j++)
                      {
                        send(queue.at(j)->sockID, "A game is ready, press 'r' to accept!\n", strlen("A game is ready, press 'r' to accept!\n"), 0);
                      }
                    }
                  }
                }
                else if (currentClient > -1 && clients.at(currentClient).isInGame == false && clients.at(currentClient).isInQueue == false && clients.at(currentClient).isReady == false && clients.at(currentClient).spectating == false)
                {
                  send(clients.at(currentClient).sockID, "Queue is full, try again\n", strlen("Queue is full, try again\n"), 0);
                  send(clients.at(currentClient).sockID, MENU, strlen(MENU), 0);
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
                if (currentClient > -1 && clients.at(currentClient).isInGame == false && clients.at(currentClient).isInQueue == false && clients.at(currentClient).isReady == false && clients.at(currentClient).spectating == false)
                {
                  if (games.size() > 0)
                  {
                    send(clients.at(currentClient).sockID, "Choose a game to watch or press 'Enter' to leave!\n", strlen("Choose a game to watch or press 'Enter' to leave!\n"), 0);
                    for (size_t j = 0; j < games.size(); j++)
                    {
                      clients.at(currentClient).spectating = true;
                      memset(buffer, 0, sizeof(buffer));
                      sprintf(buffer, "%ld\n", j + 1);
                      send(clients.at(currentClient).sockID, buffer, strlen(buffer), 0);
                      memset(recvBuffer, 0, sizeof(recvBuffer));
                      sprintf(recvBuffer, "-2");
                    }
                  }
                  else
                  {
                    send(clients.at(currentClient).sockID, "No games active. Try again later!\n", strlen("No games active. Try again later!\n"), 0);
                    send(clients.at(currentClient).sockID, MENU, strlen(MENU), 0);
                    clients.at(currentClient).spectating = false;
                    clients.at(currentClient).gameID = -1;
                  }
                }
              }
              else if (strcmp(recvBuffer, "3") == 0)
              {
                currentClient = -1;
                for (size_t j = 0; j < clients.size() && currentClient == -1; j++)
                {
                  if (clients.at(j).sockID == i)
                  {
                    currentClient = j;
                  }
                }
                if (timeScore.size() > 0 && clients.at(currentClient).isInGame == false && clients.at(currentClient).isInQueue == false && clients.at(currentClient).isReady == false)
                {
                  for (size_t j = 0; j < timeScore.size(); j++)
                  {
                    for (size_t g = 0; g < timeScore.size(); g++)
                    {
                      if (timeScore.at(g) > timeScore.at(j))
                      {
                        size_t copy = timeScore.at(g);

                        timeScore.at(g) = timeScore.at(j);
                        timeScore.at(j) = copy;
                      }
                    }
                  }
                  for (size_t j = 0; j < timeScore.size(); j++)
                  {
                    memset(buffer, 0, sizeof(buffer));
                    sprintf(buffer, "Winners average time: %ld\n", timeScore.at(j));
                    send(clients.at(currentClient).sockID, buffer, strlen(buffer), 0);
                  }
                  send(clients.at(currentClient).sockID, MENU, strlen(MENU), 0);
                }
                else if (timeScore.size() == 0 && clients.at(currentClient).isInGame == false && clients.at(currentClient).isInQueue == false && clients.at(currentClient).isReady == false)
                {
                  send(clients.at(currentClient).sockID, "No times avaible, try again later!\n", strlen("No times avaible, try again later!\n"), 0);
                  send(clients.at(currentClient).sockID, MENU, strlen(MENU), 0);
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

                  if (currentClient > -1 && queue.at(currentClient)->isInGame == false && queue.at(currentClient)->isInQueue == true && queue.at(currentClient)->isReady == false && clients.at(currentClient).spectating == false)
                  {
                    queue.at(currentClient)->isInQueue = false;
                    send(queue.at(currentClient)->sockID, MENU, strlen(MENU), 0);
                    queue.erase(queue.begin() + currentClient);
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
                      }
                      else if (clients.at(j).sockID == queue.at(1)->sockID)
                      {
                        clients.at(j).gameID = games.size() - 1;
                        gettimeofday(&clients.at(j).tid, NULL);
                      }
                    }

                    for (int i = 0; i < 2; i++)
                    {
                      queue.erase(queue.begin());
                    }
                    if (games.size() > 0)
                    {
                      for (size_t j = 0; j < games.size(); j++)
                      {
                        if (games.at(j)->player1->isReady == true && games.at(j)->player2->isReady == true)
                        {
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
                        }
                      }
                    }
                  }
                }
              }
              else if (strcmp(recvBuffer, "1") == 0 || strcmp(recvBuffer, "2") == 0 || strcmp(recvBuffer, "3") == 0)
              {
              }
              else
              {
                send(i, "ERROR Wrong format on the message\n", strlen("ERORR Wrong format on the message\n"), 0);
              }
            }
            if (games.size() > 0)
            {

              currentClient = -1;
              for (size_t j = 0; j < clients.size() && currentClient == -1; j++)
              {
                if (clients.at(j).sockID == i)
                {
                  currentClient = j;
                }
              }
              char checking[256];
              size_t checker = -1;
              for (size_t j = 0; j < games.size(); j++)
              {
                for (size_t g = 0; g < clients.size(); g++)
                {
                  if (clients.at(g).sockID == i)
                  {
                    currentClient = g;
                  }
                  for (size_t b = 0; b < games.size(); b++)
                  {
                    if (clients.at(currentClient).spectating == true && clients.at(currentClient).gameID == checker && clients.at(currentClient).sockID == i && clients.at(currentClient).isInGame == false)
                    {

                      if (strcmp(recvBuffer, "") == 0)
                      {
                        clients.at(currentClient).spectating = false;
                        send(clients.at(currentClient).sockID, MENU, strlen(MENU), 0);
                        memset(recvBuffer, 0, sizeof(recvBuffer));
                        sprintf(recvBuffer, "-2");
                      }
                      snprintf(checking, sizeof(checking), "%zu", j + 1);
                      if (strcmp(recvBuffer, checking) == 0)
                      {
                        clients.at(currentClient).gameID = j;
                        sprintf(buffer, "Spectating gameID: %ld. Press enter to go back!\n", clients.at(currentClient).gameID);
                        send(clients.at(currentClient).sockID, buffer, strlen(buffer), 0);
                        memset(recvBuffer, 0, sizeof(recvBuffer));
                        sprintf(recvBuffer, "-2");
                      }
                    }
                    else if (clients.at(currentClient).spectating == true && clients.at(currentClient).gameID == b && clients.at(currentClient).sockID == i)
                    {
                      if (strcmp(recvBuffer, "") == 0 || (recieve > 0 && strcmp(recvBuffer, "-2") != 0))
                      {
                        clients.at(currentClient).gameID = -1;
                        send(clients.at(currentClient).sockID, "Choose a game to watch or press 'Enter' to leave!\n", strlen("Choose a game to watch or press 'Enter' to leave!\n"), 0);
                        for (size_t k = 0; k < games.size(); k++)
                        {
                          memset(buffer, 0, sizeof(buffer));
                          sprintf(buffer, "%ld\n", k + 1);
                          send(clients.at(currentClient).sockID, buffer, strlen(buffer), 0);
                          memset(recvBuffer, 0, sizeof(recvBuffer));
                          sprintf(recvBuffer, "-2");
                        }
                      }
                    }
                  }
                  if (clients.at(g).spectating == true)
                  {
                    break;
                  }
                  else if ((gameTimer.tv_sec - clients.at(g).tid.tv_sec) < 3 && ((gameTimer.tv_sec - games.at(j)->tid.tv_sec) > 3) && clients.at(g).sockID == i && clients.at(g).isInGame == true && clients.at(g).gameID == j)
                  {
                    if (strcmp(recvBuffer, "1") == 0)
                    {
                      clients.at(g).answer = 1;
                      gettimeofday(&clients.at(g).tid, NULL);
                    }
                    else if (strcmp(recvBuffer, "2") == 0)
                    {
                      clients.at(g).answer = 2;
                      gettimeofday(&clients.at(g).tid, NULL);
                    }
                    else if (strcmp(recvBuffer, "3") == 0)
                    {
                      clients.at(g).answer = 3;
                      gettimeofday(&clients.at(g).tid, NULL);
                    }
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
