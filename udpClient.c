#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <time.h>
#include <sys/time.h>

#define MAXLINE 4096
#define SIZE 256

int main(int argc, char *argv[])
{
	
	int sockfd, maxfd, portno, n, wait, firstTime =0, packetsRecieved =0, pktLoss =0;
	struct sockaddr_in serv_addr, cli_addr;//server and client addresses. type man struct sockaddr_in for more info
	struct hostent *server;
	struct timeval start, end, difference, timeout;//used for the select statement. 
	char buffer[SIZE];
	double timeDifference, max, min, average = 0.0;//time difference for 
	char message[SIZE] = "PING";
	fd_set readfds; //used for select. Set of file descriptors to check. 
	timeout.tv_sec =1; //setting the time out to 1 second
	timeout.tv_usec =0;
	if (argc < 3)
	{
		fprintf(stderr,"usage %s <hostname> <port>\n", argv[0]);
		exit(EXIT_FAILURE);
	}	
	portno = atoi(argv[2]);//setting up the port number
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);//setting up the socket
	if (sockfd < 0) //is setting up the socket failed
	{
		perror("ERROR opening socket");
		exit(EXIT_FAILURE);
	}

	server = gethostbyname(argv[1]);//setting up the server information
	if (server == NULL)
	{
		fprintf(stderr,"ERROR, no such host\n");
		exit(EXIT_FAILURE);
	}

	bzero((char *) &serv_addr, sizeof(serv_addr));//clearing the serv_addr information
	serv_addr.sin_family = AF_INET;//internet connection
	bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);//copying the hostent server
	//information into the serv_addr
	serv_addr.sin_port = htons(portno);

	maxfd = sockfd +1;
	char recvline[SIZE +1];
	message[4] = '\0';
	
	int i =0;
	int len;
	for(i; i < 10; i++)
	{	
		//The following statements must be done before select since we are in a loop
		
		FD_ZERO(&readfds);//clearing the read file descriptor
		FD_SET(sockfd, &readfds);//adding the read file descriptor to the set
		
		
		sendto(sockfd,message, sizeof(message), 0, (const struct sockaddr *) &serv_addr, sizeof(serv_addr));//sending message to server
		gettimeofday(&start, NULL);//starting the timer		
		
		wait = select(maxfd, &readfds, NULL, NULL, &timeout);			
		//if there was no packet loss, meaning the server sent the "pong" message back to the client
		if(wait >0)
		{

			packetsRecieved++;
			n = recvfrom(sockfd, recvline, MAXLINE, 0, NULL, NULL);
			printf("%s ", recvline);
			recvline[n] = '\0';
			if(n <0)
			{
				perror("ERROR reading from socket");
			}
			gettimeofday(&end, NULL);
			timeDifference = difftime(end.tv_usec, start.tv_usec) /1000;
		
			printf("Sent... RTT=%f \n", timeDifference);
			average += timeDifference;
			if(firstTime ==0) //setting max and min to the time difference the first time
			{
				max = timeDifference;
				min = timeDifference;	
			}
			else
			{
				if(timeDifference > max)
				{
					max = timeDifference;
				}
				else if(timeDifference < min){
					min = timeDifference;
				}
			}
			firstTime++;
		}
		//case being that the message was lost in the server
		else
		{
			printf("Sent... Timeout \n");
			pktLoss++;
			timeout.tv_sec =1; //setting the time out to 1 second
			timeout.tv_usec =0;
		}
	}


	char percent = '%';
	average = average / packetsRecieved; //dividing the number the total time by the number of packets recieved
	
	printf("10 pkts xmitted, %d rcvd, %d%c loss \n", packetsRecieved, pktLoss*10, percent);
	printf("min: %f ms, max: %f ms, avg: %f \n", min, max, average);
	close(sockfd);

	return 0;
}
