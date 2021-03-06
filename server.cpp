/*
* Course: CS 100 Fall 2013
*
* First Name: Sultan
* Last Name: Khan
* Username: skhan024
* email address: skhan024@ucr.edu
*
*
* Assignment: 9
*
* I hereby certify that the contents of this file represent
* my own original individual work. Nowhere herein is there
* code from any outside resources such as another individual,
* a website, or publishings unless specifically designated as
* permissible by the instructor or TA. */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdlib.h>
#include <iostream>
#include <sys/wait.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
using namespace std;

///////////////FUNCTIONS////////////////////////////
void run();
void getfiles( int client_sockfd, string & dir);
void upload( int fd, int client_sockfd, int filesize );
////////////////////////////////////////////////////
//int counter = 0;

int main()
{
    run();
}

void run()
{
  int server_sockfd, client_sockfd;
  int server_len, client_len;
  struct sockaddr_in server_address;
  struct sockaddr_in client_address;

  // create socket
  server_sockfd = socket(AF_INET, SOCK_STREAM, 0);

  // Name the socket
  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = htonl(INADDR_ANY);
  server_address.sin_port = htons(47478);
  server_len = sizeof(server_address);
  bind(server_sockfd, (struct sockaddr *)&server_address, server_len);

  // create the connection queue
  listen(server_sockfd, 10);

  for(int i=0; i<10;i++)
    {
      cout << "Server waiting for path" << endl;
      // wait for client filepath
      client_len = sizeof(client_address);
      client_sockfd = accept(server_sockfd, (struct sockaddr *)&client_address, (socklen_t*)&client_len );
      
      int id = fork();
      if(id == -1)
	{
	  cout << "Fork fail" << endl;
	  exit(1);
	}
      else if(id == 0) // CHILD
	{
	 
	  cout << "Client acepted" << endl;
	  
	  // read the filepath
	  char ch[100];
	  int temp = read(client_sockfd, ch, 100);
	  usleep(300000);
	  
	  // pass filepath to function 
	  string dir(ch);
	  memset(ch, 0, 100);
	  getfiles(client_sockfd, dir);
	}
      // PARENT just continues
    }
  // Close file descriptors
  close(client_sockfd);
  close(server_sockfd);
}

void getfiles( int client_sockfd, string & dir)
{
  DIR *dp;
  int fd; 
  string filepath;
  struct dirent *dirp;
  struct stat filestat;
  
  // open directory from given path
  if ((dp = opendir( dir.c_str() )) == NULL) { cout << "Server Error opendir" <<  endl; exit(1); }
  
  while ((dirp = readdir( dp )))
    {
      // Concatonate to make complete filepath ex: bin/rm
      filepath = dir + "/" + dirp->d_name;
      
      stat(filepath.c_str(), &filestat);
      
      // If filepath exists and is not a directory
      if(filestat.st_mode & S_IFREG)
	{
	  cout << filepath << endl;
	  // Send file name to client
	  cout << "File is " << dirp->d_name << endl;
	  write(client_sockfd, dirp->d_name, 100);
	  usleep(30000);
	  
	  // Open the filepath
	  if( (fd = open(filepath.c_str(), O_RDONLY)) == -1) { cout << "S Error fd" << endl; exit(1); }
	  
	  // Uplaod file content and get size
	  
	  //  upload(fd, client_sockfd, filestat.st_size);
	  close(fd);
	  cout << "File Uploaded" << endl;
	}
      usleep(30000);
    }
}

void upload( int fd, int client_sockfd,  int filesize )
{
  int size;
  bool last = false;
  char buffer[filesize];
  // while(!last)
//     {
//       // If buffer is smaller, then write and break out loop
//       if( (size = read(fd, buffer, 1000000)) < 1000000 )
// 	{
// 	  last = true;
// 	  cout << "Server last is true, buffsize = "<< size << endl;
// 	}
      
//       write(client_sockfd, buffer, size);
//       // usleep(30000);
//       memset(buffer, 0, 1000000);
//     }
  
 //  write(client_sockfd, "OMGPLEASEWORK", 13);
  read(fd, buffer, filesize);
  usleep(30000);
  write(client_sockfd, &filesize, sizeof(filesize));
  usleep(30000);
  write(client_sockfd, buffer, filesize);
  usleep(30000);
}
