#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>
#define BUF_SIZE 0
#define PORT 8080 
//Should listen to port 37 but its restricted
void main(int argc, char **argv){
	int port = PORT;
	int sockfd;
	struct sockaddr_in serverAddr;
	char buffer [BUF_SIZE];
	socklen_t addr_size;
	//Socket file descriptor
	sockfd=socket(PF_INET, SOCK_DGRAM, 0);
	
	memset(&serverAddr, '\0', sizeof(serverAddr));
	//Server info
	serverAddr.sin_family=AF_INET;
	serverAddr.sin_port=htons(port);
    	inet_pton(AF_INET, "127.0.0.1",&serverAddr.sin_addr);

	//Send empty datagram
 	sendto(sockfd, buffer, BUF_SIZE, 0, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	addr_size=sizeof(serverAddr);
	uint32_t time_data;	
	recvfrom(sockfd, &time_data, 4, 0, (struct sockaddr*)&serverAddr, &addr_size);
	//Convert from network byte order to host byte order
	time_data = ntohl(time_data);
	//Subtract the 70 years to get time from 1970, standard time
	time_data -= 2208988800L;
	//Convert to time struct
	time_t current_time = time_data;
	//Print the time
	printf("The time is: %s\n", ctime(&current_time));
	//Close socket
	close(sockfd);
	
}
