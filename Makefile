a4p1Server: a4p1Server.c
	gcc -pthread a4p1Server.c -o a4p1Server
a4p1Client: a4p1Client.c
	gcc a4p1Client.c -o a4p1Client
a4p2Server: a4p2Server.c
	gcc -pthread a4p2Server.c -o a4p2Server
a4p2Client: a4p2Client.c
	gcc -pthread a4p2Client.c -o a4p2Client
