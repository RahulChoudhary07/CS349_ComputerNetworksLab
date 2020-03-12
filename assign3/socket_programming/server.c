#include "decoder.c"
#include "encoder.c"
#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <arpa/inet.h>
#include <string.h> 

void clientController(int clientfd,struct sockaddr_in *client){
	printf("New client just connected!!\n");
	char *ip = inet_ntoa(client->sin_addr); //ip address of client
	int port = client->sin_port; // port of client
	char buffer[1600]; //buffer to store sending value
	while(1){ //while client doesnt send close connection request, wait for msg from client
		bzero(buffer,1600); 
		int st = read(clientfd,buffer,1501); //read msg from socket
		if(buffer[0]=='3'){ //first char of msg is msg type. 3 means close connection
			printf("\tConnection Closed\n");
			break;
		}
		printf("Message received from client %s:%d\n\tEncoded Message ==>\n\t\t%s\n",ip,port,buffer+1); //print encoded msg
		printf("\tDecoded Message ==>\n\t\t%s\n",decode(buffer+1));
		bzero(buffer,1501);
		buffer[0]='2'; //write to buffer
		strcpy(buffer+1,encode("ACK")); //append encoded value of "ACK" after msg type
		write(clientfd,buffer,strlen(buffer)); //send acknowledgement
	}
	close(clientfd); //close connection
	printf("A client just disconnected!!\n");
	exit(0);
}
int main(int argc, char *argv[]){
	struct sockaddr_in server,client;
	int serverSocket = socket(AF_INET,SOCK_STREAM,0); //tcp socket
	if(serverSocket == -1){ //couldnt create socket
		printf("SOCKET FAILURE\n");
		exit(0);
	}
	server.sin_family = AF_INET;  //code for address family - IPv4
	server.sin_addr.s_addr = INADDR_ANY; //address of host, accepts connection for all values of host
	server.sin_port = htons(atoi(argv[1]));  //arguement 1 is server port no., entered by user
	memset(&server.sin_zero,8, 0); 
	socklen_t length = sizeof(struct sockaddr_in);
	if(bind(serverSocket,(struct sockaddr*) &server, length) < 0){ //if port is already in use binding fails
		printf("BINDING FAILED\n");
		exit(0);
	}
	if(listen(serverSocket,5) == -1){ //listen to this socket
		printf("TOO MANY CLIENTS\n");
		exit(0);
	}
	printf("Server is now on!!\n");
	fflush(stdout);
	while(1){
		int clientSocket = accept(serverSocket,(struct sockaddr *) &client,&length); //client tries to connect
		if(clientSocket < 0){ // this means connection was not made
			printf("SERVER COULDN'T ACCEPT THE CLIENT\n");
			exit(0);
		}
		int status = fork(); //fork to create a child process to handle this client, so that multiple clients can be handled concurrently
		switch(status)
		{
	 		case -1:
	 		 // error while creating child process
				printf("COULDN'T CREATE CONNECTION - TOO MANY CONNECTIONS\n");
				break;
			case 0:
			 //child process
				close(serverSocket); //server socket is handled by parent process
				clientController(clientSocket,&client); // this functions handles a client
				break; 
			default:
			 //parent process
				close(clientSocket);  // client socket is now handled by child process	
		}
		
	}
	return 0;


}
