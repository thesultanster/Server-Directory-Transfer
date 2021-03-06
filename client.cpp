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
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <netdb.h>
#include <sys/wait.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string>
#include <pthread.h>
#include <string.h>
#include <sstream>
using namespace std;

pthread_mutex_t mutex;
int counter = 0; 

//FUNCTIONS///////////////////////////////////////
void* run(void *args);
string writepath(int & sockfd, char* argv);
void create(int sockfd, string & dirname);
void download( int fd, int sockfd );
//////////////////////////////////////////////////

////STRUCT
struct info
{
  string path;
  string argument;
};
//////////


////MAIN
// da main
int main( int argc, char* argv[])
{
  int res; 
  pthread_t a_thread;
  
  if(argc < 3)
    {
      cout << "Too Few Arguments, needs address and filepath(s)" << endl;
      exit(1);
    }

  // populate structure
  info myinfo;
  string mytemp(argv[1]);
  myinfo.argument = mytemp;
  string mytemp2(argv[2]);
  myinfo.path = mytemp2;

  // call 10 threads
  for(int i = 0; i < 10; i++)
    if( (res = pthread_create(&a_thread, NULL, run, &myinfo)) == -1) {cout << "pthreadcreate fail" << endl; return;}
  
  sleep(1);
  cout << "end" << endl;
}


///RUN
//
void *run(void* args)
{
  info mystruct = *(info *)args;
  
  //get count
  pthread_mutex_lock(&mutex);
  counter++;
  
  pthread_mutex_unlock(&mutex);
  
  int sockfd;
  int len;
  int result;
  struct sockaddr_in address;
  struct hostent *hostinfo; 

  
  pthread_mutex_lock(&mutex);
  if((hostinfo = gethostbyname(mystruct.argument.c_str())) == NULL)
    {
      cerr << " Error gethostbyname()" << endl;
      exit(1);
    }
  // pthread_mutex_unlock(&mutex);

  //creat socket name for client
  sockfd = socket(AF_INET, SOCK_STREAM, 0);

  // Name the socket
  address.sin_family = AF_INET;
  address.sin_addr = *((struct in_addr *)hostinfo->h_addr);
  address.sin_port = htons(47478);
  len =  sizeof(address);

  //connect to server socket
  result = connect(sockfd, (struct sockaddr *)&address, len);
  if(result == -1)
    {
      cout << "Client Connection Error " << endl;
      exit(1);
    }

  // write filepath to server and return parsed string
  string dirname = writepath(sockfd, (char *)mystruct.path.c_str());
  usleep(300000);

  // create the directory. name + number
  stringstream sstm;
  sstm << dirname.c_str() << counter;
  string daname = sstm.str();
  
  //pthread_mutex_lock(&mutex);
  system(("mkdir " + daname).c_str());
  pthread_mutex_unlock(&mutex);

  // Get name, open files, add content
  create(sockfd,dirname);
  close(sockfd);
}

////WRITEPATH
// write filepath to server and return parsed string
string writepath(int & sockfd, char* argv)
{
  // write the filepath to server 
  write(sockfd, argv, 100);
  
  //parse "/" out to return clean directory file
  string dirname(argv);
  if(dirname.find("/") != string::npos) 
    dirname = dirname.substr(dirname.rfind("/")+1);

  return dirname; 
}

////CREATE
// Get name, open files, add content
void create(int sockfd, string & dirname)
{
  int size;
  char name[100];
  memset(name, 0, 100);
  
  // Iterate until name reads nothing
  while( (size = read(sockfd, name, 100)) != 0)
    {
      //cout << "Client Name recieved, size " << size << endl;
      usleep(30000);
      
      int fd;
      string path = dirname + "/" + name;
    
      cout << "Child found filepath: " << dirname + "/" +name << endl;

      // Get file's file descriptor, open it and set permissions
      if( (fd = open((char *)path.c_str(),  O_CREAT, 00700) ) == -1 ) { cout << "C Error fd" << endl; return; }
      
      // Download file content
      // download(fd, sockfd);
      close(fd);
      memset(name, 0, 100);
      //cout << "File Download" << endl;
      usleep(30000);
    }
}

////DOWNLOAD
// Download file through packages
void download( int fd, int sockfd )
{
  int size;
  read(sockfd, &size, sizeof(size));
  char buffer[size];
  memset(buffer, 0, size);
  
  //usleep(30000);
  // while( (size = read(sockfd, buffer, 1000000)) != 13 && (strcmp(buffer,"OMGPLEASEWORK")) != 0)
//     {
      
//       if(size == 100)
// 	    cout <<"client recieved buff: " <<buffer << endl;
//       else
// 	    cout << "client buff size: "<<size << endl; 
//       write(fd, buffer, 1000000);
//       memset(buffer, 0, 1000000);
//     }
  
  read(sockfd, buffer, size);
  usleep(30000);
  write(fd, buffer, size);
  cout << "File copied" << endl;
  usleep(30000);
}
