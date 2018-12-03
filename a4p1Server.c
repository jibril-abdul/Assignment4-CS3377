#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

struct serverParm {
           int connectionDesc;
};

//global pointer variable for command line arguments
char ** myArgs;

void *serverThread(void *parmPtr) {

#define PARMPTR ((struct serverParm *) parmPtr)
    int recievedMsgLen;
    char messageBuf[1025];

    /* Server thread code to deal with message processing */
    printf("DEBUG: connection made, connectionDesc=%d\n",
            PARMPTR->connectionDesc);
    if (PARMPTR->connectionDesc < 0) {
        printf("Accept failed\n");
        return(0);    /* Exit thread */
    }

    //open a file to write to fileName is given by 2nd
    // argument passed when calling Server
    int fileToWrite = open(myArgs[2], O_RDWR | O_CREAT | O_APPEND);

    //store file in string in order to concatenate
    char fileStr[13] = " > Result.txt";
    char command[1024];

    /* Receive messages from sender... */
    while ((recievedMsgLen=
            read(PARMPTR->connectionDesc,messageBuf,sizeof(messageBuf)-1)) > 0)
    {
      recievedMsgLen[messageBuf] = '\0';

      // If usr typed in 'exit' then exit from shell
      if(strstr(messageBuf, "exit") != NULL) {
        exit(0);
      }

/**** SERVER CONSOLE OUTPUT ***/
      //Server should console.log
        //1.  Hostname  //2.  date  //3.  Thread ID
        //4.  Server PID - should be the parent
        //5.  Transaction from the client (messageBuf)
        //6.  Output of transaction (system(messageBuf))
        system("hostname");
        system("date");
        printf("Thread ID: %d\n", (int)pthread_self());
        printf("Server PID: %d\n", (int)getpid());
        printf("Transaction received from Client: %s\n", messageBuf);
        printf("Output of Transaction:\n");
        system(messageBuf);

/**** SERVER LOGFILE OUTPUT ***/
    //change the output of stdout using dup
      int saveSTDOUT = dup(1);
      dup2(fileToWrite, 1);

      //Server should output to the file
        //1.  Hostname //2.  date //3.  Thread ID
        //4.  Server PID - should be the parent
        //5.  Transaction from the client (messageBuf)
        //6.  Output of transaction (system(messageBuf))
        system("hostname");
        system("date");
        printf("Thread ID: %d\n", (int)pthread_self());
        printf("Server PID: %d\n", (int)getpid());
        printf("Transaction received from Client: %s\n", messageBuf);
        printf("Output of Transaction:\n");
        system(messageBuf);

    // return stdout to proper place
    dup2(saveSTDOUT, 1);

/**** SENDING OUTPUT FROM SERVER TO CLIENT ***/

        //Return the messageBuf input by user
        if (write(PARMPTR->connectionDesc, messageBuf, sizeof(messageBuf)-1) < 0) {
               perror("Server: write error");
               return(0);
           }
    }
    close(PARMPTR->connectionDesc);  /* Avoid descriptor leaks */
    free(PARMPTR);                   /* And memory leaks */
    return(0);                       /* Exit thread */
}

/*** MAIN ***/
int main (int argc, char **argv) {
    int listenDesc;
    struct sockaddr_in myAddr;
    struct serverParm *parmPtr;
    int connectionDesc;
    pthread_t threadID;
    myArgs = argv;

    /* Create socket from which to read */
    if ((listenDesc = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("open error on socket");
        exit(1);
    }

    /* Create "name" of socket */
    myAddr.sin_family = AF_INET;
    myAddr.sin_addr.s_addr = INADDR_ANY;

    //Server is taking cli arg[1] for port number
    myAddr.sin_port = htons((int) strtol(argv[1], (char **)NULL, 10));

    if (bind(listenDesc, (struct sockaddr *) &myAddr, sizeof(myAddr)) < 0) {
        perror("bind error");
        exit(1);
    }

    /* Start accepting connections.... */
    /* Up to 5 requests for connections can be queued... */
    listen(listenDesc,5);

    while (1) /* Do forever */ {
        /* Wait for a client connection */
        connectionDesc = accept(listenDesc, NULL, NULL);

        /* Create a thread to actually handle this client */
        parmPtr = (struct serverParm *)malloc(sizeof(struct serverParm));
        parmPtr->connectionDesc = connectionDesc;
        if (pthread_create(&threadID, NULL, serverThread, (void *)parmPtr)
              != 0) {
            perror("Thread create error");
            close(connectionDesc);
            close(listenDesc);
            exit(1);
        }

        printf("Parent ready for another connection\n");
    }

}
