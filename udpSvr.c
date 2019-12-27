#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>   /* inet(3) functions */
#include <time.h>
#include <netinet/in.h>
#include <sys/un.h>

#define MAXLINE 4096

int main(int argc, char **argv)
{
	int sockfd, portno;//socket and port number
	struct sockaddr_in servaddr, cliaddr;//server address and client address
	
	
	if (argc < 2) //if the number of command line arguments is less than 2
	{
		fprintf(stderr,"usage %s hostname port\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	portno = atoi(argv[1]);//setting the port number
	
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);//setting up the socket
	if (sockfd < 0) //if setting up the socket failed
	{
		perror("ERROR opening socket");
		exit(EXIT_FAILURE);
	}
	bzero(&servaddr, sizeof(servaddr)); //erasing the data for the servaddr.
	servaddr.sin_family = AF_INET;//setting to internet
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);//converting from host byte order to network byte order. INADDR_ANY 
	//means that the socket accepts connections to all IPs of the machine (IP address can be anything). 
	servaddr.sin_port = htons(portno);//setting the port

	if((bind(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr))) <0) //if binding falied
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}

	char call[MAXLINE] = "PONG";
	call[4] = '\0';
	int random, n;
	int maxFd = sockfd +1;
	socklen_t len; 
	char mesg[MAXLINE]; 
	srand(time(NULL));	
	printf("[server]: ready to accept data....\n");
	len = sizeof(cliaddr);//must have this particular part set up correctly or the first message the client sends wont
	//be sent back to the client!
	for( ; ;) //runs forever until a signal kills this program(SIGINT, SIGKILL, etc)
	{	
		random = rand();	
		
		n = recvfrom(sockfd, mesg, MAXLINE, 0, (struct sockaddr *) &cliaddr, &len);//recieved message from socket	
		if(random %10 <7) //condition on sending data back to the client
		{
			printf("[client]: %s \n", mesg);
			sendto(sockfd, call, n, 0, (struct sockaddr *) &cliaddr, len);
		}
		else //condition for not sending data to the cleint
		{
			printf("[server]: dropped packet \n");
		}
	
	}
} 
