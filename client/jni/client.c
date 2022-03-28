#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

/*
 * Create a client source
 * Connect to the server
 * Sent message and wait for encryption
 * Print the encrytion
 *
 * Created by wic 3/23/2022
*/

int main(int argc, char *argv[]){
		int sockfd, portno, n;
		struct sockaddr_in serv_addr;
		struct hostent *server;
		char buffer[256];

		portno = 2050;
		sockfd = socket(AF_INET, SOCK_STREAM, 0);
		if(sockfd < 0){
				printf("ERROR opening socket");
				exit(1);
		}
		server = gethostbyname("127.0.0.1");
		if(server == NULL){
				printf("ERROR, no such host!\n");
				exit(1);
		}
		bzero((char*)&serv_addr, sizeof(serv_addr));
		serv_addr.sin_family = AF_INET;
		bcopy((char*)server->h_addr, (char*)&serv_addr.sin_addr.s_addr, server->h_length);
		serv_addr.sin_port = htons(portno);
		
		// connect to the server
		if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
			printf("ERROR connecting");
			exit(1);
		}

		printf("Please enter the message:\n");
		bzero(buffer,256);
		
		while(1){
			bzero(buffer,256);
			fgets(buffer, 255, stdin);


			n = write(sockfd, buffer, strlen(buffer));
			if(n < 0){
				printf("ERROW writing to the socket!");
				exit(1);
			}
			
			// check if the user want to quit
			if(strcmp(buffer, ":q\n") == 0){
					printf("Clinet closing...\n");
					close(sockfd);
					return 0;
			}


			n = read(sockfd, buffer,255);
			
			if(n < 0){
				printf("ERROW reading from the socket!");
				exit(1);
			}
			printf("From server: %s", buffer);
		}
		return 0;
}
