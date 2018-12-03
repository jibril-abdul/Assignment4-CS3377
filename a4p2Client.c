#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>


#define MAXLINE 4096 /*max text line length*/
// #define SERV_PORT 3000 /*port*/

int main(int argc, char **argv)
{
 int sockfd;
 struct sockaddr_in servaddr;
 char sendline[MAXLINE], recvline[MAXLINE];


 //basic check of the arguments
 //additional checks can be inserted
 if (argc < 2) {
  perror("Usage: TCPClient <Server IP> <Server Port>");
  exit(1);
 }

 //Create a socket for the client
 //If sockfd<0 there was an error in the creation of the socket
 if ((sockfd = socket (AF_INET, SOCK_STREAM, 0)) <0) {
  perror("Problem in creating the socket");
  exit(2);
 }

 //Creation of the socket
 memset(&servaddr, 0, sizeof(servaddr));
 servaddr.sin_family = AF_INET;
 servaddr.sin_addr.s_addr = inet_addr(argv[1]);
 //servaddr.sin_addr.s_addr= inet_aton((int) strtol(argv[1], (char **)NULL, 10), );
// inet_aton((int) strol(argv[1], (char **)NULL, 10), *(servaddr.sin_addr.s_addr));
 servaddr.sin_port =  htons((int) strtol(argv[2], (char **)NULL, 10));
 //servaddr.sin_port =  htons(SERV_PORT); //convert to big-endian order

 //Connection of the client to the socket
 if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr))<0) {
  perror("Problem in connecting to the server");
  exit(3);
 }

 //open output File
 int fileToWrite = open(argv[3], O_RDWR | O_CREAT | O_APPEND);

 while (fgets(sendline, MAXLINE, stdin) != NULL) {

  send(sockfd, sendline, strlen(sendline), 0);

  if (recv(sockfd, recvline, MAXLINE,0) == 0){
   //error: server terminated prematurely
   perror("The server terminated prematurely");
   exit(4);
  }



  //print the header hostname and date
  /**** CLIENT CONSOLE OUTPUT ***/
        //Client should console.log
          //1.  Hostname  //2.  Date  //3. Result from server
  system("hostname;date");
  printf("_ _ _ _ _ _ _ _ _ _ _ _ _\n");
  printf("Output of transaction:\n");
  system(recvline);
  printf("\n");

/**** CLIENT FILE OUTPUT ***/
    //Client should console.log
      //1.  Hostname  //2.  Date  //3. Result from server
      //4. Transaction request made from client
//change output to file
int saveSTDOUT = dup(1);
dup2(fileToWrite, 1);

  system("hostname;date");
  printf("_ _ _ _ _ _ _ _ _ _ _ _ _\n");
  printf("Output of transaction:\n");
  system(recvline);
  printf("\n");

  // return stdout to proper place
  dup2(saveSTDOUT, 1);

 }
 //close file
 close(fileToWrite);

 exit(0);
}
