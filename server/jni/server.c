#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

pthread_mutex_t count_mutex     = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  cond   = PTHREAD_COND_INITIALIZER;
int flag = 0;
volatile int count = 2;


void *serve(void * newsockfd);
int main(int argc, char *argv[]){
		int sockfd, newsockfd, portno, clilen, n;
		char buffer[256];
		struct sockaddr_in serv_addr, cli_addr;
		sockfd = socket(AF_INET, SOCK_STREAM, 0);
		if(sockfd < 0){
				printf("ERROR opening socket!\n");
				exit(1);
		}
		bzero((char *) &serv_addr, sizeof(serv_addr));
		serv_addr.sin_family = AF_INET;
		portno = 2050;
		serv_addr.sin_port = htons(portno);
		serv_addr.sin_addr.s_addr = INADDR_ANY;
		if(bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
				printf("Error on binding!\n");
				exit(1);
		}
		listen(sockfd, 5);
		clilen = sizeof(cli_addr);
		printf("Server initiating...\n");

		while(1){
			pthread_t thread;
			
			//accept client
			newsockfd = accept(sockfd, (struct sockaddr*) &cli_addr, &clilen);
			if(newsockfd == -1){
					printf("errno is %s\n", strerror(errno));
					exit(1);
			}
			
			//create thread
			if(pthread_create(&thread, NULL, serve, (void*)&newsockfd) != 0){
					fprintf(stderr,"Error - pthread_create() return code: %s\n",strerror(errno));
					exit(EXIT_FAILURE);	
			}

			if(flag == 1) break;
		}
		close(sockfd);
		return 0;
}

void *serve(void *sockfd){
		int newsockfd = (int)(*((int*)sockfd));
		int n = 0, flag = 0;
		char wait_message[] = "From server: Please wait...\n";

		char buffer[256], waiting_buffer[256];
		
		//wait for resources
		if(count < 0){
				printf("ERROR count less than zero!\n");
				exit(1);
		}
		while(count == 0) {
				bzero(waiting_buffer,256);
				n = read(newsockfd, waiting_buffer, 255);
				if(n < 0){
					printf("ERROR reading from socket!\n");
					exit(1);
				}

				if(count > 0) {
					flag = 1;
					break;
				}

				n = write(newsockfd, wait_message,255);
				
		}
		pthread_mutex_lock(&count_mutex);
		count--;
		pthread_mutex_unlock(&count_mutex);

		while(1){
			//read from client
			if(!flag){	
				bzero(buffer, 256);
				n = read(newsockfd, buffer, 255);
				if(n < 0) {
					printf("ERROR reading from socket!\n");
					exit(1);
				}
			}

			else{
					flag = 0;
					strcpy(buffer, waiting_buffer);
			}

			if(strcmp(buffer,":q\n") == 0){
					//release resource
					pthread_mutex_lock(&count_mutex);
					count++;
					pthread_mutex_unlock(&count_mutex);
					
					printf("Server thread closing...\n");
					close(newsockfd);
					pthread_detach(pthread_self());
					return NULL;
			}

			printf("Receiving message: %s", buffer);

			char *ptr = buffer;
		
			//encryption
			while(*ptr != '\0'){
				if(*ptr >= 'a' && *ptr <= 'z'){
						*ptr = (char)((int)((*ptr - 'a' + 3) % 26 + (int)('a')));
				}
		
				else if(*ptr >= 'A' && *ptr <= 'Z'){
						*ptr = (char)((int)((*ptr - 'A' + 3) % 26 + (int)('A')));
				}
				
				ptr++;
			}

			n = write(newsockfd, buffer, 255);
			if(n < 0) {
				printf("ERROR writing from socket");
			}
		}
}
