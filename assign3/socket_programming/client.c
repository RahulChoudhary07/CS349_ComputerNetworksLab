#include "decoder.c"
#include "encoder.c"
#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <arpa/inet.h>
#include <string.h> 

#define sckt struct sockaddr_in

int main(int argc, char *argv[]){

	sckt server; //struct is part of socket.c
	char response; 
	int clientSocket = socket(AF_INET,SOCK_STREAM,0); //AF_NET is IPv4, SOCK_STREAM means TCP - socket is created
	char buffer[1501]; //buffer to store sending value
	if(clientSocket == -1){ //socket couldn't be created
		printf("SOCKET FAILURE\n");
		exit(0);
	} 

	server.sin_family = AF_INET; //address family - ipv4
	if(inet_aton(argv[1],&server.sin_addr)==0){ //server ip address, inputted by user 
		printf("SERVER IP ADDRESS ERROR\n");
		exit(0);
	} 

	int serverPort = atoi(argv[2]); // server port, inputted by user
	server.sin_port = htons(serverPort); 
	socklen_t length = sizeof(sckt); 
	if(connect(clientSocket,(struct sockaddr *) &server, length) == -1){ // connection establishment
		printf("COULDN'T CONNECT TO SERVER\n");
		exit(0);
	} 

	while(1){
		
		printf("Do you want to send message? - enter 'y' else 'n' and press enter: \n");
		scanf("%c", &response);

		if(response == 'n'){ //user enters n so send close connections request to server
			char* encodedMessage = encode("connection_close");  //encode
			buffer[0] =  '3'; 	 //msg type as 3 - means close connection
			strcpy(buffer+1, encodedMessage); // append encoded value of "connection_close" after msg type
			write(clientSocket, buffer, strlen(buffer)); // write in socket
			printf("Connection Closed\n"); 
			break; 
		}
		//scanf("%c", &response);              
		getchar();
		printf("What message do you want to send?\n");
		//bzero(msg, 1500);
		bzero(buffer, 1501); 
		int i = 0;
		for(;;){ //maximum buffer size is 1500
			scanf("%c", &response);        //input msg      
			if(response == '\n') 
				break;
			if (i == 1500){
				printf("Only first 1500 characters of message are being sent\n");
				break;
			}
			buffer[i++] = response; 
		}
		strcpy(buffer+1, encode(buffer)); //// append encoded value of msg after msg typer
		buffer[0] =  '1'; //set msg type as 1
		write(clientSocket, buffer, strlen(buffer)); //write to socket
		bzero(buffer, 1501);
		int received = read(clientSocket, buffer, 50); //recieve acknowledgement from server
		strcpy(buffer+1, decode(buffer+1)); //decode msg

		if(buffer[0]!='2'){ //msg type 2 means acknowledgement
			printf("Acknowledgement not received !!\nResend the message !!\n"); 
			continue; 
		}

		
		printf("Message received from server with port %d and IP %s\n",serverPort, argv[1]); 
		printf("%s\n", buffer+1);
	}

	close(clientSocket); // close the socket

return 0;
}
