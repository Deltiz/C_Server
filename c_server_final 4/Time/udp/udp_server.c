#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#define BUF_SIZE 0
#define PORT 8080 
//Should listen to port 37 but its restricted
void main(int argc, char **argv){
	int port = PORT;
	int sockfd;
	struct sockaddr_in si_me, si_other;
	char buffer [BUF_SIZE];
	socklen_t addr_size;
	while(1) {
		//Socket file descriptor
		sockfd=socket(AF_INET, SOCK_DGRAM, 0);
		
		memset(&si_me, '\0', sizeof(si_me));
		//Server info
		si_me.sin_family=AF_INET;
		si_me.sin_port=htons(port);
		si_me.sin_addr.s_addr = INADDR_ANY;

		//Bind
		bind(sockfd, (struct sockaddr*)&si_me, sizeof(si_me));
		printf("[+]Socket bound to server address\n");
		
		addr_size=sizeof(si_other);
		//Recieves empty datagram
		recvfrom(sockfd, buffer, BUF_SIZE, 0, (struct sockaddr*)&si_other, &addr_size);
		//Get current time
		time_t current_time = time(NULL);
		//Save as 32bit integer
		uint32_t time_data = current_time;//Time since 1970
						  //Need +70yrs since time from 1900
		time_data += 2208988800L; //70 years = 2,208,988,800 seconds
		//Convert to network byte order
		time_data = htonl(time_data);
		//Send the time data to client
		sendto(sockfd, &time_data, 4, 0, (struct sockaddr*)&si_other, addr_size);
		//Close connectec socket
		close(sockfd);
	}
	
}

